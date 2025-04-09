# Run all the tests
doag-tests: $(BUILD)tests/doag/counting
	$(BUILD)tests/doag/counting

#
# --- Generate the test scripts ---
#

$(BUILD)tests/doag/counting: tests/doag/counting.c $(BUILD)libdoag.a
	@mkdir -p "$(BUILD)tests/doag"
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/doag/counting.c -ldoag -lgmp
