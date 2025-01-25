#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"



void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    printf("\t -u - Update hours on userID, usage: -u idN, hoursN\n");
    printf("\t -r - Remove id\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    char *addstring = NULL;
    char *update = NULL;
    char *rmid = NULL;
    bool newfile = false;
    int c;
    bool list = false;
    bool idlist = false;

    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:u:r:li")) != -1) {
        switch (c) {
            case 'n':
                    newfile = true;
                    break;
            case 'f':
                    filepath = optarg;
                    break;
            case 'a':
                    addstring = optarg;
                    break;
            case 'l':
                    list = true;
                    break;
            case 'i':
                    //print the ids only
                    idlist = true;
                    break;

            case 'u':
                    //update hours
                    update = optarg;
                    break;
            case 'r':
                    rmid = optarg;
                    break;
            case '?':
                    printf("Unknown option -%c\n", c);
                    break;
            default:
                    return -1;
        }

    }

        if (filepath == 0) {
            printf("Filepath is a required argument\n");
            print_usage(argv);
            return 0;
        }

    if (newfile) {
       dbfd = create_db_file(filepath);
       if (dbfd == STATUS_ERROR) {
        printf("Unable to create database file\n");
        return -1;
       }

       if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
        free(dbfd);
        printf("Failed to create database header\n");
       
       }
    }
    
     else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }
        if(validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Failed to validate database header!");
            return -1;
        }
    }
    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees");
        free(dbhdr);
        free(dbfd);
        free(employees);
        return 0;
    }

    if (addstring) {
        dbhdr->count++;
        employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
        add_employee(dbhdr,employees,addstring);
    }

    //update hours on userid

    if (update) {
        printf("Updating hours on user\n");
        employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
        update_hours(dbhdr,employees,update);
    }

    //rm userid
    if (rmid) {
        dbhdr->count--;
        employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
       printf("removing id function entered\n");
        output_file_rm(dbhdr, employees, rmid);
    }
        //output_file(dbfd, dbhdr, employees);

    //    update_hours(dbhdr,employees,update);
    //}
    if (list) {
        list_employees(dbhdr, employees);
    }
    if (idlist) {
        idlist_employees(dbhdr, employees);
    }


    printf("Newfile: %d\n", newfile);
    printf("Filepath: %s\n", filepath);



    output_file(dbfd, dbhdr, employees);

    return 0;
}

