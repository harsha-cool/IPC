#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "sys/un.h"
#include "unistd.h"

#define SOCKET_NAME "/tmp/ee"
#define BUFFER_SIZE 128
int main() {
    struct sockaddr_un name;

    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];

    // Step-1 Unlink the previous socket
    unlink(SOCKET_NAME);

    // Step-2 Create Master Socket for STREAM
    connection_socket = socket(AF_UNIX,SOCK_STREAM,0);
    if(connection_socket==-1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    else printf("Master Socket Success\n");

    //Step-3 Initialize name to use in bind()
    memset(&name,0,sizeof (struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path,SOCKET_NAME,sizeof (name.sun_path)-1);

    // Step-4 Bind to initmate OS
    ret = bind(connection_socket,(const struct sockaddr *) &name,sizeof (struct sockaddr_un));
    if(ret==-1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("bind() call Sucess\n");

    // Step-5 Prepare for accepting 20 connection
    ret = listen(connection_socket,20);
    if(ret==-1){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Step-6 infinite loop for server
    for(;;) {
        // Step-7 Wait for incoming connection using accept()
        printf("waiting on  accpt() sys call\n");
        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from client\n");
        result = 0;
        for (;;) {
            //Step-8 prepare the buffer to recv the data
            memset(buffer, 0, BUFFER_SIZE);
            printf("Waiting for the data from the client\n");

            // Step-9 read() blocking system call
            ret = read(data_socket, buffer, BUFFER_SIZE);
            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            // Step-10 store the data sent
            memcpy(&data, buffer, sizeof(int));
            result += data;
            if (data == 0) break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "Result is: %d", result);
        printf("Sending final result back to client\n");

        // Step-11 send the result using write()
        ret = write(data_socket, buffer, BUFFER_SIZE);
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        // Step-12 close the client handle
        close(data_socket);
    }

    // Step-13 close the master socket
    close(connection_socket);
    printf("Connection succeded\n");

    // Step-14 Unlink the socket
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}

