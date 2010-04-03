SRC = ./src
TEST = ./tests
BUILD_ROOT = ./build
CHECK = /usr/local/lib/libcheck.a
CC = clang
STATEFULSTRING_SRC = $(SRC)/statefulstring
TOKEN_SRC = $(SRC)/token
TOKENIZER_SRC = $(SRC)/csstokenizer

all: clean check

clean:
	rm -f $(BUILD_ROOT)/*

statefulstring.out: $(STATEFULSTRING_SRC).h $(STATEFULSTRING_SRC).c
token.out: $(TOKEN_SRC).h $(TOKEN_SRC).c
tokenizer.out: $(TOKENIZER_SRC).h $(TOKENIZER_SRC).c

checkstatefulstring: statefulstring.out $(TEST)/check_statefulstring.c
	@$(CC) -o $(BUILD_ROOT)/checkstatefulstring $(STATEFULSTRING_SRC).c $(TEST)/check_statefulstring.c $(CHECK)
	@$(BUILD_ROOT)/checkstatefulstring

checktoken: statefulstring.out token.out $(TEST)/check_token.c
	@$(CC) -o $(BUILD_ROOT)/checktoken $(STATEFULSTRING_SRC).c $(TOKEN_SRC).c $(TEST)/check_token.c $(CHECK)
	@$(BUILD_ROOT)/checktoken

checktokenizer: statefulstring.out token.out tokenizer.out $(TEST)/check_tokenizer.c
	@$(CC) -o $(BUILD_ROOT)/checktokenizer $(STATEFULSTRING_SRC).c $(TOKEN_SRC).c $(TOKENIZER_SRC).c $(SRC)/tokenizer_charactertests.c $(TEST)/check_tokenizer.c $(CHECK)
	@$(BUILD_ROOT)/checktokenizer

scan:
	~/Downloads/checker-238/scan-build --experimental-checks -o ./reports -V gcc -o $(BUILD_ROOT)/checktokenizer $(STATEFULSTRING_SRC).c $(TOKEN_SRC).c $(TOKENIZER_SRC).c $(SRC)/tokenizer_charactertests.c $(TEST)/check_tokenizer.c $(CHECK)

analyze: clean
	@$(CC) --analyze $(STATEFULSTRING_SRC).c $(TOKEN_SRC).c $(TOKENIZER_SRC).c $(SRC)/tokenizer_charactertests.c $(TEST)/check_tokenizer.c $(CHECK)
	@rm -rf ./*.plist

check: checkstatefulstring checktoken checktokenizer
