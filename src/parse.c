#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

//TODO: Remove entry
//TODO: Update hours

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
    int i = 0;
    for (; i < dbhdr->count; i++) {
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n", employees[i].hours);

    }
}

int rmid_user(struct dbheader_t *dbhdr, struct employee_t *employees, int rmid) {
    //int i = 0;
    //int counter = 0;
 // counter = atoi(rmid);
        //printf("FFFFF Employee %d\n", counter);
        //printf("Debug: rmid = %d, name = %s\n", rmid, employees[rmid].name ? employees[rmid].name : "(NULL)");

       //  printf("Removing user with ID %d: %d\n", rmid, employees[rmid].name);

        //printf("Removing user %s\n", employees[rmid].name);
       // employees[dbhdr->count].name == NULL;
      //  employees[dbhdr->count].address == NULL;
       // employees[dbhdr->count].hours == NULL;
        //employees[counter] == NULL;

     //   printf("Removing after user %s\n", employees[dbhdr->count].name);

        //strncpy(employees[dbhdr->counter].name,NULL,sizeof(NULL));

        //strncpy(employees[dbhdr->count], 0, sizeof(employees[dbhdr->counter].name));


    //strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
    //strncpy(employees[dbhdr->count-1].address, addr, sizeof(employees[dbhdr->count-1].address));


        return STATUS_SUCCESS;
    
}


void idlist_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
    int i = 0;
    for (; i < dbhdr->count; i++) {
        printf("Employee %d\n", i);
    }
}

int update_hours(struct dbheader_t *dbhdr, struct employee_t *employees, char *update) {
    printf("%s\n",update);

    int position = atoi(strtok(update, ","));
    int *hours = strtok(NULL, ",");

    //int counter = 0;
    //counter = atoi(count);

    printf("\tOriginal Hours: %d\n", employees[position].hours);

    printf("Before: %s %s\n", position, hours);
    employees[position].hours = atoi(hours);
    printf("After: %s %s\n", position, hours);

return STATUS_SUCCESS;

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
    printf("%s\n",addstring);

    char *name = strtok(addstring, ",");

    char *addr = strtok(NULL, ",");

    char *hours = strtok(NULL, ",");

    printf("%s %s %s\n", name, addr, hours);


    strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
    strncpy(employees[dbhdr->count-1].address, addr, sizeof(employees[dbhdr->count-1].address));

    employees[dbhdr->count-1].hours = atoi(hours);


    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
    if (fd < 0) {
        printf("God a bad FD from the user\n");
        return STATUS_ERROR;
    }


    int count = dbhdr->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == -1 ) {
        printf("Malloc failed!\n");
        return STATUS_ERROR;
    } 

    read(fd, employees, count*sizeof(struct employee_t));

    int i = 0;
    for (; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;

}


void output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    if (fd < 0) {
        printf("God a bad fd from the user\n");
        return STATUS_ERROR;
    }

    int realcount = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);
    
    write(fd, dbhdr, sizeof(struct dbheader_t));
        
    int i = 0;
    for (; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i],sizeof(struct employee_t));
    } 
        
    return STATUS_SUCCESS;

}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) {
        printf("God a bad FD from the user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Malloc failed to create dbheader\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);
    
    if (header->magic != HEADER_MAGIC) {
        printf("Improper header magic!\n");
        free(header);
        return -1;
    }


    if (header->version != 1) {
        printf("Improper header version!\n");
        free(header);
        return -1;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("Corrupted database!\n");
        free(header);
        return -1;
    }

    *headerOut = header;

}

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Malloc failed to create db header\n");
        free(header);
        free(headerOut);
        return STATUS_ERROR;
    }

    header->version = 0x01;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);
	
    *headerOut = header; 

    return STATUS_SUCCESS;
}


//16:06 minute video