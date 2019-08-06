#include "include/memoria.h"

#define SHMSZ 27

int main()
{
    /*
   

    struct memoria *memory;
    int shmid;
    key_t key;
    char *shm, *s;

    
    key = 5678;

   
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        return (1);
    }

    if ((memory = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        return (1);
    }

    
    for (s = shm; *s != NULL; s++)
        putchar(*s);

    putchar('\n');

    
    printf("Altero la memoria\n");
    *shm = '*';
     */
   printf("pid del padre %d\n", getpid());
        
	char buf[100] = {0};
	char *args[10] = {0};
	while(1){
		
		printf("ingrese un numero: ");
		fgets(buf, 100, stdin);
		reemplazar(buf);
		while(validar_num(buf)<0){
            printf("Ingrese un numero valido: ");
		    fgets(buf, 100, stdin);
		    reemplazar(buf);
        }
		int i = 0;
		char *valor;
		args[i] = strdup(strtok(buf, " "));
		i++;		
		while( (valor = strtok(NULL, " ")) != NULL ){
			args[i] = strdup(valor);
			i++;
		}
       

		printf("%s\n", buf);
		fflush(stdout);
		
		
	}
    return (0);
}

 

