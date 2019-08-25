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
    int sockfd1;

    //Direccion del servidor
    struct sockaddr_in direccion_cliente;

    memset(&direccion_cliente, 0, sizeof(direccion_cliente)); //ponemos en 0 la estructura direccion_servidor

    //llenamos los campos
    direccion_cliente.sin_family = AF_INET;                     //IPv4
    direccion_cliente.sin_port = htons(PUERTO_AZ5);             //Convertimos el numero de puerto al endianness de la red
    direccion_cliente.sin_addr.s_addr = inet_addr("127.0.0.1"); //Nos tratamos de conectar a esta direccion
    //AF_INET + SOCK_STREAM = TCP

    if ((sockfd1 = connect_retry(direccion_cliente.sin_family, SOCK_STREAM, 0, 
    (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente))) < 0)
    {
        printf("falló conexión\n");
        exit(-1);
    }
    //En este punto ya tenemos una conexión válida
    char *buf = (char *)malloc(sizeof(char));
    printf("soccccc: %i\n",sockfd1);
    while (1)
    {
        printf("Esperando......\n ");
        int n = 0;
        while ((n = read(sockfd1, buf,strlen(buf))) > 0)
        {
            printf("Alerta de %s\n",buf);
            printf("No ahi tiempo que perder tiene que presionar AZ5(ENTER) para estabilizar el reactor");
            fgets(buf, strlen(buf), stdin);
            reemplazar(buf);
            buf="activar";
            int n1=0;
            n1 = write(sockfd1, buf, strlen(buf));
            if (n1 < 0)
            {
                perror("Error en el envio");
                close(sockfd1);
            }
            fflush(stdout);
        }
        close(sockfd1);
        break;
    }
    return 0;
}
