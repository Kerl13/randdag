# Command line utility
$(BUILD)doag/doag: src/doag/cli.c
$(BUILD)doag/doag: $(BUILD)libdoag.a
$(BUILD)doag/doag: $(BUILD)common/cli.o
$(BUILD)doag/doag: src/cli/cli.h src/cli/cli.c
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ $(BUILD)common/cli.o src/doag/cli.c src/cli/cli.c -ldoag -lgmp

# Static library
$(BUILD)libdoag.a: $(BUILD)common/graphs.o
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
