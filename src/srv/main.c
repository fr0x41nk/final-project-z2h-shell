#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "common.h"
#include "srvpoll.h"
#include "file.h"
#include "parse.h"
#include "srvpoll.h"

#define MAX_CLIENTS 256
clientstate_t clientStates[MAX_CLIENTS] = {0};


void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    printf("\t -u - Update hours on userID, usage: -u idN, hoursN\n");
    printf("\t -r - Remove id\n");
    return;
}

void poll_loop(unsigned short port, struct dbheader_t *dbhdr, struct employee_t *employees) {
  int listen_fd, conn_fd, freeSlot;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  struct pollfd fds[MAX_CLIENTS + 1];
  //struct clientstate_t clientStates;
  int nfds = 1;
  int opt = 1;

  
  init_clients(&clientStates);

  // create listening socket
  if ((listen_fd, socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
      }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }

      //set up sevrer address structure

      memset(&server_addr, 0, sizeof(server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_addr.s_addr = INADDR_ANY;
      server_addr.sin_port = htons(port);

      if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
	perror("bind");
	exit(EXIT_FAILURE);
								 
      }

      //listen
      if (listen(listen_fd, 10) == -1) {
	perror("listen");
	exit(EXIT_FAILURE);
      }

      printf("Server listening on port %d\n", PORT);

      memset(fds, 0, sizeof(fds));
      fds[0].fd = listen_fd;
      fds[0].events = POLLIN;
      nfds = 1;

    while (1) {
	int ii = 1;
	for (int i = 0; i < MAX_CLIENTS; i++) {
	  if(clientStates[i].fd != -1) {
	    fds[ii].fd = clientStates[i].fd;
	    fds[ii].events = POLLIN;
	    i++;
	  }
	}
	//wait for an event on one of the sockets
	int n_events = poll(fds, nfds, -1);
	if (n_events == -1) {
	  perror("poll");
	  exit(EXIT_FAILURE);
	}

	// check for new connoctions
	if (fds[0].revents & POLLIN) {
	  if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
	    perror("accept");
	    continue;
	  }

	  printf("New connection from %s:%d\n",
		 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
         
         freeSlot = find_free_slot(&clientStates);
         if (freeSlot == -1) {
            printf("Server full: closing new connection\n");
            close(conn_fd);
         } else {
            clientStates[freeSlot].fd = conn_fd;
            clientStates[freeSlot].state = STATE_CONNECTED;
            nfds++;
            printf("Slot %d has fd &d\n", freeSlot, clientStates[freeSlot].fd);
         }

         n_events--;
    }
    // check each client for read/write activity

    for (int i = 1;i <= nfds &&n_events > 0; i ++) {
        if (fds[i].revents & POLLIN) {
            n_events--;

            int fd = fds[i].fd;
            int slot = find_slot_by_fd(&clientStates,fd);
            ssize_t bytes_read = read(fd, &clientStates[slot].buffer, sizeof(clientStates[slot].buffer));
            if (bytes_read <= 0) {
                close(fd);
                if (slot == -1) {
                    printf("Tried to close fd that doesnt exist?\n");
                } else {
                    clientStates[slot].fd = -1;
                    clientStates[slot].state = STATE_DISCONNECTED;
                    printf("Client disconnected or error\n");
                    nfds--;
                }
            } else {
                printf("Receveied data from client: %s\n", clientStates[slot].buffer);
            }
        }
    }

}
}



	  





int main(int argc, char *argv[]) {
    char *filepath = NULL;
    char *addstring = NULL;
    char *update = NULL;
    char *rmid = NULL;
    char *portarg = NULL;
    int port = 0;
    bool newfile = false;
    int c;
    bool list = false;
    bool idlist = false;

    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:p:u:r:li")) != -1) {
        switch (c) {
            case 'n':
                    newfile = true;
                    break;
            case 'f':
                    filepath = optarg;
                    break;
            case 'p':
                    portarg = optarg;
                    port = atoi(portarg);
                    if (port == 0) {
                        printf("Bad port: %s\n", portarg);
                    }
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
        //dbhdr->count--;
       // employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
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

    poll_loop(port,dbhdr,employees);

    output_file(dbfd, dbhdr, employees);

    return 0;
}

