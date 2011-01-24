TARGET_EXE := rhizome
CFLAGS := -Wall -g -O0 -std=c99
LDFLAGS := -lglfw -lGL -lGLU -lm

BUILD_DIR := build

CC := clang

SRC := \
	array.c \
	dummy_scene.c \
	game.c \
	group.c \
	handle.c \
	input_handler.c \
	main.c \
	player_input.c \
	quaternion.c \
	renderer.c \
	sprite.c \
	transform.c \
	vect.c
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))
DEPS := $(patsubst %.c, $(BUILD_DIR)/%.P, $(SRC))

$(TARGET_EXE): $(OBJS)
	@echo linking $@...
	@$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@echo building $@...
	@mkdir -p $(BUILD_DIR)
	@$(CC) -MD -o $@ $< -c $(CFLAGS)
	@cp $(BUILD_DIR)/$*.d $(BUILD_DIR)/$*.P;
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	     -e '/^$$/ d' -e 's/$$/ :/' < $(BUILD_DIR)/$*.d >> $(BUILD_DIR)/$*.P
	@rm -f $(BUILD_DIR)/$*.d

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET_EXE)

-include $(DEPS)
