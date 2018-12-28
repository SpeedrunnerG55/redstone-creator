#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct vector{
  int xCmp;
  int yCmp;
};

struct point{
  int x;
  int y;
};

//used to get the vector from one point to another
struct vector getPointVector(struct point a, struct point b){
  struct vector ret;
  ret.xCmp = b.x - a.x;
  ret.yCmp = b.y - a.y;
  return ret;
}

//returns the distance of a vector using pythagrians theorm
float getDistance(struct vector arg){
  return sqrt((arg.xCmp * arg.xCmp) + (arg.yCmp * arg.yCmp));
}

int main(){

  int height = 14;
  int width = 14;

  int x1 = -4;
  int y1 = -12;
  // int z1 = 1;

  int x2 = -12;
  int y2 = -4;
  // int z2;

  struct point a;
  a.x = x1;
  a.y = y1;

  struct point b;
  b.x = x2;
  b.y = y2;

  struct vector aTob = getPointVector(a,b);

  for(int i = height - 1; i > -height; i--){
    for(int j = -width + 1; j < width; j++){

      struct point c;
      c.x = j;
      c.y = i;

      struct vector aToc = getPointVector(a,c);

      struct vector cTob = getPointVector(c,b);

      _Bool condition = getDistance(aToc) + getDistance(cTob) == getDistance(aTob);

      if(condition > 0){
        printf("%i",condition);
      }
      else if(i == 0 && j == 0){
        printf("+");
      }
      else if(i == 0){
        printf("-");
      }
      else if(j == 0){
        printf("|");
      }
      else{
        printf(" ");
      }
    }
    printf("\n");
  }

  return 0;
}
