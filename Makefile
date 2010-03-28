SRC = ./src
TEST = ./tests
BUILD_ROOT = ./build
CHECK = /usr/local/lib/libcheck.a

checkstatefulstring: $(SRC)/statefulstring.h $(SRC)/statefulstring.c $(TEST)/check_statefulstring.c
	@clang -o $(BUILD_ROOT)/checkstatefulstring $(SRC)/statefulstring.c $(TEST)/check_statefulstring.c $(CHECK)
	@$(BUILD_ROOT)/checkstatefulstring

check: checkstatefulstring
