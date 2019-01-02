#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tables.h"


/*
more of a quality of life
feature to keep track of
my selection between functions
*/

char * script = NULL;
int scriptLength = 0;

//counters to esimate execution time
unsigned int waits = 0;
unsigned int longwaits = 0;

void wait(){
  waits++;
}

void printFileBuffer(char * arg){
  FILE * mPtr = fopen("script.txt","w");
  fprintf(mPtr,"$${\n");

  fprintf(mPtr,"%s\n",arg);
  free(arg);

  fprintf(mPtr,"UNSET(@done);\n");
  fprintf(mPtr,"}$$\n");
  fclose(mPtr);

  printf("done\n estimated execution time: %i \n",longwaits + waits);
}

short commandBuffer = 50;

void toScript(char* message){
  _Bool printing = 0;
  if(printing){
    short length = strlen(message);
    scriptLength += length;
    script = realloc(script,scriptLength + 1);
    strncat(script,message,length);
  }
}

void comment(char * message){
  char comment[80] = "// ";
  strncat(comment,message,80);
  strncat(comment," \n",80);
  toScript(comment);
}

char *blockTable[] ={
  "air",
  "observor",
  "redstone",
  "redstone_block",
  "redstone_lamp",
  "redstone_repeater",
  "redstone_torch",
  "slab",
  "sticky_piston",
  "wood",
  "wool",
  "ice"
};

unsigned char blockLookup(char* type){

  size_t numElements = sizeof(blockTable) / sizeof(blockTable[0]);
  for(size_t i = 0; i < numElements; i++){
    if(!strncmp(type,blockTable[i],strlen(blockTable[i])) && strlen(type) == strlen(blockTable[i])){
      return i;
    }
  }
  //error not in table
  printf("what is %s?\n",type);
  return -1;
}

char* reverseBlockLookup(unsigned char type){
  unsigned char numElements = sizeof(blockTable) / sizeof(blockTable[0]);
  if(type < numElements)
  return blockTable[type];
  else{
    //error not in table
    printf("what is %i?\n",type);
    return "NULL"; //lol
  }
}

_Bool mask(unsigned char type){
  _Bool mask[] = {0,0,0,0,0,0,0,0,0,0,0};
  return mask[type];
}

unsigned int collerLookup(char type, int value){

  //minecraft wool RGB values
  int woolMap[16] = {
    0xE9ECEC,0xF07613,0xBD44B3,0x3AAFD9,
    0xF8C627,0x70B919,0xED8DAC,0x3E4447,
    0x8E8E86,0x158991,0x792AAC,0x35399D,
    0x724728,0x546D1B,0xA12722,0x141519
  };

  int woodmap[4] = {
    0xc15000,0x397a03,0x02367f,0x89057a
  };

  //other materials
  int redstone = 0x560000;
  int redstone_torch = 0xdb754c;
  int redstone_block = 0xbf0118;
  int redstone_repeater = 0xa8a8a8;
  int redstone_lamp = 0x751d0b;
  int sticky_piston = 0x5e7a09;
  int observor = 0x6d6362;
  int slab = 0x8d9093;
  int ice = 0xa5c8ff;

  if(!mask(type)){
    if(type == blockLookup("wood")){
      return woodmap[value];
    }
    else if(type == blockLookup("wool")){
      return woolMap[value];
    }
    else if(type == blockLookup("redstone")){
      return redstone;
    }
    else if(type == blockLookup("redstone_block")){
      return redstone_block;
    }
    else if(type == blockLookup("redstone_repeater")){
      return redstone_repeater;
    }
    else if(type == blockLookup("redstone_torch")){
      return redstone_torch;
    }
    else if(type == blockLookup("sticky_piston")){
      return sticky_piston;
    }
    else if(type == blockLookup("redstone_lamp")){
      return redstone_lamp;
    }
    else if(type == blockLookup("observor")){
      return observor;
    }
    else if(type == blockLookup("slab")){
      return slab;
    }
    else if(type == blockLookup("ice")){
      return ice;
    }
  }
  return 0;
}

struct block{
  char type;
  unsigned char value;
};

// x z y
struct block ***map;

struct point{
  short x;
  short z;
  short y;
};

//Block map
//keep track of raw size for convienience
short mapW = 1,mapH = 1,mapD = 1;
//teh extent of the canvas as a function of 2 of its corners
short mapXA = 0,mapZA = 0,mapYA = 0,mapXB = 0,mapZB = 0,mapYB = 0;

//keep track of raw size for convienience
short selW = 1,selH = 1,selD = 1; //if i really have a selection larger than 255 i have a problem...
//pos1/pos2
short selXA = 0,selZA = 0,selYA = 0,selXB = 0,selZB = 0,selYB = 0;

//can be placed anywhere and wil show the selection
void showSelection(unsigned int counter){
  printf("SEL #%-3i H:%-5iW:%-5iD:%-5i A X:%-5iY:%-5iZ:%-5i B X:%-5iY:%-5iZ:%-5i\n",counter,selH,selW,selD,selXA,selZA,selYA,selXB,selZB,selYB);
}

//can be placed anywhere and wil show the selection
void showMap(unsigned int counter){
  printf("MAP #%-3i H:%-5iW:%-5iD:%-5i A X:%-5iY:%-5iZ:%-5i B X:%-5iY:%-5iZ:%-5i\n",counter,mapH,mapW,mapD,mapXA,mapZA,mapYA,mapXB,mapZB,mapYB);
}

_Bool outofBounds(){
  return(selXA < mapXA || selZA < mapZA || selYA < mapYA || selXB > mapXB || selZB > mapZB || selYB > mapYB);
}

short* getSelExtent(char Dir){
  switch (Dir) {
    case 'l': if(selXA > selXB) return &selXA; else return &selXB; break;
    case 'r': if(selXA < selXB) return &selXA; else return &selXB; break;
    case 'u': if(selZA > selZB) return &selZA; else return &selZB; break;
    case 'd': if(selZA < selZB) return &selZA; else return &selZB; break;
    case 'f': if(selYA > selYB) return &selYA; else return &selYB; break;
    case 'b': if(selYA < selYB) return &selYA; else return &selYB; break;
  }
  return NULL; //error
}

short* getMapExtent(char Dir){
  switch (Dir) {
    case 'l': if(mapXA > mapXB) return &mapXA; else return &mapXB; break;
    case 'r': if(mapXB < mapXA) return &mapXB; else return &mapXA; break;
    case 'u': if(mapZA > mapZB) return &mapZA; else return &mapZB; break;
    case 'd': if(mapZB < mapZA) return &mapZB; else return &mapZA; break;
    case 'f': if(mapYA > mapYB) return &mapYA; else return &mapYB; break;
    case 'b': if(mapYB < mapYA) return &mapYB; else return &mapYA; break;
  }
  return NULL; //error
}

struct extents{
  short l;
  short r;
  short u;
  short d;
  short f;
  short b;
};

struct extents getAllSelExtents(){
  struct extents ret;
  ret.l = (selXA > selXB? selXA:selXB);
  ret.r = (selXA < selXB? selXA:selXB);
  ret.u = (selZA > selZB? selZA:selZB);
  ret.d = (selZA < selZB? selZA:selZB);
  ret.f = (selYA > selYB? selYA:selYB);
  ret.b = (selYA < selYB? selYA:selYB);
  return ret;
}

struct extents getAllMapExtents(){
  struct extents ret;
  ret.l = (mapXA > mapXB? mapXA:mapXB);
  ret.r = (mapXA < mapXB? mapXA:mapXB);
  ret.u = (mapZA > mapZB? mapZA:mapZB);
  ret.d = (mapZA < mapZB? mapZA:mapZB);
  ret.f = (mapYA > mapYB? mapYA:mapYB);
  ret.b = (mapYA < mapYB? mapYA:mapYB);
  return ret;
}

//RGB masks
int R = 0xFF0000;
int G = 0x00FF00;
int B = 0x0000FF;

void tint(unsigned int *A, unsigned int C){
  *A = (*A & ~R) | (((*A & R) + C) & R);
  *A = (*A & ~G) | (((*A & G) + C) & G);
  *A = (*A & ~B) | (((*A & B) + C) & B);
}

void printPixel(unsigned int collor,FILE * _Ptr){
  int pixel[3];
  pixel[0] = (collor & R) >> 16;
  pixel[1] = (collor & G) >> 8;
  pixel[2] = (collor & B);
  fprintf(_Ptr,"%i %i %i ",pixel[0],pixel[1],pixel[2]);
}

void darken(unsigned int* whole,float scaler){
  // remove old part then get new part ... getting the new part was hard...
  *whole = (*whole & ~R) | (((int)((*whole & R) * (scaler / 2)) & R) + (((*whole & R)/3) & R));
  *whole = (*whole & ~G) | (((int)((*whole & G) * (scaler / 2)) & G) + (((*whole & G)/3) & G));
  *whole = (*whole & ~B) | (((int)((*whole & B) * (scaler / 2)) & B) + (((*whole & B)/3) & B));
}

_Bool inSel(short x,short z,short y){
  struct extents sel = getAllSelExtents();
  return (x <= sel.l && x >= sel.r && z <= sel.u && z >= sel.d && y <= sel.f && y >= sel.b);
}

void buildImmages(){

  unsigned int selCollor = 0xDBDADC;

  unsigned int backround = 0x201020;

  printf("%i %i %i\n",mapW,mapH,mapD);

  FILE * Top = fopen("topView.pnm","w");
  fprintf(Top,"P3\n");
  fprintf(Top,"%i %i\n",mapW,mapD);
  printf("%i %i\n",mapW,mapD);
  fprintf(Top,"255\n");

  for(short y = mapD - 1; y >=0; y--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int collor = backround;
      for(short z = 0; z < mapH; z++){
        unsigned int blockCollor = collerLookup(map[x][z][y].type,map[x][z][y].value);
        if(blockCollor > 0){
          collor = blockCollor;
          float scaler = (float)z / (mapH);
          darken(&collor,scaler);
        }
      }
      if(inSel(x,selZA,y)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Top);
    }
    fprintf(Top,"\n");
  }
  fclose(Top);

  FILE * Front = fopen("frontview.pnm","w");
  fprintf(Front,"P3\n");
  fprintf(Front,"%i %i\n",mapW,mapH);
  printf("%i %i\n",mapW,mapH);
  fprintf(Front,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int  collor = backround;
      for(short y = mapD - 1; y >= 0; y--){
        int blockCollor = collerLookup(map[x][z][y].type,map[x][z][y].value);
        if(blockCollor != 0){
          collor = blockCollor;
          float scaler = (float)(mapD - y) / mapD;
          darken(&collor,scaler);
        }
      }
      if(inSel(x,z,selYA)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Front);
    }
    fprintf(Front,"\n");
  }
  fclose(Front);

  FILE * Left = fopen("leftview.pnm","w");
  fprintf(Left,"P3\n");
  fprintf(Left,"%i %i\n",mapD,mapH);
  printf("%i %i\n",mapD,mapH);
  fprintf(Left,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short y = mapD - 1; y >= 0; y--){
      unsigned int  collor = backround;
      for(short x = 0; x < mapW; x++){
        unsigned int blockCollor = collerLookup(map[x][z][y].type,map[x][z][y].value);
        if(blockCollor != 0){
          collor = blockCollor;
          float scaler = (float)(mapW - x) / mapW;
          darken(&collor,scaler);
        }
      }
      if(inSel(selXA,z,y)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Left);
    }
    fprintf(Left,"\n");
  }
  fclose(Left);
}

void updateSelDementions(char Dir){
  if(Dir == 'u' || Dir == 'd')//if the direction is up or down the selection will get shorter
  selH = (*getSelExtent('u') - *getSelExtent('d')) + 1;
  else if(Dir == 'l' || Dir == 'r')//if the direction is left or right the selection will get thinner
  selW = (*getSelExtent('l') - *getSelExtent('r')) + 1;
  else if(Dir == 'f' || Dir == 'b')//if the direction is forward or backward the selection will get shallower
  selD = (*getSelExtent('f') - *getSelExtent('b')) + 1;
}

//allocate more memory
void expandBlockMap(unsigned short change, char Dir){

  if(mapW > 500 || mapH > 500 || mapD > 500){
    printf("WARNING LARGE MAP!\n");
    buildImmages();
    getchar();
    showMap(0);
  }

  if(Dir == 'l' || Dir == 'u' || Dir == 'f'){
    *getMapExtent(Dir) += change; // twards positive numbers get bigger
  }
  if(Dir == 'r' || Dir == 'd' || Dir == 'b'){
    *getMapExtent(Dir) -= change; // twards negitive numbers get smaller
  }

  unsigned short tempW = mapW;
  unsigned short tempH = mapH;
  unsigned short tempD = mapD;

  //calculate the highth the selW and the breadth...
  if(Dir == 'r' || Dir == 'l')//if the direction is left or right the map will get thinner
  mapW = abs(mapXB - mapXA) + 1;
  else if(Dir == 'u' || Dir == 'd')//if the direction is up or down the map will get shorter
  mapH = abs(mapZB - mapZA) + 1;
  else if(Dir == 'f' || Dir == 'b')//if the direction is forward or backward the map will get shallower
  mapD = abs(mapYB - mapYA) + 1;

  //memory allocation
  if(Dir == 'r' || Dir == 'l'){
    map = realloc(map,mapW * sizeof(struct block**));
    for(short i = tempW; i < mapW; i++){
      map[i] = malloc(mapH * sizeof(struct block*));
      for(short j = 0; j < mapH; j++){
        map[i][j] = malloc(mapD * sizeof(struct block));
        for(short k = 0; k < mapD; k++){
          map[i][j][k].type = 0;
          map[i][j][k].value = 0;
        }
      }
    }
    //shift data
    if(Dir == 'r'){
      unsigned short shiftAmt = mapW - tempW;
      for(short i = mapW - 1; i >= shiftAmt; i--){
        map[i] = map[i-shiftAmt];
      }
      //reallocate new empty space (unshifted portion)
      for(short i = shiftAmt - 1; i >= 0; i--){
        map[i] = malloc(mapH * sizeof(struct block*));
        for(short j = 0; j < mapH; j++){
          map[i][j] = malloc(mapD * sizeof(struct block));
          for(short k = 0; k < mapD; k++){
            map[i][j][k].type = 0;
            map[i][j][k].value = 0;
          }
        }
      }
    }
    if(Dir == 'r'){
      //adjust selection for shift
      selXA += change;
      selXB += change;
      mapXA += change;
      mapXB += change;
    }
  }

  else if(Dir == 'u' || Dir == 'd'){
    for(short i = 0; i < mapW; i++){
      map[i] = realloc(map[i],mapH * sizeof(struct block*));
      for(short j = tempH; j < mapH; j++){
        map[i][j] = malloc(mapD * sizeof(struct block));
        for(short k = 0; k < mapD; k++){
          map[i][j][k].type = 0;
          map[i][j][k].value = 0;
        }
      }
      //shift data
      if(Dir == 'd'){
        unsigned short shiftAmt = mapH - tempH;
        for(short j = mapH - 1; j >= shiftAmt; j--){
          map[i][j] = map[i][j-shiftAmt];
        }
        //reallocate new empty space (unshifted portion)
        for(short j = shiftAmt - 1; j >= 0; j--){
          map[i][j] = malloc(mapD * sizeof(struct block));
          for(short k = 0; k < mapD; k++){
            map[i][j][k].type = 0;
            map[i][j][k].value = 0;
          }
        }
      }
    }
    if(Dir == 'd'){
      //adjust selection for shift
      selZA += change;
      selZB += change;
      mapZA += change;
      mapZB += change;
    }
  }

  else if(Dir == 'f' || Dir == 'b'){
    for(short i = 0; i < mapW; i++){
      for(short j = 0; j < mapH; j++){
        map[i][j] = realloc(map[i][j],mapD * sizeof(struct block));
        for(short k = tempD; k < mapD; k++){
          map[i][j][k].type = 0;
          map[i][j][k].value = 0;
        }
        //shift data
        if(Dir == 'b'){
          unsigned short shiftAmt = mapD - tempD;
          for(short k = mapD - 1; k >= shiftAmt; k--){
            map[i][j][k].type = map[i][j][k-shiftAmt].type;
            map[i][j][k].value = map[i][j][k-shiftAmt].value;
          }
          //0 the data unshifted
          for(short k = shiftAmt - 1; k >= 0; k--){
            map[i][j][k].type = 0;
            map[i][j][k].value = 0;
          }
        }
      }
    }
  }
  if(Dir == 'b'){
    //adjust selection for shift
    selYA += change;
    selYB += change;
    mapYA += change;
    mapYB += change;
  }
}

void checkAndExpand(unsigned short distance,char Dir){

  //test a shifted cuboid to //moves, stacks and annything you want to check at a distance
  //if distacne is 0 the test is the selection ;)
  short selPos = *getSelExtent(Dir) + distance; // twards positive numbers
  short selNeg = *getSelExtent(Dir) - distance; // twards negitive numbers

  short getmapExtent = *getMapExtent(Dir);

  if(Dir == 'l' || Dir == 'u' || Dir == 'f'){
    if(selPos > getmapExtent){
      expandBlockMap(selPos - getmapExtent,Dir);
    }
  }
  if(Dir == 'r' || Dir == 'd' || Dir == 'b'){
    if(selNeg < getmapExtent){
      expandBlockMap(getmapExtent - selNeg,Dir);
    }
  }
}



void waitUntlDone(){
  char *command = "UNSET(@done); log(waiting for done); DO; WAIT(20ms); UNTIL(@done); log(Done!); \n";
  toScript(command);
  longwaits++;
}

void replace(char* typeA, _Bool specificA,char valueA, char* typeB,_Bool specificB, char valueB){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//replace %s",typeA);
  toScript(command);
  if(specificA){
    snprintf(command,commandBuffer,":%i",valueA);
    toScript(command);
  }
  snprintf(command,commandBuffer," %s",typeB);
  toScript(command);
  if(specificB){
    snprintf(command,commandBuffer,":%i",valueB);
    toScript(command);
  }
  toScript(");\n");
  waitUntlDone();

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){

        if(blockLookup(typeA) == map[i][j][k].type){
          struct block B;
          B.type = blockLookup(typeB);
          if(specificB)
          B.value = valueB;
          else
          B.value = 0;
          if((specificA && valueA == map[i][j][k].value) || !specificA){
            map[i][j][k] = B;
          }
        }
      }
    }
  }
}

//commands i can do
void setBlock(char* type, unsigned char value, _Bool wait){
  //in game
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//set %s:%i);\n",type,value);
  toScript(command);
  if(wait)
  waitUntlDone();
  //in memory
  struct extents selExt = getAllSelExtents();

  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){
        struct block B;
        B.type = blockLookup(type);
        B.value = value;

        map[i][j][k] = B;
      }
    }
  }
}

void overlay(char* type, unsigned char value){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//overlay %s:%i);\n",type,value);
  toScript(command);

  checkAndExpand(1,'u');
  waitUntlDone();

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short i = selExt.r; i <= selExt.l; i++){
    for(short k = selExt.b; k <= selExt.f; k++){
      for(short j = selExt.u; j >= selExt.d; j--){ //height last

        //get block
        struct block B;
        B.type = blockLookup(type);
        B.value = value;

        //only fill in air and the block under it is not air
        if(map[i][j][k].type != 0){
          map[i][j + 1][k] = B; //place block
          break; //only place 1 block per xz coordinate (only the top portion of the selected blocks)
        }
      }
    }
  }
}

//can be placed anywhere and wil shift the selection
void selectionShift(char Dir, unsigned short amount){
  //smol switch (relitivlely speaking)
  checkAndExpand(amount,Dir);
  switch (Dir) {
    case 'l': selXA += amount; selXB += amount; break;
    case 'r': selXA -= amount; selXB -= amount; break;
    case 'u': selZA += amount; selZB += amount; break;
    case 'd': selZA -= amount; selZB -= amount; break;
    case 'f': selYA += amount; selYB += amount; break;
    case 'b': selYA -= amount; selYB -= amount; break;
  }
}

void shift(unsigned short amount,char Dir){
  if(amount > 0){
    char command[commandBuffer];
    snprintf(command,commandBuffer,"echo(//shift %i %c);\n",amount,Dir);
    toScript(command);
    selectionShift(Dir,amount);
    wait();
  }
}

struct block*** copySel(){
  struct extents selExt = getAllSelExtents();
  struct block*** ret = malloc(selW * sizeof(struct block**));
  for(short i = selExt.r; i <= selExt.l; i++){
    ret[i-selExt.r] = malloc(selH * sizeof(struct block*));
    for(short j = selExt.d; j <= selExt.u; j++){
      ret[i-selExt.r][j-selExt.d] = malloc(selD * sizeof(struct block));
      for(short k = selExt.b; k <= selExt.f; k++){

        //get blocks, the right blocks and put them right in the right location
        ret[i-selExt.r][j-selExt.d][k-selExt.b].type = map[i][j][k].type;
        ret[i-selExt.r][j-selExt.d][k-selExt.b].value = map[i][j][k].value;

        //remove old block
        map[i][j][k].type = 0;
        map[i][j][k].value = 0;
      }
    }
  }
  return ret;
}

void pasteSel(struct block*** arg,short amount, char Dir){

  struct extents selExt = getAllSelExtents();

  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){

        //calculate new position of block. Keep unchanged coordinates the same with default values
        unsigned short blockBX = i;
        unsigned short blockBY = j;
        unsigned short blockBZ = k;

        switch (Dir) { //must only movve in one direction
          case 'l': blockBX += amount; break;
          case 'u': blockBY += amount; break;
          case 'f': blockBZ += amount; break;
          case 'r': blockBX -= amount; break;
          case 'd': blockBY -= amount; break;
          case 'b': blockBZ -= amount; break;
        }
        //place block
        map[blockBX][blockBY][blockBZ] = arg[i-selExt.r][j-selExt.d][k-selExt.b];
      }
    }
  }
}

void move(unsigned short amount,_Bool shift, char Dir){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//move %i %c",amount,Dir);
  toScript(command);
  //preemtivle test the area im moving into b/c its not in the selection
  checkAndExpand(amount,Dir); //wow that was easy

  if (shift){
    toScript(" -s);\n");
  }
  else{
    toScript(");\n");
  }
  waitUntlDone();

  //in memory

  struct block*** temp = copySel();
  //although these forloops are identical i must run through them twice or els ill run into undisiered effects of the blocks being moved multiple times
  pasteSel(temp,amount,Dir);

  //free the copy
  struct extents selExt = getAllSelExtents();
  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      free(temp[i-selExt.r][j-selExt.d]);
    }
    free(temp[i-selExt.r]);
  }
  free(temp);

  //shift after operation
  if (shift){
    //looks framilliar? it should its the smol switch again, whre moving the selection
    selectionShift(Dir,amount);
  }
}

void stack(short amount,char Dir, char* options){
  if(amount > 0){

    char command[commandBuffer];
    snprintf(command,commandBuffer,"echo(//stack %i %c",amount,Dir);
    toScript(command);

    _Bool preserveAir = 0;

    for(unsigned int i = 0; i < strlen(options); i++){
      snprintf(command,commandBuffer," -%c",options[i]);
      toScript(command);
      preserveAir |= options[i] == 'a';
    }
    toScript(");\n");

    waitUntlDone();

    //this is going to be a pain..

    //check last stack tile distacne
    //determin how much to shift the test selection based on the size of the dimention of the
    //selection paralel to the direction...
    short shift = 0;
    switch (Dir) {
      case 'l':
      case 'r':
      shift = selW * amount; break;//that was easyer that i thought, its the same both ways
      case 'u':
      case 'd':
      shift = selH * amount; break;
      case 'f':
      case 'b':
      shift = selD * amount; break;
    }

    //check shifted region
    checkAndExpand(shift,Dir);

    //get extents
    struct extents selExt = getAllSelExtents();

    //now the real painfull part
    for(short p = 0; p < amount; p++){ //amount to tile
      for(short i = selExt.r; i <= selExt.l; i++){
        for(short j = selExt.d; j <= selExt.u; j++){
          for(short k = selExt.b; k <= selExt.f; k++){

            //get new origin
            short blockBX = i;
            short blockBY = j;
            short blockBZ = k;

            switch (Dir) { //now direction really matters
              case 'l': blockBX += selW * (p+1); break;
              case 'u': blockBY += selH * (p+1); break;
              case 'f': blockBZ += selD * (p+1); break;
              case 'r': blockBX -= selW * (p+1); break; //1h so changing the sign is all i have to do here well that was easyer that i thought it would be...
              case 'd': blockBY -= selH * (p+1); break;
              case 'b': blockBZ -= selD * (p+1); break;
            }

            //new block = old block
            if(!preserveAir || !map[i][j][k].type == 0){
              map[blockBX][blockBY][blockBZ] = map[i][j][k];
            }
          }
        }
      }
    }

    //shift after block operation
    for(unsigned int i = 0; i < strlen(options); i++){
      if(options[i] == 's'){
        //looks framilliar? it should its the smol switch again, whre moving the selection
        selectionShift(Dir,shift);
      }
    }
  }
}

void expand(unsigned short amount,char Dir){
  if(amount <= 0){
    return;
  }

  wait();
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//expand %i %c);\n",amount,Dir);
  toScript(command);

  //check to allocate memory to expand into
  checkAndExpand(amount,Dir);

  //this is a lot smaller than the 1ge switch i had before lol
  if(Dir == 'l' || Dir == 'u' || Dir == 'f'){
    *getSelExtent(Dir) += amount;
  }
  if(Dir == 'r' || Dir == 'd' || Dir == 'b'){
    *getSelExtent(Dir) -= amount;
  }

  //calculate the hight the selW and the breth
  updateSelDementions(Dir);
}

char oppDir(char Dir){
  switch (Dir) {
    case 'l': return 'r';
    case 'r': return 'l';
    case 'u': return 'd';
    case 'd': return 'u';
    case 'b': return 'f';
    case 'f': return 'b';
    default : printf("Invalid Direction recieved :%c\n",Dir);
  }
  return '\0';
}

//turns direction 90 degrees left or right from current direction
char turnDir(char Dir, char turnDir){
  char dirNum = 0; //default number negitive to catch errors
  switch (Dir) { //base on 1 so never goes below 0, unless there is an error
    case 'r': dirNum = 0; break;
    case 'f': dirNum = 1; break;
    case 'l': dirNum = 2; break;
    case 'b': dirNum = 3; break;
    default : printf("Invalid Turn Direction recieved :%c\n",Dir);
  }
  if(turnDir == 'l'){
    dirNum++;
    dirNum %= 4;
  }
  else{
    dirNum--;
    if(dirNum < 0)
    dirNum = 3;
  }
  switch (dirNum) {
    case 0: return 'r';
    case 1: return 'f';
    case 2: return 'l';
    case 3: return 'b';
  }
  return '\0'; //null when something went wrong
}

void contract(unsigned short amount,char Dir){
  wait();

  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//contract %i %c);\n",amount,Dir);
  toScript(command);

  //big ol switch mk2
  //no need to check for map expansion lol im contracting
  if(Dir == 'l' || Dir == 'u' || Dir == 'f'){
    *getSelExtent(oppDir(Dir)) += amount;
  }
  if(Dir == 'r' || Dir == 'd' || Dir == 'b'){
    *getSelExtent(oppDir(Dir)) -= amount;
  }

  //calculate the hight the selW and the breth
  updateSelDementions(Dir);
}

void setRepeater(char Dir){
  comment("placing repeater based on direction");
  char* type = "redstone_repeater";
  unsigned char offset = 0;
  switch (Dir) {
    case 'f': offset = 0; break;
    case 'r': offset = 1; break;
    case 'b': offset = 2; break;
    case 'l': offset = 3; break;
  }
  //place block depenting on dirrection and offset on actual Dir
  toScript("if(DIRECTION == \"N\"); ");
  setBlock(type,(0+offset)%4,0);
  toScript("elseif(DIRECTION == \"E\"); ");
  setBlock(type,(1+offset)%4,0);
  toScript("elseif(DIRECTION == \"S\"); ");
  setBlock(type,(2+offset)%4,0);
  toScript("elseif(DIRECTION == \"W\"); ");
  setBlock(type,(3+offset)%4,0);
  toScript("endif;\n");
  comment("wait at end of if");
  waitUntlDone();
}

void setredTorch(char Dir){
  if(Dir != 'u')
  comment("placing torch based on direction");
  char* type = "redstone_torch";
  //even bigger switch
  switch (Dir) {
    case 'u':
    setBlock(type,5,0);
    break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,1,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,2,0);
    toScript("endif; \n");
    break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,1,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,1,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,3,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,1,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,4,0);
    toScript("endif;\n");
    break;
  }
  if(Dir != 'u')
  comment("wait at end of if");
  waitUntlDone();
}

void setObservor(char Dir){
  if(Dir != 'u' && Dir != 'd')
  comment("placing observor based on direction");
  char* type = "observor";
  //even bigger switch mk 2
  switch (Dir) {
    case 'u':
    setBlock(type,0,0);
    break;
    case 'd':
    setBlock(type,1,0);
    break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,5,0);
    toScript("endif;\n");
    break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,4,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,2,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,3,0);
    toScript("endif;\n");
    break;
  }
  if(Dir != 'u' && Dir != 'd')
  comment("wait at end of if");
  waitUntlDone();
}

void setPiston(char Dir, _Bool sticky){
  if(Dir != 'u' && Dir != 'd')
  comment("placing piston based on direction");

  char* type;

  if(sticky)
  type = "sticky_piston";
  else
  type = "piston";
  //even bigger switch mk 3
  switch (Dir) {
    case 'u':
    setBlock(type,1,0);
    break;
    case 'd':
    setBlock(type,0,0);
    break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,5,0);
    toScript("endif;\n");
    break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,4,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,3,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,2,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    setBlock(type,4,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    setBlock(type,5,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    setBlock(type,2,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    setBlock(type,3,0);
    toScript("endif;\n");
    break;
  }
  if(Dir != 'u' && Dir != 'd')
  comment("wait at end of if");
  waitUntlDone();
}

unsigned char wire(short inStrength,unsigned char outToll, char* type, unsigned char value, unsigned short amount, char Dir){
  comment("wire");
  //current position of end of selection
  unsigned short pos = 0;
  //current state of the output strength
  short outStrength = inStrength - amount;
  //support structure for redstone
  setBlock(type,value,1);
  stack(amount,Dir,"");
  //shift up to do redstone
  shift(1,'u');
  //check to see if the wire is longer than the input strength
  //if so it is safe to place the first repeater where the signal ends
  if(inStrength < amount && outStrength < outToll){
    shift(inStrength,Dir);
    pos += inStrength;
    setRepeater(Dir);
    outStrength = 16 + pos - amount;
  }
  //check to see if there is enough room for at least 1 full 16 line
  if((amount - inStrength - 1) / 16 > 0){
    shift(16,Dir);
    setRepeater(Dir);
    //check to see if there is at least one more if so use stack to fill in the rest
    if(((amount - inStrength - 1) / 16 > 0)){
      expand(15,oppDir(Dir));
      //-s option ensures im at the end of the lines and in position for the final line if needed
      stack(((amount - inStrength) / 16) - 1,Dir,"s");
      contract(15,Dir);
    }
    pos += ((amount - inStrength) / 16) * 16;
    outStrength = 16 + pos - amount;
    if((amount - inStrength) % 16 == 0){
      move(1,0,oppDir(Dir));
      outStrength--;
    }
  }
  //if the output is still too weak go to the end and place a repeater just behind it
  //this will boost the output to the proper strength reguardless of previous actions
  if(outStrength < outToll){
    shift(amount - pos - 1,Dir);
    pos += amount - pos - 1;
    setRepeater(Dir);
    outStrength = 16 + pos - amount;
    shift(1,Dir);
    outStrength = 15;
  }
  //if the output is proper strength go to end
  else{
    shift(amount - pos,Dir);
    pos += amount - pos;
  }
  //place redstone
  expand(amount,oppDir(Dir));
  replace("air",0,0,"redstone",0,0);
  contract(amount,Dir);
  //go back down into end position
  shift(1,'d');

  comment("endwire");

  return outStrength;
}

struct vector{
  short xCmp;
  short yCmp;
  short zCmp;
};

//used to get the vector from one point to another
struct vector getPointVector(struct point a, struct point b){
  struct vector ret;
  ret.xCmp = b.x - a.x;
  ret.yCmp = b.z - a.z;
  ret.zCmp = b.y - a.y;
  return ret;
}

//returns the distance of a vector using pythagrians theorm
float getDistance(struct vector arg){
  float ret = sqrt((arg.xCmp * arg.xCmp) + (arg.yCmp * arg.yCmp) + (arg.zCmp * arg.zCmp));
  return ret;
}

void line(char* type, unsigned char value){
  comment("line");

  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//line %s:%i)\n",type,value);
  toScript(command);

  waitUntlDone();

  struct point a;
  a.x = selXA;
  a.z = selZA;
  a.y = selYA;

  struct point b;
  b.x = selXB;
  b.z = selZB;
  b.y = selYB;

  //get the distance from a to b
  struct vector aTob = getPointVector(a,b);

  //get extents
  struct extents selExt = getAllSelExtents();

  //now the real painfull part
  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){
        //block location
        struct point c;
        c.x = i;
        c.z = j;
        c.y = k;

        //get the distance from a to c and from c to b
        struct vector aToc = getPointVector(a,c);
        struct vector cTob = getPointVector(b,c);

        //if the sum of the distances from a to c and from c to b are the same as from a to b, c must be on the line from a to b
        if(getDistance(aToc) + getDistance(cTob) == getDistance(aTob)){
          struct block B;
          B.type = blockLookup(type);
          B.value = value;

          //place block at point c
          map[i][j][k] = B;
        }
      }
    }
  }
  comment("endline");
}

void stairs(char* type, unsigned char value, char Dir, char up, char distance){
  comment("stairs");

  expand(distance,Dir);
  expand(distance,up);
  line(type,value);
  shift(1,'u');
  line("redstone",0);
  shift(1,'d');
  contract((distance),Dir);
  contract((distance),up);
  comment("endstairs");
}

//clear selection
void delete(){
  comment("delete");

  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//set 0);");
  toScript(command);

  waitUntlDone();
  //get extents
  struct extents selExt = getAllSelExtents();

  //now the real painfull part
  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){

        // printf("%i %i %i > %i %i\n",blockX,blockY,blockZ,B.type,B.value);
        map[i][j][k].type = 0;
        map[i][j][k].value = 0;
      }
    }
  }
  comment("enddelete");
}

void buildDecoderTop(short inBits, char Dir, char stSide, unsigned char rows, unsigned char box){
  comment("buildDecoderTop");
  unsigned short entries = pow(2,inBits);
  //build top structure
  //single bit
  setBlock("wood",2,1);
  overlay("redstone",0);
  shift(1,Dir);
  shift(1,'u');
  setObservor(Dir);
  shift(1,Dir);
  setBlock("wood",3,1);
  shift(1,'d');
  setPiston('d',1);
  expand(2,oppDir(Dir));
  expand(1,'u');
  expand(1,oppDir(stSide));
  //stack it to the other 5 places
  stack(inBits - 1,oppDir(stSide),"");

  //change the wool collors to make it look better
  shift(2,oppDir(stSide));
  replace("wood",1,2,"wood",1,0); //bottom
  replace("wood",1,3,"wood",1,1); //top
  //do the 3 other middle bits all at once
  stack(inBits - 3,oppDir(stSide),""); // -2 for not the first and last bit and an additional -1 becuase its the 3 middle bits
  shift(2,stSide);

  //expand selection to stack to rest of entries
  expand(((inBits - 1) * 2) - 1,oppDir(stSide));
  //stack it to the rest of the entries
  stack(entries - 1,Dir,"");

  //change the wool collors to make it look better
  for(short j = 0; j < rows; j++){
    for(short k = 0; k < 4; k++){
      //took me quite a while to come up with this specific pattern, it better look good
      int collor = collorMap[((box >> 2) & 1) ^ ((k >> 1) & 1)][(k & 1)][(box&3) ^ j];
      replace("wood",1,k,"wool",1,collor); //bottom
    }
    stack((entries / rows) - 1,Dir,"");
    if(j != rows - 1){
      shift(entries / rows * 3,Dir);
    }
  }
  shift((entries / rows * 3) * (rows - 1),oppDir(Dir));

  //go to binary Pattern
  contract((inBits - 1) * 2,stSide);
  contract(2,Dir);
  contract(1,'d');
  shift(1,'d');
  comment("endbuildDecoderTop");
}

void fillBinPtrn(short inBits, unsigned char significance[], char Dir, char stSide){
  comment("fillBinPtrn");
  //number of entries
  unsigned short entries = pow(2,inBits);

  //encode the decoder (binary with custom bit encoding)
  for(short iB = 0; iB < inBits; iB++){
    //create bases
    //bottom side (inverted bits)
    shift(1,'d');
    setBlock("redstone_block",0,1);
    if(significance[iB] > 1){
      expand(2,Dir);
      stack(significance[iB]-1,Dir,"");
      contract(2,oppDir(Dir));
    }
    //top side (normal bits)
    shift(1,'u');
    shift(significance[iB] * 3,Dir);
    setBlock("redstone_block",0,1);
    if(significance[iB] > 1){
      expand(2,oppDir(Dir));
      stack(significance[iB]-1,Dir,"");
      contract(2,Dir);
    }
    shift(significance[iB] * 3,oppDir(Dir));

    //stack to rest of entries
    if(entries / significance[iB] > 2){
      //expand to get both sides
      expand(1,'d');
      expand(6*significance[iB]-1,Dir);
      stack((entries / (significance[iB] * 2)-1),Dir,"a");
      contract(6*significance[iB]-1,oppDir(Dir));
      contract(1,'u');
    }
    //go to next bit
    if(iB != inBits - 1){
      shift(2,oppDir(stSide));
    }
  }

  //return from pattern
  shift(((inBits - 1) * 2),stSide);
  shift(1,'u');
  shift(2,oppDir(Dir));
  comment("endfillBinPtrn");
}

void buildDecoder(short inBits,char Dir,char stSide,unsigned char rows,unsigned char significance[],unsigned char box){
  comment("buildDecoder");
  //scripted actions to build top structure
  buildDecoderTop(inBits,Dir,stSide,rows,box);
  //fills in binary pattern
  fillBinPtrn(inBits,significance,Dir,stSide);
  comment("endbuildDecoder");
}

void tower(unsigned char amount,char up,char* type, unsigned char value){
  comment("tower");
  amount--;
  if(amount > 0){
    if(up == 'u'){
      setBlock(type,value,1);
      overlay("redstone",0);
      shift(2,'u');
      setObservor(up);
      if(amount > 1){
        stack(amount - 1,'u',"s");
      }
      overlay(type,value);
    }
    else{
      setObservor('d');
      if(amount > 1){
        stack(amount - 1,'d',"s");
      }
      shift(1,'d');
      setBlock(type,value,1);
      shift(1,'d');
    }
  }
  comment("endtower");
}

void flipFlop(char Dir){
  comment("FlipFlop");
  shift(1,'u');
  shift(1,Dir);
  setPiston(Dir,1);
  shift(1,Dir);
  setBlock("redstone_block",0,1);
  shift(1,Dir);
  shift(1,'d');
  comment("endFlipFlop");
}

void buildOutLines(unsigned char outBits, short inBits,char inSide, char outSide, char stSide){
  comment("buildOutLines");

  char num_levels;

  if(outBits > (inBits - 1)){ //this mess or
    num_levels = outBits / (inBits - 1);
    if(outBits % (inBits - 1) > 1){ //the last level can handle one more then the rest so only increase if the overflow is greater than 1
      num_levels++;
    }
  }
  else{
    num_levels = 1; // just one
  }

  unsigned short entries = pow(2,inBits);

  //create 1 1x15 wire section
  setBlock("wool",5,1); //a little touch to make it look a little better
  shift(1,oppDir(inSide));
  setBlock("wool",7,1);
  stack(13,oppDir(inSide),""); //1x15 wire section
  shift(1,inSide);
  shift(1,'u');
  setRepeater(outSide); //place first repeater at end of first output wire
  expand(14,oppDir(inSide));
  replace("air",0,0,"redstone",1,0); //plce redstone on wire

  expand(1,stSide);
  expand(1,'d');


  //outputs remaining to wire
  unsigned char bits = outBits;

  expand(2,'d');
  if(num_levels > 1){
    stack(inBits - 2,oppDir(stSide),"");
    bits -= inBits - 1;
    stack(num_levels - 1,'d',"");
    shift(4,'d');
    if(num_levels > 2){
      expand((num_levels - 3) * 4,'d');
      stack(inBits - 2,oppDir(stSide),"");
      contract((num_levels - 3) * 4,'d');
      bits -= (inBits - 1) * (num_levels - 2); //just made a (inBits - 2) x (num_levels - 2) box of out lines
      shift(4,'d');
    }
    //last leel
    //prevent the area around the encoder from being affected by a stack
    move(1,0,stSide);
    shift(1,stSide);
    stack(1,oppDir(stSide),"");
    shift(1,oppDir(stSide));
    stack(bits - 1,oppDir(stSide),"");
    shift((num_levels - 1) * 4,'u');
  }
  else{
    stack(bits - 1,oppDir(stSide),"");
  }


  //extend wire sections to the end
  expand(((inBits - 1) * 2) - 1,oppDir(stSide)); //expand selection to selecto the 4 1x15 wire sections (expand 2 extra for next operation)
  expand((num_levels - 1) * 4,'d');

  unsigned char wireStacks = entries / 5; // the number of enteries * the length of each entry / the length or each line reduced
  unsigned char trimAmt = (entries * 3) - ((wireStacks) * 15) + 1;

  //trim of ends
  stack(1,oppDir(inSide),""); //stack the 4 1x15 wire once
  shift(trimAmt,oppDir(inSide));
  stack(wireStacks-1,oppDir(inSide),""); //then stack to the rest 1 - with the strange shift
  shift(trimAmt,inSide);

  //negate expantion for wire extension and bring selection to 1x1x1
  contract(((inBits - 1) * 2), stSide);
  contract((num_levels * 4) - 1,'u');
  contract(3,oppDir(inSide));
  contract(11,inSide);

  //shift selection up into position for next operation
  shift(1,'u');
  shift(1,stSide);
  comment("endbuildOutLines");
}

void buildandSupport(short inBits, unsigned char outBits, short inSide, char stSide){

  comment("buildandSupport");

  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  setBlock("wool",4,1);
  overlay("redstone",0);
  expand(1,'u'); //include redstone
  expand(2,oppDir(inSide)); //make selection 3 blocks wide
  stack(entries-1,oppDir(inSide),""); //stack to rest of entries
  expand((entries-1) * 3,oppDir(inSide)); //expand selection to rest of entries

  if(num_levels > 1){
    expand(2,'d');
    stack(num_levels - 1,'d',"");
    expand((num_levels - 1) * 4,'d');
    //stack into structure with a option to not overwrite blocks
    stack((inBits * 2) - 1,oppDir(stSide),"a");
    delete(); //remove start blocks
    contract(((num_levels - 1) * 4) + 2,'u');
  }
  else{
    stack((inBits * 2) - 1,oppDir(stSide),"a");
    delete(); //remove start blocks
  }

  //collaps and shift selection for encoding
  contract(((entries-1) * 3) + 2,inSide);
  contract(1,'d');
  shift(1,oppDir(stSide));
  comment("endbuildandSupport");
}

void dataEntry(char* type, unsigned char value, unsigned char entry,char inSide,char stSide, unsigned char max){

  comment("dataEntry");

  // buildImmages();
  // getchar();

  //next entry
  unsigned char shiftcount = 1;
  unsigned char pos = 0;
  for(short oB = max - 1; oB >= 0; oB--){ //get the most significant bit first
    if((1 & entry >> oB)){
      shift(shiftcount,oppDir(stSide));
      pos += shiftcount;
      replace("wool",1,4,type,1,value);//looks better i think
      shift(1,inSide);
      setredTorch(inSide);
      shift(2,'d');
      replace("wool",1,7,type,1,value);//looks better i think
      shift(2,'u');
      shift(1,oppDir(inSide));
      //the next one is at least 2 shifts away
      shiftcount = 2;
    }
    //if no command inbetween shifts increase next shift amount
    else{
      //the next one is 2 more than it is currently
      shiftcount += 2;
    }
  }
  //return to start
  if(pos != 0){
    shift(pos,stSide);
    pos -= pos;
  }
  comment("enddataEntry");
}

void lvlDown(char stSide){
  comment("lvlDown");
  shift(1,stSide);
  setredTorch(stSide);
  shift(2,'d');
  setBlock("wool",4,1);
  overlay("redstone",0);
  shift(1,oppDir(stSide));
  setredTorch(oppDir(stSide));
  shift(2,'d');
  comment("endlvlDown");
}

void buildEncoder(unsigned char rows, unsigned char collumns,unsigned char Table[rows][collumns],short inBits, unsigned char outBits, char stSide,char inSide, char outSide){
  comment("buildEncoder");

  //how manny num_levels of outputs will i need?
  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  //build encoder sturcture middle section (big nand gate per entry)

  //get to start of output wires
  shift(4,'d');
  shift(1,oppDir(stSide));

  //nothing mutch to see here just some scripted actions to build support structures
  buildOutLines(outBits,inBits,inSide,outSide,stSide);
  buildandSupport(inBits,outBits,inSide,stSide);

  //start encoding...
  for(unsigned char row = 0; row < rows; row++){
    for(unsigned char col = 0; col < collumns; col++){
      if(Table[row][col] != 0){
        unsigned char entry = Table[row][col];// get entry to encode from table
        unsigned char offset = 0;
        unsigned char towers = 0;
        for(unsigned char level = 0; level < num_levels; level++){
          unsigned char partition; //the part of the entry left to encode
          unsigned char max;
          if(level < num_levels - 1){ //all but the last level
            max = inBits - 1; //how manny bits i can write this level
            partition = entry >> (outBits - (max + offset)); //get the next most significant bunch first
          }
          else{
            if(outBits > inBits - 1){
              max = (outBits - ((num_levels - 1) * (inBits - 1))); //the rest of the bits can fit
            }
            else{
              max = outBits; //all of the bits can fit
            }
            partition = entry;
          }

          if (level == num_levels - 1 && num_levels > 1){
            shift(1,stSide);
          }
          unsigned char dataValue = entry % 16;
          dataEntry("wool",dataValue,partition,inSide,stSide, max);
          if (level == num_levels - 1 && num_levels > 1){
            shift(1,oppDir(stSide));
          }
          if((level != num_levels - 1) && (entry >> (outBits - (max + offset))) != 0){ //only encode data if there is data left to encode
            lvlDown(stSide); //some more scripted actions
            towers++; //keep track of how far down i am
          }
          offset += max;
        }
        if(num_levels > 1){
          shift(towers * 4,'u');
        }
      }
      //next entry
      shift(3,oppDir(inSide));
    }
  }
  //go back to origin
  shift((entries * 3) + 3,inSide);
  shift(2,'u');
  comment("endbuildEncoder");
}

void buildSegment(char Dir){
  comment("buildSegment");

  shift(1,Dir);
  setBlock("redstone_lamp",0,1);
  stack(2,Dir,"");
  shift(1,oppDir(Dir));
  comment("endbuildSegment");
}

void seperateNibble(char* type, unsigned char value, char inSide, char stSide){
  comment("seperateNibble");

  shift(6,oppDir(inSide));
  shift(2,'d');
  shift(1,stSide);
  setBlock(type,value,1);
  expand(1,'u');
  stack(5,'u',"");
  contract(1,'d');
  shift(2,'u');
  shift(6,inSide);
  shift(1,oppDir(stSide));
  comment("endseperateNibble");
}

void build7segLights(char* type, unsigned char value, char stSide, char inSide){
  comment("build7segLights");

  //get to corner of display
  shift(1,stSide);
  shift(5,oppDir(inSide));
  shift(3,'d');

  expand(12,'u');
  expand(8,oppDir(stSide));
  expand(1,oppDir(inSide));
  setBlock(type,15,1);
  contract(1,oppDir(inSide));
  contract(1,oppDir(stSide));
  contract(1,stSide);
  replace("wool",0,0,type,1,value);
  contract(1,oppDir('u'));
  contract(1,oppDir('d'));
  setBlock("ice",0,1);
  shift(1,inSide);
  contract(1,oppDir(stSide));
  contract(5,stSide);
  contract(9,'d');
  contract(1,'u');

  for(short i = 0; i < 2;i++){
    buildSegment('u');
    buildSegment('r');
    if(i != 2)
    shift(4,'u');
  }
  shift(4,oppDir(stSide));
  for(short i = 0; i < 2;i++){
    buildSegment('d');
    if(i != 2){
      buildSegment('l');
      shift(4,'d');
    }
  }
  //return to origin
  shift(5,stSide);
  shift(5,inSide);
  shift(1,'u');
  comment("endbuild7segLights");
}

void LWire(char Dir, char inSide, _Bool move){
  comment("LWire");
  char* type = "wool";
  unsigned char value = 7;
  if(move){
    wire(10,4,type,value,2,oppDir(inSide));
    shift(1,Dir);
    wire(10,4,type,value,2,oppDir(inSide));
  }
  else{
    wire(10,4,type,value,4,oppDir(inSide));
  }
  shift(4,inSide);
  comment("endLWire");
}

void stairWire(char Dir, char up, char inSide, _Bool move){
  comment("stairWire");
  char* type = "wool";
  unsigned char value = 7;
  stairs(type,value,oppDir(inSide),up,2);
  if(move)
  shift(1,Dir);
  wire(10,4,type,value,2,oppDir(inSide));
  shift(4,inSide);
  shift(2,oppDir(up));
  comment("endstairWire");
}

void build7segWires(char inSide,char stSide){
  comment("build7segWires");

  //1
  LWire(oppDir(stSide),inSide,1);
  shift(1,oppDir(stSide));
  //2
  stairWire(oppDir(stSide),'u',inSide,1);
  shift(1,oppDir(stSide));
  //3
  stairWire(stSide,'d',inSide,1);
  shift(3,oppDir(stSide));
  //4
  LWire(stSide,inSide,1);
  shift(4,'u');
  //5
  LWire(stSide,inSide,0);
  shift(2,stSide);
  //6
  stairWire(stSide,'u',inSide,0);
  shift(2,stSide);
  //7
  LWire(stSide,inSide,0);
  //return to origin
  shift(1,stSide);
  comment("endbuild7segWires");
}


void buildOutput(unsigned char nibbles, char stSide, char backward){
  comment("buildOutput");

  short inBits = 4;
  unsigned char outBits = 7;
  unsigned char significance[] = {8,4,2,1};
  unsigned char rows = 1;
  unsigned char collumns = 16;

  for(unsigned char nibble = 0; nibble < nibbles; nibble++){
    buildDecoder(inBits,backward,stSide,rows,significance,nibble);
    buildEncoder(rows,collumns,segmentTable[0],inBits,outBits,stSide,oppDir(backward),backward);

    char dataValue = (nibble / 2) % 16;

    shift(49,backward);
    shift(8,'d');
    build7segLights("wool",dataValue,'l','f');
    if(nibble % 2 == 0 && nibble)
    seperateNibble("wool",7,oppDir(backward),stSide);
    build7segWires('f','l');
    shift(49,oppDir(backward));
    shift(4,'u');


    if(nibble != nibbles - 1)
    shift(inBits * 2,oppDir(stSide));
  }
  comment("endbuildOutput");
}

struct bussLocation{
  //location of bit 0
  char direction; //the direction the buss is running
  char stSide;// the side of the most significant bit
};


struct buss{
  char* name;
  unsigned char* collors;
  unsigned char* strength;
  unsigned char width;
  struct bussLocation loc;
};

void freeBuss(struct buss arg){
  printf("free(%s)\n",arg.name);
  // free(arg.name);
  free(arg.collors);
  free(arg.strength);
}

struct buss buildStables(struct buss roundKey, char Dir,char stSide){
  comment("buildStables");

  short inBits = 6;
  unsigned char outBits = 4;
  unsigned char significance[] = {32,8,4,2,1,16};
  unsigned char boxes = 8;
  unsigned char rows = 4;
  unsigned char collumns = 16;

  struct buss ret;
  ret.width = outBits * boxes;
  ret.collors = malloc(ret.width);
  ret.strength = malloc(ret.width);

  for(unsigned char box = 0; box < boxes; box++){
    buildDecoder(inBits,Dir,stSide,rows,significance,box);
    buildEncoder(rows,collumns,stables[box],inBits,outBits,stSide,oppDir(Dir),oppDir(Dir));
    for(int i = 0; i < outBits; i++){
      ret.collors[(box*outBits )+ i] = box % 16;
    }
    if(box != boxes - 1)
    shift(inBits * 2,oppDir(stSide));
    else{
      shift(((boxes - 1) * (inBits * 2) - 1),stSide);
      shift(4,'d');
    }
  }

  //terminate key
  freeBuss(roundKey);


  ret.loc.direction = oppDir(Dir);
  ret.loc.stSide = stSide;
  ret.name = "Stable Results";
  comment("endbuildStables");
  return ret;
}

void printBussInfo(struct buss arg,char* state){
  printf("BUSS INFO *********\n");
  printf("Buss  :%12s \n",arg.name);
  printf("Width :%12i \n",arg.width);
  printf("Collors :");
  for(int i = 0; i < arg.width; i++){
    if(i % 8 == 0){
      printf("\n");
    }
    printf("%2i ",arg.collors[i]);
  }
  printf("\n");
  printf("Strength :");
  for(int i = 0; i < arg.width; i++){
    if(i % 8 == 0){
      printf("\n");
    }
    printf("%2i ",arg.strength[i]);
  }
  printf("\n");
  printf("State :%12s\n",state);
  printf("BUSS LOCATON ******\n");
  printf("Dir   :%12c   \n",arg.loc.direction);
  printf("stSide:%12c   \n",arg.loc.stSide);
  printf("*******************\n");
  printf("\n");
}


struct buss buildPermute(char* name,struct buss inBuss, unsigned char permTable[], unsigned char permLength, unsigned char groupSize, unsigned char groupSpacing, char forward, char stSide, char inSide,_Bool startAtIn, _Bool stayAtOut){
  comment("buildPermute");
  if(startAtIn){
    if(stSide == inSide){
      shift(2,'u');
      shift(2,oppDir(inSide));
      shift(2,oppDir(forward));
    }
    else{
      shift(2,'u');
      shift(permLength * 2,oppDir(inSide));
      shift(2,oppDir(forward));
    }
  }

  struct buss outBuss;
  outBuss.width = permLength;
  outBuss.collors = malloc(permLength);
  outBuss.strength = malloc(permLength);
  outBuss.name = name;
  outBuss.loc.direction = oppDir(forward);
  outBuss.loc.stSide = stSide;

  for(short i = 0; i < permLength; i++){
    unsigned char group = (permTable[i] - 1) / groupSize;
    unsigned char extLength = group * groupSpacing;
    unsigned char outLength = (permTable[i] * 2) + extLength;
    short inLength;

    //wool collor for theis line
    char dataValueIn = inBuss.collors[permTable[i] - 1];
    unsigned char inStreangth = inBuss.strength[permTable[i] - 1];

    unsigned char dataValueOut = i % 16;
    outBuss.collors[i] = dataValueOut;

    //go to end
    shift(outLength,forward);
    //go down to bottom
    shift(2,'d');

    //determin input length based on side
    if(stSide == inSide)
    inLength = (i + 1) * 2;
    else
    inLength = ((permLength - i)*2);

    //shift to input
    shift(inLength,inSide);

    //wire to input
    inStreangth = wire(inStreangth,4,"wool",dataValueIn,inLength,oppDir(inSide));

    //where the busses meet
    stairs("redstone_lamp",0,oppDir(forward),'u',2);

    //wire to output
    inStreangth = wire(inStreangth,4,"wool",dataValueOut,outLength - 2,oppDir(forward));

    //output reached record its strength
    outBuss.strength[i] = inStreangth;

    //dont shift at the end
    if(i != permLength -1){
      shift(2,oppDir(stSide));
    }
  }
  shift((permLength -1) * 2,stSide);

  if(!stayAtOut){
    if(stSide == inSide){
      shift(2,'d');
      shift(2,inSide);
      shift(2,forward);
    }
    else{
      shift(2,'d');
      shift(permLength * 2,inSide);
      shift(2,forward);
    }
  }

  //old buss is terminated
  // freeBuss(inBuss);

  comment("endbuildPermute");
  return outBuss;
}

void breakout(char* type, unsigned char dataValue,char outSide){
  comment("breakout");

  shift(1,outSide);
  setBlock(type,dataValue,1);
  shift(1,'u');
  setRepeater(outSide);
  shift(5,'d');
  shift(1,oppDir(outSide));
  comment("endbreakout");
}

void shiftBit(char* type, unsigned char value, char shiftDir, char outSide, unsigned char shift_amt){
  comment("shiftBit");

  setBlock(type,value,1);
  if(shift_amt == 1){
    shift(1,shiftDir);
    setredTorch(shiftDir);
    shift(1,'u');
    setBlock(type,value,1);
    shift(1,oppDir(shiftDir));
    setredTorch(oppDir(shiftDir));
    shift(1,'u');
    setBlock(type,value,1);
  }
  for(unsigned char i = 0; i < 2 * shift_amt; i++){
    if((i !=  (3 * shift_amt) - 1)){
      shift(1,shiftDir);
      setredTorch(shiftDir);
      shift(1,'u');
      setBlock(type,value,1);
    }
  }
  breakout(type,value,outSide);
  comment("endshiftBit");
}

void cycleBit(char *type, unsigned char value,char shiftDir, char inSide, char outSide, unsigned char shift_amt, unsigned char shiftLength, unsigned char bitIndex){
  comment("cycleBit");
  setBlock("redstone_lamp",0,1);
  shift(1,'d');
  unsigned char strength = 13;
  shift(1,oppDir(inSide));
  stairs(type,value,oppDir(inSide),'u',1);
  strength -= 1;
  strength = wire(strength,4,type,value,(bitIndex * 2),oppDir(inSide));
  stairs(type,value,oppDir(shiftDir),'u',2);
  strength -= 2;
  strength = wire(strength,2,type,value,(shiftLength * 2) - (3 + (shift_amt * 2) - 1),oppDir(shiftDir));
  strength = wire(strength,5,type,value,(bitIndex * 2),outSide);
  stairs(type,value,inSide,'u',1);
  strength -= 1;
  shift(1,'u');
  shift(1,outSide);
  setBlock("redstone_lamp",0,1);
  breakout(type,value,outSide);
  comment("endcycleBit");
}

struct buss keyShiftSide(struct buss key,_Bool half,unsigned char shiftIndex){
  comment("keyShiftSide");

  char Stside = key.loc.stSide;
  char outSide = oppDir(key.loc.direction);
  char inSide = outSide;

  unsigned char shiftLength = PC1Length / 2;
  unsigned char shift_amt = keyshifts[shiftIndex]; //(1 or 2) but could be annything

  unsigned char offset;
  if(half){
    offset = shiftLength;
  }
  else{
    offset = 0;
  }

  struct buss temp;
  temp.collors = malloc(shift_amt);
  temp.strength = malloc(shift_amt);

  for(unsigned char i = 0; i < shiftLength; i++){
    if(i < shift_amt){
      cycleBit("wool",key.collors[i + offset],Stside,inSide,outSide,shift_amt,shiftLength,i); //cycle bit to other side
      shift((shiftLength * 2) - (shift_amt * 2) - 2, Stside); //move selection back to this side
      short index = i + offset + shiftLength - shift_amt;
      temp.collors[i] = key.collors[index]; //save collor into temp for when i need it later (swap)
      key.collors[index] = key.collors[i + offset]; //get coller from other side
      key.strength[index] = 15;
    }
    else{
      short index = i + offset - shift_amt;
      shiftBit("wool",key.collors[i + offset],Stside,outSide,shift_amt); //shift over by amount
      shift((shift_amt * 2) + 2, oppDir(Stside));
      if((i >= shiftLength - shift_amt)){ //get the coller from the the temp buss because this is actually a swap
        key.collors[index] = temp.collors[(i + shift_amt) - shiftLength];
        key.strength[index] = 15;
      }
      else{ //get the coller to the left
        key.collors[index] = key.collors[i + offset];
        key.strength[index] = 15;
      }
    }
  }

  //get back to start at end of second shift
  if(half)
  shift((PC1Length) * 2,Stside);

  comment("endkeyShiftSide");
  return key;
}

struct buss Keyshift(struct buss key,unsigned char shiftIndex){
  comment("Keyshift");
  key = keyShiftSide(key,0,shiftIndex);
  key = keyShiftSide(key,1,shiftIndex);
  shift(4,'u');
  comment("endKeyshift");
  return key;
}

struct buss setKeySchedual(){
  comment("setKeySchedual");

  char PC1stSide = 'r';
  char PC1inSide = 'r';
  char PC1outSide = 'f';

  char PC2stSide = 'b';
  char PC2inSide = 'b';
  char PC2outSide = 'l';

  struct buss key;
  key.collors = malloc(64); //input key
  key.strength = malloc(64); //input key

  for(int i = 0; i < 64; i++){
    key.collors[i] = i% 16;
  }

  struct buss PC1Results = buildPermute("PC1",key,PC1,PC1Length,1,0,PC1outSide,PC1stSide,PC1inSide,1,1);

  shift(1,'u');

  size_t num_shifts = sizeof(keyshifts) / sizeof(keyshifts[0]);

  struct buss roundKey;

  for(size_t i = 0; i < num_shifts; i++){
    roundKey = Keyshift(PC1Results,i);
    if(i != num_shifts - 1)
    buildPermute("PC2",roundKey,PC2,PC2Length,1,0,PC2outSide,PC2stSide,PC2inSide,1,0);
    else
    roundKey = buildPermute("PC2",roundKey,PC2,PC2Length,1,0,PC2outSide,PC2stSide,PC2inSide,1,1); //stay at end at end and only get the buss info at end
  }
  comment("endsetKeySchedual");
  return roundKey;
}

struct material{
  float *Kd;
  char illum;
  float d;
};

float *getKd(char type, unsigned char value){
  float* ret = malloc(3 * sizeof(float));
  int collor = collerLookup(type,value);
  ret[0] = (float)(collor & R)/R;
  ret[1] = (float)(collor & G)/G;
  ret[2] = (float)(collor & B)/B;
  return ret;
}

void printMaterial(FILE * Mtl,char* type,unsigned char value,struct material mat){
  fprintf(Mtl,"newmtl %s:%i\n",type,value);
  fprintf(Mtl,"Kd %.4f %.4f %.4f \n",mat.Kd[0],mat.Kd[1],mat.Kd[2]);
  fprintf(Mtl,"illum %i\n",mat.illum);
  fprintf(Mtl,"d %f\n",mat.d);
}

void buildMaterialLibrary(){

  FILE * Mtl = fopen("virtexMap.mtl","w");//file pointer

  size_t numElements = sizeof(blockTable) / sizeof(blockTable[0]);

  struct material materialMap[numElements][16];

  unsigned char woolID = blockLookup("wool");
  unsigned char woodID = blockLookup("wood");
  unsigned char iceID = blockLookup("ice");

  for(int i = 0; i < 16; i++){
    materialMap[woolID][i].Kd = getKd(woolID,i);
    materialMap[woolID][i].illum = 0;
    materialMap[woolID][i].d = 1;
    printMaterial(Mtl,"wool",i,materialMap[woolID][i]);
    free(materialMap[woolID][i].Kd);
  }

  for(int i = 0; i < 4; i++){
    materialMap[woodID][i].Kd = getKd(woodID,i);
    materialMap[woodID][i].illum = 0;
    materialMap[woodID][i].d = 1;
    printMaterial(Mtl,"wood",i,materialMap[woodID][i]);
    free(materialMap[woodID][i].Kd);
  }

  for(size_t i = 0; i < numElements; i++){
    if(i == iceID){
      materialMap[i][0].illum = 4; //ice is transparrant
      materialMap[i][0].d = .3;
    }
    else{
      materialMap[i][0].illum = 0;
      materialMap[i][0].d = 1;
    }
    if(i != woolID && i != woodID){ //exclude these because we did them in a different for loop
      materialMap[i][0].Kd = getKd(i,0); //0 because these have only 1 state (unless i want to be crazy with the redstone)
      printMaterial(Mtl,reverseBlockLookup(i),0,materialMap[i][0]);
      free(materialMap[i][0].Kd);
    }
  }

  fclose(Mtl);
}

struct virtex{
  float x;
  float z;
  float y;
};

void printFaceQuad(FILE * Obj,unsigned char v_index[4], int vc){
  fprintf(Obj,"f %i %i %i %i\n",v_index[0] + vc,v_index[1] + vc,v_index[2] + vc,v_index[3] + vc);
}

void printVirtex(FILE * Obj,struct virtex v){
  fprintf(Obj,"v %.1f %.1f %.1f\n",v.x,v.z,v.y);
}

void showReferenceTable(unsigned char referenceTable[6][4],_Bool facePresent[6],_Bool schedualVirtex[8]){
  printf(" -- Ref Table --\n");
  for(int i = 0; i < 8; i++){
    printf("%i ",schedualVirtex[i]);
  }
  printf("\n");
  for(int i = 0; i < 6; i++){
    printf("%i ",facePresent[i]);
  }
  printf("\n");
  for(int i = 0; i < 6; i++){
    if(facePresent[i]){
      for(int j = 0; j < 4; j++){
        printf("%i",referenceTable[i][j] - 1);
      }
      printf("\n");
    }
  }
  printf("\n");
}

void reflowTable(unsigned char referenceTable[6][4],_Bool facePresent[6],_Bool schedualVirtex[8]){
  _Bool change;
  // getchar();
  do {
    for(int k = 1; k < 8; k++){
      showReferenceTable(referenceTable,facePresent,schedualVirtex);
      getchar();
      change = 0;
      if(!schedualVirtex[k - 1] && schedualVirtex[k]){
        schedualVirtex[k - 1] = 1;
        schedualVirtex[k] = 0;
        for(int i = 0; i < 6; i++){
          if(facePresent[i]){
            for(int j = 0; j < 4; j++){
              if(referenceTable[i][j] - 1 == k)
              referenceTable[i][j]--;
              change = 1;
            }
          }
        }
      }
    }
  } while(change);
  showReferenceTable(referenceTable,facePresent,schedualVirtex);
}

struct redShape{
  _Bool extend[4];
  _Bool face[4];
};

struct redShape getRedStoneShape(short x, short z, short y){

  char redType = blockLookup("redstone");

  _Bool lastX = (x == mapW - 1);
  _Bool frstX = (x == 0);
  _Bool lastZ = (z == mapH - 1);
  _Bool frstZ = (z == 0);
  _Bool lastY = (y == mapD - 1);
  _Bool frstY = (y == 0);

  _Bool L = !lastX && map[x+1][z][y].type != 0;
  _Bool R = !frstX && map[x-1][z][y].type != 0;
  _Bool B = !lastY && map[x][z][y+1].type != 0;
  _Bool F = !frstY && map[x][z][y-1].type != 0;

  // _Bool LU = !lastZ && !lastX && map[x+1][z+1][y].type == redType;
  // _Bool RU = !lastZ && !frstX && map[x-1][z+1][y].type == redType;
  // _Bool BU = !lastZ && !lastY && map[x][z+1][y+1].type == redType;
  // _Bool FU = !lastZ && !frstY && map[x][z+1][y-1].type == redType;

  _Bool LD = !lastX && !frstZ && map[x+1][z-1][y].type == redType;
  _Bool RD = !frstX && !frstZ && map[x-1][z-1][y].type == redType;
  _Bool BD = !frstZ && !lastY && map[x][z-1][y+1].type == redType;
  _Bool FD = !frstZ && !frstY && map[x][z-1][y-1].type == redType;

  struct redShape ret;

  ret.extend[0] = (L || (!L && LD));
  ret.extend[1] = (R || (!R && RD));
  ret.extend[2] = (B || (!B && BD));
  ret.extend[3] = (F || (!F && FD));

  ret.face[0] = !L || (ret.extend[2] || ret.extend[3]);
  ret.face[1] = !R || (ret.extend[2] || ret.extend[3]);
  ret.face[2] = !B || (ret.extend[0] || ret.extend[1]);
  ret.face[3] = !F || (ret.extend[0] || ret.extend[1]);

  return ret;
}


_Bool smallBlock(char type){
  _Bool small = (type == blockLookup("redstone") || type == blockLookup("redstone_repeater") || type == blockLookup("redstone_torch"));
  return small;
}

//create .obj file from bock map
void buildWaveFront(){
  FILE * Obj = fopen("virtexMap.obj","w");//file pointer

  fprintf(Obj,"mtllib virtexMap.mtl\n");

  int vc = 0;//virtex count
  for(short x = 0; x < mapW; x++){
    for(short z = 0; z < mapH; z++){
      for(short y = 0; y < mapD; y++){
        if(map[x][z][y].type != 0){

          char type = map[x][z][y].type;
          char* name = reverseBlockLookup(type);
          unsigned char data = map[x][z][y].value;

          fprintf(Obj,"usemtl %s",name);

          if(!strncmp(name,"wool",sizeof("wool")) || !strncmp(name,"wood",sizeof("wood")))
          fprintf(Obj,":%i\n",data);
          else
          fprintf(Obj,":0\n");

          //demention modifyers
          float U_mod = .5;
          float D_mod = .5;
          float L_mod = .5;
          float R_mod = .5;
          float F_mod = .5;
          float B_mod = .5;

          //a bunch of if then logic to set up possible transforms
          struct redShape redShape = getRedStoneShape(x,z,y);
          if(type == blockLookup("redstone")){
            F_mod = .3;
            L_mod = .3;
            B_mod = .3;
            R_mod = .3;

            U_mod = -.3;

            if(redShape.extend[0]) L_mod = .5;
            if(redShape.extend[1]) R_mod = .5;
            if(redShape.extend[2]) B_mod = .5;
            if(redShape.extend[3]) F_mod = .5;

          }

          else if(type == blockLookup("slab")){
            D_mod = 0;
          }

          else if(type == blockLookup("redstone_torch")){
            U_mod = .2;
            if(data != 5)//upright torch
            D_mod = .2;
            L_mod = .2;
            R_mod = .2;
            F_mod = .2;
            B_mod = .2;
          }

          else if(type == blockLookup("redstone_repeater")){
            U_mod = -.1;
          }

          //virtex map to blace a cuboid
          _Bool vMap[8][3] = {
            {1,0,0},
            {0,0,0},
            {0,0,1},
            {1,0,1},
            {1,1,0},
            {1,1,1},
            {0,1,1},
            {0,1,0},
          };

          struct virtex v[8];

          //apply transform
          for(int i = 0; i < 8; i++){
            if(vMap[i][0]) v[i].x = x+L_mod;
            else           v[i].x = x-R_mod;
            if(vMap[i][1]) v[i].z = z+U_mod;
            else           v[i].z = z-D_mod;
            if(vMap[i][2]) v[i].y = y+B_mod;
            else           v[i].y = y-F_mod;
          }

          unsigned char referenceTable[6][4] = {
            {6,4,1,5}, //left
            {8,2,3,7}, //right
            {7,3,4,6}, //front
            {5,1,2,8}, //back
            {5,8,7,6}, //top
            {1,4,3,2}, //bottom

          };

          char adjacent[6];

          _Bool small = type == blockLookup("redstone") || type == blockLookup("redstone_repeater") || type == blockLookup("redstone_torch");

          adjacent[0] = (x == mapW - 1? 0 : map[x + 1][z][y].type);
          adjacent[1] = (x == 0? 0 : map[x - 1][z][y].type);
          adjacent[2] = (y == mapD - 1? 0 : map[x][z][y + 1].type);
          adjacent[3] = (y == 0? 0 : map[x][z][y - 1].type);
          adjacent[4] = (z == mapH - 1? 0 : map[x][z + 1][y].type);
          adjacent[5] = (z == 0? 0 : map[x][z - 1][y].type);


          for(int i = 0; i < 8; i++){
            printVirtex(Obj,v[i]);
            vc++; //current count of all virtexes
          }

          //refectored aww yeee :D
          for( int i = 0; i < 6; i++){
            if(!((type == blockLookup("ice") && adjacent[i] != 0) || (type == blockLookup("redstone") && i == 5 && !smallBlock(adjacent[5])) || (type == blockLookup("redstone") && (i != 4 && i != 5 && !redShape.face[i])) || (!small && !smallBlock(adjacent[i]) && !(adjacent[i] == 0 || adjacent[i] == blockLookup("ice"))))){
              printFaceQuad(Obj,referenceTable[i],vc - 8);
            }
          }
        }
      }
    }
  }
  fclose(Obj); //close file pointer
}


struct buss bussStraight(struct buss arg, short distance){
  comment("bussStraight");
  unsigned char width = arg.width;
  char bussDir = arg.loc.direction;
  for(unsigned char i = 0; i < width; i++){
    arg.strength[i] = wire(arg.strength[i],5,"wool",arg.collors[i],distance,bussDir);
    if(i != width - 1){
      shift(2,oppDir(arg.loc.stSide));
      shift(distance,oppDir(bussDir));
    }
  }
  shift((width - 1) * 2,arg.loc.stSide);
  comment("endbussStraight");
  return arg;
}

void tower2(char* type, unsigned char value,unsigned char levels){
  comment("tower2");
  setBlock(type,value,1);
  shift(1,'u');
  setredTorch('u');
  expand(1,'d');
  stack(levels,'u',"s");
  replace("redstone_torch",0,0,"redstone",1,0);
  contract(1,'d');
  comment("endtower2");
}

struct buss bussUp(_Bool type, struct buss arg, short distance, char up, _Bool flip){
  comment("bussUp");


  if(flip){
    arg.loc.direction = oppDir(arg.loc.direction);
  }

  char Dir = arg.loc.direction;


  unsigned char width = arg.width;
  for(unsigned char i = 0; i < width; i++){
    if(type){
      if(i == 0){
        tower(distance,up,"wool",arg.collors[i]);

        flipFlop(Dir);

        expand(distance - 2,oppDir(up));
        expand(1,oppDir(arg.loc.stSide));

        if(up == 'd'){
          shift(3,oppDir(Dir));
          shift(2,'u');
          stack(arg.width - 1,oppDir(arg.loc.stSide),"");
        }
        else{
          expand(2,'d');
          shift(3,oppDir(Dir));
          stack(arg.width - 1,oppDir(arg.loc.stSide),"");
          contract(2,'u');
        }

        shift(1,up);
        contract(distance - 2,up);
        expand(2,Dir);
        stack(arg.width - 1,oppDir(arg.loc.stSide),"");
        contract(2,oppDir(Dir));
        contract(1,arg.loc.stSide);
      }
      if(i != width - 1){
        shift(2,oppDir(arg.loc.stSide));
        setBlock("wool",arg.collors[i+1],1);
      }
      else{
        shift(1,'d');
        shift(3,Dir);
      }
      arg.strength[i] = 14;
    }
    else{
      tower2("wool",arg.collors[i],distance / 2);
      if(i != width - 1){
        shift(distance,oppDir(up));
        shift(2,oppDir(arg.loc.stSide));
      }
    }
  }
  shift((width - 1) * 2,arg.loc.stSide);
  comment("endbussUp");
  return arg;
}

struct buss bussStairs(struct buss arg, short distance, char up){
  comment("bussStairs");
  char Dir = arg.loc.direction;
  unsigned char width = arg.width;
  for(unsigned char i = 0; i < width; i++){
    stairs("wool",arg.collors[i],Dir,up,distance);
    if(i != width - 1){
      shift(2,oppDir(arg.loc.stSide));
      shift(distance,oppDir(up));
      shift(distance,oppDir(Dir));
    }
  }
  shift((width - 1) * 2,arg.loc.stSide);
  comment("endbussStairs");
  return arg;
}



struct buss createTestBuss(char* name,unsigned char width,char direction, char stSide){
  struct buss ret;
  ret.name = name;
  ret.width = width;
  ret.collors = malloc(width);
  ret.strength = malloc(width);
  ret.loc.stSide = stSide;
  ret.loc.direction = direction;
  for(int i = 0; i < width; i++){
    ret.strength[i] = 3;
    ret.collors[i] = i % 16;
  }
  return ret;
}

void freeBlockMap(){
  printf("free W%i D%i H%i %lu \n",mapW,mapD,mapH,mapW*mapD*mapH * sizeof(struct block));
  for(short i = 0; i < mapW; i++){
    for(short j = 0; j < mapH; j++){
      free(map[i][j]);
    }
    free(map[i]);
  }
  free(map);
  printf("free(bMap.map);\n");
}


void bitXOR1(char Dir){

  char* type = "wool";

  unsigned char valueA = 5;
  unsigned char valueB = 4;

  comment("bitXOR1");
  setBlock(type,valueA,1);
  shift(1,'u');
  setRepeater(Dir);
  shift(2,'d');
  shift(1,Dir);
  setBlock(type,valueB,1);
  overlay("redstone",0);
  shift(2,'u');
  setBlock(type,valueA,1);
  overlay("redstone",0);
  shift(1,Dir);
  shift(1,'u');
  setBlock(type,valueA,1);
  shift(1,Dir);
  setredTorch(Dir);
  expand(1,oppDir(Dir));
  expand(1,'d');
  stack(1,'d',"s");
  contract(1,oppDir(Dir));
  contract(1,'u');
  shift(1,'u');
  setredTorch('u');
  shift(3,'d');
  wire(14,1,type,valueB,1,Dir);
  shift(1,Dir);
  shift(1,'u');
  setBlock(type,valueB,1);
  shift(1,'u');
  setredTorch('u');
  shift(1,'u');
  setBlock("wool",5,1);
  overlay("redstone",0);
  comment("endbitXOR1");
}

void bitXOR2(char Dir){

  char* type = "wool";

  unsigned char valueA = 5;
  unsigned char valueB = 4;

  comment("bitXOR2");
  shift(1,Dir);
  shift(1,'u');
  setBlock(type,valueB,1);
  expand(1,'u');
  stack(1,'u',"");
  shift(1,'u');
  stack(1,'u',"");
  shift(1,'d');
  shift(1,Dir);
  contract(1,'u');
  setBlock(type,valueA,1);
  expand(1,'d');
  stack(1,'u',"");
  expand(1,oppDir(Dir));
  expand(3,'u');
  stack(1,Dir,"");
  shift(1,Dir);
  stack(2,Dir,"");
  shift(1,oppDir(Dir));
  contract(3,'d');
  contract(1,'u');
  contract(1,oppDir(Dir));
  setredTorch(oppDir(Dir));
  shift(2,'u');
  setredTorch(oppDir(Dir));
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir);
  shift(2,'d');
  setredTorch(Dir);
  shift(2,'d');
  setredTorch(Dir);
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir);
  shift(2,'u');
  setRepeater(oppDir(Dir));
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir);
  shift(2,'d');
  setredTorch('u');
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir);
  shift(2,'d');
  setRepeater(oppDir(Dir));
  shift(1,'u');
  shift(1,Dir);
  setredTorch(oppDir(Dir));
  shift(2,'d');
  setredTorch(oppDir(Dir));
  shift(4,'u');
  setRepeater(Dir);
  shift(1,Dir);
  comment("endbitXOR2");
}

struct buss XOR(_Bool XORType,struct buss inBitsA, struct buss inBitsB){
  comment("XOR");
  unsigned char width = inBitsA.width;
  char stSide = inBitsA.loc.stSide;
  char Dir = inBitsA.loc.direction;

  if(width != inBitsB.width){
    printf("*** Warning Unequal Buss Size XOR1 ***\n");
    printf("%s XOR %s\n",inBitsA.name,inBitsB.name);
  }

  if((XORType && inBitsA.loc.direction != inBitsB.loc.direction) || (!XORType && inBitsA.loc.direction != oppDir(inBitsB.loc.direction))){
    printf("*** Warning BussDirection missmatch ***\n");
    printf("%s XOR %s\n",inBitsA.name,inBitsB.name);
  }

  if(XORType){
    bitXOR1(Dir);
    expand(4,oppDir(Dir));
    expand(1,'u');
    expand(3,'d');
    expand(1,oppDir(stSide));
    stack(width - 1,oppDir(stSide),"");
    contract(1,stSide);
    contract(4,Dir);
    contract(1,'d');
    contract(3,'u');
  }
  else{
    bitXOR2(Dir);
    expand(6,oppDir(Dir));
    expand(4,'d');
    expand(1,oppDir(stSide));
    stack(width - 1,oppDir(stSide),"");
    contract(1,stSide);
    contract(6,Dir);
    contract(4,'u');
  }
  freeBuss(inBitsB);

  comment("endXOR");
  return inBitsA; //more modifacations to A may be needed (new collors?)
}

struct buss* allocateBlock(){
  struct buss* Block = malloc(2 * sizeof(struct buss));
  Block[0].name = "left_Block";
  Block[0].loc.direction = 'b';
  Block[1].name = "Right_Block";
  Block[1].loc.direction = 'f';
  for(unsigned char H = 0; H < 2; H++){ //both halves
    Block[H].width = PLength;
    Block[H].loc.stSide = 'l';
    Block[H].collors = malloc(PLength);
    Block[H].strength = malloc(PLength);
    Block[H].collors = malloc(PLength);
    Block[H].strength = malloc(PLength);
    for(int i = 0; i < PLength; i++){
      Block[H].collors[i] = i % 16;
      Block[H].strength[i] = 10;
    }
  }
  return Block;
}

struct buss turnBuss(struct buss arg, char direction,_Bool flip, unsigned char depth){
  comment("turnBuss");
  char oldDir = arg.loc.direction;
  char newDir = turnDir(oldDir,direction);

  short width = arg.width;

  for(int i = 0; i < width; i++){
    if(flip){
      if(arg.loc.stSide == newDir){
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],((width - 1) - i) * 2,oldDir);
        tower(depth,'d',"wool",arg.collors[i]);
        arg.strength[i] = wire(16,1,"wool",arg.collors[i],i * 2,newDir);
        if(i != width -1){
          shift(((width - 1) - i) * 2,oppDir(oldDir));
          shift(i * 2,oppDir(newDir));
          shift(2,oppDir(newDir));
          shift(depth,'u');
        }
      }
      else{
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],i * 2,oldDir);
        tower(depth,'d',"wool",arg.collors[i]);
        arg.strength[i] = wire(16,1,"wool",arg.collors[i],((width - 1) - i) * 2,newDir);
        if(i != width -1){
          shift(i * 2,oppDir(oldDir));
          shift(((width - 1) - i) * 2,oppDir(newDir));
          shift(2,newDir);
          shift(depth,'u');
        }
      }
    }
    else{
      if(arg.loc.stSide == newDir){
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],i * 2,oldDir);
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],i * 2,newDir);
        if(i != width -1){
          shift(i * 2,oppDir(oldDir));
          shift(i * 2,oppDir(newDir));
          shift(2,oppDir(newDir));
        }
      }
      else{
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],((width - 1) - i) * 2,oldDir);
        arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],((width - 1) - i) * 2,newDir);
        if(i != width -1){
          shift(((width - 1) - i) * 2,oppDir(oldDir));
          shift(((width - 1) - i) * 2,oppDir(newDir));
          shift(2,newDir);
        }
      }
    }
  }
  if(!flip){
    if(arg.loc.stSide != newDir){
      shift((width - 1) * 2,oldDir);
    }
    else{
      shift((width - 1) * 2,oppDir(oldDir));
    }
  }
  else{
    if(arg.loc.stSide != newDir){
      shift((width - 1) * 2,oppDir(oldDir));
    }
    else{
      shift((width - 1) * 2,oldDir);
    }
    arg.loc.stSide = oppDir(arg.loc.stSide);

  }
  arg.loc.direction = newDir;
  arg.loc.stSide = turnDir(arg.loc.stSide,direction);
  comment("endturnBuss");
  return arg;
}

struct buss bussFlipFlop(struct buss arg){
  flipFlop(arg.loc.direction);
  shift(1,'u');
  shift(1,oppDir(arg.loc.direction));
  expand(1,oppDir(arg.loc.direction));
  expand(1,oppDir(arg.loc.stSide));
  stack(arg.width - 1,oppDir(arg.loc.stSide),"");
  contract(1,arg.loc.stSide);
  shift(1,arg.loc.direction);
  contract(1,arg.loc.direction);
  shift(1,'d');
  for(short i = 0; i < arg.width; i++){
    arg.strength[i] = 15;
  }
  return arg;
}

struct buss halfSwapSides(struct buss inBuss,short amount,char Dir){
  comment("halfSwapSides");

  short bits = inBuss.width;

  for(short i = 0; i < 4; i++){

    struct buss subBuss;
    subBuss.width = bits/4;
    subBuss.collors = malloc(subBuss.width);
    subBuss.strength = malloc(subBuss.width);
    subBuss.name = "Bit_Shift_Sub_Buss";
    subBuss.loc.stSide = inBuss.loc.stSide;
    subBuss.loc.direction = inBuss.loc.direction;

    for(short j = 0; j < bits/4; j++){
      subBuss.collors[j] = inBuss.collors[(i * bits/4) + j];
      subBuss.strength[j] = inBuss.strength[(i * bits/4) + j];
    }

    if(i != 3){
      subBuss = bussStraight(subBuss,3);
      subBuss = turnBuss(subBuss,Dir,1,6 - (i * 2));
    }
    else{
      subBuss = bussUp(1,subBuss,2,'u',0);
      subBuss = turnBuss(subBuss,Dir,1,2);
    }

    subBuss = bussStraight(subBuss,(((amount - 2) - (bits/4 - 1)) * 2) + 4);
    char shiftDir = oppDir(subBuss.loc.direction);

    subBuss = turnBuss(subBuss,Dir,0,0); //direction not used
    subBuss = bussStraight(subBuss,2);

    if(i != 3){
      subBuss = bussUp(1,subBuss,6 - (i * 2),'u',0);
      shift(1,subBuss.loc.direction);
      subBuss = bussStraight(subBuss,1);
    }
    else{
      subBuss = bussFlipFlop(subBuss);
      printBussInfo(subBuss,"afterFlip");
      shift(1,subBuss.loc.direction);
      subBuss = bussStraight(subBuss,1);
    }
    if(i != 3){
      shift(4,oppDir(subBuss.loc.direction));
    }
    shift((((amount - 2) - (bits/4 - 1)) * 2) + 2,shiftDir);

    for(short j = 0; j < bits/4; j++){
      inBuss.collors[(i * bits/4) + j] = subBuss.collors[j];
      inBuss.strength[(i * bits/4) + j] = subBuss.strength[j];
    }

  }
  inBuss.loc.direction = oppDir(inBuss.loc.direction);
  comment("endhalfSwapSides");
  return inBuss;
}

struct buss* buildRound(struct buss roundKey, struct buss* block){
  comment("buildRound");

  char roundDir = roundKey.loc.direction;
  char roundStSide = roundKey.loc.stSide;
  bussUp(1,roundKey,4,'d',0);
  shift(1,'d');
  bussStraight(roundKey,0);
  shift(5,'u');
  shift(8,roundDir);
  bussStraight(roundKey,(PLength * 2) + 1);
  struct buss sBoxResults = buildStables(roundKey,roundDir,roundStSide);
  struct buss PResults = buildPermute("P_Results",sBoxResults,P,PLength,4,4,'f',oppDir(roundDir),roundDir,1,1);
  shift(6,'d');
  shift(1,'b');
  shift(((PLength * 2)),roundDir);
  struct buss EResults = buildPermute("E_Results",block[1],E,ELength,1,0,roundDir,'b','b',1,1);
  bussStraight(EResults,(PLength * 2) - 1);
  shift(1,'u');
  shift(8,oppDir(roundDir));
  XOR(0,roundKey,EResults);
  shift(8,'d');
  shift(2,roundDir);
  shift(3,'b');
  bussUp(0,block[0],4,'u',0);
  shift(1,'u');
  shift(1,'f');
  block[0] = XOR(1,block[0],PResults);
  block[0] = bussStraight(block[0],1);
  block[0] = halfSwapSides(block[0],PLength,oppDir(roundDir));
  block[1] = bussUp(0,block[1],8,'u',0);
  shift(14,'d');
  block[1] = bussStraight(block[1],(roundKey.width * 2) + 4);
  block[1] = bussUp(1,block[1],6,'u',0);
  shift(roundKey.width, roundDir);
  block[1] = halfSwapSides(block[1],PLength,oppDir(roundDir));
  shift(roundKey.width,oppDir(roundDir));
  block[1] = bussUp(0,block[1],4,'u',0);
  block[1] = bussStraight(block[1],(roundKey.width * 2) + 3);
  block[1] = bussUp(0,block[1],4,'u',0);
  comment("endbuildRound");
  return block;
}

struct buss* bussTap(struct buss arg,char direction){
  comment("bussTap");

  char oldDir = arg.loc.direction;
  char newDir = turnDir(oldDir,direction);

  short width = arg.width;

  struct buss* ret = malloc(2 * sizeof(struct buss));
  for(int i = 0; i < 2; i ++){
    ret[i].collors = malloc(width);
    ret[i].strength = malloc(width);
    ret[i].loc.stSide = arg.loc.stSide;
    ret[i].width = arg.width;
  }

  ret[1].loc.direction = newDir;
  ret[0].loc.direction = oldDir;

  for(int i = 0; i < width; i++){

    ret[0].collors[i] = arg.collors[i];
    ret[1].collors[i] = arg.collors[i];

    short longDist = ((width - 1) - i) * 2;
    short shortDist = i * 2;

    if(arg.loc.stSide == newDir){
      arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],longDist,oldDir);
      shift(1,'u');
      setRepeater(oldDir);
      shift(1,oldDir);
      setBlock("redstone_lamp",0,1);
      shift(1,'d');
      shift(1,oldDir);
      ret[0].strength[i] = wire(15,1,"wool",arg.collors[i],shortDist,oldDir);
      shift(shortDist + 1,oppDir(oldDir));
      shift(1,'d');
      ret[1].strength[i] = wire(15,1,"wool",arg.collors[i],0,newDir);
      shift(1,'d');
      ret[1].strength[i] = wire(ret[1].strength[i],1,"wool",arg.collors[i],shortDist + 1,newDir);
      shift(2,'u');
      if(i != width - 1){
        shift(longDist + 1,oppDir(oldDir));
        shift(shortDist + 3,oppDir(newDir));
      }
      else{
        shift(longDist + 1,oppDir(newDir));
        shift(1,oppDir(oldDir));
      }
    }
    else{
      arg.strength[i] = wire(arg.strength[i],1,"wool",arg.collors[i],shortDist,oldDir);
      shift(1,'u');
      setRepeater(oldDir);
      shift(1,oldDir);
      setBlock("redstone_lamp",0,1);
      shift(1,'d');
      shift(1,oldDir);
      ret[0].strength[i] = wire(15,1,"wool",arg.collors[i],longDist,oldDir);
      shift(longDist + 1,oppDir(oldDir));
      shift(1,'d');
      ret[1].strength[i] = wire(15,1,"wool",arg.collors[i],0,newDir);
      shift(1,'d');
      ret[1].strength[i] = wire(ret[1].strength[i],1,"wool",arg.collors[i],longDist + 1,newDir);
      shift(2,'u');
      if(i != width - 1){
        shift(shortDist + 1,oppDir(oldDir));
        shift((longDist + 1) - 2,oppDir(newDir));
      }
      else{
        shift(shortDist + 1,oppDir(newDir));
        shift(1,oppDir(oldDir));
      }
    }
  }
  ret[1].loc.stSide = turnDir(ret[1].loc.stSide,direction);
  if(arg.loc.stSide != newDir){
    shift((width - 1) * 2,oppDir(oldDir));
  }
  else{
    shift((width - 1) * 2,oldDir);
  }
  ret[1].loc.stSide = oppDir(ret[1].loc.stSide);

  comment("endbussTap");
  return ret;
}

struct buss* buildRoundKeyBuss(struct buss roundKey){

  short spaceBetween = 8;

  comment("buildRoundKeyBuss");
  struct buss* DESKeys = bussTap(roundKey,'l');
  DESKeys[0].name = "encrypt";
  DESKeys[1].name = "decrypt";
  printBussInfo(DESKeys[1],"beforeBussDown");
  shift(4,'d');
  for(int i = 1; i < 16; i++){
    bussTap(roundKey,'l');
    if(i != 15)
    shift(4,'d');
  }
  shift(roundKey.width * 2,oppDir(roundKey.loc.stSide));
  shift(2,'d');
  shift(1,roundKey.loc.direction);

  for(int i = 0; i < 16; i++){
    bussStraight(DESKeys[1],(i * 2) + 4);
    struct buss outKey = bussUp(1,DESKeys[1],(i * 8) + spaceBetween,'d',1);
    shift(1,outKey.loc.direction);
    bussStraight(outKey,(i * 2));
    shift(3,outKey.loc.direction);
    if(i != 15){
      shift(3,DESKeys[1].loc.direction);
      shift((i * 8) + spaceBetween + 4,'u');
    }
  }

  freeBuss(roundKey);
  comment("endbuildRoundKeyBuss");
  return DESKeys;
}

int main(){

  //memory allocation
  map = malloc(mapW * sizeof(struct block**));
  map[0] = malloc(mapH * sizeof(struct block*));
  map[0][0] = malloc(mapD * sizeof(struct block));


  struct buss Test = createTestBuss("TEST_1",48,'r','b');
  struct buss* block = allocateBlock();

  buildOutput(16,'l','b');

  // buildStables(Test,'r','b');

  // buildOutLines(7,4,'f','b','l');


  // Test = turnBuss(Test,'r',1,2);
  // Test = bussStraight(Test,3);
  // Test = turnBuss(Test,'r',1,2);

  // Test = bussFlipFlop(Test);

  // buildRound(Test,block);

  // buildRoundKeyBuss(Test);

  // bussTap(Test,'l');

  // setKeySchedual();

  // Test = turnBuss(Test,'l',1,2,'u');


  buildMaterialLibrary();
  buildWaveFront();
  buildImmages();
  printFileBuffer(script);



  //end portion of main (freeing and closing pointers)
  freeBlockMap();

  return 0;
}
