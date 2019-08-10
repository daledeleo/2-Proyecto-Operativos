#include "memoria.h"
#define MAXSLEEP 64

int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t alen)
{
	int numsec, fd; /* * Try to connect with exponential backoff. */
	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1)
	{
		if ((fd = socket(domain, type, protocol)) < 0)
			return (-1);
		if (connect(fd, addr, alen) == 0)
		{ /* * Conexión aceptada. */
			return (fd);
		}
		close(fd); //Si falla conexion cerramos y creamos nuevo socket

		/* * Delay before trying again. */
		if (numsec <= MAXSLEEP / 2)
			sleep(numsec);
	}
	return (-1);
}

int main()
{
	int sockfd;
	

	//Direccion del servidor
	struct sockaddr_in direccion_cliente;

	memset(&direccion_cliente, 0, sizeof(direccion_cliente)); //ponemos en 0 la estructura direccion_servidor

	//llenamos los campos
	direccion_cliente.sin_family = AF_INET;						//IPv4
	direccion_cliente.sin_port = htons(PUERTO);					//Convertimos el numero de puerto al endianness de la red
	direccion_cliente.sin_addr.s_addr = inet_addr("127.0.0.1"); //Nos tratamos de conectar a esta direccion
	//AF_INET + SOCK_STREAM = TCP

	if ((sockfd = connect_retry(direccion_cliente.sin_family, SOCK_STREAM, 0, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente))) < 0)
	{
		printf("falló conexión\n");
		exit(-1);
	}
	//En este punto ya tenemos una conexión válida
	char *buf=(char *)malloc(sizeof(char));
	while (1)
	{
		printf("ingrese un numero: ");
		fgets(buf,100, stdin);
		reemplazar(buf); //quitamos el salto de linea
		while(validar_num(buf) < 0 ){
			fflush(stdout);
			printf("ingrese un numero valido: ");
			fgets(buf, 100, stdin);
			reemplazar(buf); 
		}
		//printf("Se envio: %s\n",buf);
		int n=write(sockfd,buf,strlen(buf));
		//printf("la cantidad de caracteres enviados fueron: %i\n",n);
		if(n < 0){
			perror("Error en el envio");
			close(sockfd);
		}
		fflush(stdout);
	}
	return 0;
}
