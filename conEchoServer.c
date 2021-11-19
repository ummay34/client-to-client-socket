#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections*/

int main (int argc, char **argv){
    int listenfd, connfd, connfd1, n, n1;
    pid_t childpid, childpid1;
    socklen_t clilen;
    char buf[MAXLINE], buf1[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //Create a socket for the Socket
    //If sockfd<0 there was an error in the creation of the socket

    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
    }


    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    //bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    //listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {

        clilen = sizeof(cliaddr);
        //accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("%s\n","Received request from client1...");

        connfd1 = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("%s\n","Received request from client 2...");

        if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

        printf ("%s\n","Child created for dealing with client1 requests");

        //close listening socket
        close (listenfd);

        while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
            printf("String received from client1: %s", buf);
            send(connfd1, buf, sizeof(buf), 0);
            bzero(buf, sizeof(buf));
        }

        if (n < 0)
            printf("%s\n", "Read error");
            exit(0);
        }

        if ( (childpid1 = fork ()) == 0 ) {//if it’s 0, it’s child process

        printf ("%s\n","Child created for dealing with client2 requests");

        while ( (n1 = recv(connfd1, buf1, MAXLINE,0)) > 0)  {
            printf("String received from client2: %s", buf1);
            send(connfd, buf1, sizeof(buf1), 0);
            bzero(buf1, sizeof(buf1));
        }

        if (n1 < 0)
            printf("%s\n", "Read error");
            exit(0);
        }

        //close socket of the server
        close(connfd);
    }
}
