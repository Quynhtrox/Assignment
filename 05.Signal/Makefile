.PHONY := Signal_Assignment

CC := gcc
TARGET := Assignment1 Assignment2 Assignment3 Assignment4_sender Assignment4_received

Assignment1: Assignment1.c 
	$(CC) $^ -o $@

Assignment2: Assignment2.c 
	$(CC) $^ -o $@

Assignment3: Assignment3.c 
	$(CC) $^ -o $@

Assignment4_sender: Assignment4_sender.c 
	$(CC) $^ -o $@

Assignment4_received: Assignment4_received.c 
	$(CC) $^ -o $@

all: Assignment1 Assignment2 Assignment3 Assignment4

clean:
	rm -rf $(TARGET)