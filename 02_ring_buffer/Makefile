CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
TARGET  = ring_demo
SRCS    = main.c ring_buffer.c app_uart.c app_filter.c
OBJS    = $(SRCS:.c=.o)
HDRS    = ring_buffer.h app_uart.h app_filter.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all run clean
