$(BUILD)argtable.o: lib/argtable3/argtable3.h
$(BUILD)argtable.o: lib/argtable3/argtable3.c
	@mkdir -p "$(BUILD)"
	$(CC) -Wall -Wextra -pedantic -Wmissing-prototypes -Wno-clobbered -O2 -std=c99 -o $@ -c lib/argtable3/argtable3.c


# Note on the -Wno-clobbered flag
#
# A warning is issued by GCC in the trex_compile function, see :
# https://github.com/argtable/argtable3/issues/53.
# We can safely ignore it since we do not use the arg_rex* stuff.
