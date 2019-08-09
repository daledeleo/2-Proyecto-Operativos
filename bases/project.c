#include "memoria.h"

#define SHMSZ     27

int main()
{
	//Declaring process variables.
	int server_sockfd, client_sockfd;
	int server_len ; 
	int rc ; 
	unsigned client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	//Remove any old socket and create an unnamed socket for the server.
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(7734) ; 
	server_len = sizeof(server_address);

	rc = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	printf("RC from bind = %d\n", rc ) ; 
	
	//Create a connection queue and wait for clients
	rc = listen(server_sockfd, 5);
	printf("RC from listen = %d\n", rc ) ; 

	client_len = sizeof(client_address);
	client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
	printf("after accept()... client_sockfd = %d\n", client_sockfd) ; 

	while(1)
	{
		char ch;
		printf("server waiting\n");
		rc = read(client_sockfd, &ch, 1);
		printf("Char from client= %c\n", ch ) ; 		
		if (ch=='X') break ; 
		ch++;
		write(client_sockfd, &ch, 1);
	}

	printf("server exiting\n");

	close(client_sockfd);

}
