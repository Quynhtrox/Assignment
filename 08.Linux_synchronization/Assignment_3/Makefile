CC := gcc
TARGET := writer_POSIX reader_POSIX writer_SYSV reader_SYSV

writer_POSIX: writer_POSIX.c
	$(CC) $^ -o $@

reader_POSIX: reader_POSIX.c
	$(CC) $^ -o $@

writer_SYSV: writer_SYSV.c 
	$(CC) $^ -o $@

reader_SYSV: reader_SYSV.c 
	$(CC) $^ -o $@

all: writer_POSIX reader_POSIX writer_SYSV reader_SYSV

clean: 
	rm -rf $(TARGET)