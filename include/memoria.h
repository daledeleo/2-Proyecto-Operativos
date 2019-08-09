
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//Estructuras
typedef struct memoria
{
    char *valor; //El valor de K que van a compartir los procesos
} objeto;

void reemplazar(char *linea)
{
    while (*linea != 0)
    {
        if (*linea == '\n')
        {
            *linea = 0;
        }
        linea++;
    }
}
int validar_num(char numero[50])
{
    int i = 0, sw = 0, j;
    j = strlen(numero);
    while (i < j && sw == 0)
    {
        if (isdigit(numero[i]) != 0)
        {
            printf("salio");
            i++;
        }
        else
        {
            sw = 1;
            return -1;
        }
    }
    return 1;
}