# end device
# State Machine Version
PROG := enddevice
SRCS := enddevice.c FSM.c State.c timerevent.c aes.c frame.c radio.c lora.c appdata.c cayenne_lpp.c showtime.c
HDRS := enddevice.h FSM.h State.h timerevent.h aes.h frame.h radio.h lora.h appdata.h cayenne_lpp.h showtime.h
OBJS := $(SRCS:%.c=%.o)

CC=gcc
CFLAGS= -Wall
LIBS=-lwiringPi -lrt -lgps -lm

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o $(PROG)
	