$(BUILD)argtable.o: lib/argtable-3.1.5/argtable3.h
$(BUILD)argtable.o: lib/argtable-3.1.5/argtable3.c
	@mkdir -p "$(BUILD)"
	$(CC) $(CFLAGS) -Wno-clobbered -o $@ -c lib/argtable-3.1.5/argtable3.c


# Note on the -Wno-clobbered flag
#
# A warning is issued by GCC in by the trex_compile function, see :
# https://github.com/argtable/argtable3/issues/53.
# We can safely ignore it as long as we do not use the arg_rex* stuff.
