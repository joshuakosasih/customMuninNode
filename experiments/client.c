#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
 
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    while(1)
    {
		
        printf("Enter message : ");
        scanf("%s" , message);
         
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
    	
		/////////////////////
		
		/*  Receive an integer
			int32_t ret;
			char *data = (char*)&ret;
			int left = sizeof(ret);
			int rc;
			do {
				rc = read(sock, data, left);
				data += rc;
				left -= rc;
			}
			while (left > 0);
			int *num = ntohl(ret);
		*/
		
		
		int32_t ret;
		
		int* num;
		
		printf("Array received\n");
		for (int idx=0; idx < 3; idx++) {
			char *data = (char*)&ret;
			int left = sizeof(ret);
			int rc;
			do {
				rc = read(sock, data, left);
				data += rc;
				left -= rc;
			}
			while (left > 0);
			num = ntohl(ret);
			
			printf("%d\n", num);
		
		}
		
		////////////////
		
		
	}
     
    close(sock);
    return 0;
}
