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
#include <math.h>
#include <time.h>

#define PUERTO 5600 //puerto para el servidor y el cliente
#define PUERTO_AZ5 6000 //puerto para mandar la señal AZ5
#define TIME 2      //Representacion del tiempo t(en segundos)
#define SE_MOVIO 2
#define NO_SE_MOVIO 0
#define ESTA_EN_MOVIMIENTO 1
#define ULTIMA_EN_MOVERSE 1

sem_t sem1; //para controlar las tablas por pantalla
sem_t sem2; //para el cronometro

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

int Az5_active(struct barra *gt,float k){
    for(int i=0;i<15;i++){
        gt[i].posicion=30;
        k=k-gt[i].delta_k;
        if(k<=0){
            k=0;
        }
    }
    return 1;
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

void cambiar_movimiento_all(struct barra *gt)
{
    for (int i = 0; i < 16; i++)
    {
        gt[i].se_movio = NO_SE_MOVIO;
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
        gt.delta_k = 0.0;
        list[i] = gt;
    }
}

int mover_barra(struct barra bar, struct barra bar2, int mover)
{
    int mov = bar.posicion + mover;
    if (mov >= 30)
    {
        bar.posicion=30;
        return 1;
    }
    if (mov <= 0)
    {
        bar.posicion=0;
        return 1;
    }
    return 1;
}

/*1 si todas se movieron -1 si no*/
int todas_se_movieron(struct barra *list)
{
    int ban = 0;
    int bah = 0;
    for (int i = 0; i < 15; i++)
    {
        ban = list[i].se_movio + ban;
        bah = bah + list[i].posicion;
    }
    if (ban == 15 * SE_MOVIO)
    {
        return 1; //ya se movieron todas
    }
    return -1; //aun no se han movido todas
}
//
void imprimir_barras(struct barra *list, float valor_k)
{
    if(es_el_comienzo(list)>0){
        list[15].posicion=0;
    }
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
    for (int i = 0; i < 9; i++)
    {
        if (gt[i].condicion == ULTIMA_EN_MOVERSE)
        {
            printf("ultimo movimiento fue las barras: barra%i barra%i\n", (i + 1), encontrar_par(i) + 1);
            return i;
        }
    }
    return -1; //no hubo ultimo movimiento
}

int quitar_barras(struct barra *gt,int eval){
    for(int i=0;i<16;i++){
        
    }
    return -1;
}

/*indice de la barra priorietaria*/
int prioriedad_index(struct barra *gt, float delta_k)
{

    if (es_el_comienzo(gt) > 0)
    {
        /*numero = rand () % (N-M+1) + M;   // Este está entre M y N*/
        return (int)(rand() % 9);
    }
    else
    {
        int ind = determinar_ultimo_movimiento(gt);
        /*(1) Barras que no se encuentren ya en movimiento y (2) barras que 
            sean perpendiculares al ultimo par de
            barras que se encuentran en movimiento.*/
        if (ind >= 0)
        {
            if (todas_se_movieron(gt) > 0)
            {
                cambiar_movimiento_all(gt);
            }
            int i = encontrar_perpendicular(ind);
            while (gt[i].se_movio != NO_SE_MOVIO)
            {
                ind = (int)(rand() % 9);
                i = encontrar_perpendicular(ind);
            }
            return i;
        }
    }
    printf("Error al sacar la prioriedad en las barras\n");
    return -1;
}
/*  0 si faltan de moverse 10m de profundidad
    1 si faltan de moverse 15m de profundidad
    2 si faltan de moverse 20m de profundidad
    3 si faltan de moverse 25m de profundidad
    4 si faltan de moverse 30m de profundidad
*/
int determinar_profundidad(struct barra *gt, float k)
{
    int var = 0;
    for (int i = 0; i < 15; i++)
    {
        var = gt[i].posicion + var;
    }
    if (var < 15 * profundidades[0])
    {
        return 0;
    }
    else if (var >= 15 * profundidades[0] && var < 15 * profundidades[1])
    {
        return 1;
    }
    else if (var >= 15 * profundidades[1] && var < 15 * profundidades[2])
    {
        return 2;
    }
    else if (var >= 15 * profundidades[2] && var < 15 * profundidades[3])
        return 3;

    else if (var >= 15 * profundidades[3])
    {
        return 4;
    }
    printf("Error en el determinar la profundidad\n");
    return -1;
}
