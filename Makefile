SRC = ./src
TEST = ./tests
BUILD_ROOT = ./build
CHECK = /usr/local/lib/libcheck.a
#CC = gcc -std=c99
CC  = clang
CFLAGS = -g 

STATEFULSTRING_OBJECTS = $(SRC)/statefulstring.o
TOKEN_OBJECTS = $(STATEFULSTRING_OBJECTS) $(SRC)/token.o
TOKENIZER_OBJECTS = $(TOKEN_OBJECTS) $(SRC)/tokenizer.o $(SRC)/tokenizer_charactertests.o $(SRC)/tokenizer_statefulstringtests.o

.PHONY: all clean check

all: clean check

clean:
	@find . -name '*.o' | xargs rm -f
	@rm -rf $(BUILD_ROOT)/*

check: checkstatefulstring checktoken checktokenizer
	@echo "-------------------------------------"
	@echo "Finished compiling, now running tests"
	@echo "-------------------------------------"
	@$(BUILD_ROOT)/checkstatefulstring
	@$(BUILD_ROOT)/checktoken
	@$(BUILD_ROOT)/checktokenizer

checkstatefulstring: $(STATEFULSTRING_OBJECTS) $(TEST)/check_statefulstring.o
	@$(CC) $(CFLAGS) $(STATEFULSTRING_OBJECTS) $(TEST)/check_statefulstring.o $(CHECK) -o $(BUILD_ROOT)/$(@)

checktoken: $(TOKEN_OBJECTS) $(TEST)/check_token.o
	@$(CC) $(CFLAGS) $(TOKEN_OBJECTS) $(TEST)/check_token.o $(CHECK) -o $(BUILD_ROOT)/$(@)

checktokenizer: $(TOKENIZER_OBJECTS) $(TEST)/check_tokenizer.o
	@$(CC) $(CFLAGS) $(TOKENIZER_OBJECTS) $(TEST)/check_tokenizer.o $(CHECK) -o $(BUILD_ROOT)/$(@)

analyze: clean
	@find . -name '*.c' | xargs $(CC) --analyze
	@rm -rf ./*.plist

