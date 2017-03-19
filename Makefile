CFLAGS = -g -std=c99 -Wall -lgmp
SRCS = cli.c main.c votes.c
OBJS = $(SRCS:.c=.o)
MAIN = votecounter

all: $(MAIN)

$(MAIN): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(MAIN)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(MAIN)
