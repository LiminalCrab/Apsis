#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
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

    /* Socket file descriptor */
    int so_fide;
    if ((so_fide = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* 
    sets socket option layer in file descriptor 
    to that maintains the same port number 
    */
    if (setsockopt(so_fide, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
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

    if (bind(so_fide, (struct sockaddr *) &address, sizeof(address)) <0 )
    {
         perror("bind failed");
         exit(EXIT_FAILURE);

    }

    return 0;
}