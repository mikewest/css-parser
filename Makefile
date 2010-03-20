
CFILES = main.c logging.c token.c tokenizer.c

tokenize:	$(CFILES)
	clang -o ./tokenize $(CFILES)
