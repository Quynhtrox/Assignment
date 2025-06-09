CC := gcc

file: file.c
	$(CC) $^ -o $@

Same-ls: Same-ls.c
	$(CC) $^ -o $@

all: file Same-ls

clean:
	rm -rf file README.txt Same-ls
