PRJ_NAME := Chat_app
CLIENT := client_tls

CC := gcc
FLAGS := -lpthread -lssl -lcrypto

all: 
	$(CC) client_tls.c -o $(CLIENT) $(FLAGS)

clean:
	rm -rf $(CLIENT)