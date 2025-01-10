#include <stdio.h>

typedef struct {
	char a: 3;
	int b: 16;
} foo;

int main(int argc, char **argv) {

	foo x = {
		.a = 0x3,
		.b = 0x7AAA,
	};

	unsigned char *p = (unsigned char *)&x;

	size_t i = 0;

	for (; i < sizeof x; i++) {
		printf("%02X\n", p[i]);
	}

	return 0;

}
