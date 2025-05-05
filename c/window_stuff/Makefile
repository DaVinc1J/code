CC = clang
CFLAGS_MAIN = -g -std=c11 -ltcmalloc
LDFLAGS_MAIN = -lSDL2 -lSDL2_ttf -lGL -lGLU -lc -nostdlib++ -lm

CFLAGS_TEST = -g -std=c11 -ltcmalloc
LDFLAGS_TEST = -lvulkan -lc -lm

MAIN_FILES = fonts.c main.c ui.c
TEST_FILES = vulkan_fonts.c vulkan_main.c vulkan_ui.c
MAIN_OUTPUT = main
TEST_OUTPUT = test

.PHONY: all main test clean clean_main clean_test

all: main

main: $(MAIN_FILES)
	$(CC) $(CFLAGS_MAIN) $(LDFLAGS_MAIN) -o $(MAIN_OUTPUT) $^
	./$(MAIN_OUTPUT)

test: $(TEST_FILES)
	$(CC) $(CFLAGS_TEST) $(LDFLAGS_TEST) -o $(TEST_OUTPUT) $^
	./$(TEST_OUTPUT)

clean: clean_main clean_test

clean_main:
	rm -f $(MAIN_OUTPUT)

clean_test:
	rm -f $(TEST_OUTPUT)
