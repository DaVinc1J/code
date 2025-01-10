CC = clang
CFLAGS = -g -F/Library/Frameworks -std=c11
LDFLAGS = -framework SDL2 -framework SDL2_ttf -Wl,-rpath,/Library/Frameworks -lc -nostdlib++ -lobjc

FILES = fonts.c main.c ui.c
OUTPUT = test1
ENTITLEMENTS = entitlements.plist

all: $(OUTPUT)
	./$(OUTPUT)

$(OUTPUT): $(FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@#
	codesign --force --sign - --entitlements $(ENTITLEMENTS) $@

clean:
	rm -f $(OUTPUT)

.PHONY: all clean
