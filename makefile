EXEC = shell
FILE = testfile.txt

shell: main.c
	gcc main.c -o shell

clean:
	rm $(EXEC)

test: shell
	./shell

file: shell
	./shell $(FILE)

valgrind: shell
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./shell
