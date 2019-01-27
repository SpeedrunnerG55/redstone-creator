#include <stdio.h>

int shortDelay = 1000;
int longDelay = 1000;

void wait(int delay, FILE *fPtr){
  fprintf(fPtr,"wait(%ims);\n",delay);
}

void shift(int amount,char direction, FILE *fPtr){
  fprintf(fPtr,"echo(//shift %i %c);\n",amount,direction);
  wait(shortDelay,fPtr);
}

void overlay(char* type,FILE *fPtr){
  fprintf(fPtr,"echo(//overlay %s);\n",type);
  wait(longDelay,fPtr);
}

int main() {

  int permLength = 48;

  FILE *fPtr;
  fPtr = fopen("overlay.txt","w");

  fprintf(fPtr,"$${\n");

  for(int i = 0; i < permLength; i++){
    overlay("redstone",fPtr);
    shift(2,'f',fPtr);
  }

  fprintf(fPtr,"}$$\n");

  printf("done\n");

  fclose(fPtr);

  return 0;
}
