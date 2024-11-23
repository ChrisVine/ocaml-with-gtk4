Using OCaml with GTK-4
======================

This repository provides an example program for using OCaml with
GTK-4, employing OCaml's FFI for C libraries.

The code is heavily commented, and I keep it as a reminder to myself
of the basics when considering a new project.  As it might be handy as
an example for others looking for a GUI for OCaml programs, it is
released under the Unlicense (see the LICENSE file for details,
including disclaimers).

No individual OCaml wrapper for GTK-4 is provided.  The schema adopted
by the example is that the GUI is provided by the GTK-4 library using
C code, but everything else in the program, including i/o and the
"business" logic, is intended to occur in the type-safe and
memory-safe world of OCaml.  The code includes examples of how an
OCaml worker thread can have its continuation execute in the GTK
thread (the main program thread).

The program requires OCaml >= 4.08 and dune >= 3.0, and some version
of the GTK-4 C library to be installed.  It will compile with OCaml-5.

The first time the program is compiled, adjust the install directories
and permissions in `Makefile` as necessary.  Then if libintl is
installed run the following as user (not as root), although no
meaningful translations are in fact provided at present so this can be
omitted:

```
make translations
```

Then to build, as user (not as root) run:

```
make
```

The generated binary comprises file `example-prog`.
