SOURCE_ROOT = ./src
BUILD_ROOT = ./build
CFILES = $(SOURCE_ROOT)/main.c \
		 $(SOURCE_ROOT)/logging.c \
		 $(SOURCE_ROOT)/token.c \
		 $(SOURCE_ROOT)/tokenizer.c \
		 $(SOURCE_ROOT)/tokenizer_helpers.c \
		 $(SOURCE_ROOT)/tokenizer_charactertests.c

all:	analyze tokenize

tokenize:	$(CFILES)
	clang -o $(BUILD_ROOT)/tokenize $(CFILES)


analyze:	$(CFILES)
	clang --analyze $(CFILES)
