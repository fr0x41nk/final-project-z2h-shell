// playground for select() w/o network to start with

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

int main(void) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds); //stdin //we could pass network socket fd instead of 0

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv); 

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now!\n");
    else
        printf("No data within five seconds\n");

}
