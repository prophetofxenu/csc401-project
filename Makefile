CC = g++
CFLAGS = -Wall -Wextra -g
CXXFLAGS = $(CFLAGS) -std=c++17
INCLUDES = -I include/
LIBS = -lpthread -l:libcryptopp.so

INCLUDE = include
SRC = src
BUILD = build

OBJECTS =  $(BUILD)/add_rfc_msg.o
OBJECTS += $(BUILD)/add_rfc_res.o
OBJECTS += $(BUILD)/central_index.o
OBJECTS += $(BUILD)/client_socket.o
OBJECTS += $(BUILD)/crypto_common.o
OBJECTS += $(BUILD)/encrypted_client_socket.o
OBJECTS += $(BUILD)/encrypted_server_socket.o
OBJECTS += $(BUILD)/encrypted_server_socket_listener.o
OBJECTS += $(BUILD)/get_rfc_msg.o
OBJECTS += $(BUILD)/get_rfc_res.o
OBJECTS += $(BUILD)/list_rfc_msg.o
OBJECTS += $(BUILD)/list_rfc_res.o
OBJECTS += $(BUILD)/lookup_rfc_msg.o
OBJECTS += $(BUILD)/lookup_rfc_res.o
OBJECTS += $(BUILD)/rfc_manager.o
OBJECTS += $(BUILD)/server_socket.o
OBJECTS += $(BUILD)/server_socket_listener.o

all: setup client server

client: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(INCLUDES) $(OBJECTS) $(LIBS) $(SRC)/client.cpp -o $@

server: $(OBJECTS)
	$(CC) $(CXXFLAGS) $(INCLUDES) $(OBJECTS) $(LIBS) $(SRC)/server.cpp -o $@

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

setup:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm client server

