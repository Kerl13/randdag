# Randdag

C library for the counting and uniform generation of various models of DAGs
(Directed Acylic Graphs).

## Build

Run `make`.

For each DAG model, this will produce in `build/$MODEL_NAME/` a static library
and an executable providing a command line interface to the counting and
sampling functions.

Randdag depends on the GMP library.

## Usage

The executables are self-documenting, try the `-h` command line option.

To use randdag, include the appropriate header file (see the `includes/` folder)
in your C files and link against the library you wish to use **and** GMP, e.g.
`-ldoag -lgmp`.
