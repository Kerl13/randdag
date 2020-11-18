# Randdag

C library for the counting and uniform generation of various models of DAGs
(Directed Acylic Graphs).

## Build

Run `make`.
For each DAG model, this will produce in `build/$MODEL_NAME/` a static library
and an executable providing a command line interface to the counting and
sampling functions.
