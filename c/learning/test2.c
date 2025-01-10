/*
Flow/Functions/Pointers
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void summonEvilManThatSpeaksNothingButTruth(bool summon) {
  if (summon) {
    printf("\n ** yuh ** \n┌П┐(►˛◄ ’!)\n\n");
  }
}

int main(void) {
  
  bool summon = true;

  summonEvilManThatSpeaksNothingButTruth(summon);

  uint8_t yes = 255;

  printf ("%zu\n", sizeof(yes));

}
