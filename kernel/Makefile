.PHONY: all clean

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))


all:$(OBJS)

$(BUILDDIR)/%.o : %.c
	$(CC) $(CPPFLAGS) -o $@ $<
