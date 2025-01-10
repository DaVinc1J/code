#include <stdio.h>
#include <stdint.h>

typedef struct {
	int16_t v[2];
} words;

void fuckery(int32_t *pv, words *pw) {
	
	int i = 0;

	for (; i < 5; i++) {
		(*pv)++;

		printf("%x, %x-%x\n", *pv, pw->v[1], pw->v[0]);

	}

}

int main(int argc, char **argv) {
	int32_t v = 0x12345678;

	words *pw = (words *)&v;

	fuckery(&v, pw);

}
