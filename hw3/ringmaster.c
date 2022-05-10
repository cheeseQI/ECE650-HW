#include "potato.h"
#include "errorhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define PENDINGQUEUE 512  //how many pending connections queue will hold before kernel reject the new one
#define STOPSENDFLAG -1
/**
 * @brief get sockaddr(support IPv4 and IPv6). Referred from Beej's Guide
 * 
 * @param sa is the socket address
 * @return void* converted net input address
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr); //ipv4
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr); //ipv6
}
/**
 * @brief display the result for the game
 * 
 * @param p final potato passed back by player
 */
void displayResult(potato * p) {
    printf("Trace of potato:\n");
    for (int i = 0; i < p->pathCount - 2; i ++) {
        printf("%d,", p->path[i]);
    }
    printf("%d\n", p->path[p->pathCount - 2]);
}
/**
 * @brief ringmaster shut down the game, by sending dead potato to all players
 * 
 * @param fdmax max file descriptor
 * @param listen_fd listener fd itself does not need to be shut down now
 * @param flush_fds the file desctiptor set
 * @param p potato that will be send to players
 */
void shutDownGame(int fdmax, int listen_fd, fd_set flush_fds, potato * p) {
    p->isAlive = 0;
    for(int j = 0; j <= fdmax; j ++) { //notify all players with dead potato
        if (FD_ISSET(j, &flush_fds)) {
            if (j == listen_fd) continue;
            if (send(j, p, sizeof(potato), 0) == -1) perror("send");
        }
    }
}
/**
 * @brief make the ringmaster as a server, binding to a servive port
 * 
 * @param masterInfo address information containing ip/port/type
 * @return int socket file descriptor for this server
 */
int bindAsServer(struct addrinfo * masterInfo) {
    int sock;
    int FLAG = 1; //indicating non-zero is true
    struct addrinfo * p;
    // loop through all the results and bind the first usable ip::port we can use
    for(p = masterInfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket failed: ");
            continue;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &FLAG, sizeof(int)) == -1) {
            perror("setsockopt failed: ");
            exit(EXIT_FAILURE);
        }
        if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("bind failed: ");
            continue;
        }
        break;
    }
    if (p == NULL) { //no available binding target
        perror("server failed: ");
        exit(1);
    }
    if (listen(sock, PENDINGQUEUE) == -1) {
        perror("listen failed: ");
        exit(1);
    }
    freeaddrinfo(masterInfo);
    return sock;
}
/**
 * @brief notify all players about their neibour information, and player information
 * 
 */
void initalNotification(int playersNum, int fdmax, fd_set read_fds, int listen_fd, in_port_t ports[playersNum], struct sockaddr_storage addrArray[playersNum]) {
    int playerId = 0;
    for (int k = 0; k <= fdmax; k ++) {
        if (FD_ISSET(k, &read_fds)) {
            if (k == listen_fd) continue;
            struct sockaddr_storage leftAddr, rightAddr;
            memset(&leftAddr, 0, sizeof(leftAddr));
            memset(&rightAddr, 0, sizeof(rightAddr));
            in_port_t leftPort, rightPort;
            memset(&leftPort, 0, sizeof(leftPort));
            memset(&rightPort, 0, sizeof(rightPort));
            leftAddr = playerId == 0 ? addrArray[playersNum - 1] : addrArray[playerId - 1];
            leftPort = playerId == 0 ? ports[playersNum - 1] : ports[playerId - 1];
            rightAddr = playerId == playersNum - 1 ? addrArray[0] : addrArray[playerId + 1];
            rightPort = playerId == playersNum - 1 ? ports[0] : ports[playerId + 1];
            send(k, &leftPort, sizeof(leftPort), 0); //neighbour addr info
            send(k, &rightPort, sizeof(rightPort), 0);
            send(k, &leftAddr, sizeof(leftAddr), 0);
            send(k, &rightAddr, sizeof(rightAddr), 0);
            send(k, &playerId, sizeof(playerId), 0); //player info
            send(k, &playersNum, sizeof(playersNum), 0); 
            playerId ++;
        }
    }
}
//get random, since the time is short, the rand is almost same
int getRandomPlayer(int playersNum) {
    srand((unsigned int) time(NULL));
    return rand() % playersNum;
}

//sample input: ./ringmaster 7300 2 2
int main(int argc, char *argv[]) {
    if (checkRingArgument(argc, argv)) {
        return 1;
    }
    potato p; //game variables
    const char * port = argv[1]; 
    int playersNum = atoi(argv[2]);
    int hopsNum = atoi(argv[3]);
    int playerfdList[playersNum];
    int currPlayer = 0; 
    int listen_fd, service_fd; //socket variables
    struct addrinfo hints, *masterInfo;
    int rv;
    fd_set flush_fds;
    fd_set read_fds;
    int fdmax;
    struct sockaddr_storage playerAddr;
    struct sockaddr_storage addrArray[playersNum];
    in_port_t ports[playersNum];

    /*** start build server***/
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, port, &hints, &masterInfo)) != 0) {
        perror("cannot get hostname correctly");
        return EXIT_FAILURE;
    }
    printf("Potato Ringmaster\nPlayers = %d\nHops = %d\n", playersNum, hopsNum);
    listen_fd = bindAsServer(masterInfo);
    
    FD_ZERO(&flush_fds); //Listen and build service
    FD_ZERO(&read_fds);
    FD_SET(listen_fd, &flush_fds);
    fdmax = listen_fd;
    while(1) {                 
        read_fds = flush_fds; //flush last status
        if (playersNum == 0) {//all players had disconnected
            close(listen_fd);
            return EXIT_SUCCESS;
        }
        if (currPlayer == playersNum) { //enough players->trigger game
            int beginner = getRandomPlayer(playersNum);
            initalNotification(playersNum, fdmax, read_fds, listen_fd, ports, addrArray);
            initPotato(&p, hopsNum);
            if (hopsNum == 0) { //corner case: hops = 0
                shutDownGame(fdmax, listen_fd, flush_fds, &p);
                currPlayer = STOPSENDFLAG; //stop giving potato in the future.
                continue;
            }
            //adjustPotato(&p, beginner);
            p.path[p.pathCount] = beginner;
            p.pathCount ++;
            if (send(playerfdList[beginner], &p, sizeof(potato), 0) == -1) perror("send");
            currPlayer = STOPSENDFLAG;
            printf("Ready to start the game, sending potato to player %d\n", beginner);
        }

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            return EXIT_FAILURE;
        }
        for(int i = 0; i <= fdmax; i ++) { //when observed updates, find target fd
            if (FD_ISSET(i, &read_fds)) {
                if (i == listen_fd) { //update from listening socket, create service socket for acccepting
                    socklen_t addrLen = sizeof(playerAddr);
                    memset(&playerAddr, 0, addrLen);
                    service_fd = accept(listen_fd, (struct sockaddr *)&playerAddr,  &addrLen);
                    if (service_fd == -1) {
                        perror("accept");
                    } else {
                        printf("Player %d is ready to play\n", currPlayer);
                        addrArray[currPlayer] = playerAddr;
                        playerfdList[currPlayer] = service_fd;
                        recv(service_fd, &ports[currPlayer], sizeof(ports[currPlayer]), 0);
                        FD_SET(service_fd, &flush_fds);
                        fdmax = fdmax > service_fd ? fdmax : service_fd;
                        currPlayer ++;
                    }
                } else { //update from service socket
                    if ((recv(i, &p, sizeof(p), 0)) <= 0) { //got any error, or connection is closed by the player
                        close(i);
                        FD_CLR(i, &flush_fds);
                        playersNum --;
                    } else { //get potato back
                        displayResult(&p);
                        shutDownGame(fdmax, listen_fd, flush_fds, &p);
                    }
                }
            }
        }
    }
}

