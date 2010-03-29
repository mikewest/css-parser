SRC = ./src
TEST = ./tests
BUILD_ROOT = ./build
CHECK = /usr/local/lib/libcheck.a

STATEFULSTRING_SRC = $(SRC)/statefulstring

CSSTOKEN_SRC = $(SRC)/csstoken


statefulstring.out: $(STATEFULSTRING_SRC).h $(STATEFULSTRING_SRC).c
csstoken.out: $(CSSTOKEN_SRC).h $(CSSTOKEN_SRC).c

checkstatefulstring: statefulstring.out $(TEST)/check_statefulstring.c
	@clang -o $(BUILD_ROOT)/checkstatefulstring $(STATEFULSTRING_SRC).c $(TEST)/check_statefulstring.c $(CHECK)
	@$(BUILD_ROOT)/checkstatefulstring

checkcsstoken: statefulstring.out csstoken.out $(TEST)/check_csstoken.c
	@clang -o $(BUILD_ROOT)/checkcsstoken $(STATEFULSTRING_SRC).c $(CSSTOKEN_SRC).c $(TEST)/check_csstoken.c $(CHECK)
	@$(BUILD_ROOT)/checkcsstoken

check: checkstatefulstring checkcsstoken
