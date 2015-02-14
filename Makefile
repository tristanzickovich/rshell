CFLAGS=-g -ansi -pedantic -Wall -Werror

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell
	g++ $(CFLAGS) src/ls.cpp -o bin/ls
	g++ $(CFLAGS) src/cp.cpp -o bin/cp

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell

ls:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/ls.cpp -o bin/ls

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/cp.cpp -o bin/cp


