ifndef RHIZOME_PREFIX
RHIZOME_PREFIX := rhizome
endif

SRC += \
	$(RHIZOME_PREFIX)/array.c \
	$(RHIZOME_PREFIX)/game.c \
	$(RHIZOME_PREFIX)/group.c \
	$(RHIZOME_PREFIX)/handle.c \
	$(RHIZOME_PREFIX)/input_handler.c \
	$(RHIZOME_PREFIX)/player_input.c \
	$(RHIZOME_PREFIX)/quaternion.c \
	$(RHIZOME_PREFIX)/renderer.c \
	$(RHIZOME_PREFIX)/sprite.c \
	$(RHIZOME_PREFIX)/transform.c \
	$(RHIZOME_PREFIX)/vect.c
