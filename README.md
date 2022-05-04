# Randdag

C library for the counting and uniform generation of various models of DAGs
(Directed Acylic Graphs).


## Building

### The libraries

Running `make` produces 2 static libraries in the `build/` folder, one for each
DAG model:

- `libdoag.a`: Directed Ordered Acyclic Graphs;
- `libldag.a`: Labelled Directed Acyclic Graphs.

Each library exposes a function for counting the number of graphs of given
number of sources, edges an vertices and as well as two random sampling
functions:

- one for the **uniform** random sampling of graphs with a given number of edges
  (`xxx_unif_m`);
- one for the **uniform** random sampling of graphs with a given number of edges
  and vertices.

See the header files in `includes/` for more detail on the usage of each
function.
The counting and random sampling algorithms implemented here, as well as the
different DAG models, are described in
[this pre-print](https://wkerl.me/papers/lagos2020.pdf).

Note that randdag depends on the [GMP](https://gmplib.org/) library.
In order to use one of the libraries, you have to include the appropriate header
file in your C code and link against the library you wish to use **and** GMP,
e.g. `-ldoag -lgmp`.


### The executables

Running `make` also produces 2 executables `build/doag/doag`, and
`build/ldag/ldag`.
They provide a command line interface to the counting an sampling functions.
The executables should be self-documenting, try the `--help` flag:

```
usage: build/doag/doag [-h] [-c <M>] [-s <file>] [-d <file>] [-l <file>]
  -h, --help           display this help and exit
  -c, --count=<M>      count DAGs with up to M edges, defaults to 30
  -s, --sample=<file>  write a uniform DAG with M edges to <file>
  -d, --dump=<file>    dump counting info to <file>
  -l, --load=<file>    load counting info from <file>
```

## Examples

Well-documented examples of use of the randdag libraries can be found in the
`examples` folder.
Take a look at the code and run make from this folder for building the examples.


## Documentation

A succint API documentation can be generated using
[`doxygen`](https://www.doxygen.nl/index.html).
To do so, run `make doc` and open a browser at `build/doc/html/index.html`.


## Dependencies and Compliance

Randdag depends on the [GMP](https://gmplib.org/) library for big integer
computations.

### Libraries

The code for the 3 libraries *should* be compliant with the ANSI C standard,
this means that you should expect it to compile and run on any system where you
manage to compile GMP.

The makefiles however require a POSIX-compliant environment to be used.
Windows users will have to build randdag by hand, sorry…

### Executables

The executables have a few more compatibility requirements:

- In addition to GMP, the executables require the `getopt` library to be
  available on your system. This is the case on GNU Linux for instance.

- At the moment, we use the `getrandom` Linux-specific function to seed the
  pseudo-random number generator. This might change in the future.

- The executables are C99-compliant.


## License

This work is distributed under the GPL license version 3 (see the `LICENSE`
file).
