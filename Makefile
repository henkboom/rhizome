TARGET_EXE := rhizome
CFLAGS := -Wall -g -O0
LDFLAGS := -lglfw -lGL -lGLU -lm

CC := clang

SRC := \
	array.c \
	game.c \
	handle.c \
	main.c \
	renderer.c \
	transform.c \
	vect.c
OBJS := $(SRC:.c=.o)
DEPS := $(SRC:.c=.P)

$(TARGET_EXE): $(OBJS)
	@echo linking $@...
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@echo building $@...
	@$(CC) -MD -o $@ $< -c $(CFLAGS)
	@cp $*.d $*.P;
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	     -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P
	@rm -f $*.d

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET_EXE)

-include $(DEPS)
