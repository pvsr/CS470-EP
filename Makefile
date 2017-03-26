CFLAGS = -g -std=c99 -Wall -Wextra
SRCS = cli.c main.c votes.c listpr.c io.c
OBJS = $(SRCS:.c=.o)
MAIN = votecounter

all: $(MAIN)

$(MAIN): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(MAIN)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(MAIN) *.vf *.html
