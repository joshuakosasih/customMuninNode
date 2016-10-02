#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
 
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
     
    // Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        // Send the message back to client
    
    
		////////////
		/*  Send an integer
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
		*/
    
		
		int32_t conv[3];
		conv[0] = htonl(230);
		conv[1] = htonl(150);
		conv[2] = htonl(350);
		
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
		
    
    /////////////////
    
    
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
     
    return 0;
}
