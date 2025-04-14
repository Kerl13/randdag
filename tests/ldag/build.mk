LDAG_TESTS = \
	$(BUILD)tests/ldag/forests \
	$(BUILD)tests/ldag/small_cases \
	$(BUILD)tests/ldag/unary_binary \

# Run all the tests
ldag-tests: $(LDAG_TESTS)
	for t in $(LDAG_TESTS); do ./$$t; done

#
# --- Generate the test scripts ---
#

$(BUILD)tests/ldag/forests: tests/ldag/forests.c $(BUILD)libldag.a
	@mkdir -p "$(BUILD)tests/ldag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/ldag/forests.c -lldag -lgmp

$(BUILD)tests/ldag/small_cases: tests/ldag/small_cases.c $(BUILD)libldag.a
	@mkdir -p "$(BUILD)tests/ldag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/ldag/small_cases.c -lldag -lgmp

$(BUILD)tests/ldag/unary_binary: tests/ldag/unary_binary.c $(BUILD)libldag.a
	@mkdir -p "$(BUILD)tests/ldag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/ldag/unary_binary.c -lldag -lgmp
