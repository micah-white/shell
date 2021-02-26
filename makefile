EXEC = shell
FILE = testfile.txt

shell: main.c
	gcc main.c -o shell

clean:
	rm *.o shell

test: shell
	./shell

file: shell
	./shell $(FILE)