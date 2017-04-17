=====================
Really removing files
=====================

This program takes filenames as arguments. It overwrites the contents of
these files with zeros, replaces the filename with a random one and then
unlinks the files.

On modern hard disks, this seems to be enough to obliterate a file, *depending
of the filesystem that is used!*

It will e.g. not work on log-structured file systems or copy-on-write file
systems. On such file systems there is no guarantee that writing to the
beginning of a file actually overwrites the same physical area that the content
originally was in.


License
-------
See the LICENSE file.


Building the program
--------------------
Have a look at the Makefile. At the top of the Makefile there are some
options that can be customized. Usually it is fine to leave them as they are.

If you want to use different compiler and linker options, modify the
``CFLAGS``
and ``LDFLAGS`` respectively. If you want to use another compiler instead of
gcc, you will probably have to change them, along with the ``CC`` variable that
specifies the compiler. If you don't know what this means, leave ``CC``,
``CFLAGS`` and ``LFLAGS`` as they are.

The variables whose name ends in DIR specify where different parts of the
program are installed.

It is usually not necessary to edit the makefile below the line
"##### Maintainer stuff goes here:"

To build the program, simply issue the "make" command. This should work
without producing errors. Building the program has been tested with both
``gcc`` and ``clang``, and with both FreeBSD's ``make`` and GNU ``make``.

There is a special ``Makefile.clang`` that tries to build the software using
link-time optimizations for clang 4.0. Only use this if you know what you are
doing.


Installing the program
----------------------

If building the program succeeded, you should issue the ``su`` command to
become root, and then give the ``make install`` command from the source
directory to install the program.

This will install the program in the directory designated as ``BINDIR`` in the
makefile. By default this is set to ``/usr/local/bin``.

The manual page is installed in the subdirectory man1 of the directory
designated as "MANDIR" in the makefile. By default this is set to the
path ``/usr/local/man``. The manual page will be compressed with gzip.
