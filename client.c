#include "utilities.h"

//funzione che prende in input da tastiera il messaggio che si vuole inviare al server
void mex(char *buf){
    printf("> ");
    fflush(stdout);
    memset(buf, '\0', BUFSIZE);
    if (read(STDIN_FILENO, buf, BUFSIZE) == -1){
        perror("leggendo dallo standard input:");
        exit(EXIT_FAILURE);
    }
}

int main(void){
    struct sockaddr_un serv_addr;
    int sockfd;
    
    //creazione e connessione al server con utilizzo di socket
    SYSCALL(sockfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

    int dummy_int, n;
    SYSCALL(dummy_int, connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "connect");
    char buf[BUFSIZE];
    //permette di continuare a comunicare con il server finchè non si riceve la stringa "quit"
    do{
        mex(buf);
        if (strncmp(buf, "quit", 4) == 0){
            break;
        }
        
        //invio del messaggio al server
        SYSCALL(dummy_int, writen(sockfd, buf, strlen(buf)), "writen");

        //ricezione del messaggio dal server
        SYSCALL(n, read(sockfd, buf, BUFSIZE), "read");
        buf[n] = '\0';  //inserisco carattere di terminazione per permettere poi di stampare il messaggio correttamente

        printf("%s\n", buf);
    } while (1);

    //chiusura socket
    close(sockfd);

    return 0;
}