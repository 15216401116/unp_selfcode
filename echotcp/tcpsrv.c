#include    "unp.h"

void str_echo( int sockfd )
{
    ssize_t n;
    char    buf[MAXLINE];
again:
    printf("Get message from client:\n");
    while((n=read(sockfd, buf,MAXLINE))>0)
    {
        printf("%s",buf);
        printf("send this message to client\n");
        write(sockfd, buf, n);
        memset(buf,0,MAXLINE);
        printf("Get message from client:\n");
    }
    if( n<0 && errno == EINTR)
        goto again;
    else if ( n<0 )
    {
        printf("Read error\n");

    }
}


int
main(int argc, char **argv)
{
    int                 listenfd, connfd;
    struct sockaddr_in  servaddr, cliaddr;
    char                buff[MAXLINE];
    time_t              ticks;
    socklen_t           clilen;
    pid_t               childpid;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        printf("Socket err. errno.%02d is: %s/n", errno, strerror(errno));

    printf("SOCKET\n");
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(9999); 

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))==-1)
        printf("bind error\n");


    if (listen(listenfd, LISTENQ)==-1)
        printf("listen error\n");

    printf("LISTEN\n");
    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        printf("accept success\n");
        if ((childpid = fork())==0)
        {
            printf("Inside child work process\n");
            close(listenfd);
            str_echo(connfd);
            close(connfd);
            exit(0);

        }
        else
        {
            printf("Inside parent process\n");
            close(connfd);

        }
    }
}
