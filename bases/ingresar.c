#include "memoria.h"

#define SHMSZ 27

int main()
{
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


   printf("pid del padre %d\n", getpid());
        
	char buf[100] = {0};
	while(1){
		
		printf("ingrese un numero: ");
		fgets(buf, 100, stdin);
		reemplazar(buf);
		while(validar_num(buf)<0){
            printf("Ingrese un numero valido: ");
		    fgets(buf, 100, stdin);
		    reemplazar(buf);
        }
		printf("%s\n", buf);
        *shm=buf;
		fflush(stdout);
		
		
	}
    return (0);
}

 

