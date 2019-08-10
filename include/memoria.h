#include <sys/types.h> /* some systems still require this */
#include <sys/stat.h>
#include <stdio.h>  /* for convenience */
#include <stdlib.h> /* for convenience */
#include <stddef.h> /* for offsetof */
#include <string.h> /* for convenience */
#include <unistd.h> /* for convenience */
#include <signal.h> /* for SIG_ERR */
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <ctype.h>

#define PUERTO 5600 //puerto para el servidor y el cliente
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
int validar_num(char *numero)
{
    int valor = 0;

    for (char *ptr = numero; *ptr; ++ptr)
    {
        if (isalpha(*ptr))
        {
            puts("El dato no es numerico");
            valor = -1;
            return -1;
        }
        else
        {
            valor *= 10;
            valor += *ptr - '0';
        }
    }

    if (valor >= 0){
        return 1;
    }
    return 1;
}