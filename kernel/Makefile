.PHONY: all clean

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))



all:$(OBJS)

$(BUILDDIR)/%.o : %.c $(BUILDDIR)/%.d
	$(CC) $(CPPFLAGS) -o $@ $<


$(BUILDDIR)/%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(OBJS:.o=.d)


