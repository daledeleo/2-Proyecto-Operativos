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
#include <semaphore.h>

#define PUERTO 5600 //puerto para el servidor y el cliente
#define TIME 3      //Representacion del tiempo t(en segundos)
#define SE_MOVIO 2
#define NO_SE_MOVIO 0
#define ESTA_EN_MOVIMIENTO 1
#define ULTIMA_EN_MOVERSE 1

sem_t sem1; //para controlar las tablas por pantalla

/*funciones para los hilos*/
void *actualizar_k();
void *resultados_piston(void *param);
int profundidades[] = {10, 15, 20, 25, 30}; /*profundidades de las barras*/

//Estructuras

typedef struct barra
{

    int posicion;     /*profundidad que se sumerge la barra*/
    int condicion;    /*ultima en moverse(1) o no(0)*/
    int longitud_max; /*Loguitud de la barra*/
    int se_movio;     /*si se movio vale 2, si no se movio vale 0, esta en movimiento 1, para saber cual fue la ultima en moverse*/
    float delta_k;

} objeto_barra;

typedef struct memory
{
    struct barra *gt;
    float *k_final;
} objeto_memory;

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

int es_el_comienzo(struct barra *gt)
{
    for (int i = 0; i < 15; i++)
    {
        if (gt[i].delta_k != 0)
        {
            return -1;
        }
    }
    return 1;
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

//solo se pasa en indice y retorna el indice de la barra par
int encontrar_par(int indice)
{
    if (indice < 0 || indice > 15)
    {
        printf("Error: sale del indice del arreglo\n");
        return -1;
    }
    if (indice >= 0 && indice <= 7)
    {
        return indice + 8;
    }
    else
    {
        return indice - 8;
    }
}

void cambiar_movimiento_all_except(struct barra *gt, int index)
{
    for (int i = 0; i < 8; i++)
    {
        if (index != i)
        {
            gt[i].condicion = NO_SE_MOVIO;
            gt[encontrar_par(i)].condicion = NO_SE_MOVIO;
            break;
        }
    }
}
void desplazar(float *k, float valor)
{
    float temp = k[1];
    k[1] = valor;
    k[0] = temp;
}

void iniciar_barras(struct barra *list)
{
    for (int i = 0; i < 16; i++)
    {
        struct barra gt;
        gt.posicion = 0;
        gt.condicion = NO_SE_MOVIO;
        gt.longitud_max = 30;
        gt.se_movio = NO_SE_MOVIO;
        gt.delta_k = 0;
        list[i] = gt;
    }
}

int mover_barra(struct barra bar, struct barra bar2, int mover)
{
    int temp = bar.posicion + mover;
    if (temp > 30 || temp < 0)
    {
        printf("No se puede hacer dicho movimiento\n");
        return -1;
    }
    return 1;
}

/*1 si todas se movieron -1 si no*/
int todas_se_movieron(struct barra *list)
{
    for (int i = 0; i < 16; i++)
    {
        if (list[i].delta_k == 0)
        {
            return -1; //aun no se han movido todas
        }
    }
    return 1; //ya se movieron todas
}
//
void imprimir_barras(struct barra *list, float valor_k)
{
    printf("*****************************************************************************************\n");
    printf("**El valor de k es: %.3f**\n", valor_k);
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

//solo se pasa en indice y retorna el indice de la barra perpendicular
int encontrar_perpendicular(int indice)
{
    if (indice < 0 || indice > 15)
    {
        printf("Error(perp): sale del indice del arreglo \n");
        return -1;
    }
    if (indice >= 0 && indice <= 7)
    {
        return indice + 4;
    }
    else
    {
        return indice - 4;
    }
}

int determinar_ultimo_movimiento(struct barra *gt)
{
    for (int i = 0; i < 16; i++)
    {
        if (gt[i].condicion == ULTIMA_EN_MOVERSE)
        {
            printf("ultimo movimiento fue las barras: barra%i barra%i\n", (i + 1), encontrar_par(i) + 1);
            return i;
        }
    }
    return -1; //no hubo ultimo movimiento
}
/*0 si delta_k=0 1 si delta_k=0.1 1 si delta_k=0.1 1 si delta_k=0.1 1 si delta_k=0.1 1 si delta_k=0.1 */
int 

/*indice de la barra priorietaria*/
int prioriedad_index(struct barra *gt)
{

    if (es_el_comienzo(gt) > 0)
    {
        /*numero = rand () % (N-M+1) + M;   // Este está entre M y N*/
        return (int)(rand() % 16);
    }
    else
    {
        int ind = determinar_ultimo_movimiento(gt);
        /*(1) Barras que no se encuentren ya en movimiento y (2) barras que 
            sean perpendiculares al ultimo par de
            barras que se encuentran en movimiento.*/
        if (ind >= 0)
        {
            int i = encontrar_perpendicular(ind);
            if (todas_se_movieron(gt) < 0)
            {
                while (gt[i].delta_k != 0)
                {
                    ind = (int)(rand() % 16);
                    i = encontrar_perpendicular(ind);
                }
                return i;
            }else if(){

            }
        }
        //return (int)(rand() % 16);
    }
    printf("Error al sacar la prioriedad en las barras\n");
    return -1;
}

int determinar_profundidad(struct barra *gt, float k)
{
    for (int i = 0; i < 15; i++)
    {
        if (gt[i].posicion < 10)
        {
            return 0;
        }
    }
    for (int i = 0; i < 15; i++)
    {
        if (gt[i].posicion < 15)
        {
            return 1;
        }
    }
    for (int i = 0; i < 15; i++)
    {
        if (gt[i].posicion < 20)
        {
            return 2;
        }
    }
    for (int i = 0; i < 15; i++)
    {
        if (gt[i].posicion < 25)
        {
            return 3;
        }
    }
    return 4;
}
/*
void mover(struct barra *gt, float *k_final)
{
    while ((k_final[1] < 0.5 || k_final[1] > 1.5) && k_final[1] != 1)
    {
        int index1 = prioriedad_index(gt);
        if (index1 >= 0)
        {
            int index2 = encontrar_par(index1);
            int ban;
            gt[index2].se_movio = ESTA_EN_MOVIMIENTO;
            gt[index1].se_movio = ESTA_EN_MOVIMIENTO;

            int indexr = determinar_profundidad(gt, k_final[1]);
            int signo;
            if (k_final[1] < 1)
            {
                signo = 1;
            }
            else
            {
                signo = -1;
            }
            int mover = profundidades[indexr]; //para sacar una profundidad aletatoria que cumpla
            printf("Se va a mover: %i\n", mover);
            printf("Se va a mover la barra%i\n", index1 + 1);
            printf("Se va a mover la barra%i\n", index2 + 1);
            ban = mover_barra(gt[index1], gt[index2], mover); //Se mueven ambas barras
            if (ban > 0)
            {

                if (mover == 10 || mover == -10)
                {
                    gt[index1].delta_k = signo * 0.1;
                    gt[index2].delta_k = signo * 0.1;
                }
                else if (mover == 15 || mover == -15)
                {
                    gt[index1].delta_k = signo * 0.3;
                    gt[index2].delta_k = signo * 0.3;
                }
                else if (mover == 20 || mover == -20)
                {
                    gt[index1].delta_k = signo * 0.4;
                    gt[index2].delta_k = signo * 0.4;
                }
                else if (mover == 25 || mover == -25)
                {
                    gt[index1].delta_k = signo * 0.5;
                    gt[index2].delta_k = signo * 0.5;
                }
                else if (mover == 30 || mover == -30)
                {
                    gt[index1].delta_k = signo * 0.55;
                    gt[index2].delta_k = signo * 0.55;
                }
                desplazar(k_final, k_final[1] + gt[index1].delta_k + gt[index2].delta_k);
                gt[index1].se_movio = SE_MOVIO;
                gt[index2].se_movio = SE_MOVIO;

                gt[index2].posicion = mover; //- gt[index2].posicion;
                gt[index1].posicion = mover; //- gt[index1].posicion;
                gt[index1].condicion = ULTIMA_EN_MOVERSE;
                gt[index2].condicion = ULTIMA_EN_MOVERSE;
                cambiar_movimiento_all_except(gt, index1);
            }
            sleep(TIME - 1);
            gt[index1].se_movio = NO_SE_MOVIO;
            gt[index2].se_movio = NO_SE_MOVIO;
        }
    }
}
*/