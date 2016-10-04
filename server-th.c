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
        printf("Could not create socket");
    }
    puts("Socket created");
     
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9999);
     
    // Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    // Listen
    listen(socket_desc , 3);
     
    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
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
        puts("Handler assigned");
    
		//pthread_join( sniffer_thread , NULL);
        
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
    // Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    //char *message ,client_message[2000];
    char *message;
    char client_message[2000];
     
     
    // Send some messages to the client
	message = "I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something\n";
    write(sock , message , strlen(message));
    
    
    
    
    // Get hostname & send banner
    
    char hostname[200];
    //char* banner = strdup("# munin node at ");
	char banner[] = "# munin node at ";
	
    if ((gethostname(hostname, 200)) == 0) {	
		strcat(banner, hostname);
		strcat(banner, "\n");	
		if (send(sock, banner, strlen(banner)+1, 0) < 0) {
			
			puts("Error: cannot send banner");
		}
		printf("You are logged in to: %s\n", hostname);
    } else {
		puts("Error: cannot get hostname");
    }
 
    
    
    // Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
    
		
		char* config_result[12];
		
		if (strcmp(client_message, "cap\n") == 0) {
			
			//char str[100] = "\0";
			char str[] = "cap multigraph dirtyconfig";
			//strcat(str, "cap multigraph dirtyconfig");
			//strcat(str, "\n");
			
			if (send(sock, str, strlen(str), 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}	
			
		}
		
		else if (strcmp(client_message, "nodes\n") == 0) {

			//char nodes_msg[20] = "\0";
			
			char nodes_msg[20];
			
			strcat(nodes_msg, hostname);
			//strcat(nodes_msg, "\n.");

			if (send(sock, nodes_msg, strlen(nodes_msg) + 1, 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}		
			
		}

		else if (strcmp(client_message, "list\n") == 0) {
		
		
	
		}
		
		else if (strcmp(client_message, "config memory\n") == 0) {
			// call memory plugin to get the config data
			CONFIG_MEMORY(config_result);

			
			for (int idx = 0; idx < 12; idx++) {
				
				int pjg_conv = strlen(config_result[idx])+1;
				
				if (send(sock, config_result[idx], pjg_conv, 0) < 0) {
					printf("send failed\n");
					exit(-1);
				}
				
			}
		
		}
		
		else if (strcmp(client_message, "fetch memory\n") == 0) {
			
			// call memory plugin to get the fetch data
			int_fast64_t fetch_result[3];
			FETCH_MEMORY(fetch_result);

			char fetch_str[20];
			char* fetch_msg[3];

			fetch_msg[0] = "total.value ";
			fetch_msg[1] = "used.value ";
			fetch_msg[2] = "free.value ";

			for (int i = 0;i < 3;i++) {
			
				printf("FETCHED %d - %ld\n", i, fetch_result[i]);
			
				sprintf(fetch_str, "%s%ld", fetch_msg[i], fetch_result[i]);
				
				printf("fetch_str: %s\n", fetch_str);
					
				strcat(fetch_str, "\n");
				
				
				if (send(sock, fetch_str, strlen(fetch_str)+1, 0) < 0) {
					printf("Send failed\n");
					return 1;
				}
			
			}
				
		}
		
		else if (strcmp(client_message, "version\n") == 0) {
		

			//char* version_msg = strdup("Crazy node on ");
			
			char version_msg[] = "Crazy node on ";
			strcat(version_msg, hostname);
			strcat(version_msg, " version: 0.1");

			if (send(sock, version_msg, strlen(version_msg), 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}		
		
		} 
		
		else if (strcmp(client_message, "quit\n") == 0) {

			char quit_msg[] = "bye";
			
			if (send(sock, quit_msg, strlen(quit_msg), 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}

			close(socket_desc);
			close(sock);

			puts("Connection closed by client");
			break;

		} 
		
		else {
			
			printf("WORD: %s\n", client_message);
			
			char unknown_msg[] = "# Unknown command. Try cap, list, nodes, config, fetch, version or quit";
	
			if (send(sock, unknown_msg, strlen(unknown_msg), 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}	
			
		}
		
		//char nl[] = "\n";
		
		if (send(sock, "\n", strlen("\n"), 0) < 0) {
			printf("Send failed\n");
			exit(-1);
		}	
		 
    }
    
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    // Free the socket pointer
    free(socket_desc);
     
    return 0;
}
