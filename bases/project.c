#include "memoria.h"
#define BUFLEN 128

//[0]fision en la generacion actual
//[1]fision en una generacion previa
int delta_k[] = {1,1};


int sockfd;

//Direccion del servidor
struct sockaddr_in direccion_servidor;

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen);

//funcion que verifica el valor de k
void *actualizar_k()
{
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
		char buf[BUFLEN] = {0};
		while ((n = read(clfd, buf, BUFLEN)) > 0)
		{
			printf("Se recibio: %i\n", atoi(buf));
			//printf("La cantidad de caracteres recibidos fueron: %i\n",n);
			memset(buf, 0, BUFLEN);
			desplazar(delta_k,atoi(buf));
		}
		if (n < 0)
		{
			printf(" recv error");
			close(clfd);
		}
		desplazar(delta_k,atoi(buf));
	}
}

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

	pthread_t tid;		 /* the thread identifier */
	pthread_t tida[2];   /* the thread identifier */
	pthread_attr_t attr; /* set of attributes for the thread */
	struct barra list_barras[15];
	
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,actualizar_k,NULL); //este hilo se encarga

	//Inicializar barras
	iniciar_barras(list_barras);//hay 16 barras se moveran de acuerdo a su indice

	while(1){
		sleep(3);
		printf("Me desperte....\n");
		printf("El valor actual de k es : %i\n",(delta_k[0]));
		printf("El valor previo de k es : %i\n",(delta_k[1]));
		printf("La division k_actual entre k_previo: %.5d\n",(delta_k[0]/delta_k[1]));
	}
	
}
