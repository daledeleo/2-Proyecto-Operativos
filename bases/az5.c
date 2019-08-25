#include "memoria.h"

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PUERTO_AZ5);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    while(1){
        int n=0;
        printf("Esperando......\n");
        n=read(sock, buffer, 3);
        if(n<0){
            perror("Error read az5.c");
        }
        printf("alerta de %s\n",buffer);
        printf("No ahi tiempo que perder presione AZ5(ENTER) para bajar todas las barras(): ");
        fgets(buffer,100,stdin);
        char *enviar="activar";
        n=send(sock,enviar,100,0);
        if(n<0){
            perror("Error send az5.c");
        }
        fflush(stdout);
    }
    close(sock);
    return 0;
}
