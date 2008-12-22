# Time-stamp: <2008-12-22 14:07:03 rsmith>
# This is the Makefile for Real ReMove.

# Package name and version: BASENAME-VMAJOR.VMINOR.VPATCH.tar.gz
BASENAME = rrm
VMAJOR   = 1
VMINOR   = 0
VPATCH   = 0

# Define the C compiler to be used, usually gcc.
#CC = gcc

# Add appropriate CFLAGS and LFLAGS
CFLAGS ?= -O2 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes
CFLAGS += -pipe -fmerge-constants --fast-math -DNDEBUG
#CFLAGS += -pipe -fmerge-constants --fast-math
LFLAGS += -s -pipe -fmerge-constants

# Base directory
PREFIX ?= /usr/local

# Location where the binary will be installed.
BINDIR = $(PREFIX)/bin

# Location where the manual-page will be installed.
MANDIR = $(PREFIX)/man

# Location where the documentatiion will be installed.
DOCSDIR= $(PREFIX)/share/doc/$(BASENAME)

##### Maintainer stuff goes here:

# Standard files that need to be included in the distribution
DISTFILES = README INSTALL LICENSE Makefile $(BASENAME).1 depend

# Source files.
SRCS = rrm.c

# Extra stuff to add into the distribution.
XTRA_DIST=

##### No editing necessary beyond this point
# Object files.
OBJS = $(SRCS:.c=.o)

# Predefined directory/file names
PKGDIR  = $(BASENAME)-$(VMAJOR).$(VMINOR).$(VPATCH)
TARFILE = $(PKGDIR).tar.gz
#beginskip
BACKUP  = $(BASENAME)-backup-$(VMAJOR).$(VMINOR).$(VPATCH).tar.gz
#endskip

# Version number
VERSION = -DVERSION=\"$(VMAJOR).$(VMINOR).$(VPATCH)\"
# Program name
PACKAGE = -DPACKAGE=\"$(BASENAME)\"
# Add to CFLAGS
CFLAGS += $(VERSION) $(PACKAGE)

all: $(BASENAME) $(BASENAME).1 README INSTALL

# builds a binary.
$(BASENAME): $(OBJS)
	$(CC) $(LFLAGS) $(LDIRS) -o $(BASENAME) $(OBJS) $(LIBS)

#beginskip

replace.sed: Makefile
	@echo "s/_PACKAGE/$(BASENAME)/g" >replace.sed
	@echo "s/_VERSION/$(VMAJOR).$(VMINOR).$(VPATCH)/g" >>replace.sed
	@echo "s/_DATE/`date '+%Y-%m-%d'`/g" >>replace.sed
	@echo "s|_DOCSDIR|$(DOCSDIR)|g" >>replace.sed
	@echo "s|_MANDIR|$(MANDIR)|g" >>replace.sed
	@echo "s|_BINDIR|$(BINDIR)|g" >>replace.sed
	@echo "s|_CFLAGS|\"$(CFLAGS)\"|g" >>replace.sed
	@echo "s|_LFLAGS|\"$(LFLAGS)\"|g" >>replace.sed

README: README.in replace.sed
	sed -Ef replace.sed README.in >README

INSTALL: INSTALL.in replace.sed
	sed -Ef replace.sed INSTALL.in >INSTALL

# create the manual pages.
$(BASENAME).1: $(BASENAME).1.in replace.sed
	sed -Ef replace.sed $(BASENAME).1.in >$(BASENAME).1

#endskip
# Remove all generated files.
clean::
	rm -f $(OBJS) $(BASENAME) *~ core gmon.out $(TARFILE)* \
	$(BACKUP) $(LOG)
#beginskip
	rm -f README INSTALL $(BASENAME).1 $(BASENAME).5
#endskip


# Install the program and manual page. You should be root to do this.
install: $(BASENAME) $(BASENAME).1 LICENSE README
	@if [ `id -u` != 0 ]; then \
		echo "You must be root to install the program!"; \
		exit 1; \
	fi
	install -d $(BINDIR)
	install -m 755 $(BASENAME) $(BINDIR)
	install -d $(MANDIR)/man1
	install -m 644 $(BASENAME).1 $(MANDIR)/man1
	gzip -f $(MANDIR)/man1/$(BASENAME).1
.if !defined(NOPORTDOCS)
	install -d $(DOCSDIR)
	install -m 644 LICENSE README $(DOCSDIR)
.endif

uninstall::
	@if [ `id -u` != 0 ]; then \
		echo "You must be root to uninstall the program!"; \
		exit 1; \
	fi
	rm -f $(BINDIR)/$(BASENAME)
	rm -f $(MANDIR)/man1/$(BASENAME).1*
	rm -rf $(DOCSDIR)

#beginskip
# Build a tar distribution file. Only needed for the maintainer.
dist: clean replace.sed $(DISTFILES) $(XTRA_DIST)
	rm -rf $(PKGDIR)
	mkdir -p $(PKGDIR)
	cp $(DISTFILES) $(XTRA_DIST) *.c $(PKGDIR)
	rm -f $(PKGDIR)/Makefile
	awk -f makemakefile.awk Makefile >$(PKGDIR)/Makefile
	tar -czf $(TARFILE) $(PKGDIR)
	rm -rf $(PKGDIR)
	md5 $(TARFILE) >$(TARFILE).md5
	gpg -a -b $(TARFILE)

# Builds the program from the distribution file, as a test.
fromdist: $(TARFILE)
	tar xzf $(TARFILE)
	cd $(PKGDIR); make
	rm -rf $(PKGDIR)

# Make a backup. Only for the maintainer.
backup: clean $(LOG)
	rm -rf /tmp/$(PKGDIR)
	mkdir -p /tmp/$(PKGDIR)
	cp -R -p * /tmp/$(PKGDIR)
	CURDIR=`pwd`
	cd /tmp ; tar -czf $(CURDIR)/$(BACKUP) $(PKGDIR)
	rm -rf /tmp/$(PKGDIR)

# Run 'make depend' to get the dependencies.
depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(SRCS) >depend

#endskip
# Dependencies after this line.
include depend
