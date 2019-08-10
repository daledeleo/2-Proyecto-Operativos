all: project ingresar

project: bases/project.c 
	gcc -Wall -g -I include/ $^ -pthread -o $@

ingresar: bases/ingresar.c 
	gcc -Wall -g -I include/ $^ -o $@

clean:
	rm -rf ingresar project