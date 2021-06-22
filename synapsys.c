#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>

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
    int max_clients = 30;
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
        activity = select (max_sd + 1, &read_fide, NULL, NULL, NULL);
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

            if ( send (new_socket, message, strlen(message), 0) != (ssize_t)strlen)
            {
                perror("send");
            }

            for (int i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; i++)
        {
            int sd = client_socket[i];

            if (FD_ISSET(sd, &read_fide))
            {
                int valread;
                if ((valread = read(sd, buffer, 1024)) == 0)
                {
                    getpeername(sd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
                    printf("Host disconnected\n ip: %s \n, port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close( sd ); 
                    client_socket[i] = 0;
                }

                else
                {
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }

    }


    return 0;
}