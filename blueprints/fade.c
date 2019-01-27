#include <stdio.h>

int R = 0xFF0000;
int G = 0x00FF00;
int B = 0x0000FF;

void tint(int *A, int C){
  *A = (*A & ~R) | (((*A & R) + C) & R);
  *A = (*A & ~G) | (((*A & G) + C) & G);
  *A = (*A & ~B) | (((*A & B) + C) & B);
}

void printPixel(int collor){
  unsigned char pixel[3];
  pixel[0] = (collor & R) >> 16;
  pixel[1] = (collor & G) >> 8;
  pixel[2] = (collor & B);
  printf("%X %X %X \n",pixel[0],pixel[1],pixel[2]);
}

void darken(int part,int* whole,float scaler){
  // remove old part then get new part
  *whole = (*whole & ~part) | (((int)((*whole & part) * (scaler / 2)) & part) + (((*whole & part)/ 6) & part));
}

int main(){
  int length = 50;
  for(int i = length; i >= 0; i--){
    int collor = 0xEE0050;
    float scaler = (float)(length - i)/ length;
    printf("scaler :%f ",scaler);
    darken(R,&collor,scaler);
    darken(G,&collor,scaler);
    darken(B,&collor,scaler);
    printPixel(collor);
  }

  return 0;
}
