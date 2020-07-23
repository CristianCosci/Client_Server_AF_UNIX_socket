#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//per server
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define SOCKNAME "./socket"
#define BUFSIZE 256

//macro per chiamate di sistema
#define SYSCALL(returnval, systemcall, e) \
    if ((returnval = systemcall) == -1){ \
        perror(e);       \
        exit(errno);     \
    }

//funzioni per comunicazione tra client e server (ricezione e invio messaggi)(prese da esercizio su esercitazione)
static inline int readn(long fd, void *buf, size_t size){
    size_t left = size;
    int r;
    char *bufptr = (char *)buf;
    while (left > 0){
        if ((r = read((int)fd, bufptr, left)) == -1){
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            return 0;
        left -= r;
        bufptr += r;
    }
    return size;
}

static inline int writen(long fd, void *buf, size_t size){
    size_t left = size;
    int r;
    char *bufptr = (char *)buf;
    while (left > 0){
        if ((r = write((int)fd, bufptr, left)) == -1){
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            return 0;
        left -= r;
        bufptr += r;
    }
    return 1;
}
