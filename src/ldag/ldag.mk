# Command line utility
$(BUILD)ldag/ldag: src/ldag/cli.c
$(BUILD)ldag/ldag: $(BUILD)libldag.a
$(BUILD)ldag/ldag: $(BUILD)common/cli.o
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ src/ldag/cli.c $(BUILD)common/cli.o -lldag -lgmp

# Static library
$(BUILD)libldag.a: $(BUILD)common/graphs.o
$(BUILD)libldag.a: $(BUILD)ldag/counting.o
	$(AR) rc $@ $?
	$(RANLIB) $@

$(BUILD)ldag/counting.o: src/ldag/counting.c includes/ldag.h
	@mkdir -p "$(BUILD)ldag"
	$(CC) $(CFLAGS) -o $@ -c src/ldag/counting.c
