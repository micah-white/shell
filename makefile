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

valgrind: shell
	valgrind --leak-check=full --show-leak-kinds=all ./shell $(FILE)	