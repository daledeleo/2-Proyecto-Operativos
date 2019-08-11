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
#include <pthread.h>

#define PUERTO 5600 //puerto para el servidor y el cliente

/*funciones para los hilos*/
void *actualizar_k();
void *resultados(void *param);

//Estructuras
typedef struct barra
{
    int posicion;     //profundidad que se sumerge la barrra
    int condicion;    //1 se mueve arriba y -1 se mueve hacia abajo
    int longitud_max; //longitud de la barra
    int se_movio;     // si se movio vale 1 si no se movio vale 0
    float delta_k;

} objeto_barra;

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
            return -1;
        }
        else
        {
            valor *= 10;
            valor += *ptr - '0';
        }
    }
    if (valor >= 0)
    {
        return 1;
    }
    return 1;
}
void desplazar(int *k, int valor)
{
    int temp = k[1];
    k[1] = valor;
    k[0] = temp;
}
void iniciar_barras(struct barra *list)
{
    for (int i = 0; i < 16; i++)
    {
        struct barra gt;
        gt.posicion = 0;
        gt.condicion = 1;
        gt.longitud_max = 30;
        gt.se_movio = 0;
        gt.delta_k = 0;
        list[i] = gt;
    }
}

void mover_barra(struct barra bar, int mover)
{
    int signo = (int)(mover / mover);
    if (mover == 10 || mover == -10)
    {
        bar.delta_k = signo * 0.1;
    }
    else if (mover == 15 || mover == -15)
    {
        bar.delta_k = signo * 0.3;
    }
    else if (mover == 20 || mover == -20)
    {
        bar.delta_k = signo * 0.4;
    }
    else if (mover == 25 || mover == -25)
    {
        bar.delta_k = signo * 0.5;
    }
    else if (mover == 30 || mover == -30)
    {
        bar.delta_k = signo * 0.55;
    }
    int temp = bar.posicion + mover;
    if (temp > 30 || temp < 0)
    {
        printf("No se puede hacer dicho movimiento\n");
        return;
    }
    bar.posicion = temp;
}
//
void imprimir_barras(struct barra *list, int valor_k)
{
    printf("*****************************************************************************************\n");
    printf("El valor de k es: %i\n",valor_k);
    printf("\t\tbarra1\tbarra2\tbarra3\tbarra4\tbarra5\tbarra6\tbarra7\tbarra8\n");
    printf("posicion:\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n", list[0].posicion, list[1].posicion,
           list[2].posicion, list[3].posicion, list[4].posicion, list[5].posicion, list[6].posicion, list[7].posicion);
    printf("delta k:\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", list[0].delta_k, list[1].delta_k,
           list[2].delta_k, list[3].delta_k, list[4].delta_k, list[5].delta_k, list[6].delta_k, list[7].delta_k);
    printf("\t\tbarra9\tbarra10\tbarra11\tbarra12\tbarra13\tbarra14\tbarra15\tbarra16\n");
    printf("posicion:\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n", list[8].posicion, list[9].posicion,
           list[10].posicion, list[11].posicion, list[12].posicion, list[13].posicion, list[14].posicion, list[15].posicion);
    printf("delta k:\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", list[8].delta_k, list[9].delta_k,
           list[10].delta_k, list[11].delta_k, list[12].delta_k, list[13].delta_k, list[14].delta_k, list[15].delta_k);
    printf("**********************************************FIN********************************************\n");

}
//solo se pasa en indice y retorna el indice de la barra par
int encontrar_par(int indice){
    if(indice<0 || indice>15){
        printf("Error: sale del indice del arreglo\n");
        return -1;
    }
    return ((indice + 8) % 15)+1;
}

//solo se pasa en indice y retorna el indice de la barra perpendicular
int encontrar_perpendicular(int indice){
    if(indice<0 || indice>15){
        printf("Error: sale del indice del arreglo\n");
        return -1;
    }
    return ((indice + 4) % 15)+1;
}

