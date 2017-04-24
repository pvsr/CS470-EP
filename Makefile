CC=mpicc
CFLAGS = -g -std=c99 -Wall -Wextra -lgmp -lpthread
SRCS = cli.c main.c votes.c listpr.c io.c ranked.c fptp.c
OBJS = $(SRCS:.c=.o)
MAIN = votecounter

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(MAIN) *.vf *.html
