
SRCS=$(wildcard *.asm)
OBJS=$(patsubst %.asm,$(BUILDDIR)/%.o,$(SRCS))

all:$(OBJS)

$(BUILDDIR)/%.o : %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm *.o
