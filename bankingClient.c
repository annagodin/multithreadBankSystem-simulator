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
#define MAX 500 
// #define SA struct sockaddr
int exitClient = 0;


void * readFromServer(void* args){
	char buff[MAX];
	memset(buff, 0, sizeof(buff));
	int networkSockFD = *(int*)args;
	int status;
	for(;;){
		status = read(networkSockFD, buff, sizeof(buff));
		buff[strlen(buff)-1]='\0';
		if (status>0){
			printf("\nFrom Server: '%s'\n", buff); 
	   		memset(buff, 0, sizeof(buff));
		}
	    
	}


	// while (status = read(networkSockFD, buff, sizeof(buff))>=0) { 
	//        printf("From Server: %s\n", buff); 
	//        memset(buff, 0, sizeof(buff));   
	//        if (strcmp(buff, "quit")==0){
	//        		break;
	//     		exitClient=1;
 //        	} 
	// }
	

	printf("** Closing connection\n");
    close(networkSockFD);
    free(args);
    exit(1);
    return 0;
	
    
}

void writeToServer(int sockfd) {

	int inSession = 0; 
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
	        buff[strlen(buff)-1]='\0';
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
				
			}

			else if(strncmp("quit", buff, 4) == 0){
				//disconnects client from server
				//pthread_exit
				//free()
				//all that jazz

				// printf("valid input\n");
				valid = 1;
				printf("** Closing connection, exiting client\n");
				return;

        	} 

        	else {
				valid = 0;
				//printf("invalid input\n");
			}

		
			
			// if(valid == 0){
				
			// }

			
		} while (valid==0);

		write(sockfd, buff, sizeof(buff)); 
		bzero(buff, sizeof(buff)); 
		sleep(2);
	}
       
} 



int main(int argc, char *argv[]) { 
    int sockfd;
   // int connfd; 
   // struct sockaddr_in servaddr, cli; 
  
//------------ANNAS TRYING NEW THINGS------------------------
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;

    // void *thread_sfd1 = malloc(sizeof(sockfd));
    // void *thread_sfd2 = malloc(sizeof(sockfd));

    if (argc<3){
    	fprintf(stderr, "ERROR: not enough arguments, must specify machine name and port\n");
   		exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = 0;
	hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;  
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;       
     /* Any protocol */
    
    char* machineName = malloc(sizeof(char)*(strlen(argv[1])+20));
    strcpy(machineName,argv[1]);
    strcat(machineName,".cs.rutgers.edu");
    s = getaddrinfo(machineName, "9382", &hints, &result);
    if (s!=0){
    	fprintf(stderr, "ERROR: getaddrinfo: %s\n", gai_strerror(s));
    	exit(EXIT_FAILURE);
    }

     /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */

    // memcpy(thread_sfd1, &sockfd, sizeof(int));
    // memcpy(thread_sfd2, &sockfd, sizeof(int));

   for (rp = result; rp != NULL; rp = rp->ai_next) {
       sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
       if (sockfd == -1)
           continue;

       if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1){
          printf("** Anna's socket successfully created\n");
          break;                  /* Success */
       }
       printf("** Could not find server. Attempting to reconnect in 3 sec. . .\n");
       sleep(3);
       close(sockfd);
   }

   if (rp == NULL) {               /* No address succeeded */
       fprintf(stderr, "ERROR: Could not connect\n");
       exit(EXIT_FAILURE);
     }

     freeaddrinfo(result);

     printf("** Successfully connected to the server\n");


//----------END NEW METHOD--------------------------


//-----------OLD METHOD---------------------------------------
    // socket create and varification
    // sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    // if (sockfd == -1) { 
    //     printf("socket creation failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("Socket successfully created..\n"); 
    // bzero(&servaddr, sizeof(servaddr)); 


    // //assign IP, PORT
    // servaddr.sin_family = AF_INET; 
    // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    // servaddr.sin_port = htons(PORT);


    // //connect the client socket to server socket
    // if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
    //     printf("connection with the server failed...\n"); 
    //     exit(0); 
    // } 
    // else
    //     printf("connected to the server..\n");
//-----------END OLD METHOD--------------------------------
    
     // printf("sockfd: %d\n", sockfd);

     // printf("new for thread sockfd: %d\n", &thread_sfd1);

     pthread_t readServer;

    if (pthread_create(&readServer, NULL, &readFromServer, (void*)&sockfd ) != 0){
         fprintf(stderr, "ERROR: Can't create server reading thread: %s\n", strerror(errno));
         exit(EXIT_FAILURE);
    }

    pthread_detach(readServer);

    //function for writing to server
    writeToServer(sockfd);

    //close the socket
    close(sockfd);

    return 0;
}
