# Command line utility
$(BUILD)doag/doag: src/doag/cli.c
$(BUILD)doag/doag: $(BUILD)libdoag.a
$(BUILD)doag/doag: $(BUILD)common/cli.o
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ src/doag/cli.c $(BUILD)common/cli.o -ldoag -lgmp

# Static library
$(BUILD)libdoag.a: $(BUILD)common/graphs.o
$(BUILD)libdoag.a: $(BUILD)common/memo.o
$(BUILD)libdoag.a: $(BUILD)doag/counting.o
$(BUILD)libdoag.a: $(BUILD)doag/sampling.o
	ar rc $@ $?
	ranlib $@

$(BUILD)doag/counting.o: src/doag/counting.c includes/doag.h
	@mkdir -p "$(BUILD)/doag"
	$(CC) $(CFLAGS) -o $@ -c src/doag/counting.c
$(BUILD)doag/sampling.o: src/doag/sampling.c includes/doag.h
	@mkdir -p "$(BUILD)/doag"
	$(CC) $(CFLAGS) -o $@ -c src/doag/sampling.c
