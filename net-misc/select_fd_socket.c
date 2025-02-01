#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>




typedef enum {
    PROTO_HELLO,
} proto_type_e;


//TLV
typedef struct {
    proto_type_e type;
    unsigned short len; 
} proto_hdr_t;

void handle_client(int fd) {
    char buf[4096] = {0};
    proto_hdr_t *hdr = buf;
    hdr->type = htonl(PROTO_HELLO);
    hdr->len = sizeof(int);
    int reallen = hdr->len;
    hdr->len = htons(hdr->len);

    int *data = (int*)&hdr[1]; //pont to after the struct, write 1 aftev struct in mem
    *data = htonl(1); //write htonl 1 to where data  points
    
    write(fd, hdr, sizeof(proto_hdr_t) + reallen);
}

int main() {





    struct sockaddr_in serverInfo = {0};
    struct sockaddr_in clientInfo = {0};
    int clientSize = 0;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = 0;
    serverInfo.sin_port = htons(5555);


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    printf("%d\n", fd);

    //bind

    if (bind(fd, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    //listen
    if (listen(fd, 0) == -1) {
        perror("listen");
        close(fd);
        return -1;
    }


fd_set current_sockets, ready_sockets;
FD_ZERO(&current_sockets);
FD_SET(fd, &current_sockets); 

while (1) {
        ready_sockets = current_sockets;

    //accept
    if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
        perror("select error");
        return -1;
    }

    for (int i=0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &ready_sockets)) {
            if (i == fd) {
                //this is a new connection
                printf("FD: %d\n", i); // print server socket

                int cfd = accept(fd, (struct sockaddr *)&clientInfo,&clientSize);
                FD_SET(cfd, &current_sockets);
            } else {
                printf("FD: %d\n", i); // print client socket
        handle_client(i); 
        close(i);
        FD_CLR(i, &current_sockets);
            }


            }
        }
    }




}