$(BUILD)common/graphs.o: src/common/graphs.c includes/common.h
	@mkdir -p "$(BUILD)/common"
	$(CC) $(CFLAGS) -o $@ -c src/common/graphs.c

$(BUILD)common/memo.o: src/common/memo.c includes/common.h
	@mkdir -p "$(BUILD)/common"
	$(CC) $(CFLAGS) -o $@ -c src/common/memo.c

$(BUILD)common/cli.o: src/common/cli.c includes/common.h
	@mkdir -p "$(BUILD)/common"
	$(CC) $(CFLAGS) -o $@ -c src/common/cli.c
