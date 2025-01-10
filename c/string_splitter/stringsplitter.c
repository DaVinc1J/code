/*
String Splitter Function, first step to making a full hill cypher program !!!!!
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void summonEvilManThatSpeaksNothingButTruth(bool summon) {
  if (summon) {
    printf("\n ** String Splitter ** \n┌П┐(►˛◄ ’!)\n\n");
  }
}

int main(void) {
  
  bool summon = true;
  
  //yes, a seperate function was very much needed for this
  summonEvilManThatSpeaksNothingButTruth(summon);
  
  // counters
  int i, row, clm;

  // matrix/string data
  const int matrixSize = 4;
  char inputString[] = "yippeeee!!hereis a longer string! :D";

  // useful calculated stuff with CONSTANTS mmmm const 
  const int sizeOfString = (sizeof(inputString) / sizeof(char)) - 1;
  const int repeats = (sizeOfString + (matrixSize * matrixSize) - 1) / (matrixSize * matrixSize); /* funny comment fr */
  
  // initialise matrix to 0 to avoid any potential data issues
  // even tho we are memsetting, why not
  int splitMatrix[matrixSize][matrixSize] = {0};

  // counter! could calculate this but thats slower
  int index = 0;

  // for loop, per matrix
  for (int i = 0; i < repeats; i++) {

    // fill in the matrix
    // for loop, per row
    for (row = 0; row < matrixSize; row++) {

      // for loop, per column
      // this will run through every value in the matrix
      for (clm = 0; clm < matrixSize; clm++) {

        // convert char to int
        // if reaching past the string then fill with zeros
        
        if (index < sizeOfString) {
          splitMatrix[row][clm] = inputString[index++];
        } else {
          splitMatrix[row][clm] = 0;
        }
      }
    }



    // print matrices for debugging
    printf("matrix %d:\n", i + 1);
    for (int row = 0; row < matrixSize; row++) {
      for (int clm = 0; clm < matrixSize; clm++) {
        printf("%d ", splitMatrix[row][clm]);
      }
      printf("\n");
    }
    printf("\n");
  }

  // here is where the matrix would get exported or modified
  // but we aren't doing that rn so instead we...
  // set the entire matrix back to zero again
  memset(splitMatrix, 0, sizeof(splitMatrix));
  
  // why return 0?
  // hehehehe >:)
  return 0;
}
