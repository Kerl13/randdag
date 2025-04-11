# Run all the tests
ldag-tests: $(BUILD)tests/ldag/counting
	$(BUILD)tests/ldag/counting

#
# --- Generate the test scripts ---
#

$(BUILD)tests/ldag/counting: tests/ldag/counting.c $(BUILD)libldag.a
	@mkdir -p "$(BUILD)tests/ldag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/ldag/counting.c -lldag -lgmp
