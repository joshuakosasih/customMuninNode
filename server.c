#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include "plugins/memory.h"

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
     
    // Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9999);
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    
    puts("bind done");
     
    listen(socket_desc , 3);
     
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    // Get hostname & send banner
    char hostname[200];
    char* banner = strdup("# munin node at ");

    if ((gethostname(hostname, 200)) == 0) {	
	strcat(banner, hostname);
	strcat(banner, "\n");	
	if (send(client_sock, banner, strlen(banner)+1, 0) < 0) {
		puts("Error: cannot send banner");
	}
	printf("You are logged in to: %s\n", hostname);
    } else {
	puts("Error: cannot get hostname");
    }
 
    // Initialize some messages to send
    char* cap_msg = strdup("cap multigraph dirtyconfig\n");

    char nodes_msg[20] = "\0";
    strcat(nodes_msg, hostname);
    strcat(nodes_msg, "\n.\n");

    char* dot_msg = strdup(".\n");

    char* quit_msg = strdup("bye\n");

    char* unknown_msg = strdup("# Unknown command. Try cap, list, nodes, config, fetch, version or quit\n");

    // Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {

	printf("Message: %s\n", client_message);

	char* config_result[12];
	int_fast64_t* fetch_result;
	
	if (strcmp(client_message, "cap\n") == 0) {
		
		if (send(client_sock, cap_msg, strlen(cap_msg)+1, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}	
		
	} else if (strcmp(client_message, "nodes\n") == 0) {

		if (send(client_sock, nodes_msg, strlen(nodes_msg)+2, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}		
		
	} else if (strcmp(client_message, "list\n") == 0) {
		
		
	
	} else if (strcmp(client_message, "config memory\n") == 0) {
		
		// call memory plugin to get the config data
		CONFIG_MEMORY(config_result);

		for (int i = 0; i < 12; i++) {
			char config_msg[100] = "\0";
			strcat(config_msg, config_result[i]);
			strcat(config_msg, "\n");
			if (send(client_sock, config_msg, strlen(config_msg)+1, 0) < 0) {
				printf("Send failed\n");
				return 1;
			}
		}
		if (send(client_sock, dot_msg, 2, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}
		
/*
	} else if (strcmp(client_message, "fetch memory\n") == 0) {
puts("going to fetch memory");
		// call memory plugin to get the fetch data
		fetch_result = FETCH_MEMORY();

puts("fetch done");		
		char fetch_str[20];
		char* fetch_msg[3];

		fetch_msg[0] = strdup("total.value ");
		fetch_msg[1] = strdup("used.value ");
		fetch_msg[2] = strdup("free.value ");

		for (int i = 0;i < 3;i++) {
printf("making message %d\n", i);
			sprintf(fetch_str, "%ld", fetch_result[i]);
			strcat(fetch_msg[i], fetch_str);
			strcat(fetch_msg[i], "\n");
printf("sending message %d\n", i);
			if (send(client_sock, fetch_msg[i], strlen(fetch_msg[i])+1, 0) < 0) {
				printf("Send failed\n");
				return 1;
			}
		}
*/
	} else if (strcmp(client_message, "version\n") == 0) {
		

		char* version_msg = strdup("Crazy node on ");
		strcat(version_msg, hostname);
		strcat(version_msg, " version: 0.1\n");

		if (send(client_sock, version_msg, strlen(version_msg)+1, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}		
		
	} else if (strcmp(client_message, "quit\n") == 0) {

		if (send(client_sock, quit_msg, strlen(quit_msg)+1, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}

		close(socket_desc);
    		close(client_sock);

		puts("Connection closed by client");
		return 0;

	} else {
		
		if (send(client_sock, unknown_msg, strlen(unknown_msg)+1, 0) < 0) {
			printf("Send failed\n");
			return 1;
		}	
		
	}

        // Send the message back to client
    
    
		////////////

		// Komen salah satu untuk mencoba nya
/*

		// Send an integer
		int32_t conv = htonl(230);
		char *data = (char*)&conv;
		int left = sizeof(conv);
		int rc;
		do {
			rc = write(client_sock, data, left);
			data += rc;
			left -= rc;
		}
		while (left > 0);

    
		// Send array of integer		
		//int32_t conv[3];
		int_fast64_t conv[3];
		conv[0] = htonl(123456789);
		conv[1] = htonl(999876527);
		conv[2] = htonl(292839928);
		
		char *data;
		
		for (int idx = 0; idx < 3; idx++) {
			data = (char*)&conv[idx];
			int left = sizeof(conv[idx]);
			int rc;
			do {
				rc = write(client_sock, data, left);
				data += rc;
				left -= rc;
			}
			while (left > 0);
		}
	*/
		
		// Send array of string
/*
		char* conv[4];
		conv[0] = "hello, world!\n";
		conv[1] = "thx for coming. btw this is a very 12345 long string\n";
		conv[2] = "this is a sample text\n";
		conv[3] = "Bye!\n";
		
		int left;

		if (strcmp(client_message, "cap\n") == 0) {
			conv[0] = "cap multigraph dirtyconfig\n";
		}
		else if (strcmp(client_message, "quit\n") == 0) {
			if (send(client_sock, conv[3], strlen(conv[3])+1, 0) < 0) {
				printf("Send failed\n");
				return 1;
			}
			close(socket_desc);
    			close(client_sock);
			printf("All closed");
			return 1;
		}
		
		for (int idx = 0; idx < 3; idx++) {
			int pjg_conv = strlen(conv[idx])+1;

			if (send(client_sock, conv[idx], pjg_conv, 0) < 0) {
				printf("Send failed\n");
				return 1;
			}
		}
*/

	memset(client_message, 0, sizeof client_message);
	
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
     
    close(socket_desc);
    close(client_sock); 
    
    return 0;
}
