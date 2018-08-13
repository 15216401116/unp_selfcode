#include<sys/types.h>
#include<sys/socket.h>
#define SA  struct sockaddr
#define MAXLINE     4096    /* max text line length */
#define BUFFSIZE    8192    /* buffer size for reads and writes */
#include <errno.h>
#include <stdio.h> 
#include<netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define LISTENQ     1024
#include    <time.h>
#include <signal.h>
#include <sys/wait.h>
void (*Signal(int signo, void(*func)(int sg)))(int)
 {
     struct sigaction act, oldact;
     act.sa_handler = func;
     sigemptyset(&act.sa_mask);
     act.sa_flags = 0;
     if(sigaction(signo, &act, &oldact)<0)
     {
         return SIG_ERR;
     }
     return oldact.sa_handler;
 }

