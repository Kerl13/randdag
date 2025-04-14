# Run all the tests
DOAG_TESTS = \
	$(BUILD)tests/doag/forests \
	$(BUILD)tests/doag/small_cases \
	$(BUILD)tests/doag/unary_binary \

doag-tests: $(DOAG_TESTS)
	for t in $(DOAG_TESTS); do ./$$t; done

#
# --- Generate the test scripts ---
#

$(BUILD)tests/doag/forests: tests/doag/forests.c $(BUILD)libdoag.a
	@mkdir -p "$(BUILD)tests/doag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/doag/forests.c -ldoag -lgmp

$(BUILD)tests/doag/unary_binary: tests/doag/unary_binary.c $(BUILD)libdoag.a
	@mkdir -p "$(BUILD)tests/doag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/doag/unary_binary.c -ldoag -lgmp

$(BUILD)tests/doag/small_cases: tests/doag/small_cases.c $(BUILD)libdoag.a
	@mkdir -p "$(BUILD)tests/doag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/doag/small_cases.c -ldoag -lgmp
