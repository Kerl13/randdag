$(BUILD)argtable.o: lib/argtable-3.1.5/argtable3.h
$(BUILD)argtable.o: lib/argtable-3.1.5/argtable3.c
	@mkdir -p "$(BUILD)"
	$(CC) $(CFLAGS) -o $@ -c lib/argtable-3.1.5/argtable3.c
