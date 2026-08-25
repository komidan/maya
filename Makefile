# OPTIONS
CC := gcc

TARGET   := maya
SRCDIR   := src
INCDIR   := include
LIBDIR   := lib
BUILDDIR := build
BINDIR   := bin

CFLAGS  += -g -Wall -Wextra -Wpedantic -std=c99 -I$(INCDIR)
LDFLAGS += -L$(LIBDIR)

# FILES
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# LINK
$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# COMPILE
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# CLEAN
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

.PHONY: clean

# The irony of using Make...
