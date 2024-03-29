# This is the Makefile for Real ReMove.

# Package name and version: BASENAME-VMAJOR.VMINOR.VPATCH.tar.gz
BASENAME:= rrm
VMAJOR:= 2
VMINOR:= 0
VPATCH:= 0

# Add appropriate CFLAGS and LDFLAGS
CFLAGS = -Os -Wall -Wextra -pedantic
# Uncomment the setting below for development.
#CFLAGS = -Os -Weverything -pedantic
# Build a static binary by default.
LDFLAGS += -s -static

# Base directory
PREFIX ?= /usr/local

# Location where the binary will be installed.
BINDIR = $(PREFIX)/bin

# Location where the manual-page will be installed.
MANDIR = $(PREFIX)/man

##### Maintainer stuff goes here:
# Source files.
SRCS:= rrm.c

##### No editing necessary beyond this point
# Object files.
OBJS:= $(SRCS:.c=.o)

BACKUP:= $(BASENAME)-backup-$(VMAJOR).$(VMINOR).$(VPATCH).tar.gz

# Version number
VERSION:= -DVERSION=\"$(VMAJOR).$(VMINOR).$(VPATCH)\"
CFLAGS += $(VERSION)

all: $(BASENAME) $(BASENAME).1 INSTALL

# builds a binary.
$(BASENAME): $(OBJS) Makefile
	$(CC) $(LDFLAGS) $(LDIRS) -o $(BASENAME) $(OBJS) $(LIBS)

replace.sed: Makefile
	@echo "s/_PACKAGE/$(BASENAME)/g" >replace.sed
	@echo "s/_VERSION/$(VMAJOR).$(VMINOR).$(VPATCH)/g" >>replace.sed
	@echo "s/_DATE/`date '+%Y-%m-%d'`/g" >>replace.sed
	@echo "s|_MANDIR|$(MANDIR)|g" >>replace.sed
	@echo "s|_BINDIR|$(BINDIR)|g" >>replace.sed
	@echo "s|_CFLAGS|\"$(CFLAGS)\"|g" >>replace.sed
	@echo "s|_LDFLAGS|\"$(LDFLAGS)\"|g" >>replace.sed

INSTALL: INSTALL.in replace.sed
	sed -Ef replace.sed INSTALL.in >INSTALL

# create the manual pages.
$(BASENAME).1: $(BASENAME).1.in replace.sed
	sed -Ef replace.sed $(BASENAME).1.in >$(BASENAME).1

# Remove all generated files.
clean::
	rm -f $(OBJS) $(BASENAME) *~ core gmon.out $(BACKUP) $(LOG)
	rm -f INSTALL $(BASENAME).1 $(BASENAME).5

# Install the program and manual page. You should be root to do this.
install: $(BASENAME) $(BASENAME).1
	install -d $(BINDIR)
	install -m 755 $(BASENAME) $(BINDIR)
	install -d $(MANDIR)/man1
	install -m 644 $(BASENAME).1 $(MANDIR)/man1
	gzip -f $(MANDIR)/man1/$(BASENAME).1

uninstall::
	@if [ `id -u` != 0 ]; then \
		echo "You must be root to uninstall the program!"; \
		exit 1; \
	fi
	rm -f $(BINDIR)/$(BASENAME)
	rm -f $(MANDIR)/man1/$(BASENAME).1*

# Run 'make depend' to get the dependencies.
depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(SRCS) >depend

# Indent settings are in “.indent.pro”
indent: rrm.c
	indent rrm.c
	rm -f rrm.c.BAK

# Dependencies after this line.
include depend
