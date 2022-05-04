#ifndef UTILS_H
#define UTILS_H

/** Consume one argument from argv.
 * Returns NULL if argv is empty.
 * Otherwise, return the next argument, decrements *argc by one, and move *argv
 * to the next argument. */
char *next_argument(int *argc, char ***argv);

/** Parse and return the first element of *argv as a positive integer.
 * Returns -1 if it fails (not enough arguments in argv, invalid int, etc).
 *
 * The function "consumes" one command line argument.
 * This means that *argc is decremented by one and that *argv is moved to the
 * next command line argument after a successful call to parse_int. */
int parse_int(const char *usage_string, /* usage string */
              const char *progname,     /* program name (for usage) */
              int *argc,                /* a pointer to argc */
              char ***argv);            /* a pointer to argv */

/** Parse one integer from the command line and store it in n.
 * Returns a non-zero value in case of an error. */
int parse_one_int(const char *usage_string, int argc, char **argv, int *n);

/** Parse three integers from the command line and store them in n, m and k.
 * Returns a non-zero value in case of an error. */
int parse_three_ints(const char *usage_string, int argc, char **argv, int *n,
                     int *m, int *k);

#endif
