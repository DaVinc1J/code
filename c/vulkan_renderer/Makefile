CC = clang

CFLAGS = -g
LDFLAGS = -lvulkan -lm -lglfw -ltcmalloc

FILES = main.c renderer.c window.c context.c utilities.c application.c
OUTPUT = main

.PHONY: all main clean

all: main

main: $(FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) $^
	./$(OUTPUT)

clean: clean_main

clean_main:
	rm -f $(OUTPUT)
