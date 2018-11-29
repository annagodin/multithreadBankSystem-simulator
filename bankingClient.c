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

int main(int argc, char *argv[]) {
	printf("this is the banking client\n");

	//----------------------TESTING FROM GEEKS FOR GEEKS-------------------------
	struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serverAddress; 
    char *helloMessage = "Hello from the client!!!!!!!! --- FUCK THREADING AND SYSTEMS"; 
    char buffer[1024] = {0}; 

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serverAddress, '0', sizeof(serverAddress)); 
   
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    send(sock , helloMessage , strlen(helloMessage) , 0 ); 
    printf("CLIENT: Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
    
    //----------------------TESTING FROM GEEKS FOR GEEKS-------------------------

	return 0;
}