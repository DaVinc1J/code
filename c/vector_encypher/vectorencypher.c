/*
Vector Encypher Function, second step to making a full hill cypher program !!!!!
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void summonEvilManThatSpeaksNothingButTruth(bool summon) {
  if (summon) {
    printf("\n ** Vector Encypher ** \n┌П┐(►˛◄ ’!)\n\n");
  }
}

int main(void) {
  
  bool summon = true;
  
  //yes, a seperate function was very much needed for this
  summonEvilManThatSpeaksNothingButTruth(summon);
  
  // counters
  int i, j;

  const int matrixSize = 3;

  // matrix/string data
  int asciiValues[] = {121, 105, 112};

  int vectorKey[3][3] = {
    {1, 2, 3},
    {2, 3, 4},
    {2, 1, 5}
  };
  
  int cypher[3] = {0};

  for (i = 0; i < matrixSize; i++) {

    cypher[i] = 0;

    for (j = 0; j < matrixSize; j++) {
      cypher[i] += vectorKey[i][j] * asciiValues[j];
    }

    cypher[i] %= 128;

  }

  printf("Resulting vector:\n");
  for (int i = 0; i < 3; i++) {
    printf("%d ", cypher[i]);
  }
  printf("\n");

  return 0;

}
