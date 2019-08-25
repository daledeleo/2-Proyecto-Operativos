all: project ingresar az5

az5: bases/az5.c 
	gcc -Wall -g -I include/ $^ -o $@

project: bases/project.c 
	gcc -Wall -g -I include/ $^ -pthread -o $@

ingresar: bases/ingresar.c 
	gcc -Wall -g -I include/ $^ -o $@

clean:
	rm -rf ingresar project az5