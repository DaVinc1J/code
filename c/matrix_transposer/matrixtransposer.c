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
    printf("\n ** Matrix Transposer ** \n┌П┐(►˛◄ ’!)\n\n");
  }
}

int main(void) {
  
  bool summon = true;

  summonEvilManThatSpeaksNothingButTruth(summon);

  int rw;
  int clm;

  const int row = 5;
  const int column = 4;

  int matrix[row][column] = {
    {1, 2, 3, 4 },
    {5, 6, 7, 8 },
    {9, 10, 11, 12},
    {13, 14, 15, 16},
    {17, 18, 19, 20}
  };
  
  clm = sizeof(matrix[0]) / sizeof(matrix[0][0]);

  rw = sizeof(matrix) / sizeof(matrix[0]);

  int matrixT[clm][rw];

  for (int r = 0; r < rw; r++) {
    for (int c = 0; c < clm; c++) {
      *(*(matrixT + c) + r) = *(*(matrix + r) + c);
    }
  }
  
  printf("Original Matrix\n");
  for (int r = 0; r < row; r++) {
    for (int c = 0; c < column; c++) {
      printf("%d ", *(*(matrix + r) + c));
    }
    printf("\n");
  }

  printf("\nTransposed Matrix\n");
  for (int r = 0; r < clm; r++) {
    for (int c = 0; c < rw; c++) {
      printf("%d ", *(*(matrixT + r) + c));
    }
    printf("\n");
  }

  return 0;

}
