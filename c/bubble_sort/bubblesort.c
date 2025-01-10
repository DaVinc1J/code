/*
Flow/Functions/Pointers
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void summonEvilManThatSpeaksNothingButTruth(bool summon) {
  if (summon) {
    printf("\n ** Bubble Sort ** \n┌П┐(►˛◄ ’!)\n\n");
  }
}

int main(void) {
  
  bool summon = true;

  summonEvilManThatSpeaksNothingButTruth(summon);

  char list[4] = {3, 2, 1, 0};

  const int length = sizeof(list) / sizeof(list[0]);

  for (int n = length; n > 0; n--) {
    for (int i = 0; i < length - 1; i++) {
      if (list[i] > list[i + 1]) {
        int temp = list[i];

        list[i] = list[i+1];

        list[i+1] = temp;
      }
    }
  }

  for (int i = 0; i < length; i++) {
    printf("%d\n", list[i]);
  }

  return 0;

}
