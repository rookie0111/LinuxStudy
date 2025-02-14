SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
server:$(OBJS)
	$(CC) $^ -o $@ -lpthread
%:%.c
	$(CC) -c $^ -o $@ -g 
clean:
	$(RM) $(EXEX)
rebuild:clean all

