#include	"unp.h"

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        printf("Socket err. errno.%02d is: %s/n", errno, strerror(errno));

    printf("SOCKET\n");
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9999);	/* daytime server */

	if (bind(listenfd, (SA *) &servaddr, sizeof(SA))==-1)
        printf("bind error");


	if (listen(listenfd, LISTENQ)==-1)
        printf("listen error");

    printf("LISTEN\n");
	for ( ; ; ) {
		connfd = accept(listenfd, (SA *) NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        printf("CONNECTION\n");
        write(connfd, buff, strlen(buff));

		close(connfd);
	}
}
