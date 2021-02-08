SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

.PHONY: all clean
all: $(BINS)

$(BINS): %: %.c
	gcc -o $@ $<

clean:
	rm $(BINS)
