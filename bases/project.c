#include "memoria.h"
#define BUFLEN 128

//[0]valor previo de K
//[1]valor final de k
float delta_k[] = {1.0, 1.0};
int sockfd;
struct barra list_barras[15];

//Direccion del servidor
struct sockaddr_in direccion_servidor;

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen);

//hilo que controla los movimientos de las barras
void *resultados_piston(void *param)
{
	while (1)
	{
		mover(list_barras, delta_k);
	}
}
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
			printf("Se recibio: %.5f\n", atof(buf));
			//printf("La cantidad de caracteres recibidos fueron: %i\n",n);
			desplazar(delta_k, delta_k[1] + atof(buf));
			memset(buf, 0, BUFLEN);
		}
		if (n < 0)
		{
			printf(" recv error");
			close(clfd);
		}
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

	pthread_t tid; /* the thread identifier */
	pthread_t tid2;
	pthread_attr_t attr; /* set of attributes for the thread */
	pthread_attr_t attr2;

	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, actualizar_k, NULL); //este hilo se encarga de actualizar el valor de k

	//Inicializar barras
	iniciar_barras(list_barras); //hay 16 barras se moveran de acuerdo a su indice
	
	pthread_attr_init(&attr2);
	pthread_create(&tid2, &attr2, resultados_piston, NULL); //este hilo se encarga de actualizar el valor de k

	while (1)
	{
		// se compara el valor final de k
		if (delta_k[1] == 1)
		{
			printf("Estado crittico del reactor, cada evento de fisión genera exactamente un nuevo evento de fisión\n");
			imprimir_barras(list_barras, delta_k[1]);
		}
		else if (delta_k[1] < 1)
		{
			printf("Estado sub-critico del reactor, es decir la reacción en cadena no se puede sostener\n");
			imprimir_barras(list_barras, delta_k[1]);
		}
		else
		{
			/* Para valores finales de k mayores a 1*/
			printf("Estado super-critico del reactor, es decir la reacción en cadena crece exponencialmente\n");
			/* code */
			imprimir_barras(list_barras, delta_k[1]);
		}

		sleep(TIME);
		printf("El valor actual de k es : %.4f\n", delta_k[1]);
		printf("El valor previo de k fue : %.4f\n", delta_k[0]);
	}
}
