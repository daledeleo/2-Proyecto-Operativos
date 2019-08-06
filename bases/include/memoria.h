#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
        if (isdigit(numero[i]) != 0 )
        {
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