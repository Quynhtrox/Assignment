
CC := gcc
flags := 

all:
	gcc -o chat_server server_tls.c -lpthread -lssl -lcrypto
	gcc -o chat_client client_tls.c -lpthread -lssl -lcrypto


clean:
	rm -rf chat_client chat_server