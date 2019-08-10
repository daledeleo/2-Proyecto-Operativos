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
   int profundidad; //profundidad que se sumerge la barrra
   int condicion; //1 se mueve arriba y -1 se mueve hacia abajo
   int longitud_max; //longitud de la barra
   int se_movio; // si se movio vale 1 si no se movio vale 0
   float delta_k;

}objeto_barra;


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
    if (valor >= 0){
        return 1;
    }
    return 1;
}
void desplazar(int *k,int valor){
    int temp=k[1];
    k[1]=valor;
    k[0]=temp;
}
void iniciar_barras(struct barra *list){
    for(int i=0;i<16;i++){
        struct barra gt;
        gt.profundidad=0;
        gt.condicion=1;
        gt.longitud_max=30;
        gt.se_movio=0;
        gt.delta_k=0;
        list[i]=gt;
    }
}

void mover_barra(struct barra bar,int mover){
        if(mover==10){
            bar.delta_k=0.1;
        }else if(mover == 15){
            bar.delta_k=0.3;
        }else if(mover ==20){
            bar.delta_k=0.4;
        }else if(mover == 25){
            bar.delta_k=0.5;
        }else if(mover==30){
            bar.delta_k=0.55;
        }
        int temp=bar.posicion+mover;
        if(temp > 30 || temp < 0){
            printf("No se puede hacer dicho movimiento\n");
            return ;
        }
        bar.posicion=temp;
    }

}
void imprimir_barras(struct barra *list,int valor_k){
    printf("\tb\tbbarra1\tbbarra2\tbbarra3\tbbarra4\tbbarra5\tbbarra6\tbbarra7\tbbarra8\n");
    printf("posicion:%i\tb\tb%i\tb%i\tb%i\tb%i\tb%i\tb%i\tb%i\n",list[0].profundidad,list[1].profundidad,
    list[2].profundidad,list[3].profundidad,list[4].profundidad,list[5].profundidad,list[6].profundidad,list[7].profundidad);
    printf("delta k:%i\tb\tb%i\tb%i\tb%i\tb%i\tb%i\tb%i\tb%i\n",list[0].delta_K,list[1].delta_K,
    list[2].delta_K,list[3].delta_K,list[4].delta_K,list[5].delta_K,list[6].delta_K,list[7].delta_K);



}