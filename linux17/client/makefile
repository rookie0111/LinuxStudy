SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
client:$(OBJS)
	$(CC) $^ -o $@ -lpthread
%:%.c
	$(CC) -c $^ -o $@ -g 
clean:
	$(RM) $(EXEX)
rebuild:clean all

