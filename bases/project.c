#include "memoria.h"
#define BUFLEN 128

//[0]valor previo de K
//[1]valor final de k
float delta_k[] = {1.0, 1.0};
int sockfd;
int sockfd_az5;
struct barra list_barras[15];

clock_t t_ini;
char Az5[40] ={0};
int clfd;
int clfd_az5;
int secs = 0; //encargado de tener el tiempo final en el estado supercritico

//Direccion del servidor
struct sockaddr_in direccion_servidor;
struct sockaddr_in direccion_servidor_az5;

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen);

//hilo que se encarga de calcular el tiempo una vez que el reactor este en estado super-critico
void *cronometro(void *param)
{
	while (1)
	{
		while (delta_k[1] > 1.5 && secs < (TIME * 3))
		{
			/*si entra aqui, esta en estado super-critico*/
			sleep(1);
			secs++;
			sem_post(&sem2);
		}
	}
}
//hilo que solo hace la funcion del Az5
void *mandar_aviso(void *param)
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				   &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PUERTO_AZ5);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
			 sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
							 (socklen_t *)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	printf("Se habilto el ingresos del boton AZ5 en otro terminar por el socket: %i\n", new_socket);
	while (1)
	{
		if (secs >= (TIME * 3))
		{
			sem_wait(&sem2);
			char *p = "AZ5";
			int n = 0;
			n = send(new_socket, p, strlen(p), 0);
			if (n < 0)
			{
				perror("Error en send az5:");
			}
			n = read(new_socket, Az5, 50);
			if (n < 0)
			{
				perror("Error en read az5:");
			}
			printf("se recibio %s(AZ5)\n",Az5);
			secs=0;
			sleep(TIME*6);
		}
	}
	close(new_socket);
	return 0;
}
//hilo que controla los movimientos de las barras
void *resultados_piston(void *param)
{
	while (1)
	{
		/*Error del 0.5*/
		while ((delta_k[1] < 0.5 || delta_k[1] > 1.5) && delta_k[1] != 1)
		{
			if (strcmp(Az5, "activar") == 0)
			{
				Az5_active(list_barras, delta_k[1]);
				imprimir_barras(list_barras, delta_k[1]);
				sem_post(&sem1);
				printf("Espere mientras las el reactor se estabiliza a la normalidad..........\n");
				sleep(TIME*5);
				iniciar_barras(list_barras);
				delta_k[1]=1;
				delta_k[0]=1;
				imprimir_barras(list_barras, delta_k[1]);
				sem_post(&sem1);
				memset(Az5,0,strlen(Az5));
			}
			else
			{
				int index1 = prioriedad_index(list_barras, delta_k[1]);
				int index2 = encontrar_par(index1);
				if (index1 >= 0)
				{
					int ban;
					int indexr = determinar_profundidad(list_barras, delta_k[1]);
					int signo;
					if (delta_k[1] < 0.5)
					{
						signo = 1;
					}
					else
					{
						signo = -1;
					}

					int mover = -1 * signo * profundidades[indexr]; //para sacar una profundidad adecuada
					printf("Se va a mover de profundidad: %i\n", mover);
					printf("Se van a mover las barras: barra%i y barra%i\n", (index1 + 1), (index2 + 1));
					ban = mover_barra(list_barras[index1], list_barras[index2], mover); //Se mueven ambas barras
					if (ban > 0)
					{

						if (mover == 10 || mover == -10)
						{
							list_barras[index1].delta_k = signo * 0.1 + list_barras[index1].delta_k;
							list_barras[index2].delta_k = signo * 0.1 + list_barras[index2].delta_k;
						}
						else if (mover == 15 || mover == -15)
						{
							list_barras[index1].delta_k = signo * 0.3 + list_barras[index1].delta_k;
							list_barras[index2].delta_k = signo * 0.3 + list_barras[index2].delta_k;
						}
						else if (mover == 20 || mover == -20)
						{
							list_barras[index1].delta_k = signo * 0.4 + list_barras[index1].delta_k;
							list_barras[index2].delta_k = signo * 0.4 + list_barras[index2].delta_k;
						}
						else if (mover == 25 || mover == -25)
						{
							list_barras[index1].delta_k = signo * 0.5 + list_barras[index1].delta_k;
							list_barras[index2].delta_k = signo * 0.5 + list_barras[index2].delta_k;
						}
						else if (mover == 30 || mover == -30)
						{
							list_barras[index1].delta_k = signo * 0.55 + list_barras[index1].delta_k;
							list_barras[index2].delta_k = signo * 0.55 + list_barras[index2].delta_k;
						}
						desplazar(delta_k, delta_k[1] + list_barras[index1].delta_k + list_barras[index2].delta_k);

						list_barras[index2].posicion = mover; // + list_barras[index2].posicion;
						list_barras[index1].posicion = mover; //+ list_barras[index1].posicion;

						if (list_barras[index1].posicion >= 30)
						{
							list_barras[index1].posicion = 30;
							list_barras[index2].posicion = 30;
						}
						if (list_barras[index1].posicion <= 0)
						{
							list_barras[index1].posicion = 0;
							list_barras[index2].delta_k = 0.0;
							list_barras[index2].posicion = 0;
							list_barras[index1].delta_k = 0.0;
						}
						list_barras[index1].condicion = ULTIMA_EN_MOVERSE;
						list_barras[index2].condicion = ULTIMA_EN_MOVERSE;
						cambiar_movimiento_all_except(list_barras, index1);

						imprimir_barras(list_barras, delta_k[1]);
						list_barras[index1].se_movio = SE_MOVIO;
						list_barras[index2].se_movio = SE_MOVIO;
					}
				}
				else
				{
					list_barras[index1].se_movio = NO_SE_MOVIO;
					list_barras[index2].se_movio = NO_SE_MOVIO;
				}
				sem_post(&sem1);
				sleep(1);
			}
		}
		secs = 0;
	}
}

//hilo que espera por el valor de k
void *actualizar_k()
{
	memset(&direccion_servidor, 0, sizeof(direccion_servidor)); //ponemos en 0 la estructura direccion_servidor

	//llenamos los campos
	direccion_servidor.sin_family = AF_INET;					 //IPv4
	direccion_servidor.sin_port = htons(PUERTO);				 //Convertimos el numero de puerto al endianness de la red
	direccion_servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); //Nos vinculamos a la interface localhost o podemos usar INADDR_ANY para ligarnos A TODAS las interfaces

	//inicalizamos servidor (AF_INET + SOCK_STREAM = TCP)
	if ((sockfd = initserver(SOCK_STREAM, (struct sockaddr *)&direccion_servidor,
							 sizeof(direccion_servidor), 10)) < 0)
	{ //Hasta 1000 solicitudes en cola
		printf("Error al inicializar el servidor\n");
	}
	clfd = accept(sockfd, NULL, NULL);
	printf("Se habilto las modificaciones de k en otro terminar por el socket: %i\n", clfd);
	while (1)
	{
		int n = 0;
		char buf[BUFLEN] = {0};
		while ((n = read(clfd, buf, BUFLEN)) > 0)
		{
			if (strcmp(buf, "exit") == 0)
			{
				exit(1);
			}
			printf("Se recibio: %.3f\n", atof(buf));
			desplazar(delta_k, delta_k[1] + atof(buf));
			memset(buf, 0, BUFLEN);
			printf("Se actualizalo el valor de k a : %.3f\n", delta_k[1]);
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
	pthread_attr_t attr3;
	pthread_attr_t attr4;
	pthread_t calcula_tiempo; //tiempo del reactor en estado supercritico
	pthread_t manda_aviso;	//para el aviso Az5

	sem_init(&sem1, 0, 1); /*incializando el semaforo*/
	sem_init(&sem2, 0, 1); /*incializando el semaforo2*/

	pthread_attr_init(&attr);
	pthread_attr_init(&attr3);
	pthread_attr_init(&attr4);
	pthread_create(&tid, &attr, actualizar_k, NULL); //este hilo se encarga de actualizar el valor de k

	//Inicializar barras
	iniciar_barras(list_barras); //hay 16 barras se moveran de acuerdo a su indice

	pthread_attr_init(&attr2);
	pthread_create(&tid2, &attr2, resultados_piston, NULL); //este hilo se encarga de actualizar el valor de k
	pthread_create(&calcula_tiempo, &attr3, cronometro, NULL);
	pthread_create(&manda_aviso, &attr4, mandar_aviso, NULL);

	imprimir_barras(list_barras, delta_k[1]);
	while (1)
	{
		//espera hasta que una barra modifique la tabla para mostrarla
		sem_wait(&sem1);
		// se compara el valor final de k
		if (delta_k[1] == 1)
		{
			printf("Estado crittico del reactor, cada evento de fisión genera exactamente un nuevo evento de fisión\n");
			secs = 0;
		}
		else if (delta_k[1] < 1)
		{
			printf("Estado sub-critico del reactor, es decir la reacción en cadena no se puede sostener\n");
			secs = 0;
		}
		else
		{
			/* Para valores finales de k mayores a 1*/
			printf("Estado super-critico del reactor, es decir la reacción en cadena crece exponencialmente\n");
			/* code */
			printf("tiempo %i\n", secs);
		}
		printf("El valor previo de k fue : %.3f\n", delta_k[0]);
		printf("\n");
	}
	return 0;
}
