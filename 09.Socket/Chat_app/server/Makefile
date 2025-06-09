.PHONY: create_obj shared

PRJ_NAME := Chat_app
SHARED_LIB_NAME := libchatapp.so

SERVER := server_tls

DATA_FILE := chat_history.txt chat.log storage_account.txt

CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin

LIB_DIR := $(CUR_DIR)/lib
LIB_STATIC_DIR := $(LIB_DIR)/static
LIB_SHARED_DIR := $(LIB_DIR)/shared

CC := gcc
FLAGS := -lpthread -lssl -lcrypto -I$(INC_DIR)
C-FLAGS := -c -fPIC

# Create object files
create_obj:
	$(CC) $(C-FLAGS) -o $(OBJ_DIR)/shared_data.o $(SRC_DIR)/shared_data.c $(FLAGS)
	$(CC) $(C-FLAGS) -o $(OBJ_DIR)/log.o $(SRC_DIR)/log.c $(FLAGS)
	$(CC) $(C-FLAGS) -o $(OBJ_DIR)/handle_client.o $(SRC_DIR)/handle_client.c $(FLAGS)
	$(CC) $(C-FLAGS) -o $(OBJ_DIR)/server_tls.o $(CUR_DIR)/server_tls.c $(FLAGS)

# Create shared-lib
shared:
	$(CC) -shared $(OBJ_DIR)/* -o $(LIB_SHARED_DIR)/$(SHARED_LIB_NAME)

install:
	install $(LIB_SHARED_DIR)/$(SHARED_LIB_NAME) /usr/lib

all: create_obj shared install
	$(CC)  $(OBJ_DIR)/server_tls.o -L$(LIB_SHARED_DIR) -lchatapp $(FLAGS) -o $(BIN_DIR)/$(SERVER)

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(LIB_SHARED_DIR)/*
	rm -rf $(BIN_DIR)/*
	rm -rf $(DATA_FILE)