#include "potato.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define pendingQueue 3

/**
 * @brief get sockaddr(support IPv4 and IPv6). Referred from Beej's Guide
 * 
 * @param sa is the socket address
 * @return void* converted net input address
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//get random, since the time is short, the rand is almost same
int getRandomPlayer(int playersNum) { //need to be simplified with master!!!!
    srand((unsigned int) time(NULL));
    return rand() % playersNum;
}
/**
 * @brief make player as a client, connecting to a server
 * 
 * @param targetIP address information containing ip/port/type
 * @return int file descriptor for this service socket
 */
int connectToServer(struct addrinfo * targetIP) {
    int sock;
    struct addrinfo * p;
    for(p = targetIP; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket failed: ");
            continue;
        }
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("connect failed: ");
            continue;
        }
        break;
    }
    if (p == NULL) { //no available binding target
        perror("connect to serverfailed: ");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(targetIP);
    return sock;
}
/**
 * @brief make player as a server, binding to a servive port
 * 
 * @param masterInfo address information containing ip/port/type
 * @return int socket file descriptor for this listener
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
    if (p == NULL) {
        perror("server failed: ");
        exit(1);
    }
    if (listen(sock, pendingQueue) == -1) {
        perror("listen failed: ");
        exit(1);
    }
    freeaddrinfo(masterInfo);
    return sock;
}

//eg ./player dku-vcm-1505.vm.duke.edu 7300
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr,"usage: client hostname portname\n");
        exit(1);
    }
    int playerId, playerNums; //game variable
    potato p;
    memset(&p, 0, sizeof(p));
    int rv; //socket variable
    fd_set flush_fds; // manage fds
    fd_set read_fds; // fds for select()
    int fdmax;
    int sockRingFd, nextPlayerFd, listenerFd;  
    struct addrinfo hints, server_hints, neigbour_hints,*masterInfo, *nextInfo, *selfInfo;
    struct sockaddr_storage left, right, side;
    char leftBuffer[INET6_ADDRSTRLEN], rightBuffer[INET6_ADDRSTRLEN];
    in_port_t leftPort, rightPort;

    /*** binding  phase ***/
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    memset(&neigbour_hints, 0, sizeof(neigbour_hints));
    neigbour_hints.ai_family = AF_UNSPEC;
    neigbour_hints.ai_socktype = SOCK_STREAM;
    memset(&server_hints, 0, sizeof(server_hints));
    server_hints.ai_family = AF_UNSPEC;
    server_hints.ai_socktype = SOCK_STREAM;
    server_hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, "0", &server_hints, &selfInfo)) != 0) { //get hostname with free port into ip form and store into plist
        perror("cannot get addrinfo for myself");
        return EXIT_FAILURE;
    }
    listenerFd = bindAsServer(selfInfo);
    /*** connect to ringMaster phase ***/
    struct sockaddr addrWithPort;
    socklen_t adLen = sizeof(addrWithPort);
    memset(&addrWithPort, 0, adLen);
    getsockname(listenerFd, &addrWithPort, &adLen);
    in_port_t myPort = ((struct sockaddr_in *) &addrWithPort)->sin_port;
    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &masterInfo)) != 0) { //servinfo list is the ip list, if set port to 0, it will be default
        perror("cannot get addrinfo for ring master");
        return EXIT_FAILURE;
    }
    sockRingFd = connectToServer(masterInfo);
    send(sockRingFd, &myPort, sizeof(myPort), 0);
    recv(sockRingFd, &leftPort, sizeof(leftPort), 0);
    recv(sockRingFd, &rightPort, sizeof(rightPort), 0);
    recv(sockRingFd, &left, sizeof(left), 0);
    recv(sockRingFd, &right, sizeof(right), 0);
    const char * lIP = inet_ntop(left.ss_family, get_in_addr((struct sockaddr*)&left), leftBuffer, INET6_ADDRSTRLEN);
    const char * rIP = inet_ntop(right.ss_family, get_in_addr((struct sockaddr*)&right), rightBuffer, INET6_ADDRSTRLEN);
    recv(sockRingFd, &playerId, sizeof(playerId), 0);
    recv(sockRingFd, &playerNums, sizeof(playerNums), 0);
    printf("Connected as player %d out of %d total players\n", playerId, playerNums);
    /*** get get potato phase ***/
    FD_ZERO(&flush_fds);
    FD_ZERO(&read_fds);
    FD_SET(listenerFd, &flush_fds);
    FD_SET(sockRingFd, &flush_fds);
    fdmax = listenerFd > sockRingFd ? listenerFd : sockRingFd;
    int service_fd;
    while (1) {
        read_fds = flush_fds; //flush last status
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            return EXIT_FAILURE;
        }
        for (int i = 0; i <= fdmax; i ++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == sockRingFd) { //From ring master
                    recv(i, &p, sizeof(p), 0);
                    if (!p.isAlive) { //dead potato signal->close all socket
                        for (int i = 0; i <= fdmax; i ++) {
                            if (FD_ISSET(i, &read_fds)) {
                                close(i);
                            }
                        } 
                        return EXIT_SUCCESS;
                    }
                } else if (i == listenerFd) { //From listener
                    socklen_t addrLen = sizeof(side);
                    memset(&side, 0, addrLen);
                    service_fd = accept(listenerFd, (struct sockaddr *)&side,  &addrLen);
                    recv(service_fd, &p, sizeof(p), 0);
                    FD_SET(service_fd, &flush_fds);
                    fdmax = fdmax > service_fd ? fdmax : service_fd;
                } else {           //From service socket->must be potato from other player or error/close
                    if (recv(i, &p, sizeof(p), 0) <= 0) {
                        close(i);
                        FD_CLR(i, &flush_fds);
                    }
                    continue;
                }
                /*** send potato out ***/
                int next = getRandomPlayer(2); //0 or 1
                int nextId;
                if (next == 0) {
                    nextId = playerId - 1 < 0 ? playerNums - 1 : playerId - 1;
                    adjustPotato(&p, nextId);
                } else {
                    nextId = playerId + 1 >= playerNums ? 0 : playerId + 1;
                    adjustPotato(&p, nextId);
                }
                if (p.remainingHops == 0) { //decide end or not
                    printf("I'm it\n");
                    send(sockRingFd, &p, sizeof(p), 0);
                    break;
                }
                const char * nextIP = next == 0 ? lIP : rIP;
                in_port_t nextP = next == 0 ? leftPort : rightPort;
                char nextPort[6];
                sprintf(nextPort, "%d", htons(nextP));
                if ((rv = getaddrinfo(nextIP, nextPort, &neigbour_hints, &nextInfo)) != 0) {
                    fprintf(stderr, "cannot get addrinfo for next player: %s\n", gai_strerror(rv));
                    return EXIT_FAILURE;
                }
                nextPlayerFd = connectToServer(nextInfo);
                send(nextPlayerFd, &p, sizeof(p), 0); //must send out(connect to server) no matter what changes
                FD_SET(nextPlayerFd, &flush_fds);
                fdmax = fdmax > nextPlayerFd ? fdmax : nextPlayerFd;
                printf("Sending potato to %d\n", nextId);
            }
        }
    }
}