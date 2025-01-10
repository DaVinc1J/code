#include <stdio.h>
#include <unistd.h>


int main (int argc, char **argv) {

	const char *spinner = "/-\\|/-\\|";
	const char *dots[] = {".  ", ".  ", ".. ", ".. ", "...", "..."};

	for (int i = 0; i < 20; i++) {
		printf("\r%c | Loading %s", spinner[i % 8], dots[i % 6]);
		fflush(stdout);
		usleep(150000);
	}

	printf("\r%d\n", '');

	return 0;

}
