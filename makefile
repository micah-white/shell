EXEC = shell
FILE = testfile.txt

shell: main.c
	gcc main.c -g -o shell

clean:
	rm *.o shell

test: shell
	./shell

file: shell
	./shell $(FILE)

valgrind: shell
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell