#include    "../unp.h"

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
void sig_child(int signo)
{
    pid_t pid;
    int state;
    while ((pid = waitpid(-1,&state,WNOHANG)>0))
    {
        printf("Handle SIGCHLD, child %d terminated\n", pid);
    }
    return;
}


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
    struct sockaddr_in  server_addr, client_addr;
    socklen_t           server_len, client_len;
    char                buff[MAXLINE];
    char                addr[INET_ADDRSTRLEN]={0};
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
    Signal(SIGCHLD, sig_child);
    printf("LISTEN\n");
    for ( ; ; ) {
        printf("new loop\n");
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen))<0)
        {
            if (errno == EINTR)
            {
                printf("EINTR\n");
                continue;
            }
            else
                printf("accept failed\n");

        }
        printf("accept success\n");
        memset(&server_addr,0,sizeof(server_addr));
        memset(&client_addr,0,sizeof(client_addr));
        server_len = sizeof(server_addr);
        client_len = sizeof(client_addr);
        getsockname(connfd, (struct sockaddr *) &server_addr, &server_len);
        getpeername(connfd, (struct sockaddr *) &client_addr, &client_len);
        inet_ntop(AF_INET,&client_addr.sin_addr, addr,INET_ADDRSTRLEN);
        unsigned short p = ntohs(client_addr.sin_port);
        //unsigned int client_addr = ntohl(c_a.sin_addr.s_addr);
        printf("client addr:%s, client port : %u\n", addr,p);
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
