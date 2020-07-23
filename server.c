#define _POSIX_C_SOURCE 200112L 

#include "utilities.h"

//funzione di clean-up che rimuove il socket quando il server si chiude
void cleanUp(){
    if (unlink(SOCKNAME) < 0){
        perror("unlink");
        _exit(EXIT_FAILURE);
    }
    _exit(EXIT_SUCCESS);
}

//funzione che inverte il lettercase dei caratteri
void reverseLetterCase(char *x, int begin, int end){ 
    char c = *(x + begin);
    if(begin > end ){
        return;
    }
    if(isupper(c)){
        *(x + begin) = tolower(c);
    }else{
        if(islower(c)){
            *(x + begin) = toupper(c);
        }
    }
    reverseLetterCase(x, ++begin, end);
}

//funzione per controllo su caratteri white-space
int controllo(char *x, int begin, int end){
    char c = *(x + begin);
    if (begin > end){
        return 0;
    } 
    int result = isspace(c);
    if(result){
        return 1;
    }
    return controllo(x, ++begin, end);


    //altro modo per controllare i caratteri white-space inseriti
    /*
    if((c == ' ')||(c == '\f')||(c == '\t')||(c == '\r')||(c == '\v')){
        return 1;
    }
    return controllo(x, ++begin, end);
    */
}

//thread che si occupa della conversazione con un client
void *thread_gestisci_client(void *arg){
    long connfd = (long)arg;

    //ciclo ripetuto finche non viene chiusa la connessione con il relativo client
    //-riceve un messaggio dal client
    //-controlla se il messaggio ricevuto contiene caratteri white-space, nel caso ritorna un messaggio di errore al client
    //-se non ci sono caratteri white-space, inverte il lettercase del messaggio ricevuto e lo invia al client
    do{
        char buffer[BUFSIZE];
        memset(buffer, 0x0, BUFSIZE);
        int n;
        SYSCALL(n, read(connfd, buffer, BUFSIZE), "read");
        if (n == 0){
            break;
        }
        if(controllo(buffer, 0, n - 2)){
            SYSCALL(n, writen(connfd, "ERRORE: la stringa conteneva caratteri white-space." , 51), "write");
        } else{
            reverseLetterCase(buffer, 0, n - 2);
            SYSCALL(n, writen(connfd, buffer, sizeof(buffer)), "write");
        }

    } while (1);
    close(connfd);
    return NULL;
}

//funzione che genera un nuovo thread per ogni client che si connette al server
void genera_thread(int n_fd){
    pthread_attr_t tid;
    pthread_t thid;

    //inizializzazione attributo
    if (pthread_attr_init(&tid) != 0){
        perror("pthread_attr_init");
        close(n_fd);
        return;
    }
    // settiamo il thread in modalità detached
    if (pthread_attr_setdetachstate(&tid, PTHREAD_CREATE_DETACHED) != 0){
        perror("pthread_attr_setdetachstate");
        //liberare risorse usate per tid(attributo)
        pthread_attr_destroy(&tid);
        close(n_fd);
        return;
    }
                                                            
    if (pthread_create(&thid, &tid, thread_gestisci_client, (void *)(intptr_t)n_fd) != 0){
        perror("pthread_create");
        //liberare risorse usate per l'attributo
        pthread_attr_destroy(&tid);
        close(n_fd);
        return;
    }
}

//funzione per thread dedicato per la gestione dei segnali
static void * signal_thread(void *arg){
    sigset_t *set = arg;
    int s, sig;
    while(1){
        s = sigwait(set, &sig);
        if(s != 0){
            errno = s;
            perror("sigwait");
            exit(EXIT_FAILURE);
        }
        printf("rimuovo socket file \n");
        fflush(stdout);
        cleanUp();
    }
    
}

int main(void){
    if (atexit(cleanUp) < 0){
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    sigset_t set;
    sigemptyset(&set);
    pthread_t thread;

    //segnali su cui dovrà lavorare il thread
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGPIPE);
    
    //settaggio e creazione del thread dedicato che si occuperà della gestione dei segnali
    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0){
        fprintf(stderr, "pthread_sigmask failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread, NULL, &signal_thread, (void *)&set) != 0){
        fprintf(stderr, "pthread_create failed\n");
        exit(EXIT_FAILURE);
    }

    int listenfd;
    SYSCALL(listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket");
    // setto l'indirizzo
    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);
    int dummy_int;

    // assegno l'indirizzo al socket
    SYSCALL(dummy_int, bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "bind");
    SYSCALL(dummy_int, listen(listenfd, 1), "listen");

    int n_fd;
    //ciclo infinito in cui si attende la connessione di un client per poi assegnarlo ad un thread dedicato
    do{
        SYSCALL(n_fd, accept(listenfd, (struct sockaddr *)NULL, NULL), "accept");
        genera_thread(n_fd);
    } while (1);

    close(listenfd);
    return 0;
}