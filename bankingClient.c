#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <netinet/in.h>
#define PORT 9382
#define _GNU_SOURCE
#define MAX 80 
#define SA struct sockaddr

int sockfd;


void func(int sockfd) 
{ 
    int valid = 1; //bool valid input flag
    char buff[MAX]; 
    int n; 
    for (;;) { 
    	do {

    		if (valid==0){
    			printf("Invalid command, please try again : \n");
    		} else {
    			 printf("Enter a string : "); 
    		}
	       
	        bzero(buff, sizeof(buff)); 
	        n = 0; 
	        while ((buff[n++] = getchar()) != '\n'); 
	        // write(sockfd, buff, sizeof(buff)); 
	        
		
			if(strncmp("create", buff, 6) == 0){
				printf("valid input\n");
				valid = 1;
			}

			else if(strncmp("serve",buff, 5) == 0){
				printf("valid input\n");
				//puts client in service mode
				//ANNA: where do we check if the client is in service mode? create a boolean?
				valid = 1;
			}

			else if (strncmp("deposit", buff, 7) == 0){
				//server should only accept cmd if in service mode
				printf("valid input\n");
				valid = 1;
			}

			else if (strncmp("withdraw", buff, 8) == 0){
				//server should only accept cmd if in service mode
				printf("valid input\n");
				valid = 1;
			}

			else if(strncmp("query", buff, 5) == 0){
				//server should only accept cmd if in service mode
				printf("valid input\n");
				valid = 1;
			}

			else if(strncmp("end", buff, 3) == 0){
				//closes account session
				printf("valid input\n");
				valid = 1;
					exit(-1);
			}

			else if(strncmp("quit", buff, 4) == 0){
				//disconnects client from server
				//pthread_exit
				//free()
				//all that jazz

				printf("valid input\n");
				valid = 1;
				exit(-1);
			}
			else {
				valid = 0;
				printf("invalid input\n");
			}
			// if(valid == 0){
				
			// }

			
			
		} while (valid==0);

		 write(sockfd, buff, sizeof(buff)); 

		bzero(buff, sizeof(buff)); 
	        read(sockfd, buff, sizeof(buff)); 
	        printf("From Server : %s", buff); 
	        if ((strncmp(buff, "exit", 4)) == 0) { 
	            printf("Client Exit...\n"); 
	            break; 
	        } 
    } 
}


int main() 
{ 
    int connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 


    //assign IP, PORT
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT);


    //connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n");

    //function for chat
    func(sockfd);

    //close the socket
    close(sockfd);

}
