#include "memoria.h"

#define SHMSZ 27

int main()
{printf("This is the client program\n");

	int sockfd;
	int len, rc ;
	struct sockaddr_in address;
	int result;
	char ch = 'A';

   //Create socket for client.
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) { 
		perror("Socket create failed.\n") ; 
		return -1 ; 
	} 
	
	//Name the socket as agreed with server.
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(7734);
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1)
	{
		perror("Error has occurred");
		exit(-1);
	}

	while ( ch < 'Y') {

		//Read and write via sockfd
		rc = write(sockfd, &ch, 1);
		if (rc == -1) break ; 
		read(sockfd, &ch, 1);
		printf("Char from server = %c\n", ch);		
	} 
	close(sockfd);

	exit(0);
}

 

