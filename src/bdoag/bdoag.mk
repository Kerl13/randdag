# Command line utility
$(BUILD)bdoag/bdoag: src/bdoag/cli.c
$(BUILD)bdoag/bdoag: $(BUILD)libbdoag.a
$(BUILD)bdoag/bdoag: src/cli/cli.h src/cli/cli.c
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ src/bdoag/cli.c src/cli/cli.c -lbdoag -lgmp

# Static library
$(BUILD)libbdoag.a: $(BUILD)common/graphs.o
$(BUILD)libbdoag.a: $(BUILD)bdoag/sampling.o
$(BUILD)libbdoag.a: $(BUILD)bdoag/counting.o
	ar rc $@ $?
	ranlib $@

$(BUILD)bdoag/counting.o: src/bdoag/counting.c includes/bdoag.h
	@mkdir -p "$(BUILD)/bdoag"
	$(CC) $(CFLAGS) -o $@ -c src/bdoag/counting.c
$(BUILD)bdoag/sampling.o: src/bdoag/sampling.c includes/bdoag.h
	@mkdir -p "$(BUILD)/bdoag"
	$(CC) $(CFLAGS) -o $@ -c src/bdoag/sampling.c
