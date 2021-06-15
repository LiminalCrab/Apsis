#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>



int server_routine(void){
    struct sockaddr_in serv_addr;
    struct hostent *server;

    int sock_fide;

    sock_fide = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fide == -1){
        printf("Socket creation failed.\n");
        exit(0);
    }
    else {
        printf("Socket successfully established.\n");
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( 8888 );

    if (bind(sock_fide, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 8){
        perror("Bind failed. Error.");
        return 1;
    }

    printf("Socket closed.\n");
    close(sock_fide);
    return 0;

}