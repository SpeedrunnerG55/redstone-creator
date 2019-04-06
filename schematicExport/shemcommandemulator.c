#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <zlib.h>
#include "DES_tables.h"

/*
more of a quality of life
feature to keep track of
my selection between functions
*/


char *blockTable[][16] = {
  {"air","air"},
  {"observer","machine_Base"},
  {"redstone","redstone_Off","redstone_Off","redstone_Off","redstone_Off","redstone_Off"},
  {"redstone_Block","redstone_Off"},
  {"redstone_Lamp","redstone_Lamp"},
  {"redstone_repeater","redstone_Tile","redstone_Off","generic_Wood","generic_Wood","redstone_Off","redstone_Off"},
  {"redstone_torch","generic_Wood","redstone_On"},
  {"redstone_torch_off","generic_Wood","redstone_Off"},
  {"slab","slab"},
  {"sticky_piston","machine_Base","generic_Wood","slime"},
  {"piston","machine_Base","generic_Wood"},
  {"wood","wood"},
  {"wool","wool"},
  {"ice","ice"}
};

unsigned char blockLookup(char* type){

  size_t numElements = sizeof(blockTable) / sizeof(blockTable[0]);
  for(size_t i = 0; i < numElements; i++){
    if(strlen(type) == strlen(blockTable[i][0]) && !strncmp(type,blockTable[i][0],strlen(blockTable[i][0]))){
      return i;
    }
  }
  //error not in table
  printf("blockLookup() - what is %s?\n",type);
  return -1;
}

unsigned char getID(unsigned char type){
  if     (type == blockLookup("air"))               return 0;
  else if(type == blockLookup("observer"))          return 218;
  else if(type == blockLookup("redstone"))          return 55;
  else if(type == blockLookup("redstone_Block"))    return 152;
  else if(type == blockLookup("redstone_Lamp"))     return 123;
  else if(type == blockLookup("redstone_repeater")) return 93;
  else if(type == blockLookup("redstone_torch"))    return 76;
  else if(type == blockLookup("redstone_torch_off"))return 75;
  else if(type == blockLookup("slab"))              return 44;
  else if(type == blockLookup("sticky_piston"))     return 29;
  else if(type == blockLookup("piston"))            return 33;
  else if(type == blockLookup("wood"))              return 5;
  else if(type == blockLookup("wool"))              return 35;
  else if(type == blockLookup("ice"))               return 79;
  else{
    return 0;
  }
}

unsigned char reversegetID(unsigned char type){
  if     (type == 0)   return blockLookup("air");
  else if(type == 218) return blockLookup("observer");
  else if(type == 55)  return blockLookup("redstone");
  else if(type == 152) return blockLookup("redstone_Block");
  else if(type == 123) return blockLookup("redstone_Lamp") ;
  else if(type == 93)  return blockLookup("redstone_repeater");
  else if(type == 76)  return blockLookup("redstone_torch");
  else if(type == 75)  return blockLookup("redstone_torch_off");
  else if(type == 44)  return blockLookup("slab");
  else if(type == 29)  return blockLookup("sticky_piston");
  else if(type == 33)  return blockLookup("piston");
  else if(type == 5 )  return blockLookup("wood");
  else if(type == 35)  return blockLookup("wool");
  else if(type == 79)  return blockLookup("ice");
  else{
    return 0;
  }
}

_Bool mask(unsigned char type){
  if(type == blockLookup("wool")) return 0;
  return 0;
}

size_t partCount(unsigned char type){
  for(int i = 1; i < 16; i++){
    if(blockTable[type][i] == NULL){
      return i - 1;
    }
  }
  return 16;
}

char* getPartnameForBlock(unsigned char type, unsigned char part){
  unsigned char numTypes = sizeof(blockTable) / sizeof(blockTable[0]);
  unsigned char numParts = partCount(type);
  if(type < numTypes){
    if(part < numParts){
      return blockTable[type][part + 1];
    }
    else{
      //error not in table
      printf("getPartnameForBlock() - what is %i?\n",part);
      return "NULL"; //lol
    }
  }
  else{
    //error not in table
    printf("getPartnameForBlock() - what is %i?\n",type);
    return "NULL"; //lol
  }
}

char* reverseBlockLookup(unsigned char type){
  unsigned char numElements = sizeof(blockTable) / sizeof(blockTable[0]);
  if(type < numElements)
  return blockTable[type][0];
  else{
    //error not in table
    printf("reverseBlockLookup() - what is %i?\n",type);
    return "NULL"; //lol
  }
}

//this set is disjointed from the blockTable as in their order and apperarance of materials are NOT and CAN NOT BE the same
char *partTable[] ={
  "air",
  "machine_Base",
  "redstone_Off",
  "redstone_On",
  "redstone_Lamp",
  "redstone_Tile",
  "generic_Wood",
  "slab",
  "slime",
  "wood",
  "wool",
  "ice"
};

unsigned char partLookup(char* partName){
  size_t numParts = sizeof(partTable) / sizeof(partTable[0]);
  for(size_t part = 0; part < numParts; part++){
    if(strlen(partName) == strlen(partTable[part]) && !strncmp(partName,partTable[part],strlen(partTable[part]))){
      return part;
    }
  }

  //error not in table
  printf("partLookup() - what is %s?\n",partName);
  return -1;
}

char* reversepartLookup(unsigned char part){
  unsigned char numElements = sizeof(partTable) / sizeof(partTable[0]);
  if(part < numElements)
  return partTable[part];
  else{
    //error not in table
    printf("reversepartLookup() - what is %i?\n",part);
    return "NULL"; //lol
  }
}

unsigned int collorLookup(int value, unsigned char part){

  //minecraft wool RGB values
  unsigned int woolMap[16] = {
    0xE9ECEC,0xF07613,0xBD44B3,0x3AAFD9,
    0xF8C627,0x70B919,0xED8DAC,0x3E4447,
    0x8E8E86,0x158991,0x792AAC,0x35399D,
    0x724728,0x546D1B,0xA12722,0x141519
  };

  unsigned int woodmap[4] = {
    0xc15000,0x397a03,0x02367f,0x89057a
  };

  if     (part == partLookup("wood"))           return woodmap[value];
  else if(part == partLookup("wool"))           return woolMap[value];

  else if(part == partLookup("redstone_Off"))   return 0x560000;
  else if(part == partLookup("redstone_On"))    return 0xd10000;
  else if(part == partLookup("slime"))          return 0xA9C54E;
  else if(part == partLookup("generic_Wood"))   return 0xffdb8e;
  else if(part == partLookup("machine_Base"))   return 0x3d3d3d;
  else if(part == partLookup("redstone_Tile"))  return 0xa8a8a8;
  else if(part == partLookup("redstone_Lamp"))  return 0x751d0b;
  else if(part == partLookup("slab"))           return 0x8d9093;
  else if(part == partLookup("ice"))            return 0xa5c8ff;
  else if(part == partLookup("air"))            return 0x000000;

  printf("collorLookup() - could not find collor for %s\n",reversepartLookup(part));
  return 0;
}

struct block{
  unsigned char type;
  unsigned char value;
  char reldir;
  _Bool powered;
};

// x z y
struct block *map;

//Block map
//keep track of raw size for convienience
short mapW = 1,mapH = 1,mapD = 1;
//teh extent of the canvas as a function of 2 of its corners
short mapXA = 0,mapZA = 0,mapYA = 0,mapXB = 0,mapZB = 0,mapYB = 0;

//selection
//keep track of raw size for convienience
short selW = 1,selH = 1,selD = 1; //if i really have a selection larger than 255 i have a problem...
//pos1/pos2
short selXA = 0,selZA = 0,selYA = 0,selXB = 0,selZB = 0,selYB = 0;

int getSchemIndex(short x, short z, short y){
  return (mapD * mapH * x) + (mapD * z) + y;
}

int getIndex(short x, short z, short y){
  return (mapD * mapH * x) + (mapD * z) + y;
}

int getSubIndex(short x, short z, short y, short subD, short subH){
  return (subD * subH * x) + (subD * z) + y;
}

struct block*** rollMap(struct block *flat){
  struct block*** cuboid = malloc(mapW * sizeof(struct block*));
  for(int x = 0; x < mapW; x++){
    cuboid[x] = malloc(mapH * sizeof(struct block*));
    for(int z = 0; z < mapH; z++){
      cuboid[x][z] = malloc(mapD * sizeof(struct block*));
      for(int y = 0; y < mapD; y++){
        int index = getIndex(x,z,y);
        cuboid[x][z][y] = flat[index];
      }
    }
  }
  return cuboid;
}

struct block* unRollMap(struct block *** cuboid,struct block* flat){
  flat = realloc(flat,mapW * mapH * mapD * sizeof(struct block));
  for(int x = 0; x < mapW; x++){
    for(int z = 0; z < mapH; z++){
      for(int y = 0; y < mapD; y++){
        int index = getIndex(x,z,y);
        flat[index] = cuboid[x][z][y];
      }
      free(cuboid[x][z]);
    }
    free(cuboid[x]);
  }
  free(cuboid);
  return flat;
}

_Bool outofBounds(){
  return(selXA < mapXA || selZA < mapZA || selYA < mapYA || selXB > mapXB || selZB > mapZB || selYB > mapYB);
}

short* getSelExtent(char dir){
  switch (dir) {
    case 'l': if(selXA > selXB) return &selXA; else return &selXB; break;
    case 'r': if(selXA < selXB) return &selXA; else return &selXB; break;
    case 'u': if(selZA > selZB) return &selZA; else return &selZB; break;
    case 'd': if(selZA < selZB) return &selZA; else return &selZB; break;
    case 'f': if(selYA > selYB) return &selYA; else return &selYB; break;
    case 'b': if(selYA < selYB) return &selYA; else return &selYB; break;
  }
  return NULL; //error
}

short* getMapExtent(char dir){
  switch (dir) {
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

_Bool inSel(short x,short z,short y){
  struct extents sel = getAllSelExtents();
  return (x <= sel.l && x >= sel.r && z <= sel.u && z >= sel.d && y <= sel.f && y >= sel.b);
}

//RGB masks
unsigned int R = 0xFF0000;
unsigned int G = 0x00FF00;
unsigned int B = 0x0000FF;

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

void showFloat(float arg){
  unsigned char barLength = 10;
  printf("[");
  for(short i = 0; i < arg * barLength; i++) printf("#");
  for(short i = arg * barLength; i < barLength; i++) printf(" ");
  printf("]");
}

//progress bars! :D
void showProgress(short ARG,short MAX_ARG, char* message){
  //calculate progress values are inf when masked
  float Progress = ((float)(ARG + 1) / MAX_ARG);
  //at beginning of each process new line to not overwrite any previous output
  if(Progress == 0) printf("\n");
  //return to write next progression
  printf("\n");
  //print message of what the process is
  printf("%-30s",message);
  //show current progress
  printf("%5i/%-5i %3.2f %%",ARG + 1,MAX_ARG,Progress * 100); showFloat(Progress);
  //new line when finished so i dont need to impliment a seperate new line in the code seperatly
  if(Progress == 1) printf("\n");
}

void buildImmages(){

  unsigned int selTint = 0xDBDADC;
  unsigned int backround = 0x201020;

  printf("%i %i %i\n",mapW,mapH,mapD);
  FILE * Top = fopen("immages/topView.pnm","w");
  fprintf(Top,"P3\n");
  fprintf(Top,"%i %i\n",mapW,mapD);
  printf("%i %i\n",mapW,mapD);
  fprintf(Top,"255\n");
  for(short y = mapD - 1; y >=0; y--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int collor = backround;
      for(short z = mapH - 1; z >= 0; z--){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor > 0){
            collor = blockCollor;
            float scaler = (float)z / (mapH);
            darken(&collor,scaler);
            break;
          }
        }
      }
      if(inSel(x,selZA,y)){
        tint(&collor,selTint);
      }
      printPixel(collor,Top);
    }
    fprintf(Top,"\n");
    showProgress((mapD - 1) - y,mapD,"scanning y axis");
  }
  fclose(Top);

  FILE * Front = fopen("immages/frontview.pnm","w");
  fprintf(Front,"P3\n");
  fprintf(Front,"%i %i\n",mapW,mapH);
  printf("%i %i\n",mapW,mapH);
  fprintf(Front,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int  collor = backround;
      for(short y = 0; y < mapD; y++){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapD - y) / mapD;
            darken(&collor,scaler);
            break;
          }
        }
      }
      if(inSel(x,z,selYA)){
        tint(&collor,selTint);
      }
      printPixel(collor,Front);
    }
    fprintf(Front,"\n");
    showProgress((mapH - 1) - z,mapH,"scanning z axis");
  }
  fclose(Front);

  FILE * Left = fopen("immages/leftview.pnm","w");
  fprintf(Left,"P3\n");
  fprintf(Left,"%i %i\n",mapD,mapH);
  printf("%i %i\n",mapD,mapH);
  fprintf(Left,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short y = mapD - 1; y >= 0; y--){
      unsigned int  collor = backround;
      for(short x = mapW - 1; x >= 0; x--){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapW - x) / mapW;
            darken(&collor,scaler);
            break;
          }
        }
      }
      if(inSel(selXA,z,y)){
        tint(&collor,selTint);
      }
      printPixel(collor,Left);
    }
    fprintf(Left,"\n");
    showProgress((mapH - 1) - z,mapH,"scanninx z axis");
  }
  fclose(Left);
}

void printMap(_Bool line){
  if(line){
    printf("1d map unrolled \n");
    for(int i = 0; i < mapW * mapH * mapD; i++){
      if(map[i].value != 0)
      printf("%3i,",map[i].value);
      else
      printf("%3s,","");
      if(i%30 == 0)
      printf("\n");
    }
    printf("\n");
  }

  printf("map rolled up and viewed from above, in sequential sclices\n");
  printf("from the bottom to the top\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short y = mapD - 1; y >= 0; y--){
      for(short x = mapW - 1; x >= 0; x--){
        int index = getIndex(x,z,y);
        if(map[index].value != 0)
        printf("%3i,",map[index].value);
        else
        printf("%3s,","");
      }
      printf("\n");
    }
    printf("# SLICE %2i #",z);
    for(short x = mapW - 4; x >= 0; x--){
      printf("####");
    }
    printf("\n");
  }
}

//can be placed anywhere and wil show the selection
void showMap(char* state){
  printf("MAP:%20s H:%-5iW:%-5iD:%-5i A X:%-5iZ:%-5iY:%-5i B X:%-5iZ:%-5iY:%-5i\n",state,mapH,mapW,mapD,mapXA,mapZA,mapYA,mapXB,mapZB,mapYB);
}

//can be placed anywhere and wil show the selection
void showSelection(char* state){
  printf("SEL:%20s H:%-5iW:%-5iD:%-5i A X:%-5iZ:%-5iY:%-5i B X:%-5iZ:%-5iY:%-5i\n",state,selH,selW,selD,selXA,selZA,selYA,selXB,selZB,selYB);
}

//allocate more memory
void expandBlockMap(unsigned short change, char dir){

  if(mapW > 700 || mapH > 700 || mapD > 700){
    printf("WARNING LARGE MAP!\n");
    showMap("MAP TOO BIG");
    showSelection("MAP TOO BIG");
    getchar();
    buildImmages();
  }

  if(dir == 'l' || dir == 'u' || dir == 'f') *getMapExtent(dir) += change; // twards positive numbers get bigger
  if(dir == 'r' || dir == 'd' || dir == 'b') *getMapExtent(dir) -= change; // twards negitive numbers get smaller

  unsigned short tempW = mapW;
  unsigned short tempH = mapH;
  unsigned short tempD = mapD;

  //adjust selection and map set points for shift
  if     (dir == 'r'){ selXA += change; selXB += change; mapXA += change; mapXB += change; }
  else if(dir == 'd'){ selZA += change; selZB += change; mapZA += change; mapZB += change; }
  else if(dir == 'b'){ selYA += change; selYB += change; mapYA += change; mapYB += change; }

  struct block*** cuboid = rollMap(map);

  //calculate the highth the width and the breadth...
  if     (dir == 'r' || dir == 'l') mapW = abs(mapXB - mapXA) + 1;//if the direction is left or right the map will get thinner
  else if(dir == 'u' || dir == 'd') mapH = abs(mapZB - mapZA) + 1;//if the direction is up or down the map will get shorter
  else if(dir == 'f' || dir == 'b') mapD = abs(mapYB - mapYA) + 1;//if the direction is forward or backward the map will get shallower

  //memory allocation
  if(dir == 'r' || dir == 'l'){
    cuboid = realloc(cuboid,mapW * sizeof(struct block**));
    for(short x = tempW; x < mapW; x++){
      cuboid[x] = malloc(mapH * sizeof(struct block*));  // <here
      for(short z = 0; z < mapH; z++){
        cuboid[x][z] = malloc(mapD * sizeof(struct block)); // <here
        for(short y = 0; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].reldir = 0;
        }
      }
    }
    //shift data
    if(dir == 'r'){
      unsigned short shift_Amt = mapW - tempW;
      for(short x = mapW - 1; x >= shift_Amt; x--){
        cuboid[x] = cuboid[x-shift_Amt];
      }
      //reallocate new empty space (unshifted portion)
      for(short x = shift_Amt - 1; x >= 0; x--){
        cuboid[x] = malloc(mapH * sizeof(struct block*));
        for(short z = 0; z < mapH; z++){
          cuboid[x][z] = malloc(mapD * sizeof(struct block));
          for(short y = 0; y < mapD; y++){
            cuboid[x][z][y].type = 0;
            cuboid[x][z][y].value = 0;
            cuboid[x][z][y].reldir = 0;
          }
        }
      }
    }
  }

  else if(dir == 'u' || dir == 'd'){
    for(short x = 0; x < mapW; x++){
      cuboid[x] = realloc(cuboid[x],mapH * sizeof(struct block*));
      for(short z = tempH; z < mapH; z++){
        cuboid[x][z] = malloc(mapD * sizeof(struct block));  // <here
        for(short y = 0; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].reldir = 0;
        }
      }
      //shift data
      if(dir == 'd'){
        unsigned short shift_Amt = mapH - tempH;
        for(short z = mapH - 1; z >= shift_Amt; z--){
          cuboid[x][z] = cuboid[x][z-shift_Amt];
        }
        //reallocate new empty space (unshifted portion)
        for(short z = shift_Amt - 1; z >= 0; z--){
          cuboid[x][z] = malloc(mapD * sizeof(struct block));
          for(short y = 0; y < mapD; y++){
            cuboid[x][z][y].type = 0;
            cuboid[x][z][y].value = 0;
            cuboid[x][z][y].reldir = 0;
          }
        }
      }
    }
  }

  else if(dir == 'f' || dir == 'b'){
    for(short x = 0; x < mapW; x++){
      for(short z = 0; z < mapH; z++){
        cuboid[x][z] = realloc(cuboid[x][z],mapD * sizeof(struct block));
        for(short y = tempD; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].reldir = 0;
        }
        //shift data
        if(dir == 'b'){
          unsigned short shift_Amt = mapD - tempD;
          for(short y = mapD - 1; y >= shift_Amt; y--){
            cuboid[x][z][y].type = cuboid[x][z][y-shift_Amt].type;
            cuboid[x][z][y].value = cuboid[x][z][y-shift_Amt].value;
            cuboid[x][z][y].reldir = cuboid[x][z][y-shift_Amt].reldir;
          }
          //0 the data unshifted
          for(short y = shift_Amt - 1; y >= 0; y--){
            cuboid[x][z][y].type = 0;
            cuboid[x][z][y].value = 0;
            cuboid[x][z][y].reldir = 0;
          }
        }
      }
    }
  }
  map = unRollMap(cuboid,map);
}

void checkAndExpand(unsigned short distance,char dir){

  //test a shifted cuboid to //moves, stacks and annything you want to check at a distance
  //if distacne is 0 the test is the selection ;)
  short selPos = *getSelExtent(dir) + distance; // twards positive numbers
  short selNeg = *getSelExtent(dir) - distance; // twards negitive numbers

  short getmapExtent = *getMapExtent(dir);

  if(dir == 'l' || dir == 'u' || dir == 'f'){
    if(selPos > getmapExtent){
      expandBlockMap(selPos - getmapExtent,dir);
    }
  }
  if(dir == 'r' || dir == 'd' || dir == 'b'){
    if(selNeg < getmapExtent){
      expandBlockMap(getmapExtent - selNeg,dir);
    }
  }
}

void replace(char* typeA, _Bool specificA,char valueA, char* typeB,_Bool specificB, char valueB){

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int index = getIndex(x,z,y);
        if(blockLookup(typeA) == map[index].type){
          struct block B;
          B.reldir = '\0';
          B.type = blockLookup(typeB);
          if(specificB)
          B.value = valueB;
          else
          B.value = 0;
          if((specificA && valueA == map[index].value) || !specificA){
            map[index] = B;
          }
        }
      }
    }
  }
}

void set_block(struct block B){

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int index = getIndex(x,z,y);
        map[index] = B;
      }
    }
  }
}


void overlay(char* type, unsigned char value){

  checkAndExpand(1,'u');

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short y = selExt.b; y <= selExt.f; y++){
      for(short z = selExt.u; z >= selExt.d; z--){ //height last

        int origin = getIndex(x,z,y);

        //only fill in air and the block under it is not air
        if(map[origin].type != 0){
          //get block
          struct block B;
          B.type = blockLookup(type);
          B.value = value;
          B.reldir = '\0';

          int above = getIndex(x,z+1,y);
          map[above] = B; //place block
          break; //only place 1 block per xz coordinate (only the top portion of the selected blocks)
        }
      }
    }
  }
}


//can be placed anywhere and wil shift the selection
void selectionShift(char dir, unsigned short amount){
  //smol switch (relitivlely speaking)
  checkAndExpand(amount,dir);
  switch (dir) {
    case 'l': selXA += amount; selXB += amount; break;
    case 'r': selXA -= amount; selXB -= amount; break;
    case 'u': selZA += amount; selZB += amount; break;
    case 'd': selZA -= amount; selZB -= amount; break;
    case 'f': selYA += amount; selYB += amount; break;
    case 'b': selYA -= amount; selYB -= amount; break;
  }
}

void shift(unsigned short amount,char dir){
  if(amount > 0){
    selectionShift(dir,amount);
  }
}

struct block* copySel(){
  struct extents selExt = getAllSelExtents();
  struct block* ret = malloc(selW * selH * selD * sizeof(struct block));
  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int orig_index = getIndex(x,z,y);
        int dest_index = getSubIndex(x-selExt.r,z-selExt.d,y-selExt.b,selD,selH);

        //get blocks, the right blocks and put them right in the right location
        ret[dest_index] = map[orig_index];

        //remove old block
        map[orig_index].type = 0;
        map[orig_index].value = 0;
        map[orig_index].reldir = '\0';
      }
    }
  }
  return ret;
}

void pasteSel(struct block* arg,short amount, char dir){

  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){

        //calculate new position of block. Keep unchanged coordinates the same with default values
        unsigned short blockBX = x;
        unsigned short blockBZ = z;
        unsigned short blockBY = y;

        switch (dir) { //must only movve in one direction
          case 'l': blockBX += amount; break;
          case 'u': blockBZ += amount; break;
          case 'f': blockBY += amount; break;
          case 'r': blockBX -= amount; break;
          case 'd': blockBZ -= amount; break;
          case 'b': blockBY -= amount; break;
        }

        //indexes
        int orig_index = getSubIndex(x-selExt.r,z-selExt.d,y-selExt.b,selD,selH);
        int dest_index = getIndex(blockBX,blockBZ,blockBY);

        //place block
        map[dest_index] = arg[orig_index];
      }
    }
  }
}

void move(unsigned short amount,_Bool shift, char dir){
  //in memory
  struct block* temp = copySel();
  //although these forloops are identical i must run through them twice or els ill run into undisiered effects of the blocks being moved multiple times
  pasteSel(temp,amount,dir);

  //free the copy
  free(temp);

  //shift after operation
  if (shift){
    //looks framilliar? it should its the smol switch again, whre moving the selection
    selectionShift(dir,amount);
  }
}

void stack(short amount,char dir, char* options){
  if(amount > 0){

    _Bool preserveAir = 0;
    _Bool sel_shift = 0;

    for(unsigned int i = 0; i < strlen(options); i++){
      preserveAir |= options[i] == 'a';
      sel_shift |= options[i] == 's';
    }

    //this is going to be a pain..
    //check last stack tile distacne
    //determin how much to shift the test selection based on the size of the dimention of the
    //selection paralel to the direction...
    short shift = 0;
    switch (dir) {
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
    checkAndExpand(shift,dir);

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

            switch (dir) { //now direction really matters
              case 'l': blockBX += selW * (p+1); break;
              case 'u': blockBY += selH * (p+1); break;
              case 'f': blockBZ += selD * (p+1); break;
              case 'r': blockBX -= selW * (p+1); break; //1h so changing the sign is all i have to do here well that was easyer that i thought it would be...
              case 'd': blockBY -= selH * (p+1); break;
              case 'b': blockBZ -= selD * (p+1); break;
            }

            int orig_index = getIndex(i,j,k);
            int dest_index = getIndex(blockBX,blockBY,blockBZ);

            //new block = old block
            if(!preserveAir || !map[orig_index].type == 0){
              // printf("%i > %i-%i\n",map[orig_index].type,dest_index,map[dest_index].type);
              map[dest_index] = map[orig_index];
            }
          }
        }
      }
    }

    //shift after block operation
    if(sel_shift){
      //looks framilliar? it should its the smol switch again, whre moving the selection
      selectionShift(dir,shift);
    }
  }
}

void updateSelDementions(char dir){
  if(dir == 'u' || dir == 'd')//if the direction is up or down the selection will get shorter
  selH = (*getSelExtent('u') - *getSelExtent('d')) + 1;
  else if(dir == 'l' || dir == 'r')//if the direction is left or right the selection will get thinner
  selW = (*getSelExtent('l') - *getSelExtent('r')) + 1;
  else if(dir == 'f' || dir == 'b')//if the direction is forward or backward the selection will get shallower
  selD = (*getSelExtent('f') - *getSelExtent('b')) + 1;
}

char opp_dir(char dir){
  switch (dir) {
    case 'l': return 'r';
    case 'r': return 'l';
    case 'u': return 'd';
    case 'd': return 'u';
    case 'b': return 'f';
    case 'f': return 'b';
    default : printf("%15s - Invalid direction received :%c > %i\n","opp_dir()",dir,dir);
  }
  return '\0';
}

void expand(unsigned short amount,char dir){
  if(amount > 0){
    //check to allocate memory to expand into
    checkAndExpand(amount,dir);

    //this is a lot smaller than the 1ge switch i had before lol
    if(dir == 'l' || dir == 'u' || dir == 'f'){
      *getSelExtent(dir) += amount;
    }
    if(dir == 'r' || dir == 'd' || dir == 'b'){
      *getSelExtent(dir) -= amount;
    }

    //calculate the hight the selW and the breth
    updateSelDementions(dir);
  }
}


void contract(unsigned short amount,char dir){

  //no need to check for map expansion lol im contracting
  if(dir == 'l' || dir == 'u' || dir == 'f'){
    *getSelExtent(opp_dir(dir)) += amount;
  }
  if(dir == 'r' || dir == 'd' || dir == 'b'){
    *getSelExtent(opp_dir(dir)) -= amount;
  }

  //calculate the hight the selW and the breth
  updateSelDementions(dir);
}

//turns direction 90 degrees left or right from current direction
char turndir(char dir, char turndir){
  char dirNum = 0; //default number negitive to catch errors
  switch (dir) { //base on 1 so never goes below 0, unless there is an error
    case 'r': dirNum = 0; break;
    case 'f': dirNum = 1; break;
    case 'l': dirNum = 2; break;
    case 'b': dirNum = 3; break;
    default : printf("%15s - Invalid direction received :%c > %i\n","turndir()",dir,dir);
  }
  if(turndir == 'l'){
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

void setr(char dir,unsigned char m){

  //in memory
  unsigned char offset = 0;
  switch (dir) {
    case 'f': offset = 0; break;
    case 'r': offset = 1; break;
    case 'b': offset = 2; break;
    case 'l': offset = 3; break;
  }

  //pack B
  struct block B;
  B.type = blockLookup("redstone_repeater");
  B.value = (((1 + offset) % 4) + (m * 4));
  B.reldir = dir;

  set_block(B);
}

void sett(char dir){
  //pack B
  struct block B;
  B.type = blockLookup("redstone_torch");
  B.reldir = dir;
  B.powered = 0;

  //even bigger switch
  switch (dir) {
    case 'u': B.value = 5; break;
    case 'r': B.value = 3; break;
    case 'l': B.value = 4; break;
    case 'f': B.value = 1; break;
    case 'b': B.value = 2; break;
    default: B.value = 5; printf("INVALID TORCH DIRECTION RECIEVED\n"); break;
  }
  set_block(B);
}

void seto(char dir){

  //pack B
  struct block B;
  B.type = blockLookup("observer");
  B.reldir = dir;

  //even bigger switch mk 2
  switch (dir) {
    case 'u': B.value = 0; break;
    case 'd': B.value = 1; break;
    case 'r': B.value = 3; break;
    case 'l': B.value = 2; break;
    case 'f': B.value = 4; break;
    case 'b': B.value = 5; break;
  }
  set_block(B);

}

void setp(char dir, _Bool sticky){

  char* type;

  if(sticky)  type = "sticky_piston";
  else        type = "piston";

  //pack B
  struct block B;
  B.type = blockLookup(type);
  B.reldir = dir;

  //even bigger switch mk 3
  switch (dir) {
    case 'u': B.value = 1; break;
    case 'd': B.value = 0; break;
    case 'l': B.value = 2; break;
    case 'r': B.value = 3; break;
    case 'b': B.value = 4; break;
    case 'f': B.value = 5; break;
  }
  set_block(B);
}

unsigned char wire(short in_strength,unsigned char out_toll, unsigned char value, unsigned short amount, char dir){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.reldir = '\0';

  //current position of end of selection
  unsigned short pos = 0;
  //current state of the output strength
  short out_strength = in_strength - amount;
  //support structure for redstone
  set_block(B);
  stack(amount,dir,"");
  //shift up to do redstone
  shift(1,'u');

  //check to see if the wire is longer than the input strength
  //if so it is safe to place the first repeater where the signal ends
  if(in_strength < amount && out_strength < out_toll){
    shift(in_strength,dir);
    pos += in_strength;
    setr(dir,0);
    out_strength = 16 + pos - amount;
  }
  //check to see if there is enough room for at least 1 full 16 line

  if((amount - in_strength - 1) >= 16){
    shift(16,dir);
    setr(dir,0);
    //check to see if there is at least one more if so use stack to fill in the rest
    expand(15,opp_dir(dir));
    //-s option ensures im at the end of the lines and in position for the final line if needed
    stack(((amount - in_strength) / 16) - 1,dir,"s");
    contract(15,dir);

    pos += ((amount - in_strength) / 16) * 16;
    out_strength = 16 + pos - amount;
    short num = amount - in_strength;
    if(((num/16)*16) == num){
      move(1,0,opp_dir(dir));
      out_strength--;
    }
  }
  //if the output is still too weak go to the end and place a repeater just behind it
  //this will boost the output to the proper strength reguardless of previous actions
  if(out_strength < out_toll){
    shift(amount - pos - 1,dir);
    pos += amount - pos - 1;
    setr(dir,0);
    out_strength = 16 + pos - amount;
    shift(1,dir);
    out_strength = 15;
  }
  //if the output is proper strength go to end
  else{
    shift(amount - pos,dir);
    pos += amount - pos;
  }
  //place redstone
  expand(amount,opp_dir(dir));
  replace("air",0,0,"redstone",0,0);
  contract(amount,dir);
  //go back down into end position
  shift(1,'d');

  return out_strength;
}

struct point{
  short x;
  short z;
  short y;
};

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

        //get the vector from a to c and from c to b
        struct vector aToc = getPointVector(a,c);
        struct vector cTob = getPointVector(b,c);

        //if the sum of the distances from a to c and from c to b are the same as from a to b, c must be on the line from a to b
        if(getDistance(aToc) + getDistance(cTob) == getDistance(aTob)){
          struct block B;
          B.type = blockLookup(type);
          B.value = value;
          B.reldir = '\0';

          int index = getIndex(i,j,k);
          //place block at point c
          map[index] = B;
        }
      }
    }
  }
}


void stairs(char* type, unsigned char value, char dir, char up, char distance){
  expand(distance,dir);
  expand(distance,up);
  line(type,value);
  shift(1,'u');
  line("redstone",0);
  shift(1,'d');
  contract(distance,dir);
  contract(distance,up);
}

//clear selection
void delete(){

  //get extents
  struct extents selExt = getAllSelExtents();

  //now the real painfull part
  for(short i = selExt.r; i <= selExt.l; i++){
    for(short j = selExt.d; j <= selExt.u; j++){
      for(short k = selExt.b; k <= selExt.f; k++){

        int index = getIndex(i,j,k);
        map[index].type = 0;
        map[index].value = 0;
        map[index].reldir = '\0';
      }
    }
  }
}

void buildDecoderTop(short inBits, char dir, char st_side, unsigned char rows, unsigned char box){

  //pack B
  struct block bottom;
  bottom.type = blockLookup("wood");
  bottom.value = 2;
  bottom.reldir = '\0';

  struct block top;
  top.type = blockLookup("wood");
  top.value = 3;
  top.reldir = '\0';

  char opp_st_side = opp_dir(st_side);

  unsigned short entries = pow(2,inBits);
  //build top structure
  //single bit
  set_block(bottom);
  overlay("redstone",0);
  shift(1,dir);
  shift(1,'u');
  seto(dir);
  shift(1,dir);
  set_block(top);
  shift(1,'d');
  setp('d',1);
  expand(2,opp_dir(dir));
  expand(1,'u');
  expand(1,opp_st_side);
  //stack it to the other 5 places
  stack(inBits - 1,opp_st_side,"");

  //change the wool collors to make it look better
  shift(2,opp_dir(st_side));
  replace("wood",1,2,"wood",1,0); //bottom
  replace("wood",1,3,"wood",1,1); //top
  //do the 3 other middle bits all at once
  stack(inBits - 3,opp_st_side,""); // -2 for not the first and last bit and an additional -1 becuase its the 3 middle bits
  shift(2,st_side);

  //expand selection to stack to rest of entries
  expand(((inBits - 1) * 2) - 1,opp_st_side);
  //stack it to the rest of the entries
  stack(entries - 1,dir,"");

  //change the wool collors to make it look better
  for(short j = 0; j < rows; j++){
    for(short k = 0; k < 4; k++){
      //took me quite a while to come up with this specific pattern, it better look good
      int collor = collorMap[((box >> 2) & 1) ^ ((k >> 1) & 1)][(k & 1)][(box&3) ^ j];
      replace("wood",1,k,"wool",1,collor); //bottom
    }
    stack((entries / rows) - 1,dir,"");
    if(j != rows - 1){
      shift(entries / rows * 3,dir);
    }
  }
  shift((entries / rows * 3) * (rows - 1),opp_dir(dir));

  //go to binary Pattern
  contract((inBits - 1) * 2,st_side);
  contract(2,dir);
  contract(1,'d');
  shift(1,'d');
}

void fillBinPtrn(short inBits, unsigned char significance[], char dir, char st_side){

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Block");
  B.value = 0;
  B.reldir = '\0';

  char oppDir = opp_dir(dir);

  //number of entries
  unsigned short entries = pow(2,inBits);

  //encode the decoder (binary with custom bit encoding)
  for(short iB = 0; iB < inBits; iB++){
    //create bases
    //bottom side (inverted bits)
    shift(1,'d');
    set_block(B);
    if(significance[iB] > 1){
      expand(2,dir);
      stack(significance[iB]-1,dir,"");
      contract(2,oppDir);
    }
    //top side (normal bits)
    shift(1,'u');
    shift(significance[iB] * 3,dir);
    set_block(B);
    if(significance[iB] > 1){
      expand(2,oppDir);
      stack(significance[iB]-1,dir,"");
      contract(2,dir);
    }
    shift(significance[iB] * 3,oppDir);

    //stack to rest of entries
    if(entries / significance[iB] > 2){
      //expand to get both sides
      expand(1,'d');
      expand(6*significance[iB]-1,dir);
      stack((entries / (significance[iB] * 2)-1),dir,"a");
      contract(6*significance[iB]-1,oppDir);
      contract(1,'u');
    }
    //go to next bit
    if(iB != inBits - 1){
      shift(2,opp_dir(st_side));
    }
  }

  //return from pattern
  shift(((inBits - 1) * 2),st_side);
  shift(1,'u');
  shift(2,oppDir);
}

void buildDecoder(short inBits,char dir,char st_side,unsigned char rows,unsigned char significance[],unsigned char box){
  //scripted actions to build top structure
  buildDecoderTop(inBits,dir,st_side,rows,box);
  //fills in binary pattern
  fillBinPtrn(inBits,significance,dir,st_side);
}

void invertor(char value, char dir){
  struct block B;
  B.value = value;
  B.type = blockLookup("wool");
  B.reldir = '\0';
  shift(1,opp_dir(dir));
  set_block(B);
  shift(1,dir);
  sett(dir);
}

void t_flip(char dir){

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Block");
  B.value = 0;
  B.reldir = '\0';

  shift(1,'u');
  shift(1,dir);
  setp(dir,1);
  shift(1,dir);
  set_block(B);
  shift(1,dir);
  shift(1,'d');
}

unsigned char buildOutLines(unsigned char outBits, short inBits,char in_side, char outSide, char st_side){

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

  //pack B
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.reldir = '\0';

  //pack B
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 7;
  B2.reldir = '\0';

  //create 1 1x15 wire section
  set_block(B1); //a little touch to make it look a little better
  shift(1,opp_dir(in_side));
  set_block(B2);
  stack(13,opp_dir(in_side),""); //1x15 wire section
  shift(1,in_side);
  shift(1,'u');
  setr(outSide,0); //place first repeater at end of first output wire
  expand(14,opp_dir(in_side));
  replace("air",0,0,"redstone",1,0); //plce redstone on wire

  expand(1,st_side);
  expand(1,'d');


  //outputs remaining to wire
  unsigned char bits = outBits;

  expand(2,'d');
  if(num_levels > 1){
    stack(inBits - 2,opp_dir(st_side),"");
    bits -= inBits - 1;
    stack(num_levels - 1,'d',"");
    shift(4,'d');
    if(num_levels > 2){
      expand((num_levels - 3) * 4,'d');
      stack(inBits - 2,opp_dir(st_side),"");
      contract((num_levels - 3) * 4,'d');
      bits -= (inBits - 1) * (num_levels - 2); //just made a (inBits - 2) x (num_levels - 2) box of out lines
      shift(4,'d');
    }
    //last leel
    //prevent the area around the encoder from being affected by a stack
    move(1,0,st_side);
    shift(1,st_side);
    stack(1,opp_dir(st_side),"");
    shift(1,opp_dir(st_side));
    stack(bits - 1,opp_dir(st_side),"");
    shift((num_levels - 1) * 4,'u');
  }
  else{
    stack(bits - 1,opp_dir(st_side),"");
  }


  //extend wire sections to the end
  expand(((inBits - 1) * 2) - 1,opp_dir(st_side)); //expand selection to selecto the 4 1x15 wire sections (expand 2 extra for next operation)
  expand((num_levels - 1) * 4,'d');

  unsigned char wireStacks = entries / 5; // the number of enteries * the length of each entry / the length or each line reduced
  unsigned char trimAmt = (entries * 3) - ((wireStacks) * 15) + 1;

  //trim of ends
  stack(1,opp_dir(in_side),""); //stack the 4 1x15 wire once
  shift(trimAmt,opp_dir(in_side));
  stack(wireStacks-1,opp_dir(in_side),""); //then stack to the rest 1() - with the strange shift
  shift(trimAmt,in_side);

  unsigned char out_strength;

  if(in_side == outSide){
    out_strength = 15;
  }
  else{
    out_strength = trimAmt;
  }

  //negate expantion for wire extension and bring selection to 1x1x1
  contract(((inBits - 1) * 2), st_side);
  contract((num_levels * 4) - 1,'u');
  contract(3,opp_dir(in_side));
  contract(11,in_side);

  //shift selection up into position for next operation
  shift(1,'u');
  shift(1,st_side);

  return out_strength;
}

void buildandSupport(short inBits, unsigned char outBits, short in_side, char st_side){

  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = 4;
  B.reldir = '\0';

  char opp_in_side = opp_dir(in_side);
  char opp_st_side = opp_dir(st_side);

  set_block(B);
  overlay("redstone",0);
  expand(1,'u'); //include redstone
  expand(2,opp_in_side); //make selection 3 blocks wide
  stack(entries-1,opp_in_side,""); //stack to rest of entries
  expand((entries-1) * 3,opp_in_side); //expand selection to rest of entries

  if(num_levels > 1){
    expand(2,'d');
    stack(num_levels - 1,'d',"");
    expand((num_levels - 1) * 4,'d');
    //stack into structure with a option to not overwrite blocks
    stack((inBits * 2) - 1,opp_st_side,"a");
    delete(); //remove start blocks
    contract(((num_levels - 1) * 4) + 2,'u');
  }
  else{
    stack((inBits * 2) - 1,opp_st_side,"a");
    delete(); //remove start blocks
  }

  //collaps and shift selection for encoding
  contract(((entries-1) * 3) + 2,in_side);
  contract(1,'d');
  shift(1,opp_st_side);
}

void Write(unsigned char value,char in_side){
  replace("wool",1,4,"wool",1,value);//looks better i think
  shift(1,in_side);
  sett(in_side);
  shift(2,'d');
  replace("wool",1,7,"wool",1,value);//looks better i think
  shift(2,'u');
  shift(1,opp_dir(in_side));
}

void dataEntry(unsigned char value, unsigned char entry,char in_side,char st_side, unsigned char max){

  // buildImmages();
  // getchar();

  //next entry
  unsigned char shiftcount = 1;
  unsigned char pos = 0;
  for(short oB = max - 1; oB >= 0; oB--){ //get the most significant bit first
    if((1 & entry >> oB)){
      shift(shiftcount,opp_dir(st_side));
      pos += shiftcount;
      Write(value,in_side);
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
    shift(pos,st_side);
    pos -= pos;
  }
}

void lvlDown(char st_side){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = 4;
  B.reldir = '\0';
  B.powered = 0;

  shift(1,st_side);
  sett(st_side);
  shift(2,'d');
  set_block(B);
  overlay("redstone",0);
  shift(1,opp_dir(st_side));
  sett(opp_dir(st_side));
  shift(2,'d');
}

unsigned char buildEncoder(unsigned char rows, unsigned char collumns,unsigned char Table[rows][collumns],short inBits, unsigned char outBits, char st_side,char in_side, char outSide){

  //how manny num_levels of outputs will i need?
  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  //build encoder sturcture middle section (big nand gate per entry)

  //get to start of output wires
  shift(4,'d');
  shift(1,opp_dir(st_side));

  unsigned char out_strength;

  //nothing mutch to see here just some scripted actions to build support structures
  out_strength = buildOutLines(outBits,inBits,in_side,outSide,st_side);
  buildandSupport(inBits,outBits,in_side,st_side);

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
            shift(1,st_side);
          }
          unsigned char dataValue = entry % 16;
          dataEntry(dataValue,partition,in_side,st_side, max);
          if (level == num_levels - 1 && num_levels > 1){
            shift(1,opp_dir(st_side));
          }
          if((level != num_levels - 1) && (entry >> (outBits - (max + offset))) != 0){ //only encode data if there is data left to encode
            lvlDown(st_side); //some more scripted actions
            towers++; //keep track of how far down i am
          }
          offset += max;
        }
        if(num_levels > 1){
          shift(towers * 4,'u');
        }
      }
      //next entry
      shift(3,opp_dir(in_side));
    }
  }
  //go back to origin
  shift((entries * 3) + 3,in_side);
  shift(2,'u');
  return out_strength;
}

void lseg(char dir){
  //pack B
  struct block B;
  B.type = blockLookup("redstone_Lamp");
  B.value = 0;
  B.reldir = '\0';

  shift(1,dir);
  set_block(B);
  stack(2,dir,"");
  shift(1,opp_dir(dir));
}

void seperateNibble(char* type, unsigned char value, char in_side, char st_side){

  //pack B
  struct block B;
  B.type = blockLookup(type);
  B.value = value;
  B.reldir = '\0';

  shift(6,opp_dir(in_side));
  shift(2,'d');
  shift(1,st_side);
  set_block(B);
  expand(1,'u');
  stack(5,'u',"");
  contract(1,'d');
  shift(2,'u');
  shift(6,in_side);
  shift(1,opp_dir(st_side));
}

void dLights(unsigned char value, char st_side, char in_side){

  char opp_st_side = opp_dir(st_side);

  //get to corner of display
  shift(1,st_side);
  shift(5,opp_dir(in_side));
  shift(3,'d');

  //pack ice
  struct block ice;
  ice.type = blockLookup("ice");
  ice.value = value;
  ice.reldir = '\0';

  //pack case
  struct block casing;
  casing.type = blockLookup("wool");
  casing.value = 15;
  casing.reldir = '\0';

  expand(12,'u');
  expand(8,opp_st_side);
  expand(1,opp_dir(in_side));
  set_block(casing);
  contract(1,opp_dir(in_side));
  contract(1,opp_st_side);
  contract(1,st_side);
  replace("wool",0,0,"wool",1,value);
  contract(1,opp_dir('u'));
  contract(1,opp_dir('d'));
  set_block(ice);
  shift(1,in_side);
  contract(1,opp_st_side);
  contract(5,st_side);
  contract(9,'d');
  contract(1,'u');

  for(short i = 0; i < 2;i++){
    lseg('u');
    lseg('r');
    shift(4,'u');
  }
  shift(4,opp_st_side);
  for(short i = 0; i < 2;i++){
    lseg('d');
    lseg('l');
    if(!i) shift(4,'d');
    else   shift(3,'d');
  }
  //return to origin
  shift(5,st_side);
  shift(5,in_side);
}

void LWire(char dir, char in_side, _Bool move){
  unsigned char value = 7;
  if(move){
    wire(10,4,value,2,opp_dir(in_side));
    shift(1,dir);
    wire(10,4,value,2,opp_dir(in_side));
  }
  else{
    wire(10,4,value,4,opp_dir(in_side));
  }
  shift(4,in_side);
}

void swire(char Dir, char up, char inSide, _Bool move){
  char* type = "wool";
  unsigned char value = 7;
  stairs(type,value,opp_dir(inSide),up,2);
  if(move)
  shift(1,Dir);
  wire(10,4,value,2,opp_dir(inSide));
  shift(4,inSide);
  shift(2,opp_dir(up));
}


void dwires(char inSide,char stSide){

  //1
  LWire(opp_dir(stSide),inSide,1);
  shift(1,opp_dir(stSide));
  //2
  swire(opp_dir(stSide),'u',inSide,1);
  shift(1,opp_dir(stSide));
  //3
  swire(stSide,'d',inSide,1);
  shift(3,opp_dir(stSide));
  //4
  LWire(stSide,inSide,1);
  shift(4,'u');
  //5
  LWire(stSide,inSide,0);
  shift(2,stSide);
  //6
  swire(stSide,'u',inSide,0);
  shift(2,stSide);
  //7
  LWire(stSide,inSide,0);
  //return to origin
  shift(1,stSide);
}


void buildOutput(unsigned char nibbles, char st_side, char backward){

  short inBits = 4;
  unsigned char outBits = 7;
  unsigned char significance[] = {8,4,2,1};
  unsigned char rows = 1;
  unsigned char collumns = 16;

  for(unsigned char nibble = 0; nibble < nibbles; nibble++){
    buildDecoder(inBits,backward,st_side,rows,significance,nibble);
    buildEncoder(rows,collumns,segmentTable[0],inBits,outBits,st_side,opp_dir(backward),backward);

    char dataValue = (nibble / 2) % 16;

    shift(49,backward);
    shift(8,'d');
    dLights(dataValue,'l','f');
    if(nibble % 2 == 0 && nibble)
    seperateNibble("wool",7,opp_dir(backward),st_side);
    dwires('f','l');
    shift(49,opp_dir(backward));
    shift(4,'u');


    if(nibble != nibbles - 1)
    shift(inBits * 2,opp_dir(st_side));
  }
}

struct bussLocation{
  //location of bit 0
  char direction; //the direction the buss is running
  char st_side;// the side of the most significant bit
};


struct buss{
  char* name;
  unsigned char* collors;
  unsigned char* strength;
  unsigned char width;
  struct bussLocation loc;
};

void freeBuss(struct buss arg){

  if(arg.name == NULL){
    printf("NULL NAME\n");
    getchar();
  }
  char freeMessage[20];
  snprintf(freeMessage,20,"free(%s)\n",arg.name);
  free(arg.collors);
  free(arg.strength);
}

struct buss buildStables(struct buss roundKey){

  short inBits = 6;
  unsigned char outBits = 4;
  unsigned char significance[] = {32,8,4,2,1,16};
  unsigned char boxes = 8;
  unsigned char rows = 4;
  unsigned char collumns = 16;

  char dir = roundKey.loc.direction;
  char st_side = roundKey.loc.st_side;

  struct buss ret;
  ret.width = outBits * boxes;
  ret.collors = malloc(ret.width);
  ret.strength = malloc(ret.width);

  unsigned char out_strength = 0;

  for(unsigned char box = 0; box < boxes; box++){
    buildDecoder(inBits,dir,st_side,rows,significance,box);
    buildEncoder(rows,collumns,stables[box],inBits,outBits,st_side,opp_dir(dir),opp_dir(dir));
    for(int i = 0; i < outBits; i++){
      ret.collors[(box*outBits) + i] = box % 16;
    }
    if(box != boxes - 1)
    shift(inBits * 2,opp_dir(st_side));
    else{
      shift(((boxes - 1) * (inBits * 2) - 1),st_side);
      shift(4,'d');
    }
    showProgress(box,boxes,"building Stables");
  }

  for(unsigned char i = 0; i < ret.width; i++){
    ret.strength[i] = out_strength;
  }

  //terminate key
  ret.loc.direction = opp_dir(dir);
  ret.loc.st_side = st_side;
  ret.name = "Stable Results";
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
  printf("dir   :%12c   \n",arg.loc.direction);
  printf("st_side:%12c   \n",arg.loc.st_side);
  printf("*******************\n");
  printf("\n");
}

struct buss buildPermute
(
  char* name,struct buss inBuss, /*buss info 1-2*/
  unsigned char tableLength,unsigned char permTable[tableLength], /*permTable info 3-4*/
  unsigned char groupSize, unsigned char groupSpacing,/*group info 5-6*/
  char st_side,/*direction info 7-9*/
  _Bool startAtIn, _Bool stayAtOut /*movment info 10-11*/
){

  char forward = opp_dir(inBuss.loc.st_side);

  char in_side = opp_dir(inBuss.loc.direction);
  if(startAtIn){
    if(st_side == in_side){
      shift(2,'u');
      shift(2,opp_dir(in_side));
      shift(2,opp_dir(forward));
    }
    else{
      shift(2,'u');
      shift((tableLength) * 2,opp_dir(in_side));
      shift(2,opp_dir(forward));
    }
  }

  struct buss outBuss;
  outBuss.width = tableLength;
  outBuss.collors = malloc(tableLength);
  outBuss.strength = malloc(tableLength);
  outBuss.name = name;
  outBuss.loc.direction = opp_dir(forward);
  outBuss.loc.st_side = st_side;

  for(short index = 0; index < tableLength; index++){
    unsigned char group = (permTable[index] - 1) / groupSize;
    unsigned char extLength = group * groupSpacing;
    unsigned char outLength = (permTable[index] * 2) + extLength;
    short inLength;

    //wool collor for theis line
    char dataValueIn = inBuss.collors[permTable[index] - 1];
    unsigned char inStreangth = inBuss.strength[permTable[index] - 1];

    unsigned char dataValueOut = index % 16;
    outBuss.collors[index] = dataValueOut;

    //go to end
    shift(outLength,forward);
    //go down to bottom
    shift(2,'d');

    //determin input length based on side
    if(st_side == in_side)
    inLength = (index + 1) * 2;
    else
    inLength = ((tableLength - index)*2);

    //shift to input
    shift(inLength,in_side);

    //wire to input
    inStreangth = wire(inStreangth,4,dataValueIn,inLength,opp_dir(in_side));

    //where the busses meet
    stairs("redstone_Lamp",0,opp_dir(forward),'u',2);

    //wire to output
    inStreangth = wire(inStreangth,4,dataValueOut,outLength - 2,opp_dir(forward));

    //dont shift at the end
    if(index != tableLength - 1){
      shift(2,opp_dir(st_side));
    }

    //output reached record its strength
    outBuss.strength[index] = inStreangth;
  }
  shift((tableLength -1) * 2,st_side);

  if(!stayAtOut){
    shift(2,'d');
    if(st_side == in_side) shift(2,in_side);
    else                   shift(tableLength * 2,in_side);
    shift(2,forward);
  }

  return outBuss;
}

void tower(unsigned short distance,char up, unsigned char value){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.reldir = '\0';

  distance--;
  if(distance > 0){
    if(up == 'u'){
      shift(2,'u');
      seto(up);
      if(distance > 1){
        stack(distance - 1,'u',"s");
      }
      overlay("wool",value);
    }
    else{
      seto('d');
      if(distance > 1){
        stack(distance - 1,'d',"s");
      }
      shift(1,'d');
      set_block(B);
      shift(1,'d');
    }
  }
}

void tower2(unsigned char value,unsigned char levels){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.reldir = '\0';

  set_block(B);
  shift(1,'u');
  sett('u');
  expand(1,'d');
  stack(levels,'u',"s");
  replace("redstone_torch",0,0,"redstone",1,0);
  contract(1,'d');
}

struct buss bussUp(_Bool type, struct buss arg, unsigned short distance, char up, _Bool flip){

  if(flip){
    arg.loc.direction = opp_dir(arg.loc.direction);
  }

  char dir = arg.loc.direction;

  unsigned char width = arg.width;
  for(unsigned char i = 0; i < width; i++){

    char oppDir = opp_dir(dir);
    char opp_st_side = opp_dir(arg.loc.st_side);

    if(type){
      if(i == 0){
        tower(distance,up,arg.collors[0]);
        t_flip(dir);
        expand(distance - 2,opp_dir(up));
        expand(1,opp_st_side);
        if(up == 'd'){
          shift(3,oppDir);
          shift(2,'u');
          stack(arg.width - 1,opp_st_side,"");
        }
        else{
          shift(3,oppDir);
          stack(arg.width - 1,opp_st_side,"");
        }

        shift(1,up);
        contract(distance - 2,up);
        expand(2,dir);
        stack(arg.width - 1,opp_st_side,"");
        contract(2,oppDir);
        contract(1,arg.loc.st_side);
      }
      if(i != width - 1){

        //pack B
        struct block B;
        B.type = blockLookup("wool");
        B.reldir = '\0';
        B.value = arg.collors[i+1];

        shift(2,opp_st_side);
        set_block(B);
      }
      else{
        shift(1,'d');
        shift(3,dir);
      }
      arg.strength[i] = 14;
    }
    else{
      shift(1,'u');
      tower2(arg.collors[i],distance / 2);
      if(i != width - 1){
        shift(distance + 1,'d');
        shift(2,opp_st_side);
      }
    }
  }
  shift((width - 1) * 2,arg.loc.st_side);
  return arg;
}

struct buss turnBuss(struct buss arg, char direction,_Bool flip, unsigned char depth){
  char olddir = arg.loc.direction;
  char newdir = turndir(olddir,direction);
  char opp_olddir = opp_dir(olddir);
  char opp_newdir = opp_dir(newdir);

  short width = arg.width;

  for(int i = 0; i < width; i++){

    const unsigned char collor = arg.collors[i];
    short distance;

    if(flip){
      if(arg.loc.st_side == newdir){
        distance = ((width - 1) - i) * 2;
        if(distance > 0) arg.strength[i] = wire(arg.strength[i],1,collor,distance,olddir);
        tower(depth,'d',collor);
        distance = i * 2;
        if(distance > 0) arg.strength[i] = wire(arg.strength[i],1,collor,distance,newdir);
        if(i != width -1){
          shift(((width - 1) - i) * 2,opp_olddir);
          shift(i * 2,opp_newdir);
          shift(2,opp_newdir);
          shift(depth,'u');
        }
      }
      else{
        distance = i * 2;
        if(distance > 0) arg.strength[i] = wire(arg.strength[i],1,collor,distance,olddir);
        tower(depth,'d',collor);
        distance = ((width - 1) - i) * 2;
        if(distance > 0) arg.strength[i] = wire(arg.strength[i],1,collor,distance,newdir);
        if(i != width -1){
          shift(i * 2,opp_olddir);
          shift(((width - 1) - i) * 2,opp_newdir);
          shift(2,newdir);
          shift(depth,'u');
        }
      }
    }
    else{
      if(arg.loc.st_side == newdir){
        distance = i * 2;
        if(distance > 0){
          arg.strength[i] = wire(arg.strength[i],1,collor,distance,olddir);
          arg.strength[i] = wire(arg.strength[i],1,collor,distance,newdir);
        }
        if(i != width -1){
          shift(i * 2,opp_olddir);
          shift(i * 2,opp_newdir);
          shift(2,opp_newdir);
        }
      }
      else{
        distance = ((width - 1) - i) * 2;
        if(distance > 0){
          arg.strength[i] = wire(arg.strength[i],1,collor,distance,olddir);
          arg.strength[i] = wire(arg.strength[i],1,collor,distance,newdir);
        }
        if(i != width -1){
          shift(((width - 1) - i) * 2,opp_olddir);
          shift(((width - 1) - i) * 2,opp_newdir);
          shift(2,newdir);
        }
      }
    }
  }
  if(!flip){
    if(arg.loc.st_side != newdir){
      shift((width - 1) * 2,olddir);
    }
    else{
      shift((width - 1) * 2,opp_olddir);
    }
  }
  else{
    if(arg.loc.st_side != newdir){
      shift((width - 1) * 2,opp_olddir);
    }
    else{
      shift((width - 1) * 2,olddir);
    }
    arg.loc.st_side = opp_dir(arg.loc.st_side);

  }
  arg.loc.direction = newdir;
  arg.loc.st_side = turndir(arg.loc.st_side,direction);
  return arg;
}

struct buss bussStraight(struct buss arg, short distance){
  unsigned char width = arg.width;
  char bussdir = arg.loc.direction;
  for(unsigned char i = 0; i < width; i++){
    arg.strength[i] = wire(arg.strength[i],5,arg.collors[i],distance,bussdir);
    if(i != width - 1){
      shift(2,opp_dir(arg.loc.st_side));
      shift(distance,opp_dir(bussdir));
    }
  }
  shift((width - 1) * 2,arg.loc.st_side);
  return arg;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int KaLookup(unsigned char part){
  if(part == partLookup("ice")) return 0x030303;
  else if(part == partLookup("slime")) return 0x030303;
  else if(part == partLookup("redstone_On")) return 0x505050;
  else return 0x000000;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int KsLookup(unsigned char part){
  if(part == partLookup("ice")) return 0x030303;
  else if(part == partLookup("slime")) return 0x030303;
  else return 0x000000;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int TfLookup(unsigned char part){
  if(part == partLookup("ice")) return 0x030303;
  else if(part == partLookup("slime")) return 0x030303;
  else return 0x000000;
}

//illumination format 4 for transparrant materials
char illumLookup(unsigned char part){
  if(part == partLookup("ice")) return 4;
  else if(part == partLookup("slime")) return 4;
  else return 0;
}

//how mutch the light is disolved while passing through transparrand material
float dLookup(unsigned char part){
  if(part == partLookup("ice")) return .3;
  else if(part == partLookup("slime")) return .6;
  else return 1;
}

void printMaterial(FILE * Mtl,unsigned char value,unsigned char part){
  unsigned int Ka = KaLookup(part);
  unsigned int Kd = collorLookup(value,part);
  unsigned int Ks = KsLookup(part);
  unsigned int Tf = TfLookup(part);
  char illum = illumLookup(part);
  float d = dLookup(part);

  fprintf(Mtl,"\n############################\n\n");
  fprintf(Mtl,"newmtl %s:%i\n\n",reversepartLookup(part),value);

  fprintf(Mtl,"# Collor Data #\n");
  fprintf(Mtl,"Ka %.4f %.4f %.4f\n",(float)(Ka & R)/R,(float)(Ka & G)/G,(float)(Ka & B)/B);
  fprintf(Mtl,"Kd %.4f %.4f %.4f\n",(float)(Kd & R)/R,(float)(Kd & G)/G,(float)(Kd & B)/B);
  fprintf(Mtl,"Ks %.4f %.4f %.4f\n",(float)(Ks & R)/R,(float)(Ks & G)/G,(float)(Ks & B)/B);
  fprintf(Mtl,"Tf %.4f %.4f %.4f\n",(float)(Tf & R)/R,(float)(Tf & G)/G,(float)(Tf & B)/B);

  fprintf(Mtl,"# Reflection Data #\n");
  fprintf(Mtl,"illum %i\n",illum);
  fprintf(Mtl,"d %.1f\n",d);
}

void buildMaterialLibrary(){

  FILE * Mtl = fopen("waveFront/virtexMap.mtl","w");//file pointer

  size_t numParts = sizeof(partTable) / sizeof(partTable[0]);

  unsigned char woolID = partLookup("wool");
  unsigned char woodID = partLookup("wood");

  for(int i = 0; i < 16; i++){
    printMaterial(Mtl,i,woolID);
  }

  for(int i = 0; i < 4; i++){
    printMaterial(Mtl,i,woodID);
  }

  for(size_t i = 0; i < numParts; i++){
    if(i != woolID && i != woodID){ //exclude these because we did them in a different for loop
      printMaterial(Mtl,0,i);
    }
    showProgress(i,numParts,"printing material library");
  }
  fclose(Mtl);
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
  _Bool extend[8];
};

_Bool redType(char type){
  return type == blockLookup("redstone") || type == blockLookup("redstone_Block") || type == blockLookup("redstone_Lamp") || type == blockLookup("redstone_torch") || type == blockLookup("redstone_repeater") || type == blockLookup("piston") || type == blockLookup("sticky_piston");
}

_Bool solid(char type){
  return type == blockLookup("wood") || type == blockLookup("wool") || type == blockLookup("redstone_Lamp") || type == blockLookup("redstone_Block") || type == blockLookup("piston") || type == blockLookup("sticky_piston");
}

struct redShape getRedStoneShape(short x, short z, short y){

  _Bool lastX = (x == mapW - 1);
  _Bool frstX = (x == 0);
  _Bool lastZ = (z == mapH - 1);
  _Bool frstZ = (z == 0);
  _Bool lastY = (y == mapD - 1);
  _Bool frstY = (y == 0);

  _Bool L = !lastX && redType(map[getIndex(x+1,z,y)].type);
  _Bool R = !frstX && redType(map[getIndex(x-1,z,y)].type);
  _Bool F = !lastY && redType(map[getIndex(x,z,y+1)].type);
  _Bool B = !frstY && redType(map[getIndex(x,z,y-1)].type);

  _Bool U = !lastZ && solid(map[getIndex(x,z+1,y)].type);

  unsigned char redstoneID = blockLookup("redstone");

  _Bool LU = !lastZ && !lastX && map[getIndex(x+1,z+1,y)].type == redstoneID && !U;
  _Bool RU = !lastZ && !frstX && map[getIndex(x-1,z+1,y)].type == redstoneID && !U;
  _Bool FU = !lastZ && !lastY && map[getIndex(x,z+1,y+1)].type == redstoneID && !U;
  _Bool BU = !lastZ && !frstY && map[getIndex(x,z+1,y-1)].type == redstoneID && !U;

  _Bool LD = !lastX && !frstZ && map[getIndex(x+1,z-1,y)].type == redstoneID && !solid(map[getIndex(x+1,z,y)].type);
  _Bool RD = !frstX && !frstZ && map[getIndex(x-1,z-1,y)].type == redstoneID && !solid(map[getIndex(x-1,z,y)].type);
  _Bool FD = !frstZ && !lastY && map[getIndex(x,z-1,y+1)].type == redstoneID && !solid(map[getIndex(x,z,y+1)].type);
  _Bool BD = !frstZ && !frstY && map[getIndex(x,z-1,y-1)].type == redstoneID && !solid(map[getIndex(x,z,y-1)].type);

  struct redShape ret;

  ret.extend[0] = L || LU || LD || ((RU || RD || R) && !lastX && solid(map[getIndex(x+1,z,y)].type));
  ret.extend[1] = R || RU || RD || ((LU || LD || L) && !frstX && solid(map[getIndex(x-1,z,y)].type));
  ret.extend[2] = F || FU || FD || ((BU || BD || B) && !lastY && solid(map[getIndex(x,z,y+1)].type));
  ret.extend[3] = B || BU || BD || ((FU || FD || F) && !frstY && solid(map[getIndex(x,z,y-1)].type));

  ret.extend[4] = LU;
  ret.extend[5] = RU;
  ret.extend[6] = FU;
  ret.extend[7] = BU;

  return ret;
}

_Bool smallBlock(char type){
  _Bool small = (type == 0 || type == blockLookup("redstone") || type == blockLookup("redstone_repeater") || type == blockLookup("redstone_torch") || type == blockLookup("ice"));
  return small;
}

struct subVoxel{
  //weather or not the subVoxel exists entierly
  _Bool exist;
  //demention modifyers
  //L R F B U D
  float D_mod[6];
  //what faces are present
  _Bool faces[6];
};

struct subVoxelPack{
  //each sub subVoxel
  struct subVoxel* voxs;
  //number of subsubVoxels
  unsigned char num_vox;
};

unsigned char numeratedir(char dir){
  switch(dir){
    case 'l': return 0;
    case 'r': return 1;
    case 'f': return 2;
    case 'b': return 3;
    case 'u': return 4;
    case 'd': return 5;
    //error
    default : printf("%15s - Invalid direction received :%c > %i\n","numeratedir()",dir,dir);
  }
  return 0;
}

struct subVoxelPack getsubVoxels(short x, short z, short y){

  int index = getIndex(x,z,y);

  unsigned char type = map[index].type;
  unsigned char value = map[index].value;
  char dir = map[index].reldir;

  char adjacent[6];

  adjacent[0] = (x == mapW - 1? 0 : map[getIndex(x + 1,z,y)].type);
  adjacent[1] = (x == 0? 0 : map[getIndex(x - 1,z,y)].type);
  adjacent[2] = (y == mapD - 1? 0 : map[getIndex(x,z,y + 1)].type);
  adjacent[3] = (y == 0? 0 : map[getIndex(x,z,y - 1)].type);
  adjacent[4] = (z == mapH - 1? 0 : map[getIndex(x,z + 1,y)].type);
  adjacent[5] = (z == 0? 0 : map[getIndex(x,z - 1,y)].type);

  struct subVoxelPack ret;

  //allocate memory and record number of voxels posible
  size_t num_vox = partCount(type);
  ret.num_vox = num_vox;
  ret.voxs = malloc(num_vox * sizeof(struct subVoxel));

  for(int i = 0; i < 6; i++){
    //full cube
    ret.voxs[0].D_mod[i] = .5;
  }

  if(type == blockLookup("sticky_piston")){

    unsigned char N_RD = numeratedir(opp_dir(dir));
    unsigned char N_D = numeratedir(dir);

    //they all exist (this is only really usefull for redstone...)
    ret.voxs[0].exist = 1;
    ret.voxs[1].exist = 1;
    ret.voxs[2].exist = 1;

    float H_depth = 0.3; //head thickness
    float N_depth = -(.5 - H_depth); //negitive thickness
    float B_depth = .5 - H_depth; //base thickness
    float S_Depth = .05; //slime thickness
    float SN_depth = N_depth - .2;

    for(int i = 0; i < 6; i++){
      //default depth untill direction is considerered
      ret.voxs[2].D_mod[i] = .5 + S_Depth;
      ret.voxs[1].D_mod[i] = .5;
      //remove faces adjacent to big blocks
      ret.voxs[0].faces[i] = smallBlock(adjacent[i]);
      ret.voxs[1].faces[i] = smallBlock(adjacent[i]);
      ret.voxs[2].faces[i] = 1;
    }

    //boi do i love refactoring this is a lot better than a huge switch statement with tons of different indexes
    //DIMENTIONS
    ret.voxs[0].D_mod[N_D]  = B_depth;
    ret.voxs[1].D_mod[N_RD] = N_depth;
    ret.voxs[2].D_mod[N_RD] = SN_depth;
    //FACES
    ret.voxs[0].faces[N_D]  = 0;
    ret.voxs[1].faces[N_RD] = 0;
  }

  //a bunch of if then logic to set up possible transforms
  if(type == blockLookup("redstone")){

    float height = 0.2;
    float depth = .5 - height;
    float negHeight = -(.5 - height);

    struct redShape redShape;
    redShape = getRedStoneShape(x,z,y);
    ret.voxs[0].D_mod[4] = negHeight;
    ret.voxs[0].exist = 1;
    for(int i = 0; i < 6; i++){
      ret.voxs[0].faces[5] = smallBlock(adjacent[5]);
      ret.voxs[0].faces[4] = 1;
      ret.voxs[0].faces[i] = (i != 5 && i != 4 && !redShape.extend[i]);

      if(i != 5 && i != 4){
        ret.voxs[0].D_mod[i] = depth;
      }
    }
    for(size_t i = 1; i < num_vox; i++){
      ret.voxs[i].exist = redShape.extend[i - 1];
      if(ret.voxs[i].exist){
        ret.voxs[i].faces[5] = smallBlock(adjacent[5]);
        ret.voxs[i].faces[4] = 1;
        ret.voxs[i].D_mod[5] = .5;
        for(int j = 0; j < 4; j++){
          ret.voxs[i].faces[j] = 1;
          ret.voxs[i].D_mod[j] = depth;
        }

        if(redShape.extend[i + 3]){
          ret.voxs[i].D_mod[4] = .5 + height;
          ret.voxs[i].faces[i - 1] = 0;
        }
        else{
          ret.voxs[i].D_mod[4] = negHeight;
          ret.voxs[i].faces[i - 1] = 0;
        }

        ret.voxs[i].D_mod[i-1] = .5;

        switch(i - 1){
          case 0: ret.voxs[i].D_mod[1] = negHeight; ret.voxs[i].faces[1] = redShape.extend[i + 3]; break;
          case 1: ret.voxs[i].D_mod[0] = negHeight; ret.voxs[i].faces[0] = redShape.extend[i + 3]; break;
          case 2: ret.voxs[i].D_mod[3] = negHeight; ret.voxs[i].faces[3] = redShape.extend[i + 3]; break;
          case 3: ret.voxs[i].D_mod[2] = negHeight; ret.voxs[i].faces[2] = redShape.extend[i + 3]; break;
        }
      }
    }
  }

  else if(type == blockLookup("slab")){
    ret.voxs[0].D_mod[5] = 0; //how far up from
  }

  else if(type == blockLookup("redstone_torch")){

    unsigned char N_RD = numeratedir(opp_dir(dir));
    unsigned char N_D = numeratedir(dir);

    //both voxels always exist
    ret.voxs[0].exist = 1;
    ret.voxs[1].exist = 1;

    //defaults untill shifted
    for(int i = 0; i < 6; i++){
      //handle
      ret.voxs[0].D_mod[i] = .15;
      //head
      ret.voxs[1].D_mod[i] = .2; //slightly thiccer
      //all faces present by default undill direction is considered
      ret.voxs[1].faces[i] = 1;
      ret.voxs[0].faces[i] = 1;
    }

    //extend base of handle twards connecting block
    //shift head over a bit awway from connecting block moves .1 tward dir

    //DIMENTIONS
    ret.voxs[0].D_mod[N_RD] = .5;
    ret.voxs[1].D_mod[N_RD] = .1;
    ret.voxs[1].D_mod[N_D] = .3;
    //FACES
    ret.voxs[0].faces[N_D] = 0;
    ret.voxs[0].faces[N_RD] = 0;
  }

  //"redstone_repeater","redstone_Tile","redstone_Off","generic_Wood","redstone_Off"},
  else if(type == blockLookup("redstone_repeater")){

    unsigned char t = value / 4; //repeater t

    unsigned char N_RD = numeratedir(opp_dir(dir));
    unsigned char N_D = numeratedir(dir);

    float head_width = .12;  // head_width
    float head_spaceing = .04;  // head_spaceing
    float tile_Height = -.2;  // tile_Height
    float head_Height = -.1; // head_Height
    float slider_thickness = .04;  // slider_thickness

    for(int i = 0; i < 6; i++){
      //all voxels exist
      ret.voxs[i].exist = 1;
      //all true, unless i is 5 and the block below isnt small (or air)
      ret.voxs[0].faces[i] = !(i == 5 && !smallBlock(adjacent[5]));
      //all faces exist by default
      for(unsigned char j = 1; j < num_vox; j++){
        ret.voxs[j].faces[i] = 1;
        if(!(j == 2 || j == 3)){
          ret.voxs[j].D_mod[i] = head_width / 2;
        }
      }

      //these have dirrerent dimentions
      ret.voxs[2].D_mod[i] = (head_width - head_spaceing) / 2;
      ret.voxs[3].D_mod[i] = (head_width - head_spaceing) / 2;
    }

    //DIMENTIONS
    //voxel 2 sits ontop of voxel 1
    ret.voxs[1].D_mod[4] =   tile_Height + slider_thickness;
    ret.voxs[2].D_mod[5] = -(tile_Height + slider_thickness);

    //voxels 4 and 5 have the same height
    ret.voxs[4].D_mod[4] = head_Height + head_width;
    ret.voxs[5].D_mod[4] = head_Height + head_width;

    //voxel 1 and 3 sit on voxel 0
    ret.voxs[0].D_mod[4] =  tile_Height;
    ret.voxs[1].D_mod[5] = -tile_Height;
    ret.voxs[3].D_mod[5] = -tile_Height;

    //voxels 4 and 5 both share the same height and sit on the same plane ontop of voxels 2 and 3
    ret.voxs[2].D_mod[4] =  head_Height;
    ret.voxs[3].D_mod[4] =  head_Height;
    ret.voxs[4].D_mod[5] = -head_Height;
    ret.voxs[5].D_mod[5] = -head_Height;

    //voxel 1 has a width 0f 4 units
    ret.voxs[1].D_mod[N_D]  =  2*head_width + 2*head_spaceing;
    ret.voxs[1].D_mod[N_RD] =  2*head_width + 2*head_spaceing;

    //math... these have to do with setting the depth of each voxel for tick length and direction of repeater
    ret.voxs[2].D_mod[N_D]  = -head_width*t - head_spaceing*t + 2*head_width +   head_spaceing;
    ret.voxs[2].D_mod[N_RD] =  head_width*t + head_spaceing*t -   head_width - 2*head_spaceing;

    ret.voxs[3].D_mod[N_D]  =  3*head_width + 2*head_spaceing;
    ret.voxs[3].D_mod[N_RD] = -2*head_width - 3*head_spaceing;

    ret.voxs[5].D_mod[N_D]  =  3*head_width + 5*head_spaceing/2;
    ret.voxs[5].D_mod[N_RD] = -2*head_width - 5*head_spaceing/2;

    ret.voxs[4].D_mod[N_D]  = -head_width*t - head_spaceing*t + 2*head_width + 2*head_spaceing - head_spaceing/2;
    ret.voxs[4].D_mod[N_RD] =  head_width*t + head_spaceing*t -   head_width -   head_spaceing - head_spaceing/2;


    //removing unnesisary faces (tops and bottoms of torch handle and bottom of slider)
    //FACES
    ret.voxs[1].faces[5] = 0;
    ret.voxs[2].faces[4] = 0;
    ret.voxs[2].faces[5] = 0;
    ret.voxs[3].faces[4] = 0;
    ret.voxs[3].faces[5] = 0;
  }

  else if(type == blockLookup("ice")){
    ret.voxs[0].exist = 1;
    for(int i = 0; i < 6; i++){
      //only true if the adjacent block is small
      ret.voxs[0].faces[i] = adjacent[i] == 0;
    }
  }

  else{
    ret.voxs[0].exist = 1;
    for(int i = 0; i < 6; i++){
      //only place face if adjacent side is small or if the block above is a repeater
      ret.voxs[0].faces[i] = smallBlock(adjacent[i]) && !(i == 4 && adjacent[4] == blockLookup("redstone_repeater")); //this is nice
    }
  }
  return ret;
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
  fprintf(Obj,"v %.2f %.2f %.2f\n",v.x,v.z,v.y);
}

void printsubVoxel(FILE * Obj, struct subVoxel vox, int vc, short x, short z, short y){

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
    if(vMap[i][0]) v[i].x = x + vox.D_mod[0];
    else           v[i].x = x - vox.D_mod[1];
    if(vMap[i][2]) v[i].y = y + vox.D_mod[2];
    else           v[i].y = y - vox.D_mod[3];
    if(vMap[i][1]) v[i].z = z + vox.D_mod[4];
    else           v[i].z = z - vox.D_mod[5];
  }

  for(int i = 0; i < 8; i++){
    printVirtex(Obj,v[i]);
  }

  unsigned char referenceTable[6][4] = {
    {6,4,1,5}, //left
    {8,2,3,7}, //right
    {7,3,4,6}, //front
    {5,1,2,8}, //back
    {5,8,7,6}, //top
    {1,4,3,2}, //bottom
  };

  //refectored aww yeee :D
  for( int i = 0; i < 6; i++){
    if(vox.faces[i]){
      printFaceQuad(Obj,referenceTable[i],vc - 8);
    }
  }
}

//create .obj file from bock map
void buildWaveFront(){
  FILE * Obj = fopen("waveFront/virtexMap.obj","w");//file pointer

  fprintf(Obj,"mtllib virtexMap.mtl\n");

  int vc = 0;//virtex count
  unsigned char partID = 0;
  unsigned char data = 0;
  for(short x = 0; x < mapW; x++){
    for(short z = 0; z < mapH; z++){
      for(short y = 0; y < mapD; y++){
        int index = getIndex(x,z,y);
        if(map[index].type != 0){
          char type = map[index].type;
          unsigned char tempData = map[index].value;
          struct subVoxelPack voxPack;
          voxPack = getsubVoxels(x,z,y);
          for(int i = 0; i < voxPack.num_vox; i++){
            if(voxPack.voxs[i].exist){
              char* partname = getPartnameForBlock(type,i);
              unsigned char tempID = partLookup(partname);
              //only print the material type if its different from the last
              if(tempID != partID || tempData != data){
                //save laste material data
                data = tempData;
                partID = tempID;
                fprintf(Obj,"usemtl %s:",partname);
                //these blocks have different materials based on the datavalue
                if(type == blockLookup("wool") || type == blockLookup("wood"))
                fprintf(Obj,"%i\n",data);
                //the rest are just 0
                else fprintf(Obj,"0\n");
              }
              vc += 8; //current count of all virtexes
              printsubVoxel(Obj,voxPack.voxs[i],vc,x,z,y);
            }
          }
          free(voxPack.voxs);
        }
      }
    }
    showProgress(x,mapW,"printing waveFront object");
  }
  fclose(Obj); //close file pointer
}

struct buss bussstairs(struct buss arg, short distance, char up){
  char dir = arg.loc.direction;
  unsigned char width = arg.width;
  for(unsigned char i = 0; i < width; i++){
    stairs("wool",arg.collors[i],dir,up,distance);
    if(i != width - 1){
      shift(2,opp_dir(arg.loc.st_side));
      shift(distance,opp_dir(up));
      shift(distance,opp_dir(dir));
    }
  }
  shift((width - 1) * 2,arg.loc.st_side);
  return arg;
}

struct buss createBuss(char* name,unsigned char width,char direction, char st_side){
  struct buss ret;
  ret.name = name;
  ret.width = width;
  ret.collors = malloc(width);
  ret.strength = malloc(width);
  ret.loc.st_side = st_side;
  ret.loc.direction = direction;
  for(int i = 0; i < width; i++){
    ret.strength[i] = 3;
    ret.collors[i] = i % 16;
  }
  return ret;
}

void freeBlockMap(){
  printf("free W%i D%i H%i %lu \n",mapW,mapD,mapH,mapW*mapD*mapH * sizeof(struct block));
  free(map);
  printf("free(bMap.map);\n");
}

void bxor1(char dir){

  //pack B1
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.reldir = '\0';

  //pack B2
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 4;
  B2.reldir = '\0';

  set_block(B1);
  shift(1,'u');
  setr(dir,0);
  shift(2,'d');
  shift(1,dir);
  set_block(B2);
  overlay("redstone",0);
  shift(2,'u');
  set_block(B1);
  overlay("redstone",0);
  shift(1,dir);
  shift(1,'u');
  set_block(B1);
  shift(1,dir);
  sett(dir);
  expand(1,opp_dir(dir));
  expand(1,'d');
  stack(1,'d',"s");
  contract(1,opp_dir(dir));
  contract(1,'u');
  shift(1,'u');
  sett('u');
  shift(3,'d');
  wire(14,1,4,1,dir);
  shift(1,dir);
  shift(1,'u');
  set_block(B2);
  shift(1,'u');
  sett('u');
  shift(1,'u');
  set_block(B1);
  overlay("redstone",0);
}

void bxor2(char dir){

  //pack B1
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.reldir = '\0';

  //pack B2
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 4;
  B2.reldir = '\0';

  char oppDir = opp_dir(dir);

  shift(1,dir);
  shift(1,'u');
  set_block(B2);
  expand(1,'u');
  stack(1,'u',"");
  shift(1,'u');
  stack(1,'u',"");
  shift(1,'d');
  shift(1,dir);
  contract(1,'u');
  set_block(B1);
  expand(1,'d');
  stack(1,'u',"");
  expand(1,oppDir);
  expand(3,'u');
  stack(1,dir,"");
  shift(1,dir);
  stack(2,dir,"");
  shift(1,oppDir);
  contract(3,'d');
  contract(1,'u');
  contract(1,oppDir);
  sett(oppDir);
  shift(2,'u');
  sett(oppDir);
  shift(1,'u');
  shift(1,dir);
  setr(dir,0);
  shift(2,'d');
  sett(dir);
  shift(2,'d');
  sett(dir);
  shift(1,'u');
  shift(1,dir);
  setr(dir,0);
  shift(2,'u');
  setr(oppDir,0);
  shift(1,'u');
  shift(1,dir);
  setr(dir,0);
  shift(2,'d');
  sett('u');
  shift(1,'u');
  shift(1,dir);
  setr(dir,0);
  shift(2,'d');
  setr(oppDir,0);
  shift(1,'u');
  shift(1,dir);
  sett(oppDir);
  shift(2,'d');
  sett(oppDir);
  shift(1,'u');
  shift(1,dir);
  sett(dir);
  shift(2,'u');
  sett(dir);
  shift(1,oppDir);
  shift(1,'u');
  setr(dir,0);
  shift(1,dir);
}

struct buss XOR(_Bool XORType,struct buss inBitsA, struct buss inBitsB){
  unsigned char width = inBitsA.width;
  char st_side = inBitsA.loc.st_side;
  char dir = inBitsA.loc.direction;

  if(width != inBitsB.width){
    printf("*** Warning Unequal Buss Size XOR1 ***\n");
    printf("%s XOR %s\n",inBitsA.name,inBitsB.name);
  }

  if((XORType && inBitsA.loc.direction != inBitsB.loc.direction) || (!XORType && inBitsA.loc.direction != opp_dir(inBitsB.loc.direction))){
    printf("*** Warning Bussdirection missmatch ***\n");
    printf("%s XOR %s\n",inBitsA.name,inBitsB.name);
  }

  if(XORType){
    bxor1(dir);
    expand(4,opp_dir(dir));
    expand(1,'u');
    expand(3,'d');
    expand(1,opp_dir(st_side));
    stack(width - 1,opp_dir(st_side),"");
    contract(1,st_side);
    contract(4,dir);
    contract(1,'d');
    contract(3,'u');
  }
  else{
    bxor2(dir);
    expand(6,opp_dir(dir));
    expand(4,'d');
    expand(1,opp_dir(st_side));
    stack(width - 1,opp_dir(st_side),"");
    contract(1,st_side);
    contract(6,dir);
    contract(4,'u');
  }
  freeBuss(inBitsB);

  return inBitsA; //more modifacations to A may be needed (new collors?)
}

struct buss* allocateBlock(){

  unsigned char PLength = sizeof(P) / sizeof(P[0]);

  struct buss* Block = malloc(2 * sizeof(struct buss));
  Block[0].name = "left_Block";
  Block[0].loc.direction = 'b';
  Block[1].name = "Right_Block";
  Block[1].loc.direction = 'f';
  for(unsigned char H = 0; H < 2; H++){ //both halves
    Block[H].width = PLength;
    Block[H].loc.st_side = 'l';
    Block[H].collors  = malloc(PLength);
    Block[H].strength = malloc(PLength);
    for(int i = 0; i < PLength; i++){
      Block[H].collors[i] = i % 16;
      Block[H].strength[i] = 10;
    }
  }
  return Block;
}

struct buss busst_flip(struct buss arg){
  t_flip(arg.loc.direction);
  shift(1,'u');
  shift(1,opp_dir(arg.loc.direction));
  expand(1,opp_dir(arg.loc.direction));
  expand(1,opp_dir(arg.loc.st_side));
  stack(arg.width - 1,opp_dir(arg.loc.st_side),"");
  contract(1,arg.loc.st_side);
  shift(1,arg.loc.direction);
  contract(1,arg.loc.direction);
  shift(1,'d');
  for(short i = 0; i < arg.width; i++){
    arg.strength[i] = 15;
  }
  return arg;
}

struct buss halfSwapSides(struct buss inBuss,short amount,char turndir,char shiftdir){

  short bits = inBuss.width;

  for(short i = 0; i < 4; i++){

    struct buss subBuss;
    subBuss.width = bits/4;
    subBuss.collors = malloc(subBuss.width);
    subBuss.strength = malloc(subBuss.width);
    subBuss.name = "Bit_Shift_Sub_Buss";
    subBuss.loc.st_side = inBuss.loc.st_side;
    subBuss.loc.direction = inBuss.loc.direction;

    if(inBuss.loc.st_side != shiftdir){
      for(short j = 0; j < bits/4; j++){
        unsigned char index = i*bits/4 + j;
        subBuss.collors[j] = inBuss.collors[index];
        subBuss.strength[j] = inBuss.strength[index];
      }
    }
    else{
      // printBussInfo(inBuss,"halfSwapSides");
      for(short j = 0; j < bits/4; j++){
        unsigned char index = ((3 - i) *bits/4 + j);

        subBuss.collors[j] = inBuss.collors[index];
        subBuss.strength[j] = inBuss.strength[index];
      }
    }

    if(i != 3){
      subBuss = bussStraight(subBuss,2);
      subBuss = turnBuss(subBuss,turndir,1,6 - (i*2));
    }
    else{
      subBuss = bussstairs(subBuss,2,'u');
      subBuss = turnBuss(subBuss,turndir,1,2);
    }

    subBuss = bussStraight(subBuss,(((amount - 2) - (bits/4 - 1)) * 2) + 4);
    char shiftdir = opp_dir(subBuss.loc.direction);

    subBuss = turnBuss(subBuss,turndir,0,0); //direction not used
    subBuss = bussStraight(subBuss,2);

    if(i != 3){
      subBuss = bussUp(1,subBuss,6 - (i * 2),'u',0);
    }
    else{
      subBuss = busst_flip(subBuss);
    }
    shift(1,subBuss.loc.direction);
    subBuss = bussStraight(subBuss,1);

    if(i != 3){
      shift(5,opp_dir(subBuss.loc.direction));
    }
    shift((((amount - 2) - (bits/4 - 1)) * 2) + 2,shiftdir);

    //copy the collors back, they are the same (if the collors are correct so in the strength, THIS is why im so picky)
    if(inBuss.loc.st_side != opp_dir(shiftdir)){
      for(short j = 0; j < bits/4; j++){
        unsigned char index = i*bits/4 + j;


        inBuss.collors[index] = subBuss.collors[j];
        inBuss.strength[index] = subBuss.strength[j];
      }
    }
    else{
      for(short j = 0; j < bits/4; j++){
        unsigned char index = ((3 - i) *bits/4 + j);

        inBuss.collors[index]  =  subBuss.collors[j];
        inBuss.strength[index] = subBuss.strength[j];
      }
    }
    freeBuss(subBuss);
  }
  inBuss.loc.direction = opp_dir(inBuss.loc.direction);
  return inBuss;
}

struct buss* buildRound(struct buss roundKey, struct buss* block){

  char rounddir = roundKey.loc.direction;
  unsigned char roundWidth = roundKey.width;
  char oppRounddir = opp_dir(rounddir);

  bussUp(1,roundKey,4,'d',0);
  shift(1,'d');
  bussStraight(roundKey,0);
  shift(6,'u');
  shift(8,rounddir);
  unsigned char PLength = sizeof(P) / sizeof(P[0]);
  bussStraight(roundKey,(PLength * 2) + 1);
  struct buss sBoxResults = buildStables(roundKey);
  struct buss PResults = buildPermute("P_Results",sBoxResults,PLength,P,4,4,oppRounddir,1,1);
  freeBuss(sBoxResults);
  shift(6,'d');
  shift(1,'b');
  shift(((PLength * 2)),rounddir);
  unsigned char ELength = sizeof(E) / sizeof(E[0]);
  struct buss EResults = buildPermute("E_Results",block[1],ELength,E,1,0,'b',1,1);
  EResults = bussStraight(EResults,(PLength * 2) - 1);
  shift(8,oppRounddir);
  roundKey = XOR(0,roundKey,EResults);
  roundKey = bussStraight(roundKey,0);
  freeBuss(roundKey);
  shift(7,'d');
  shift(2,rounddir);
  shift(3,'b');
  bussUp(0,block[0],4,'u',0);
  shift(1,'u');
  shift(1,'f');
  block[0] = XOR(1,block[0],PResults);
  block[0] = bussStraight(block[0],1);
  shift(1,block[0].loc.direction);
  block[0] = halfSwapSides(block[0],PLength,oppRounddir,rounddir);
  block[1] = bussUp(0,block[1],4,'u',0);
  shift(12,'d');
  shift(1,'f');
  block[1] = bussStraight(block[1],(roundWidth * 2) + 2);
  block[1] = bussUp(1,block[1],6,'u',0);
  shift(1,block[1].loc.direction);
  block[1] = bussStraight(block[1],1);
  shift(roundWidth,rounddir);
  block[1] = halfSwapSides(block[1],PLength,oppRounddir,oppRounddir);
  shift(roundWidth,oppRounddir);
  block[1] = bussUp(0,block[1],4,'u',0);
  shift(1,'b');
  shift(1,'d');
  block[1] = bussStraight(block[1],(roundKey.width * 2) + 1);
  shift(2,'u');

  block[0].loc.direction = opp_dir(block[0].loc.direction);
  block[1].loc.direction = opp_dir(block[1].loc.direction);

  shift(12,opp_dir(rounddir));
  shift(5,'u');
  shift(2,'f');

  return block;
}

struct bussPair{
  struct buss Tap1;
  struct buss Tap2;
};

struct DESKeys{
  struct buss encrypt[16];
  struct buss decrypt[16];
};

struct DESKeys setKeySchedual(struct bussPair keyPair,unsigned char num_rounds){

  unsigned char distance = 14;

  unsigned char PC1Length = sizeof(PC1) / sizeof(PC1[0]);

  unsigned char num_rotations = num_rounds;

  unsigned char PC2Length = sizeof(PC2) / sizeof(PC2[0]);
  unsigned char rotated[PC2Length];

  for(unsigned char j = 0; j < PC2Length; j++){
    rotated[j] = PC2[j];
  }

  struct DESKeys keyschedual;

  const unsigned char roundHeight = 10;

  for(int s = 0; s < 2;s++){
    showProgress(s,2,"SET KEYS HALF");

    struct buss PC1Results;
    if(!s){
      PC1Results = buildPermute("PC1",keyPair.Tap2,PC1Length,PC1,1,0,'r',1,1);
      freeBuss(keyPair.Tap2);
    }
    else{
      PC1Results = buildPermute("PC1",keyPair.Tap1,PC1Length,PC1,1,0,'r',1,1);
      freeBuss(keyPair.Tap1);
    }

    for (int i=0; i<num_rotations; i++){
      showProgress(i,num_rotations,"pc2 rotations");

      int rotate_Amt;

      if(i == 0) {
        PC1Results = bussUp(1,PC1Results,3,'u',1);
      }
      else{
        PC1Results = bussUp(1,PC1Results,3,'u',1);
        PC1Results = bussUp(1,PC1Results,roundHeight - 3,'u',1);
      }

      if(s){
        shift(1,'d');
        keyschedual.encrypt[i] = buildPermute("PC2",PC1Results,PC2Length,rotated,1,0,'b',1,0);
        keyschedual.encrypt[i].name = "encrypt_key"; //give it a name
        if(i != num_rotations - 1)
        shift(1,'u');
        rotate_Amt = keyshifts[15 - i];
      }
      else{
        rotate_Amt = keyshifts[i];
      }

      short start[2] = {PC2Length - 1,PC2Length/2 - 1};
      short end[2] = {PC2Length/2,0};
      short start_Lim[2] = {PC1Length/2 + 1,1};
      short end_Lim[2] = {PC1Length,PC1Length/2};

      for(unsigned char k = 0; k < rotate_Amt; k++){
        for(short side = 0; side < 2; side++){
          for(short j = start[side]; j >= end[side]; j--){
            if(s){
              rotated[j]--;
              if(rotated[j] < start_Lim[side])
              rotated[j] = end_Lim[side];
            }
            else{
              rotated[j]++;
              if(rotated[j] > end_Lim[side])
              rotated[j] = start_Lim[side];
            }
          }
        }
      }
      if(!s){
        shift(1,'d');
        if(i != num_rotations - 1) keyschedual.decrypt[i] = buildPermute("PC2",PC1Results,PC2Length,rotated,1,0,'b',1,0);
        else                       keyschedual.decrypt[i] = buildPermute("PC2",PC1Results,PC2Length,rotated,1,0,'b',1,1); //stay at end at end
        keyschedual.decrypt[i].name = "decrypt_Key"; //give it a name
        if(i != num_rotations - 1)
        shift(1,'u');
      }
    }
    if(!s){
      shift(3,'d');
      shift(4,'b');
      shift(64 * 2 + distance + 1,'b');
      PC1Results.loc.direction = opp_dir(PC1Results.loc.direction);
      shift(roundHeight * (num_rotations - 1) + 3,'d');
    }
    else{
      shift(roundHeight * (num_rotations - 1),'d');//doing this removes an unesisary shift up after the s loop ends
    }
    freeBuss(PC1Results);
  }

  shift(2,'r');
  shift(2,'f');
  shift(2,'u');


  return keyschedual;
}


void DES(){
  unsigned char num_rounds = 16;
  unsigned char distance = 14;

  struct buss key;
  key.width = 64;
  key.collors = malloc(64); //input key
  key.strength = malloc(64); //input key
  key.name = "input_Key";
  key.loc.direction = 'f';
  key.loc.st_side = 'r';

  for(int i = 0; i < 64; i++){
    key.collors[i] = i% 16;
    key.strength[i] = 10;
  }

  unsigned char width = key.width;
  struct bussLocation loc = key.loc;

  struct bussPair keyPair;

  keyPair.Tap1.width = width;
  keyPair.Tap1.collors = malloc(width);
  keyPair.Tap1.strength = malloc(width);
  keyPair.Tap1.name = "Tap_1";
  keyPair.Tap1.loc = loc;

  keyPair.Tap2.width = width;
  keyPair.Tap2.collors = malloc(width);
  keyPair.Tap2.strength = malloc(width);
  keyPair.Tap2.name = "Tap_2";
  keyPair.Tap2.loc = loc;

  for(unsigned char i = 0; i < width; i++){
    keyPair.Tap1.collors[i] = key.collors[i];
    keyPair.Tap2.collors[i] = key.collors[i];
    keyPair.Tap1.strength[i] = key.strength[i];
    keyPair.Tap2.strength[i] = key.strength[i];
  }

  keyPair.Tap1 = bussStraight(keyPair.Tap1,4);
  keyPair.Tap1 = turnBuss(keyPair.Tap1,'r',0,0);
  keyPair.Tap1 = bussStraight(keyPair.Tap1,2);
  keyPair.Tap1 = bussUp(1,keyPair.Tap1,4,'u',1);
  shift(1,'r');
  shift(4,'d');
  shift(4,'b');
  keyPair.Tap2 = bussUp(1,keyPair.Tap2,2,'u',0);
  shift(1,keyPair.Tap2.loc.direction);
  keyPair.Tap2 = bussStraight(keyPair.Tap2,width * 2 + distance + 2);
  keyPair.Tap2 = turnBuss(keyPair.Tap2,'r',0,0);
  keyPair.Tap2 = bussStraight(keyPair.Tap2,2);
  keyPair.Tap2 = bussUp(1,keyPair.Tap2,2,'u',1);
  shift(1,keyPair.Tap2.loc.direction);
  freeBuss(key);


  //build keyscheduals
  struct DESKeys keyschedual = setKeySchedual(keyPair,num_rounds);


  //decrypt
  for(int i = 0; i < num_rounds; i++){
    struct buss* block = allocateBlock();
    keyschedual.decrypt[i] = bussStraight(keyschedual.decrypt[i],6);
    buildRound(keyschedual.decrypt[i],block);
    shift(6,opp_dir(keyschedual.decrypt[i].loc.direction));
    showProgress(i,num_rounds,"DES decrypt");
    freeBuss(block[0]);
    freeBuss(block[1]);
    free(block);
  }

  shift(64 * 2 + distance + 4,'f');
  shift(10 * (num_rounds),'d');
  shift(2,'b');

  //encrypt
  for(int i = 0; i < num_rounds; i++){
    struct buss* block = allocateBlock();
    keyschedual.encrypt[i] = bussStraight(keyschedual.encrypt[i],6);
    buildRound(keyschedual.encrypt[i],block);
    shift(6,opp_dir(keyschedual.encrypt[i].loc.direction));
    showProgress(i,num_rounds,"DES encrypt");
    freeBuss(block[0]);
    freeBuss(block[1]);
    free(block);
  }
}



void clearFirstBlock(){
  struct block B;
  B.type = 0;
  B.value = 0;
  B.reldir = '\0';
  map = malloc(sizeof(struct block));
  map[0] = B;
}

struct dimentions{
  short width;
  short height;
  short depth;
};

void mapExport(){
  //storage for export dimentions
  struct dimentions dimentions;

  //pack dimentions
  dimentions.width = mapW;
  dimentions.height = mapH;
  dimentions.depth = mapD;

  FILE * export = fopen("mapExport/map.bin","wb");

  //save dimentions
  fwrite(&dimentions,1,sizeof(struct dimentions),export);

  //save data
  fwrite(map,mapW * mapH * mapD,sizeof(struct block),export);
  fclose(export);
}

void mapImport(){

  FILE * import = fopen("mapExport/map.bin","rb");

  //storage for import dimentions
  struct dimentions dimentions;

  //get dimentions
  if(!fread(&dimentions,1,sizeof(struct dimentions),import)){
    printf("read fail");
    exit(1);
  }

  //unpack and update dimentions
  mapW = dimentions.width;
  mapH = dimentions.height;
  mapD = dimentions.depth;

  //make room
  map = malloc(mapW * mapH * mapD * sizeof(struct block));

  //get data
  if(!fread(map,mapW * mapH * mapD,sizeof(struct block),import)){
    printf("read fail");
    exit(1);
  }
  fclose(import);
}

//convert from and to litttle and BIG endian bytes
void flip(unsigned long long *arg,unsigned char size){
  switch (size) {
    case 8: *arg =
    (0xFF0000000000 & *arg) >> 40 | (0x0000000000FF & *arg) << 40 |
    (0x00FF00000000 & *arg) >> 24 | (0x00000000FF00 & *arg) << 24 |
    (0x0000FF000000 & *arg) >> 8  | (0x000000FF0000 & *arg) << 8; break;
    case 4: *arg =
    (0xFF000000 & *arg) >> 24 | (0x000000FF & *arg) << 24 |
    (0x00FF0000 & *arg) >> 8  | (0x0000FF00 & *arg) << 8; break;
    case 2: *arg =
    (0xFF00 & *arg) >> 8 | (0x00FF & *arg) << 8; break;
  }
}

void schematticImport(char* fileName){

  printf("\n SCHEMATIC IMPORT \n");

  gzFile schem;

  char path[40];
  snprintf(path,40,"schematics/%s.schematic",fileName);
  schem = gzopen(path,"rb");

  unsigned char levels = 0;

  char name[40] = ""; //string for name length bytes

  do{
    //tag structure
    unsigned char ID; //first byte
    unsigned short length; //next 2 bytes

    unsigned long long payload = 0; //depending on tag type

    unsigned char* array_payload; //depending on tag type

    //get ID
    if(!gzfread(&ID,1,1,schem)){
      printf("read fail: ID");
      getchar();
      exit(1);
    }

    printf("ID = %3i ",ID);

    switch (ID) {
      case 0:  printf("%-19s","[TAG_End]"); break;
      case 1:  printf("%-19s","[TAG_Byte]"); break;
      case 2:  printf("%-19s","[TAG_Short]"); break;
      case 3:  printf("%-19s","[TAG_Int]"); break;
      case 4:  printf("%-19s","[TAG_Long]"); break;
      case 5:  printf("%-19s","[TAG_Float]"); break;
      case 6:  printf("%-19s","[TAG_Double]"); break;
      case 7:  printf("%-19s","[TAG_Byte_Array]"); break;
      case 8:  printf("%-19s","[TAG_String]"); break;
      case 9:  printf("%-19s","[TAG_List]"); break;
      case 10: printf("%-19s","[TAG_Compound]"); break;
      case 11: printf("%-19s","[TAG_Int_Array]"); break;
      case 12: printf("%-19s","[TAG_Long_Array]"); break;
    }

    if(ID != 0){ //because tag_end does not have this data leave it out if 0
      //get ID
      if(!gzfread(&length,1,sizeof(short),schem)){
        printf("read fail: LENGTH");
        exit(1);
      }

      length = (0xFF00 & length) >> 8 | (0xFF & length) << 8;
      printf("Length = %-3i ",length);

      if(length > 0){

        name[length] = '\0';

        //get ID
        if(!gzfread(name,length,1,schem)){
          printf("read fail: NAME");
          exit(1);
        }
        //print tag name
        printf("Name = %-15s ",name);
      }
    }
    if (ID == 0 || ID == 10){
      printf("\n");
    }

    unsigned char payload_length = 0;
    unsigned long long array_length = 0;

    unsigned char tagLength = 0;

    switch (ID) {
      case 0: payload_length = 0; levels--; break;
      case 1: payload_length = 1; break;
      case 2: payload_length = 2; break;
      case 3: payload_length = 4; break;
      case 4: payload_length = 8; break;
      case 5: payload_length = 4; break;
      case 6: payload_length = 8; break;
      case 7: tagLength = 4; break;
      case 8: tagLength = 2; break;
      case 10: levels++; continue;
      case 11: continue;
      case 12: continue;
    }

    if(ID < 7 && ID > 0){

      if(!gzfread(&payload,payload_length,1,schem)){
        printf("read fail: simple TAG_Payload");
        exit(1);
      }

      flip(&payload,payload_length);

      printf("payload = %-10llu\n",payload);

      if(!strncmp(name,"Width",length)){
        mapW = payload;
      }
      else if(!strncmp(name,"Height",length)){
        mapH = payload;
      }
      else if(!strncmp(name,"Length",length)){
        mapD = payload;
      }
    }

    else if(ID >= 7 && ID < 9){
      if(!gzfread(&array_length,tagLength,1,schem)){
        printf("read fail: array length");
        exit(1);
      }

      switch (tagLength) {
        case 4: array_length = (0xFF000000 & array_length) >> 24 | (0xFF & array_length) << 24 | (0x00FF0000 & array_length) >> 8 | (0xFF00 & array_length) << 8; break;
        case 2: array_length = (0xFF00 & array_length) >> 8 | (0xFF & array_length) << 8; break;
      }

      printf("payload_length = %-10llu ",array_length);

      //make room
      array_payload = malloc(array_length + 1);

      array_payload[array_length] = '\0';

      if(!gzfread(array_payload,array_length,1,schem)){
        printf("read fail: array payload");
        exit(1);
      }

      if(!strncmp(name,"Blocks",length) || !strncmp(name,"Data",length)){
        map = realloc(map,array_length * sizeof(struct block));
      }


      if (ID == 7) {
        printf("payload = { *bytes* }\n");
        // printf("%3i",array_payload[i]);
        // if(i != array_length - 1) printf(",");

        int schemindex = 0;

        for(int y = 0; y < mapH; y++){
          for(int x = mapW - 1; x >= 0; x--){
            for(int z = 0; z < mapD; z++){
              int mapindex = getIndex(x,y,z);
              if(!strncmp(name,"Blocks",length)){
                unsigned char type =  reversegetID(array_payload[schemindex]);
                map[mapindex].type = type;
                map[mapindex].powered = 0;
              }
              else if(!strncmp(name,"Data",length)){
                unsigned char value = array_payload[schemindex];
                map[mapindex].value = value;
              }
              schemindex++;
            }
          }
        }

        // if(i % 16 == 0) printf("\n");
      }
      else{
        printf("payload = %s\n",array_payload);
      }

      free(array_payload);

    }

    else if(ID == 9){
      unsigned char listID;

      if(!gzfread(&listID,1,1,schem)){
        printf("read fail: listID");
        exit(1);
      }

      printf("list ID = %i ",listID);

      int listLength;

      if(!gzfread(&listLength,1,sizeof(int),schem)){
        printf("read fail: listLength");
        exit(1);
      }

      listLength = (0xFF000000 & listLength) >> 24 | (0xFF & listLength) << 24 | (0x00FF0000 & listLength) >> 8 | (0xFF00 & listLength) << 8;

      printf("list length = %i\n",listLength);

      if(listLength > 0){
        char* listPayload = malloc(listLength);

        switch (listID) {
          case 0: tagLength = 0; break;
          case 1: tagLength = 1; break;
          case 2: tagLength = 2; break;
          case 3: tagLength = 4; break;
          case 4: tagLength = 8; break;
          case 5: tagLength = 4; break;
          case 6: tagLength = 8; break;
        }

        if(!gzfread(listPayload,listLength,tagLength,schem)){
          printf("read fail: list payload");
          exit(1);
        }

        printf("payload = %s\n",listPayload);
      }
    }
  }while(levels);
  gzclose(schem);
}

void schematicExport(char* fileName){

  printf("\n SCHEMATIC EXPORT \n");

  gzFile schem;

  char path[40];
  snprintf(path,40,"schematics/%s.schematic",fileName);

  schem = gzopen(path,"wb");

  unsigned char ID;
  unsigned long long name_length;
  char* name;

  unsigned long long payload;
  unsigned long long composit_length;

  ID = 10;
  name = "Schematic";
  name_length = strlen(name);
  flip(&name_length,2);
  gzfwrite(&ID,1,1,schem);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  ID = 2;
  name = "Width";
  name_length = strlen(name);
  flip(&name_length,2);
  payload = mapD;
  flip(&payload,2);
  gzfwrite(&ID,1,1,schem);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);
  gzfwrite(&payload,1,sizeof(short),schem);

  ID = 2;
  name = "Height";
  name_length = strlen(name);
  flip(&name_length,2);
  payload = mapH;
  flip(&payload,2);
  gzfwrite(&ID,1,1,schem);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);
  gzfwrite(&payload,1,sizeof(short),schem);

  ID = 2;
  name = "Length";
  name_length = strlen(name);
  flip(&name_length,2);
  payload = mapW;
  flip(&payload,2);
  gzfwrite(&ID,1,1,schem);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);
  gzfwrite(&payload,1,sizeof(short),schem);

  ID = 8;
  name = "Materials";
  name_length = strlen(name);
  flip(&name_length,2);
  gzfwrite(&ID,1,1,schem);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  char* string = "Alpha";
  payload = strlen(string); flip(&payload,2);
  gzfwrite(&payload,1,sizeof(short),schem);
  gzfwrite(string,1,strlen(string),schem);

  ID = 7;
  name_length = strlen(name);
  name = "Blocks";
  name_length = strlen(name);
  gzfwrite(&ID,1,1,schem);
  flip(&name_length,2);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  composit_length = mapW * mapH * mapD;
  printf("composit_length = %llu < %i x %i x %i\n",composit_length,mapW,mapH,mapD);
  flip(&composit_length,4);

  // flip(&composit_length,4);
  gzfwrite(&composit_length,1,4,schem);

  for(int y = 0; y < mapH; y++){
    for(int x = mapW - 1; x >= 0; x--){
      for(int z = 0; z < mapD; z++){
        int index = getSchemIndex(x,y,z);
        unsigned char block = getID(map[index].type);
        gzfwrite(&block,1,1,schem);
      }
    }
  }

  ID = 7;
  name_length = strlen(name);
  name = "Data";
  name_length = strlen(name);
  gzfwrite(&ID,1,1,schem);
  flip(&name_length,2);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  composit_length = mapW * mapH * mapD;
  printf("composit_length = %llu < %i x %i x %i\n",composit_length,mapW,mapH,mapD);
  flip(&composit_length,4);

  // flip(&composit_length,4);
  gzfwrite(&composit_length,1,4,schem);

  for(int y = 0; y < mapH; y++){
    for(int x = mapW - 1; x >= 0; x--){
      for(int z = 0; z < mapD; z++){
        int index = getSchemIndex(x,y,z);
        gzfwrite(&map[index].value,1,1,schem);
      }
    }
  }

  //useless stuff i need for world edit to work

  ID = 9;
  name_length = strlen(name);
  name = "Entities";
  name_length = strlen(name);
  gzfwrite(&ID,1,1,schem);
  flip(&name_length,2);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  ID = 0;
  gzfwrite(&ID,1,1,schem);

  composit_length = 0;
  printf("composit_length = %llu < %i x %i x %i\n",composit_length,mapW,mapH,mapD);
  flip(&composit_length,4);

  // flip(&composit_length,4);
  gzfwrite(&composit_length,1,4,schem);

  ID = 9;
  name_length = strlen(name);
  name = "TileEntities";
  name_length = strlen(name);
  gzfwrite(&ID,1,1,schem);
  flip(&name_length,2);
  gzfwrite(&name_length,1,sizeof(short),schem);
  gzfwrite(name,1,strlen(name),schem);

  ID = 0;
  gzfwrite(&ID,1,1,schem);

  composit_length = 0;
  printf("composit_length = %llu < %i x %i x %i\n",composit_length,mapW,mapH,mapD);
  flip(&composit_length,4);

  // flip(&composit_length,4);
  gzfwrite(&composit_length,1,4,schem);


  ID = 0;
  gzfwrite(&ID,1,1,schem);

  gzclose(schem);
}

void runTest(){

  struct block B;
  B.type = blockLookup("wool");
  B.reldir = '\0';

  int distance = 7;

  B.value = 0; set_block(B);
  stack(--distance,'l',"");
  B.value = 1; set_block(B);
  stack(--distance,'r',"");
  B.value = 2; set_block(B);
  stack(--distance,'b',"");
  B.value = 3; set_block(B);
  stack(--distance,'f',"");
  B.value = 4; set_block(B);
  stack(--distance,'d',"");
  B.value = 5; set_block(B);
  stack(--distance,'u',"");
  B.value = 15; set_block(B);
}

void testtOrienttations(){

  seto('l');
  shift(2,'f');
  seto('r');
  shift(2,'f');
  seto('f');
  shift(2,'f');
  seto('b');
  shift(2,'f');
  seto('u');
  shift(2,'f');
  seto('d');
  shift(2,'f');

  setp('l',1);
  shift(2,'f');
  setp('r',1);
  shift(2,'f');
  setp('f',1);
  shift(2,'f');
  setp('b',1);
  shift(2,'f');
  setp('u',1);
  shift(2,'f');
  setp('d',1);
  shift(2,'f');

  setr('l',0);
  shift(2,'f');
  setr('r',0);
  shift(2,'f');
  setr('f',0);
  shift(2,'f');
  setr('b',0);
  shift(2,'f');

  sett('l');
  shift(2,'f');
  sett('r');
  shift(2,'f');
  sett('f');
  shift(2,'f');
  sett('b');
  shift(2,'f');
  sett('u');
  shift(2,'f');

}

_Bool redLogic(short x, short z, short y){

  _Bool ret = 0;

  struct block* adjacent[10];

  struct block B;
  B.type = 0;
  B.reldir = '\0';
  B.value = 0;

  struct block* indexed = &map[getIndex(x,z,y)];

  adjacent[0] = (x == 0?        &B : &map[getIndex(x + 1,z,y)]); //L
  adjacent[1] = (x == mapW - 1? &B : &map[getIndex(x - 1,z,y)]); //R
  adjacent[2] = (y == mapD - 1? &B : &map[getIndex(x,z,y + 1)]); //F
  adjacent[3] = (y == 0?        &B : &map[getIndex(x,z,y - 1)]); //B

  adjacent[4] = (y == 0? &B : &map[getIndex(x + 1,z,y + 1)]); //LU
  adjacent[5] = (y == 0? &B : &map[getIndex(x - 1,z,y + 1)]); //RU
  adjacent[6] = (y == 0? &B : &map[getIndex(x,z + 1,y + 1)]); //FU
  adjacent[7] = (y == 0? &B : &map[getIndex(x,z - 1,y + 1)]); //BU

  adjacent[8] = (z == mapH - 1? &B : &map[getIndex(x,z + 1,y)]); //U
  adjacent[9] = (z == 0?        &B : &map[getIndex(x,z - 1,y)]); //D

  if(indexed->type == blockLookup("redstone")){
    struct redShape redShape;
    redShape = getRedStoneShape(x,z,y);

    for(int i = 0; i < 4; i++){
      if(redShape.extend[i] && solid(adjacent[i]->type)){
        adjacent[i]->powered = 1;
      }
    }

    for(int i = 0; i < 8; i++){
      if(redShape.extend[i] && adjacent[i]->type == blockLookup("redstone")){
        if(adjacent[i]->value - 1 > indexed->value){
          ret = 1;
          printf("%i > %i\n",adjacent[i]->value - 1,indexed->value);
          indexed->value = adjacent[i]->value - 1;
        } else if(indexed->value - 1 > adjacent[i]->value) {
          ret = 1;
          printf("%i > %i\n",indexed->value - 1,adjacent[i]->value);
          adjacent[i]->value = indexed->value - 1;
        }
      }
    }
  } else if(indexed->type == blockLookup("redstone_torch")){
    //block above powered
    if(solid(adjacent[8]->type)){
      adjacent[8]->powered = 1;
    }
    if(indexed->reldir == 'l'){
      if(adjacent[0]->powered){
        ret = 1;
        printf("T 0");
        indexed->type = blockLookup("redstone_torch_off");
      }
    } else if(indexed->reldir == 'r'){
      if(adjacent[1]->powered){
        ret = 1;
        printf("T 0");
        indexed->type = blockLookup("redstone_torch_off");
      }
    } else if(indexed->reldir == 'b'){
      if(adjacent[2]->powered){
        ret = 1;
        printf("T 0");
        indexed->type = blockLookup("redstone_torch_off");
      }
    } else if(indexed->reldir == 'f'){
      if(adjacent[3]->powered){
        ret = 1;
        printf("T 0");
        indexed->type = blockLookup("redstone_torch_off");
      }
    } else if(indexed->reldir == 'u'){
      if(adjacent[8]->powered){
        ret = 1;
        printf("T 0");
        indexed->type = blockLookup("redstone_torch_off");
      }
    }
  } else if(indexed->type == blockLookup("redstone_torch_off")){
    //block above unpowered
    if(solid(adjacent[8]->type)){
      adjacent[8]->powered = 0;
    }

    if(indexed->reldir == 'l'){
      if(adjacent[0]->powered){
        ret = 1;
        printf("T 1");
        indexed->type = blockLookup("redstone_torch");
      }
    } else if(indexed->reldir == 'r'){
      if(adjacent[1]->powered){
        ret = 1;
        printf("T 1");
        indexed->type = blockLookup("redstone_torch");
      }
    } else if(indexed->reldir == 'b'){
      if(adjacent[2]->powered){
        ret = 1;
        printf("T 1");
        indexed->type = blockLookup("redstone_torch");
      }
    } else if(indexed->reldir == 'f'){
      if(adjacent[3]->powered){
        ret = 1;
        printf("T 1");
        indexed->type = blockLookup("redstone_torch");
      }
    } else if(indexed->reldir == 'u'){
      if(adjacent[8]->powered){
        ret = 1;
        printf("T 1");
        indexed->type = blockLookup("redstone_torch");
      }
    }
  }
  return ret;
}


void enneal(){

  _Bool change;

  do{
    change = 0;
    for(int x = 0; x < mapW; x++){
      for(int z = 0; z < mapH; z++){
        for(int y = 0; y < mapD; y++){
          if(redLogic(x,z,y)){
            change = 1;
            printf(": change detected @%i,%i,%i \n",x,z,y);
          }
        }
      }
    }
  }while(change);
}

int main(){

  getchar();

  //memory allocation
  clearFirstBlock();

  DES();

  // schematticImport("DES");
  schematicExport("DES");

  // enneal();

  // runTest();
  // testtOrienttations();

  // buildMaterialLibrary();
  // buildWaveFront();
  // buildImmages();

  //end portion of main (freeing and closing pointers)
  freeBlockMap();

  return 0;

}
