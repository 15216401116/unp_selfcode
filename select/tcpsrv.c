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
    int                 listenfd, connfd, i, maxi, maxfd, sockfd;
    int                 nready, client[FD_SETSIZE];
    fd_set              rset, allset;
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
    for (int j=0;j<FD_SETSIZE;j++)
    {
        client[j]=-1;
    }
    i = 0;
    maxi = -1;
    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    for ( ; ; ) {
        printf("new loop\n");
        rset = allset;
        nready = select(maxfd+1, &rset,NULL,NULL,NULL);
        if(FD_ISSET(listenfd, &rset))
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        }
        for(i=0;i<FD_SETSIZE;i++)
        {
            if(client[i]<0)
            {
                client[i]=connfd;
                break;
            }

        }
        if(i == FD_SETSIZE)
            printf("too many clients\n");
        FD_SET(connfd, &allset);
        if (connfd> maxfd)
        {
            maxfd = connfd;
        }
        if(i>maxi)
        {
            maxi = i;
        }
        if(--nready<=0)
        {
            continue;
        }
        for(i=0;i<=maxi;i++)
        {
            if((sockfd = client[i])<0)
                continue;
            if(FD_ISSET(sockfd, &rset))
            {

                if((n=read(sockfd, buf,MAXLINE))==0)
                {
                    close(sockfd);
                    FD_CLR(sockfd,&allset);
                    client[i]=-1;
                }
                else
                {
                    write(sockfd, buf, n);
                }
                if(--nready<=0)
                    break;
            }

        }
    }
}
