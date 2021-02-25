EXEC = shell

shell: main.c
	gcc main.c -o shell

clean:
	rm *.o shell

test: shell
	./shell