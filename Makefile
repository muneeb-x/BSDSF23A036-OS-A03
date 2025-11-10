# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -lreadline

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source files (UPDATED: added if_else.c)
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/shell.c $(SRCDIR)/execute.c \
          $(SRCDIR)/builtins.c $(SRCDIR)/history.c $(SRCDIR)/parser.c \
          $(SRCDIR)/redirection.c $(SRCDIR)/pipes.c $(SRCDIR)/jobs.c \
          $(SRCDIR)/chain.c $(SRCDIR)/if_else.c
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Target executable
TARGET = $(BINDIR)/myshell

# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Create object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Install (copy to /usr/local/bin - optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Phony targets
.PHONY: all clean install
