# Makefile

VERSION := 0.4.0
TARGET := $(CURDIR)/vysor

CC ?= gcc
CPPFLAGS := -DVERSION=\"$(VERSION)\" -I$(CURDIR)/include
CFLAGS := -O3 -march=native -ffast-math -g -Wall -Wextra
LDFLAGS := 
LDLIBS := -lncurses -lpulse-simple -lpulse -lpthread

PREFIX ?= /usr/local
BINDIR := $(DESTDIR)$(PREFIX)/bin

SOURCEDIR := $(CURDIR)/src
OBJDIR := $(CURDIR)/obj

SOURCE := $(SOURCEDIR)/main.c \
		  $(SOURCEDIR)/fft.c \
		  $(SOURCEDIR)/audio.c
OBJ := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/%.o,$(SOURCE))
DEP := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/%.d,$(SOURCE))

.PHONY: all clean install uninstall

all: $(TARGET)

clean:
	rm -f $(OBJ) $(DEP)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)/vysor

uninstall:
	rm -f $(BINDIR)/vysor

$(TARGET): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -MMD -MP

-include $(DEP)
