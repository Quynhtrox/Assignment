CC := gcc
Flag := -pthread
TARGET := thr_create thr_self race_cond thr_detach thr_mutex thr_cond thr_rwlock sema

thr_create: thr_create.c
	$(CC) $^ -o $@ $(Flag)

thr_detach: thr_detach.c
	$(CC) $^ -o $@ $(Flag)

thr_mutex: thr_mutex.c
	$(CC) $^ -o $@ $(Flag)

thr_cond: thr_cond.c
	$(CC) $^ -o $@ $(Flag)

thr_rwlock: thr_rwlock.c
	$(CC) $^ -o $@ $(Flag)

race_cond: race_cond.c
	$(CC) $^ -o $@ $(Flag)

sema: sema.c
	$(CC) $^ -o $@ $(Flag)

all: thr_create thr_detach thr_mutex thr_rwlock race_cond sema

clean:
	rm -rf $(TARGET)
