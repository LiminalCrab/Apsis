#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h> 
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 13500 // 9-15-00

/*
WHAT: Synchronize Apsis Systems. 
PURPOSE: Transmit's clock synchronization information to local devices.
*/


/* Server establishment, */
int est_server(void)
{
    int opt = 1;

    /* read set of file descriptors */
    fd_set read_fide; 
    char buffer[1025];
    char *message = "ECHO Synapsys v0.1 \r\n";

    /* Force max clients to 0 initially*/
    int max_clients;
    int client_socket[30];
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    /* master socket file descriptor */
    int maso_fide;
    if ((maso_fide = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* 
    sets socket option layer in file descriptor 
    to that maintains the same port number (13500)
    */
    if (setsockopt(maso_fide, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *) &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed to attach socket to port 13500");
        exit(EXIT_FAILURE);
    }
    /* 
    socket bindings,
    AF_INET: ipv4 protocols
    INADDR_ANY: Listen on all available interfaces.
    htons ip port number to network byte order
    */

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(maso_fide, (struct sockaddr *) &address, sizeof(address)) < 0 )
    {
         perror("bind failed");
         exit(EXIT_FAILURE);
    }
    printf("Listening on port %d \n", PORT);

    /*
    try to specify maximum of 3 pending connections for the master socket 
    */ 
    if (listen(maso_fide, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    /* Accept incoming addresses */
    int addrlen = sizeof(address);
    puts("Waiting for connections...");

    while (1) 
    {
        FD_ZERO(&read_fide);
        FD_SET(maso_fide, &read_fide);
        
        int max_sd = maso_fide;

        for (int i = 0; i < max_clients ; i++)
        {
            int sd = client_socket[i];

            if (sd > max_sd)
            {
                max_sd = sd;
            }

        }

        int activity;
        if ((activity < 0 ) && (errno != EINTR))
        {
            printf("Select error.");
        }

        int new_socket;
        if (FD_ISSET(maso_fide, &read_fide)){
            if ((new_socket = accept(maso_fide, (struct sockaddr *) &address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection\n, socket file descriptor: %d\n, IP: %s\n, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            if ( send(new_socket, message, strlen(message), 0) != strlen(message))
            {
                perror("send");
            }
        }

    }




    return 0;
}