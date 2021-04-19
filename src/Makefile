CC = g++
CFLAGS = -Wall -g
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lpthread

all: server client

server: server_socket.o server_socket_listener.o

client: client_socket.o

server_socket.o: server_socket.hpp abst_server_socket.hpp

server_socket_listener.o: server_socket_listener.hpp abst_server_socket_listener.hpp

client_socket.o: client_socket.hpp abst_client_socket.hpp


clean:
	rm *.o
	rm server
	rm client

