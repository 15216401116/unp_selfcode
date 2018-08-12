#include    "../unp.h"

void str_cli(FILE* fp, int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    printf("Input your word:\n");
    while(fgets(sendline,MAXLINE,fp)!=NULL)
    {
        printf("Send your word to server\n");
        write(sockfd,sendline,strlen(sendline));
        if(read(sockfd, recvline, MAXLINE)==0)
            printf("read end\n");
        printf("Read from server:\n");
        fputs(recvline, stdout);
        memset(sendline,0,MAXLINE);
        memset(recvline,0,MAXLINE);
        printf("Input your word\n");
    }
}

int
main(int argc, char **argv)
{
    int                 sockfd, n;
    char                recvline[MAXLINE + 1];
    struct sockaddr_in  servaddr;

    if (argc != 2)
        printf("Input err. errno.%02d is: %s/n", errno, strerror(errno));

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Socket err. errno.%02d is: %s/n", errno, strerror(errno));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(9999);  /* daytime server */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        printf("IPaddress err. errno.%02d is: %s/n", errno, strerror(errno));

    if (connect(sockfd, (SA *) &servaddr, sizeof(SA)) < 0)
        printf("Connect err. errno.%02d is: %s/n", errno, strerror(errno));
    printf("connect succeed\n");

    str_cli(stdin, sockfd);
}
