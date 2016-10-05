#define _GNU_SOURCE

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
 
#include "plugins/memory.h"



// Cara compile gcc -pthread -o server-th server-th.c plugins/memory.c


 
// Thread function
void *connection_handler(void *);
 
// Main
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    // Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");
     
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(4949);
     
    // Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    
    printf("Bind done\n");
     
    // Listen
    listen(socket_desc , 3);
     
    // Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
     
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted\n");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        // Join the thread
        //pthread_join( sniffer_thread , NULL);
        printf("Handler assigned\n");
    
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}
 


/*
 * Handles connection for each client
 */
void *connection_handler(void *socket_desc)
{
    // get the socket descriptor
    int sock = *(int*)socket_desc;
    
    // how many characters are received
    int read_size;
    
    // message to send
    char *respond_message;
    
    // message from munin-master
    char client_message[2000];
     
    
    
	// get hostname 
	char hostname[200];
	
	if ((gethostname(hostname, 200)) == 0) {
		asprintf(&respond_message, "# munin node at %s\n", hostname);
		if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
			printf("Error: cannot send banner\n");
		}
		printf("You are logged in to: %s\n", hostname);
	} else {
		printf("Error: cannot get hostname\n");
	}
	
 
    
    // special case for list => input: list hostname
    char *list_full_msg;
    asprintf(&list_full_msg, "list %s", hostname);
    
    
    
    // receive a message from client
    while((read_size = recv(sock, client_message, 2000 , 0)) > 0 )
    {
		
		// gives null terminated
		client_message[strlen(client_message)-1] = '\0'; 
		
		
		// show the message from master
		printf("Received from master: %s - %ld\n", client_message, strlen(client_message));
        
        
        // send the message back to client
		char* config_result[12];
		
		if (strcmp(client_message, "cap") == 0 || strcmp(client_message, "cap multigraph dirtyconfig") == 0) {
			
			asprintf(&respond_message, "cap multigraph dirtyconfig\n");
			
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: cap\n");
			}	
			
		}
		
		else if (strcmp(client_message, "nodes") == 0) {

			asprintf(&respond_message, "%s\n.\n", hostname);
			
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: nodes\n");
			}		
			
		}

		else if (strcmp(client_message, "list") == 0 || strcmp(client_message, list_full_msg) == 0) {
		
			// lists items available for gathering for this host.
			// E.g. load, cpu, memory, df, etalia.
			// If no host is given, default to host that runs the munin-node
		
			asprintf(&respond_message, "%s\n", "memory");
			
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: list\n");
			}		
			
		}
	
		else if (strcmp(client_message, "config") == 0 || strcmp(client_message, "fetch") == 0) {
			
			asprintf(&respond_message, "%s\n.\n", "# Unknown service");
			
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: config or fetch only\n");
			}		
				
		}
	
		else if (strcmp(client_message, "config memory") == 0) {
			
			// call memory plugin to get the config data
			CONFIG_MEMORY(config_result);
		

			for (int idx = 0; idx < 12; idx++) {

				if (idx < 11) {
					asprintf(&respond_message, "%s\n", config_result[idx]);
				} else {
					asprintf(&respond_message, "%s\n.\n", config_result[idx]);
				}

				if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
					printf("Send failed: config memory\n");
				}
							
			}
			
		}
		
		else if (strcmp(client_message, "fetch memory") == 0) {
			
			// call memory plugin to get the fetch data
			int_fast64_t fetch_result[3];
						
			FETCH_MEMORY(fetch_result);


			char fetch_str[50];			
			char* fetch_msg[3];

			fetch_msg[0] = "total.value ";
			fetch_msg[1] = "used.value ";
			fetch_msg[2] = "free.value ";
			
			
			for (int i = 1;i < 3;i++) {
			
				//printf("FETCHED %d - %ld\n", i, fetch_result[i]);
			
				if (i == 1) {
					asprintf(&respond_message, "%s%ld\n", fetch_msg[i], fetch_result[i]);
				} else {
					asprintf(&respond_message, "%s%ld\n.\n", fetch_msg[i], fetch_result[i]);
				}
				
				if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
					printf("Send failed: fetch memory\n");
				}
				
			}
					
		}
		
		else if (strcmp(client_message, "version") == 0) {
		
			asprintf(&respond_message, "%s%s%s\n", "Crazy node on ", hostname, " version: 0.1\0");
		
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: version\n");
			}		
			
		} 
		
		else if (strcmp(client_message, "quit") == 0 || strcmp(client_message, "quit ") == 0) {

			printf("Connection closed by client");
			break;

		} 
		
		else {
		
			asprintf(&respond_message, "%s\n", "# Unknown command. Try cap, list, nodes, config, fetch, version or quit");
			
			if (send(sock, respond_message, strlen(respond_message), 0) < 0) {
				printf("Send failed: unknown command\n");
			}	
			
		}
		
		memset(client_message, 0, sizeof(client_message));
		 
    }
    
     
    if(read_size == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("Receive failed");
    }
         
    // free the socket pointer
    free(socket_desc);
    close(sock);
    
    return 0;
}
