TARGET_EXE := rhizome
CFLAGS := -Wall -g -O0 -std=c99
LDFLAGS := -lglfw -lGL -lGLU -lm

CC := clang

SRC := \
	array.c \
	dummy_scene.c \
	game.c \
	handle.c \
	input_handler.c \
	main.c \
	player_input.c \
	renderer.c \
	sprite.c \
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
