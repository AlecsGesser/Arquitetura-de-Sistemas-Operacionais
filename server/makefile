all: server cliente

first: server.cpp
	gcc -lpthread -Wimplicit-function-declaration -o server server.cpp test.cpp
second: cliente.cpp
	gcc -o client cliente.cpp -lpthread
