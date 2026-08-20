# Makefile

VERSION := 0.3.0
TARGET := $(CURDIR)/vysor

CC ?= gcc
CPPFLAGS := -DVERSION=\"$(VERSION)\" -I$(CURDIR)/include
CFLAGS := -O3 -march=native -ffast-math -g -Wall -Wextra
LDFLAGS := 
LDLIBS := -lncurses -lpulse-simple -lpulse -lpthread

SOURCEDIR := $(CURDIR)/src
OBJDIR := $(CURDIR)/obj

SOURCE := $(SOURCEDIR)/main.c \
		  $(SOURCEDIR)/dft.c \
		  $(SOURCEDIR)/audio.c
OBJ := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/%.o,$(SOURCE))
DEP := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/%.d,$(SOURCE))

.PHONY: all clean 

all: $(TARGET)

clean:
	rm -f $(OBJ) $(DEP)

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -MMD -MP

-include $(DEP)
