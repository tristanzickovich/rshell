CFLAGS=-ansi -pedantic -Wall -Werror

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell
	g++ $(CFLAGS) src/ls.cpp -o bin/ls

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell

ls:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/ls.cpp -o bin/ls
