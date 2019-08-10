#include "memoria.h"
#define BUFLEN 128

//Funcion para inicializar el servidor
int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int fd;
	int err = 0;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;

	if (bind(fd, addr, alen) < 0)
		goto errout;

	if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
	{
		if (listen(fd, qlen) < 0)
			goto errout;
	}
	return fd;
errout:
	err = errno;
	close(fd); //cerrando la conexion en caso de error
	errno = err;
	return (-1);
}
//Main

int main()
{
	int sockfd;

	//Direccion del servidor
	struct sockaddr_in direccion_servidor;

	memset(&direccion_servidor, 0, sizeof(direccion_servidor)); //ponemos en 0 la estructura direccion_servidor

	//llenamos los campos
	direccion_servidor.sin_family = AF_INET;					 //IPv4
	direccion_servidor.sin_port = htons(PUERTO);				 //Convertimos el numero de puerto al endianness de la red
	direccion_servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); //Nos vinculamos a la interface localhost o podemos usar INADDR_ANY para ligarnos A TODAS las interfaces

	//inicalizamos servidor (AF_INET + SOCK_STREAM = TCP)
	if ((sockfd = initserver(SOCK_STREAM, (struct sockaddr *)&direccion_servidor,
							 sizeof(direccion_servidor), 1000)) < 0)
	{ //Hasta 1000 solicitudes en cola
		printf("Error al inicializar el servidor\n");
	}
	int clfd = accept(sockfd, NULL, NULL);
	while (1)
	{
		int n = 0;
		char buf[BUFLEN]={0};
		while ((n = read(clfd, buf, BUFLEN)) > 0)
		{
			printf("Se recibio: %i\n",atoi(buf));
			//printf("La cantidad de caracteres recibidos fueron: %i\n",n);x
			memset(buf,0,BUFLEN);
		}
		if (n < 0){
			printf(" recv error");
			close(clfd);
		}
	}
	exit(1);
}
