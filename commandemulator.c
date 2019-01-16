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

char *blockTable[][16] ={
  {"air","air"},
  {"observor","machine_Base"},
  {"redstone","redstone_Off","redstone_Off","redstone_Off","redstone_Off","redstone_Off"},
  {"redstone_Block","redstone_Off"},
  {"redstone_Lamp","redstone_Lamp"},
  {"redstone_repeater","redstone_Tile","redstone_Off","generic_Wood","generic_Wood","redstone_Off","redstone_Off"},
  {"redstone_torch","generic_Wood","redstone_On"},
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

unsigned char PartLookup(char* partName){
  size_t numParts = sizeof(partTable) / sizeof(partTable[0]);
  for(size_t part = 0; part < numParts; part++){
    if(strlen(partName) == strlen(partTable[part]) && !strncmp(partName,partTable[part],strlen(partTable[part]))){
      return part;
    }
  }

  //error not in table
  printf("PartLookup() - what is %s?\n",partName);
  return -1;
}

char* reversePartLookup(unsigned char part){
  unsigned char numElements = sizeof(partTable) / sizeof(partTable[0]);
  if(part < numElements)
  return partTable[part];
  else{
    //error not in table
    printf("reversePartLookup() - what is %i?\n",part);
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

  if     (part == PartLookup("wood"))           return woodmap[value];
  else if(part == PartLookup("wool"))           return woolMap[value];

  else if(part == PartLookup("redstone_Off"))   return 0x560000;
  else if(part == PartLookup("redstone_On"))    return 0xd10000;
  else if(part == PartLookup("slime"))          return 0xA9C54E;
  else if(part == PartLookup("generic_Wood"))   return 0xffdb8e;
  else if(part == PartLookup("machine_Base"))   return 0x3d3d3d;
  else if(part == PartLookup("redstone_Tile"))  return 0xa8a8a8;
  else if(part == PartLookup("redstone_Lamp"))  return 0x751d0b;
  else if(part == PartLookup("slab"))           return 0x8d9093;
  else if(part == PartLookup("ice"))            return 0xa5c8ff;
  else if(part == PartLookup("air"))            return 0x000000;

  printf("collorLookup() - could not find collor for %s\n",reversePartLookup(part));
  return 0;
}

struct block{
  unsigned char type;
  unsigned char value;
  char relDir;
};

// x z y
struct block *map;

struct point{
  short x;
  short z;
  short y;
};

//Block map
//keep track of raw size for convienience
short mapW = 1,mapH = 1,mapD = 1;

int getIndex(short x, short z, short y, short mapH, short mapD){
  return x*mapH + z*mapD + y;
}

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

_Bool inSel(short x,short z,short y){
  struct extents sel = getAllSelExtents();
  return (x <= sel.l && x >= sel.r && z <= sel.u && z >= sel.d && y <= sel.f && y >= sel.b);
}

void showFloat(float arg){
  unsigned char barLength = 10;
  printf("[");
  for(short i = 0; i < arg * barLength; i++) printf("#");
  for(short i = arg * barLength; i < barLength; i++) printf(" ");
  printf("]");
}

//3d progress bars! :D
void showProgress(short ARG,short MAX_ARG, char* message){
  //calculate progress values are inf when masked
  float Progress = ((float)(ARG + 1) / MAX_ARG);
  //at beginning of each process new line to not overwrite any previous output
  if(Progress == 0) printf("\n");

  //return to write next progression
  printf("\r");

  //print message of what the process is
  printf("%-30s",message);

  //show current progress
  printf("%5i/%-5i %3.2f %%",ARG + 1,MAX_ARG,Progress * 100); showFloat(Progress);

  //new line when finished so i dont need to impliment a seperate new line in the code seperatly
  if(Progress == 1) printf("\n");
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
        int index = getIndex(x,z,y,mapH,mapD);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,PartLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor > 0){
            collor = blockCollor;
            float scaler = (float)z / (mapH);
            darken(&collor,scaler);
          }
        }
      }
      if(inSel(x,selZA,y)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Top);
    }
    fprintf(Top,"\n");
    showProgress((mapD - 1) - y,mapD,"scanning y axis");
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
        int index = getIndex(x,z,y,mapH,mapD);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,PartLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapD - y) / mapD;
            darken(&collor,scaler);
          }
        }
      }
      if(inSel(x,z,selYA)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Front);
    }
    fprintf(Front,"\n");
    showProgress((mapH - 1) - z,mapH,"scanning z axis");
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
        int index = getIndex(x,z,y,mapH,mapD);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,PartLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapW - x) / mapW;
            darken(&collor,scaler);
          }
        }
      }
      if(inSel(selXA,z,y)){
        tint(&collor,selCollor);
      }
      printPixel(collor,Left);
    }
    fprintf(Left,"\n");
    showProgress((mapH - 1) - z,mapH,"scanninx z axis");
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

  //1 malloc to rule them all
  struct block* temp = malloc(mapW * mapH * mapD * sizeof(struct block));

  for(short x = 0; x < tempW; x++){
    for(short z = 0; z < tempH; z++){
      for(short y = 0; y < tempD; y++){
        int orig_index = getIndex(x,z,y,tempH,tempD); //old centext
        int dest_index = getIndex(x,z,y,mapH,mapD); //new context
        temp[orig_index] = map[dest_index]; //copy block into new context
        map[orig_index].relDir = '\0';
        map[orig_index].type = 0;
        map[orig_index].value = 0;
      }
    }
  }

  map = realloc(map,mapW * mapH * mapD * sizeof(struct block));

  for(short x = 0; x < mapW; x++){
    for(short z = 0; z < mapH; z++){
      for(short y = 0; y < mapD; y++){
        int index = getIndex(x,z,y,mapH,mapD); //new context
        map[index] = temp[index];
      }
    }
  }

  free(temp);

  //memory allocation
  if(Dir == 'r' || Dir == 'l'){
    for(short x = tempW; x < mapW; x++){
      for(short z = 0; z < mapH; z++){
        for(short y = 0; y < mapD; y++){
          int index = getIndex(x,z,y,mapH,mapD);
          map[index].type = 0;
          map[index].value = 0;
          map[index].relDir = 0;
        }
      }
      showProgress(x,mapW,"allocating memory for new x's");
    }
    //shift data
    if(Dir == 'r'){
      unsigned short shift_Amt = mapW - tempW;
      for(short x = mapW - 1; x >= shift_Amt; x--){
        for(short z = 0; z < mapH; z++){
          for(short y = 0; y < mapD; y++){
            int dest_index = getIndex(x,z,y,mapH,mapD);
            int orig_index = getIndex(x-shift_Amt,z,y,mapH,mapD);
            map[dest_index] = map[orig_index];
          }
        }
      }
      //reallocate new empty space (unshifted portion)
      for(short x = shift_Amt - 1; x >= 0; x--){
        for(short z = 0; z < mapH; z++){
          for(short y = 0; y < mapD; y++){
            int index = getIndex(x,z,y,mapH,mapD);
            map[index].type = 0;
            map[index].value = 0;
            map[index].relDir = 0;
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
    for(short x = 0; x < mapW; x++){
      for(short z = tempH; z < mapH; z++){
        for(short y = 0; y < mapD; y++){
          int index = getIndex(x,z,y,mapH,mapD);
          map[index].type = 0;
          map[index].value = 0;
          map[index].relDir = 0;
        }
      }
      //shift data
      if(Dir == 'd'){
        unsigned short shift_Amt = mapH - tempH;
        for(short z = mapH - 1; z >= shift_Amt; z--){
          for(short y = 0; y < mapD; y++){
            int dest_index = getIndex(x,z,y,mapH,mapD);
            int orig_index = getIndex(x,z-shift_Amt,y,mapH,mapD);
            map[dest_index] = map[orig_index];
          }
        }
        //reallocate new empty space (unshifted portion)
        for(short z = shift_Amt - 1; z >= 0; z--){
          for(short y = 0; y < mapD; y++){
            int index = getIndex(x,z,y,mapH,mapD);
            map[index].type = 0;
            map[index].value = 0;
            map[index].relDir = 0;
          }
        }
      }
      showProgress(x,mapW,"allocating memory for new z's");
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
    for(short x = 0; x < mapW; x++){
      for(short z = 0; z < mapH; z++){
        for(short y = tempD; y < mapD; y++){
          int index = getIndex(x,z,y,mapH,mapD);
          map[index].type = 0;
          map[index].value = 0;
          map[index].relDir = 0;
        }
        //shift data
        if(Dir == 'b'){
          unsigned short shift_Amt = mapD - tempD;
          for(short y = mapD - 1; y >= shift_Amt; y--){
            int dest_index = getIndex(x,z,y,mapH,mapD);
            int orig_index = getIndex(x,z,y-shift_Amt,mapH,mapD);
            map[dest_index] = map[orig_index];
          }
          //0 the data unshifted
          for(short y = shift_Amt - 1; y >= 0; y--){
            int index = getIndex(x,z,y,mapH,mapD);
            map[index].type = 0;
            map[index].value = 0;
            map[index].relDir = 0;
          }
        }
      }
      showProgress(x,mapW,"allocating memory for new y's");
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

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int index = getIndex(x,z,y,mapH,mapD);
        if(blockLookup(typeA) == map[index].type){
          struct block B;
          B.relDir = '\0';
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

//commands i can do
void setBlock(struct block B, _Bool wait){

  //unpack B
  char* type = reverseBlockLookup(B.type);
  unsigned char value = B.value;

  //in game
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//set %s:%i);\n",type,value);
  toScript(command);
  if(wait)
  waitUntlDone();

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int index = getIndex(x,z,y,mapH,mapD);
        map[index] = B;
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

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short y = selExt.b; y <= selExt.f; y++){
      for(short z = selExt.u; z >= selExt.d; z--){ //height last

        int origin = getIndex(x,z,y,mapH,mapD);

        //only fill in air and the block under it is not air
        if(map[origin].type != 0){
          //get block
          struct block B;
          B.type = blockLookup(type);
          B.value = value;
          B.relDir = '\0';

          int above = getIndex(x,z+1,y,mapH,mapD);
          map[above] = B; //place block
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
  }
}

struct block* copySel(){
  struct extents selExt = getAllSelExtents();
  struct block* ret = malloc(selW * selH * selD * sizeof(struct block));
  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int orig_index = getIndex(x,z,y,mapH,mapD);
        int dest_index = getIndex(x-selExt.r,z-selExt.d,y-selExt.b,mapH,mapD);

        //get blocks, the right blocks and put them right in the right location
        ret[dest_index] = map[orig_index];

        //remove old block
        map[orig_index].type = 0;
        map[orig_index].value = 0;
        map[orig_index].relDir = '\0';
      }
    }
  }
  return ret;
}

void pasteSel(struct block* arg,short amount, char Dir){

  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){

        //calculate new position of block. Keep unchanged coordinates the same with default values
        unsigned short blockBX = x;
        unsigned short blockBZ = z;
        unsigned short blockBY = y;

        switch (Dir) { //must only movve in one direction
          case 'l': blockBX += amount; break;
          case 'u': blockBZ += amount; break;
          case 'f': blockBY += amount; break;
          case 'r': blockBX -= amount; break;
          case 'd': blockBZ -= amount; break;
          case 'b': blockBY -= amount; break;
        }

        //indexes
        int orig_index = getIndex(x-selExt.r,z-selExt.d,y-selExt.b,mapH,mapD);
        int dest_index = getIndex(blockBX,blockBZ,blockBY,mapH,mapD);

        //place block
        map[dest_index] = arg[orig_index];
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

  struct block* temp = copySel();
  //although these forloops are identical i must run through them twice or els ill run into undisiered effects of the blocks being moved multiple times
  pasteSel(temp,amount,Dir);

  //free the copy
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
    _Bool sel_shift = 0;

    for(unsigned int i = 0; i < strlen(options); i++){
      snprintf(command,commandBuffer," -%c",options[i]);
      toScript(command);
      preserveAir |= options[i] == 'a';
      sel_shift |= options[i] == 's';
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

            int orig_index = getIndex(i,j,k,mapH,mapD);
            int dest_index = getIndex(blockBX,blockBY,blockBZ,mapH,mapD);

            //new block = old block
            if(!preserveAir || !map[orig_index].type == 0){
              map[dest_index] = map[orig_index];
            }
          }
        }
      }
    }

    //shift after block operation
    if(sel_shift){
      //looks framilliar? it should its the smol switch again, whre moving the selection
      selectionShift(Dir,shift);
    }
  }
}

void expand(unsigned short amount,char Dir){
  if(amount <= 0){
    return;
  }

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
    default : printf("%15s - Invalid Direction received :%c > %i\n","oppDir()",Dir,Dir);
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
    default : printf("%15s - Invalid Direction received :%c > %i\n","turnDir()",Dir,Dir);
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

void setRepeater(char Dir,unsigned char m){
  comment("placing repeater based on direction");
  unsigned char offset = 0;
  switch (Dir) {
    case 'f': offset = 0; break;
    case 'r': offset = 1; break;
    case 'b': offset = 2; break;
    case 'l': offset = 3; break;
  }

  char dirSet[4];

  for(int i = 0; i < 4; i++){
    dirSet[i] = (((i + offset) % 4) + (m * 4));
  }

  //pack B
  struct block B;
  B.type = blockLookup("redstone_repeater");
  B.relDir = Dir;

  //place block depenting on dirrection and offset on actual Dir
  toScript("if(DIRECTION == \"N\"); ");
  B.value = dirSet[0]; setBlock(B,0);
  toScript("elseif(DIRECTION == \"E\"); ");
  B.value = dirSet[1]; setBlock(B,0);
  toScript("elseif(DIRECTION == \"S\"); ");
  B.value = dirSet[2]; setBlock(B,0);
  toScript("elseif(DIRECTION == \"W\"); ");
  B.value = dirSet[3]; setBlock(B,0);
  toScript("endif;\n");
  comment("wait at end of if");
  waitUntlDone();
}

void setredTorch(char Dir){
  if(Dir != 'u')
  comment("placing torch based on direction");

  //pack B
  struct block B;
  B.type = blockLookup("redstone_torch");
  B.relDir = Dir;

  //even bigger switch
  switch (Dir) {
    case 'u': B.value = 5,setBlock(B,0); break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 1,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 2,setBlock(B,0);
    toScript("endif; \n");
    break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 1,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 1,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 3,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 1,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 4,setBlock(B,0);
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

  //pack B
  struct block B;
  B.type = blockLookup("observor");
  B.relDir = Dir;

  //even bigger switch mk 2
  switch (Dir) {
    case 'u':
    B.value = 0,setBlock(B,0);
    break;
    case 'd':
    B.value = 1,setBlock(B,0);
    break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 5,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 4,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 2,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 3,setBlock(B,0);
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

  //pack B
  struct block B;
  B.type = blockLookup(type);
  B.relDir = Dir;

  //even bigger switch mk 3
  switch (Dir) {
    case 'u': B.value = 1,setBlock(B,0); break;
    case 'd': B.value = 0,setBlock(B,0); break;
    case 'l':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 5,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'r':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 4,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'b':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 3,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 2,setBlock(B,0);
    toScript("endif;\n");
    break;
    case 'f':
    toScript("if(DIRECTION == \"W\"); ");
    B.value = 4,setBlock(B,0);
    toScript("elseif(DIRECTION == \"E\"); ");
    B.value = 5,setBlock(B,0);
    toScript("elseif(DIRECTION == \"N\"); ");
    B.value = 2,setBlock(B,0);
    toScript("elseif(DIRECTION == \"S\"); ");
    B.value = 3,setBlock(B,0);
    toScript("endif;\n");
    break;
  }
  if(Dir != 'u' && Dir != 'd')
  comment("wait at end of if");
  waitUntlDone();
}

unsigned char wire(short inStrength,unsigned char outToll, unsigned char value, unsigned short amount, char Dir){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.relDir = '\0';

  comment("wire");
  //current position of end of selection
  unsigned short pos = 0;
  //current state of the output strength
  short outStrength = inStrength - amount;
  //support structure for redstone
  setBlock(B,1);
  stack(amount,Dir,"");
  //shift up to do redstone
  shift(1,'u');
  //check to see if the wire is longer than the input strength
  //if so it is safe to place the first repeater where the signal ends
  if(inStrength < amount && outStrength < outToll){
    shift(inStrength,Dir);
    pos += inStrength;
    setRepeater(Dir,0);
    outStrength = 16 + pos - amount;
  }
  //check to see if there is enough room for at least 1 full 16 line
  if((amount - inStrength - 1) / 16 > 0){
    shift(16,Dir);
    setRepeater(Dir,0);
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
    setRepeater(Dir,0);
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

        //get the vector from a to c and from c to b
        struct vector aToc = getPointVector(a,c);
        struct vector cTob = getPointVector(b,c);

        //if the sum of the distances from a to c and from c to b are the same as from a to b, c must be on the line from a to b
        if(getDistance(aToc) + getDistance(cTob) == getDistance(aTob)){
          struct block B;
          B.type = blockLookup(type);
          B.value = value;
          B.relDir = '\0';

          int index = getIndex(i,j,k,mapH,mapD);
          //place block at point c
          map[index] = B;
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

        int index = getIndex(i,j,k,mapH,mapD);
        // printf("%i %i %i > %i %i\n",blockX,blockY,blockZ,B.type,B.value);
        map[index].type = 0;
        map[index].value = 0;
        map[index].relDir = '\0';
      }
    }
  }
  comment("enddelete");
}

void buildDecoderTop(short inBits, char Dir, char stSide, unsigned char rows, unsigned char box){

  comment("buildDecoderTop");

  //pack B
  struct block bottom;
  bottom.type = blockLookup("wood");
  bottom.value = 2;
  bottom.relDir = '\0';

  struct block top;
  top.type = blockLookup("wood");
  top.value = 3;
  top.relDir = '\0';

  char opp_stSide = oppDir(stSide);

  unsigned short entries = pow(2,inBits);
  //build top structure
  //single bit
  setBlock(bottom,1);
  overlay("redstone",0);
  shift(1,Dir);
  shift(1,'u');
  setObservor(Dir);
  shift(1,Dir);
  setBlock(top,1);
  shift(1,'d');
  setPiston('d',1);
  expand(2,oppDir(Dir));
  expand(1,'u');
  expand(1,opp_stSide);
  //stack it to the other 5 places
  stack(inBits - 1,opp_stSide,"");

  //change the wool collors to make it look better
  shift(2,oppDir(stSide));
  replace("wood",1,2,"wood",1,0); //bottom
  replace("wood",1,3,"wood",1,1); //top
  //do the 3 other middle bits all at once
  stack(inBits - 3,opp_stSide,""); // -2 for not the first and last bit and an additional -1 becuase its the 3 middle bits
  shift(2,stSide);

  //expand selection to stack to rest of entries
  expand(((inBits - 1) * 2) - 1,opp_stSide);
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

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Block");
  B.value = 0;
  B.relDir = '\0';

  char opp_Dir = oppDir(Dir);

  //number of entries
  unsigned short entries = pow(2,inBits);

  //encode the decoder (binary with custom bit encoding)
  for(short iB = 0; iB < inBits; iB++){
    //create bases
    //bottom side (inverted bits)
    shift(1,'d');
    setBlock(B,1);
    if(significance[iB] > 1){
      expand(2,Dir);
      stack(significance[iB]-1,Dir,"");
      contract(2,opp_Dir);
    }
    //top side (normal bits)
    shift(1,'u');
    shift(significance[iB] * 3,Dir);
    setBlock(B,1);
    if(significance[iB] > 1){
      expand(2,opp_Dir);
      stack(significance[iB]-1,Dir,"");
      contract(2,Dir);
    }
    shift(significance[iB] * 3,opp_Dir);

    //stack to rest of entries
    if(entries / significance[iB] > 2){
      //expand to get both sides
      expand(1,'d');
      expand(6*significance[iB]-1,Dir);
      stack((entries / (significance[iB] * 2)-1),Dir,"a");
      contract(6*significance[iB]-1,opp_Dir);
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
  shift(2,opp_Dir);
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

void tower(unsigned char amount,char up, unsigned char value){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.relDir = '\0';

  comment("tower");
  amount--;
  if(amount > 0){
    if(up == 'u'){
      shift(2,'u');
      setObservor(up);
      if(amount > 1){
        stack(amount - 1,'u',"s");
      }
      overlay("wool",value);
    }
    else{
      setObservor('d');
      if(amount > 1){
        stack(amount - 1,'d',"s");
      }
      shift(1,'d');
      setBlock(B,1);
      shift(1,'d');
    }
  }
  comment("endtower");
}

void flipFlop(char Dir){

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Block");
  B.value = 0;
  B.relDir = '\0';

  comment("FlipFlop");
  shift(1,'u');
  shift(1,Dir);
  setPiston(Dir,1);
  shift(1,Dir);
  setBlock(B,1);
  shift(1,Dir);
  shift(1,'d');
  comment("endFlipFlop");
}

unsigned char buildOutLines(unsigned char outBits, short inBits,char inSide, char outSide, char stSide){
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

  //pack B
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.relDir = '\0';

  //pack B
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 7;
  B2.relDir = '\0';

  //create 1 1x15 wire section
  setBlock(B1,1); //a little touch to make it look a little better
  shift(1,oppDir(inSide));
  setBlock(B2,1);
  stack(13,oppDir(inSide),""); //1x15 wire section
  shift(1,inSide);
  shift(1,'u');
  setRepeater(outSide,0); //place first repeater at end of first output wire
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
  stack(wireStacks-1,oppDir(inSide),""); //then stack to the rest 1() - with the strange shift
  shift(trimAmt,inSide);

  unsigned char outStrength;

  if(inSide == outSide){
    outStrength = 15;
  }
  else{
    outStrength = trimAmt;
  }

  //negate expantion for wire extension and bring selection to 1x1x1
  contract(((inBits - 1) * 2), stSide);
  contract((num_levels * 4) - 1,'u');
  contract(3,oppDir(inSide));
  contract(11,inSide);

  //shift selection up into position for next operation
  shift(1,'u');
  shift(1,stSide);
  comment("endbuildOutLines");

  return outStrength;
}

void buildandSupport(short inBits, unsigned char outBits, short inSide, char stSide){

  comment("buildandSupport");

  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = 4;
  B.relDir = '\0';

  char opp_inSide = oppDir(inSide);
  char opp_stSide = oppDir(stSide);

  setBlock(B,1);
  overlay("redstone",0);
  expand(1,'u'); //include redstone
  expand(2,opp_inSide); //make selection 3 blocks wide
  stack(entries-1,opp_inSide,""); //stack to rest of entries
  expand((entries-1) * 3,opp_inSide); //expand selection to rest of entries

  if(num_levels > 1){
    expand(2,'d');
    stack(num_levels - 1,'d',"");
    expand((num_levels - 1) * 4,'d');
    //stack into structure with a option to not overwrite blocks
    stack((inBits * 2) - 1,opp_stSide,"a");
    delete(); //remove start blocks
    contract(((num_levels - 1) * 4) + 2,'u');
  }
  else{
    stack((inBits * 2) - 1,opp_stSide,"a");
    delete(); //remove start blocks
  }

  //collaps and shift selection for encoding
  contract(((entries-1) * 3) + 2,inSide);
  contract(1,'d');
  shift(1,opp_stSide);
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

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = 4;
  B.relDir = '\0';

  comment("lvlDown");
  shift(1,stSide);
  setredTorch(stSide);
  shift(2,'d');
  setBlock(B,1);
  overlay("redstone",0);
  shift(1,oppDir(stSide));
  setredTorch(oppDir(stSide));
  shift(2,'d');
  comment("endlvlDown");
}

unsigned char buildEncoder(unsigned char rows, unsigned char collumns,unsigned char Table[rows][collumns],short inBits, unsigned char outBits, char stSide,char inSide, char outSide){
  comment("buildEncoder");

  //how manny num_levels of outputs will i need?
  char num_levels = (outBits / inBits) + 1;
  unsigned short entries = pow(2,inBits);

  //build encoder sturcture middle section (big nand gate per entry)

  //get to start of output wires
  shift(4,'d');
  shift(1,oppDir(stSide));

  unsigned char outstrength;

  //nothing mutch to see here just some scripted actions to build support structures
  outstrength = buildOutLines(outBits,inBits,inSide,outSide,stSide);
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

  return outstrength;
}

void buildSegment(char Dir){
  comment("buildSegment");

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Lamp");
  B.value = 0;
  B.relDir = '\0';

  shift(1,Dir);
  setBlock(B,1);
  stack(2,Dir,"");
  shift(1,oppDir(Dir));
  comment("endbuildSegment");
}

void seperateNibble(char* type, unsigned char value, char inSide, char stSide){
  comment("seperateNibble");

  //pack B
  struct block B;
  B.type = blockLookup(type);
  B.value = value;
  B.relDir = '\0';

  shift(6,oppDir(inSide));
  shift(2,'d');
  shift(1,stSide);
  setBlock(B,1);
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

  char opp_Stside = oppDir(stSide);

  //get to corner of display
  shift(1,stSide);
  shift(5,oppDir(inSide));
  shift(3,'d');

  //pack ice
  struct block ice;
  ice.type = blockLookup("ice");
  ice.value = value;
  ice.relDir = '\0';

  //pack case
  struct block casing;
  casing.type = blockLookup(type);
  casing.value = 15;
  casing.relDir = '\0';

  expand(12,'u');
  expand(8,opp_Stside);
  expand(1,oppDir(inSide));
  setBlock(casing,1);
  contract(1,oppDir(inSide));
  contract(1,opp_Stside);
  contract(1,stSide);
  replace("wool",0,0,type,1,value);
  contract(1,oppDir('u'));
  contract(1,oppDir('d'));
  setBlock(ice,1);
  shift(1,inSide);
  contract(1,opp_Stside);
  contract(5,stSide);
  contract(9,'d');
  contract(1,'u');

  for(short i = 0; i < 2;i++){
    buildSegment('u');
    buildSegment('r');
    if(i != 2)
    shift(4,'u');
  }
  shift(4,opp_Stside);
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
  unsigned char value = 7;
  if(move){
    wire(10,4,value,2,oppDir(inSide));
    shift(1,Dir);
    wire(10,4,value,2,oppDir(inSide));
  }
  else{
    wire(10,4,value,4,oppDir(inSide));
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
  wire(10,4,value,2,oppDir(inSide));
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

  if(arg.name == NULL){
    printf("NULL NAME\n");
    getchar();
  }

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

  unsigned char outStrength = 0;

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

  for(unsigned char i = 0; i < ret.width; i++){
    ret.strength[i] = outStrength;
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

struct buss buildPermute
(
  char* name,struct buss inBuss, /*buss info 1-2*/
  unsigned char tableLength,unsigned char permTable[tableLength], /*permTable info 3-4*/
  unsigned char groupSize, unsigned char groupSpacing,/*group info 5-6*/
  char forward, char stSide, char inSide,/*direction info 7-9*/
  _Bool startAtIn, _Bool stayAtOut /*movment info 10-11*/
){

  comment("buildPermute");
  if(startAtIn){
    if(stSide == inSide){
      shift(2,'u');
      shift(2,oppDir(inSide));
      shift(2,oppDir(forward));
    }
    else{
      shift(2,'u');
      shift((tableLength) * 2,oppDir(inSide));
      shift(2,oppDir(forward));
    }
  }

  struct buss outBuss;
  outBuss.width = tableLength;
  outBuss.collors = malloc(tableLength);
  outBuss.strength = malloc(tableLength);
  outBuss.name = name;
  outBuss.loc.direction = oppDir(forward);
  outBuss.loc.stSide = stSide;

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
    if(stSide == inSide)
    inLength = (index + 1) * 2;
    else
    inLength = ((tableLength - index)*2);

    //shift to input
    shift(inLength,inSide);

    //wire to input
    inStreangth = wire(inStreangth,4,dataValueIn,inLength,oppDir(inSide));

    //where the busses meet
    stairs("redstone_Lamp",0,oppDir(forward),'u',2);

    //wire to output
    inStreangth = wire(inStreangth,4,dataValueOut,outLength - 2,oppDir(forward));

    //output reached record its strength
    outBuss.strength[index] = inStreangth;

    //dont shift at the end
    if(index != tableLength -1){
      shift(2,oppDir(stSide));
    }
  }
  shift((tableLength -1) * 2,stSide);

  if(!stayAtOut){
    if(stSide == inSide){
      shift(2,'d');
      shift(2,inSide);
      shift(2,forward);
    }
    else{
      shift(2,'d');
      shift(tableLength * 2,inSide);
      shift(2,forward);
    }
  }

  comment("endbuildPermute");
  return outBuss;
}

void breakout(unsigned char value,char outSide){
  comment("breakout");

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.relDir = '\0';

  shift(1,outSide);
  setBlock(B,1);
  shift(1,'u');
  setRepeater(outSide,0);
  shift(5,'d');
  shift(1,oppDir(outSide));
  comment("endbreakout");
}

void tower2(unsigned char value,unsigned char levels){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.relDir = '\0';

  comment("tower2");
  setBlock(B,1);
  shift(1,'u');
  setredTorch('u');
  expand(1,'d');
  stack(levels,'u',"s");
  replace("redstone_torch",0,0,"redstone",1,0);
  contract(1,'d');
  comment("endtower2");
}

void invertor(char value, char dir){
  struct block B;
  B.value = value;
  B.type = blockLookup("wool");
  B.relDir = '\0';
  shift(1,oppDir(dir));
  setBlock(B,1);
  shift(1,dir);
  setredTorch(dir);
}

struct buss bussUp(_Bool type, struct buss arg, short distance, char up, _Bool flip){
  comment("bussUp");

  if(flip){
    arg.loc.direction = oppDir(arg.loc.direction);
  }

  char Dir = arg.loc.direction;

  unsigned char width = arg.width;
  for(unsigned char i = 0; i < width; i++){

    char opp_Dir = oppDir(Dir);
    char opp_Stside = oppDir(arg.loc.stSide);

    if(type){

      if(i == 0){

        tower(distance,up,arg.collors[0]);

        flipFlop(Dir);

        expand(distance - 2,oppDir(up));
        expand(1,opp_Stside);

        if(up == 'd'){
          shift(3,opp_Dir);
          shift(2,'u');
          stack(arg.width - 1,opp_Stside,"");
        }
        else{
          shift(3,opp_Dir);
          stack(arg.width - 1,opp_Stside,"");
        }

        shift(1,up);
        contract(distance - 2,up);
        expand(2,Dir);
        stack(arg.width - 1,opp_Stside,"");
        contract(2,opp_Dir);
        contract(1,arg.loc.stSide);
      }
      if(i != width - 1){

        //pack B
        struct block B;
        B.type = blockLookup("wool");
        B.relDir = '\0';
        B.value = arg.collors[i+1];

        shift(2,opp_Stside);
        setBlock(B,1);
      }
      else{
        shift(1,'d');
        shift(3,Dir);
      }
      arg.strength[i] = 14;
    }
    else{
      shift(1,'u');
      tower2(arg.collors[i],distance / 2);
      // printf("TC %i ",arg.collors[i+1]);
      if(i != width - 1){
        shift(distance + 1,'d');
        shift(2,opp_Stside);
      }
    }
  }
  shift((width - 1) * 2,arg.loc.stSide);
  comment("endbussUp");
  return arg;
}

unsigned char **rotateTable(unsigned char rows, unsigned char cols, unsigned char table[rows][cols],unsigned char rotations, unsigned char originaLength){

  unsigned char** ret = malloc(rows * sizeof(unsigned char*));

  for(unsigned char i = 0; i < rows; i++){
    ret[i] = malloc(cols);
  }

  for(unsigned char i = 0; i < rows; i++){
    for(unsigned char j = 0; j < cols; j++){
      ret[i][j] = table[i][j];
    }
  }

  short start[2] = {rows - 1,(rows/2) - 1};
  short end[2] = {rows / 2,0};

  short outLimmitstart[2] = {(originaLength/2) + 1,1};
  short outLimmitend[2] = {originaLength + 1,(originaLength/2) + 1};

  for(unsigned char k = 0; k < rotations; k++){
    for(short side = 0; side < 2; side++){
      for(short i = start[side]; i >= end[side]; i--){
        for(short j = cols - 1; j >= 0; j--){
          ret[i][j]++;
          if(ret[i][j] >= outLimmitend[side])
          ret[i][j] = outLimmitstart[side];
        }
      }
    }
  }

  return ret;
}

struct keySchedual{
  struct buss roundKeys[16];
};

struct keySchedual setKeySchedual(){
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
  key.name = "input_Key";

  for(int i = 0; i < 64; i++){
    key.collors[i] = i% 16;
    key.strength[i] = 10;
  }

  unsigned char PC1Length = sizeof(PC1) / sizeof(PC1[0]);
  struct buss PC1Results = buildPermute("PC1",key,PC1Length,PC1,1,0,PC1outSide,PC1stSide,PC1inSide,1,1);
  freeBuss(key);

  unsigned char num_shifts = sizeof(keyshifts) / sizeof(keyshifts[0]);

  unsigned char PC2Length = sizeof(PC2) / sizeof(PC2[0]);
  unsigned char rotated[PC2Length];

  for(unsigned char j = 0; j < PC2Length; j++){
    rotated[j] = PC2[j];
  }

  struct keySchedual keyschedual;

  for(unsigned char i = 0; i < num_shifts; i++){
    short start[2] = {PC2Length - 1,(PC2Length/2) - 1};
    short end[2] = {PC2Length / 2,0};

    short outLimmitstart[2] = {(PC1Length/2) + 1,1};
    short outLimmitend[2] = {PC1Length + 1,(PC1Length/2) + 1};

    for(unsigned char k = 0; k < keyshifts[i]; k++){
      for(short side = 0; side < 2; side++){
        for(short i = start[side]; i >= end[side]; i--){
          rotated[i]++;
          if(rotated[i] >= outLimmitend[side])
          rotated[i] = outLimmitstart[side];
        }
      }
    }

    if(i == 0){
      PC1Results = bussUp(0,PC1Results,16*12 - 6,'u',1);
      shift(15*12,'d');
    }
    else
    shift(12,'u');

    if(i != num_shifts - 1)
    keyschedual.roundKeys[i] = buildPermute("PC2",PC1Results,PC2Length,rotated,1,0,PC2outSide,PC2stSide,PC2inSide,1,0);
    else
    keyschedual.roundKeys[i] = buildPermute("PC2",PC1Results,PC2Length,rotated,1,0,PC2outSide,PC2stSide,PC2inSide,1,1); //stay at end at end
    keyschedual.roundKeys[i].name = "round_Key"; //give it a name
  }

  freeBuss(PC1Results);

  comment("endsetKeySchedual");
  return keyschedual;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int KaLookup(unsigned char part){
  if(part == PartLookup("ice")) return 0x030303;
  else if(part == PartLookup("slime")) return 0x030303;
  else if(part == PartLookup("redstone_On")) return 0x505050;
  else return 0x000000;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int KsLookup(unsigned char part){
  if(part == PartLookup("ice")) return 0x030303;
  else if(part == PartLookup("slime")) return 0x030303;
  else return 0x000000;
}

//i have no idea what this value does all i know it is in RGB format
unsigned int TfLookup(unsigned char part){
  if(part == PartLookup("ice")) return 0x030303;
  else if(part == PartLookup("slime")) return 0x030303;
  else return 0x000000;
}

//illumination format 4 for transparrant materials
char illumLookup(unsigned char part){
  if(part == PartLookup("ice")) return 4;
  else if(part == PartLookup("slime")) return 4;
  else return 0;
}

//how mutch the light is disolved while passing through transparrand material
float dLookup(unsigned char part){
  if(part == PartLookup("ice")) return .3;
  else if(part == PartLookup("slime")) return .6;
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
  fprintf(Mtl,"newmtl %s:%i\n\n",reversePartLookup(part),value);

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

  FILE * Mtl = fopen("virtexMap.mtl","w");//file pointer

  size_t numParts = sizeof(partTable) / sizeof(partTable[0]);

  unsigned char woolID = PartLookup("wool");
  unsigned char woodID = PartLookup("wood");

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

  _Bool L = !lastX && redType(map[getIndex(x+1,z,y,mapH,mapD)].type);
  _Bool R = !frstX && redType(map[getIndex(x-1,z,y,mapH,mapD)].type);
  _Bool F = !lastY && redType(map[getIndex(x,z,y+1,mapH,mapD)].type);
  _Bool B = !frstY && redType(map[getIndex(x,z,y-1,mapH,mapD)].type);

  _Bool U = !lastZ && solid(map[getIndex(x,z+1,y,mapH,mapD)].type);

  unsigned char redstoneID = blockLookup("redstone");

  _Bool LU = !lastZ && !lastX && map[getIndex(x+1,z+1,y,mapH,mapD)].type == redstoneID && !U;
  _Bool RU = !lastZ && !frstX && map[getIndex(x-1,z+1,y,mapH,mapD)].type == redstoneID && !U;
  _Bool FU = !lastZ && !lastY && map[getIndex(x,z+1,y+1,mapH,mapD)].type == redstoneID && !U;
  _Bool BU = !lastZ && !frstY && map[getIndex(x,z+1,y-1,mapH,mapD)].type == redstoneID && !U;

  _Bool LD = !lastX && !frstZ && map[getIndex(x+1,z-1,y,mapH,mapD)].type == redstoneID && !solid(map[getIndex(x+1,z,y,mapH,mapD)].type);
  _Bool RD = !frstX && !frstZ && map[getIndex(x-1,z-1,y,mapH,mapD)].type == redstoneID && !solid(map[getIndex(x-1,z,y,mapH,mapD)].type);
  _Bool FD = !frstZ && !lastY && map[getIndex(x,z-1,y+1,mapH,mapD)].type == redstoneID && !solid(map[getIndex(x,z,y+1,mapH,mapD)].type);
  _Bool BD = !frstZ && !frstY && map[getIndex(x,z-1,y-1,mapH,mapD)].type == redstoneID && !solid(map[getIndex(x,z,y-1,mapH,mapD)].type);

  struct redShape ret;

  ret.extend[0] = L || LU || LD || ((RU || RD || R) && !lastX && solid(map[getIndex(x+1,z,y,mapH,mapD)].type));
  ret.extend[1] = R || RU || RD || ((LU || LD || L) && !frstX && solid(map[getIndex(x-1,z,y,mapH,mapD)].type));
  ret.extend[2] = F || FU || FD || ((BU || BD || B) && !lastY && solid(map[getIndex(x,z,y+1,mapH,mapD)].type));
  ret.extend[3] = B || BU || BD || ((FU || FD || F) && !frstY && solid(map[getIndex(x,z,y-1,mapH,mapD)].type));

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

unsigned char numerateDir(char Dir){
  switch(Dir){
    case 'l': return 0;
    case 'r': return 1;
    case 'f': return 2;
    case 'b': return 3;
    case 'u': return 4;
    case 'd': return 5;
    //error
    default : printf("%15s - Invalid Direction received :%c > %i\n","numerateDir()",Dir,Dir);
  }
  return 0;
}

struct subVoxelPack getsubVoxels(short x, short z, short y){

  int index = getIndex(x,z,y,mapH,mapD);

  unsigned char type = map[index].type;
  unsigned char value = map[index].value;
  char Dir = map[index].relDir;

  char adjacent[6];

  adjacent[0] = (x == mapW - 1? 0 : map[getIndex(x + 1,z,y,mapH,mapD)].type);
  adjacent[1] = (x == 0? 0 : map[getIndex(x - 1,z,y,mapH,mapD)].type);
  adjacent[2] = (y == mapD - 1? 0 : map[getIndex(x,z,y + 1,mapH,mapD)].type);
  adjacent[3] = (y == 0? 0 : map[getIndex(x,z,y - 1,mapH,mapD)].type);
  adjacent[4] = (z == mapH - 1? 0 : map[getIndex(x,z + 1,y,mapH,mapD)].type);
  adjacent[5] = (z == 0? 0 : map[getIndex(x,z - 1,y,mapH,mapD)].type);

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

    unsigned char N_RD = numerateDir(oppDir(Dir));
    unsigned char N_D = numerateDir(Dir);

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

    unsigned char N_RD = numerateDir(oppDir(Dir));
    unsigned char N_D = numerateDir(Dir);

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
    //shift head over a bit awway from connecting block moves .1 tward Dir

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

    unsigned char N_RD = numerateDir(oppDir(Dir));
    unsigned char N_D = numerateDir(Dir);

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
  FILE * Obj = fopen("virtexMap.obj","w");//file pointer

  fprintf(Obj,"mtllib virtexMap.mtl\n");

  int vc = 0;//virtex count

  for(short x = 0; x < mapW; x++){
    for(short z = 0; z < mapH; z++){
      for(short y = 0; y < mapD; y++){
        int index = getIndex(x,z,y,mapH,mapD);
        if(map[index].type != 0){
          char type = map[index].type;
          unsigned char data = map[index].value;

          struct subVoxelPack voxPack;

          voxPack = getsubVoxels(x,z,y);

          for(int i = 0; i < voxPack.num_vox; i++){
            if(voxPack.voxs[i].exist){
              //these blocks have different materials based on the datavalue
              fprintf(Obj,"usemtl ");
              if(type == blockLookup("wool") || type == blockLookup("wood"))
              fprintf(Obj,"%s:%i\n",getPartnameForBlock(type,i),data);
              //the rest are just 0
              else fprintf(Obj,"%s:0\n",getPartnameForBlock(type,i));
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


struct buss bussStraight(struct buss arg, short distance){
  comment("bussStraight");
  unsigned char width = arg.width;
  char bussDir = arg.loc.direction;
  for(unsigned char i = 0; i < width; i++){
    arg.strength[i] = wire(arg.strength[i],5,arg.collors[i],distance,bussDir);
    if(i != width - 1){
      shift(2,oppDir(arg.loc.stSide));
      shift(distance,oppDir(bussDir));
    }
  }
  shift((width - 1) * 2,arg.loc.stSide);
  comment("endbussStraight");
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
  free(map);
  printf("free(bMap.map);\n");
}


void bitXOR1(char Dir){


  //pack B1
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.relDir = '\0';

  //pack B2
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 4;
  B2.relDir = '\0';

  comment("bitXOR1");
  setBlock(B1,1);
  shift(1,'u');
  setRepeater(Dir,0);
  shift(2,'d');
  shift(1,Dir);
  setBlock(B2,1);
  overlay("redstone",0);
  shift(2,'u');
  setBlock(B1,1);
  overlay("redstone",0);
  shift(1,Dir);
  shift(1,'u');
  setBlock(B1,1);
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
  wire(14,1,4,1,Dir);
  shift(1,Dir);
  shift(1,'u');
  setBlock(B2,1);
  shift(1,'u');
  setredTorch('u');
  shift(1,'u');
  setBlock(B1,1);
  overlay("redstone",0);
  comment("endbitXOR1");
}

void bitXOR2(char Dir){

  //pack B1
  struct block B1;
  B1.type = blockLookup("wool");
  B1.value = 5;
  B1.relDir = '\0';

  //pack B2
  struct block B2;
  B2.type = blockLookup("wool");
  B2.value = 4;
  B2.relDir = '\0';

  char opp_Dir = oppDir(Dir);

  comment("bitXOR2");
  shift(1,Dir);
  shift(1,'u');
  setBlock(B2,1);
  expand(1,'u');
  stack(1,'u',"");
  shift(1,'u');
  stack(1,'u',"");
  shift(1,'d');
  shift(1,Dir);
  contract(1,'u');
  setBlock(B1,1);
  expand(1,'d');
  stack(1,'u',"");
  expand(1,opp_Dir);
  expand(3,'u');
  stack(1,Dir,"");
  shift(1,Dir);
  stack(2,Dir,"");
  shift(1,opp_Dir);
  contract(3,'d');
  contract(1,'u');
  contract(1,opp_Dir);
  setredTorch(opp_Dir);
  shift(2,'u');
  setredTorch(opp_Dir);
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir,0);
  shift(2,'d');
  setredTorch(Dir);
  shift(2,'d');
  setredTorch(Dir);
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir,0);
  shift(2,'u');
  setRepeater(opp_Dir,0);
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir,0);
  shift(2,'d');
  setredTorch('u');
  shift(1,'u');
  shift(1,Dir);
  setRepeater(Dir,0);
  shift(2,'d');
  setRepeater(opp_Dir,0);
  shift(1,'u');
  shift(1,Dir);
  setredTorch(opp_Dir);
  shift(2,'d');
  setredTorch(opp_Dir);
  shift(4,'u');
  setRepeater(Dir,0);
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

  unsigned char PLength = sizeof(P) / sizeof(P[0]);

  struct buss* Block = malloc(2 * sizeof(struct buss));
  Block[0].name = "left_Block";
  Block[0].loc.direction = 'b';
  Block[1].name = "Right_Block";
  Block[1].loc.direction = 'f';
  for(unsigned char H = 0; H < 2; H++){ //both halves
    Block[H].width = PLength;
    Block[H].loc.stSide = 'l';
    Block[H].collors  = malloc(PLength);
    Block[H].strength = malloc(PLength);
    Block[H].collors  = malloc(PLength);
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
  char opp_oldDir = oppDir(oldDir);
  char opp_newDir = oppDir(newDir);

  short width = arg.width;

  for(int i = 0; i < width; i++){

    const unsigned char collor = arg.collors[i];

    if(flip){
      if(arg.loc.stSide == newDir){
        arg.strength[i] = wire(arg.strength[i],1,collor,((width - 1) - i) * 2,oldDir);
        tower(depth,'d',collor);
        arg.strength[i] = wire(16,1,collor,i * 2,newDir);
        if(i != width -1){
          shift(((width - 1) - i) * 2,opp_oldDir);
          shift(i * 2,opp_newDir);
          shift(2,opp_newDir);
          shift(depth,'u');
        }
      }
      else{
        arg.strength[i] = wire(arg.strength[i],1,collor,i * 2,oldDir);
        tower(depth,'d',collor);
        arg.strength[i] = wire(16,1,collor,((width - 1) - i) * 2,newDir);
        if(i != width -1){
          shift(i * 2,opp_oldDir);
          shift(((width - 1) - i) * 2,opp_newDir);
          shift(2,newDir);
          shift(depth,'u');
        }
      }
    }
    else{
      if(arg.loc.stSide == newDir){
        arg.strength[i] = wire(arg.strength[i],1,collor,i * 2,oldDir);
        arg.strength[i] = wire(arg.strength[i],1,collor,i * 2,newDir);
        if(i != width -1){
          shift(i * 2,opp_oldDir);
          shift(i * 2,opp_newDir);
          shift(2,opp_newDir);
        }
      }
      else{
        arg.strength[i] = wire(arg.strength[i],1,collor,((width - 1) - i) * 2,oldDir);
        arg.strength[i] = wire(arg.strength[i],1,collor,((width - 1) - i) * 2,newDir);
        if(i != width -1){
          shift(((width - 1) - i) * 2,opp_oldDir);
          shift(((width - 1) - i) * 2,opp_newDir);
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
      shift((width - 1) * 2,opp_oldDir);
    }
  }
  else{
    if(arg.loc.stSide != newDir){
      shift((width - 1) * 2,opp_oldDir);
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

struct buss halfSwapSides(struct buss inBuss,short amount,char turnDir,char shiftDir){
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

    if(inBuss.loc.stSide != shiftDir){
      for(short j = 0; j < bits/4; j++){
        unsigned char index = i*bits/4 + j;
        subBuss.collors[j] = inBuss.collors[index];
        subBuss.strength[j] = inBuss.strength[index];
      }
    }
    else{
      for(short j = 0; j < bits/4; j++){
        unsigned char index = ((3 - i) *bits/4 + j);
        subBuss.collors[j] = inBuss.collors[index];
        subBuss.strength[j] = inBuss.strength[index];
      }
    }

    if(i != 3){
      subBuss = bussStraight(subBuss,2);
      subBuss = turnBuss(subBuss,turnDir,1,6 - (i*2));
    }
    else{
      subBuss = bussStairs(subBuss,2,'u');
      subBuss = turnBuss(subBuss,turnDir,1,2);
    }

    subBuss = bussStraight(subBuss,(((amount - 2) - (bits/4 - 1)) * 2) + 4);
    char shiftDir = oppDir(subBuss.loc.direction);

    subBuss = turnBuss(subBuss,turnDir,0,0); //direction not used
    subBuss = bussStraight(subBuss,2);

    if(i != 3){
      subBuss = bussUp(1,subBuss,6 - (i * 2),'u',0);
    }
    else{
      subBuss = bussFlipFlop(subBuss);
    }
    shift(1,subBuss.loc.direction);
    subBuss = bussStraight(subBuss,1);

    if(i != 3){
      shift(5,oppDir(subBuss.loc.direction));
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
  unsigned char roundWidth = roundKey.width;
  char oppRoundDir = oppDir(roundDir);

  bussUp(1,roundKey,4,'d',0);
  shift(1,'d');
  bussStraight(roundKey,0);
  shift(5,'u');
  shift(8,roundDir);
  unsigned char PLength = sizeof(P) / sizeof(P[0]);
  bussStraight(roundKey,(PLength * 2) + 1);
  struct buss sBoxResults = buildStables(roundKey,roundDir,roundStSide);
  struct buss PResults = buildPermute("P_Results",sBoxResults,PLength,P,4,4,'f',oppRoundDir,roundDir,1,1);
  shift(6,'d');
  shift(1,'b');
  shift(((PLength * 2)),roundDir);
  unsigned char ELength = sizeof(E) / sizeof(E[0]);
  struct buss EResults = buildPermute("E_Results",block[1],ELength,E,1,0,roundDir,'b','b',1,1);
  bussStraight(EResults,(PLength * 2) - 1);
  shift(1,'u');
  shift(8,oppRoundDir);
  XOR(0,roundKey,EResults);
  shift(8,'d');
  shift(2,roundDir);
  shift(3,'b');
  bussUp(0,block[0],4,'u',0);
  shift(1,'u');
  shift(1,'f');
  block[0] = XOR(1,block[0],PResults);
  block[0] = bussStraight(block[0],1);
  shift(1,block[0].loc.direction);
  block[0] = halfSwapSides(block[0],PLength,oppRoundDir,roundDir);
  block[1] = bussUp(0,block[1],4,'u',0);
  shift(12,'d');
  block[1] = bussStraight(block[1],(roundWidth * 2) + 3);
  block[1] = bussUp(1,block[1],6,'u',0);
  shift(1,block[1].loc.direction);
  block[1] = bussStraight(block[1],1);
  shift(roundWidth, roundDir);
  block[1] = halfSwapSides(block[1],PLength,oppRoundDir,oppRoundDir);
  shift(roundWidth,oppRoundDir);
  block[1] = bussUp(0,block[1],4,'u',0);
  block[1] = bussStraight(block[1],(roundKey.width * 2) + 1);
  block[1] = bussStairs(block[1],1,'u');

  block[0].loc.direction = oppDir(block[0].loc.direction);
  block[1].loc.direction = oppDir(block[1].loc.direction);

  shift(12,oppDir(roundDir));
  shift(8,'u');
  shift(2,'f');

  comment("endbuildRound");
  return block;
}

struct bussPair{
  struct buss Tap1;
  struct buss Tap2;
};

struct bussPair bussTap(struct buss arg,char direction){

  comment("bussTap");

  //pack B
  struct block B;
  B.type = blockLookup("redstone_Lamp");
  B.value = 0;

  char oldDir = arg.loc.direction;
  char newDir = turnDir(oldDir,direction);
  char opp_oldDir = oppDir(oldDir);
  char opp_newDir = oppDir(newDir);

  short width = arg.width;


  struct bussPair ret;

  ret.Tap1.collors = malloc(width);
  ret.Tap1.strength = malloc(width);
  ret.Tap1.loc.stSide = arg.loc.stSide;
  ret.Tap1.width = width;

  ret.Tap2.collors = malloc(width);
  ret.Tap2.strength = malloc(width);
  ret.Tap2.loc.stSide = arg.loc.stSide;
  ret.Tap2.width = width;

  ret.Tap1.loc.direction = oldDir;
  ret.Tap2.loc.direction = newDir;

  for(int i = 0; i < width; i++){

    unsigned char collor = arg.collors[i];

    ret.Tap1.collors[i] = collor;
    ret.Tap2.collors[i] = collor;

    short longDist = ((width - 1) - i) * 2;
    short shortDist = i * 2;

    if(arg.loc.stSide == newDir){
      arg.strength[i] = wire(arg.strength[i],1,collor,longDist,oldDir);
      shift(1,'u');
      setRepeater(oldDir,0);
      shift(1,oldDir);
      setBlock(B,1);
      shift(1,'d');
      shift(1,oldDir);
      ret.Tap1.strength[i] = wire(15,1,collor,shortDist,oldDir);
      shift(shortDist + 1,opp_oldDir);
      shift(1,'d');
      ret.Tap2.strength[i] = wire(15,1,collor,0,newDir);
      shift(1,'d');
      ret.Tap2.strength[i] = wire(ret.Tap2.strength[i],1,collor,shortDist + 1,newDir);
      shift(2,'u');
      if(i != width - 1){
        shift(longDist + 1,opp_oldDir);
        shift(shortDist + 3,opp_newDir);
      }
      else{
        shift(longDist + 1,opp_newDir);
        shift(1,opp_oldDir);
      }
    }
    else{
      arg.strength[i] = wire(arg.strength[i],1,collor,shortDist,oldDir);
      shift(1,'u');
      setRepeater(oldDir,0);
      shift(1,oldDir);
      setBlock(B,1);
      shift(1,'d');
      shift(1,oldDir);
      ret.Tap1.strength[i] = wire(15,1,collor,longDist,oldDir);
      shift(longDist + 1,opp_oldDir);
      shift(1,'d');
      ret.Tap2.strength[i] = wire(15,1,collor,0,newDir);
      shift(1,'d');
      ret.Tap2.strength[i] = wire(ret.Tap2.strength[i],1,collor,longDist + 1,newDir);
      shift(2,'u');
      if(i != width - 1){
        shift(shortDist + 1,opp_oldDir);
        shift((longDist + 1) - 2,opp_newDir);
      }
      else{
        shift(shortDist + 1,opp_newDir);
        shift(1,opp_oldDir);
      }
    }
  }
  ret.Tap2.loc.stSide = turnDir(ret.Tap2.loc.stSide,direction);
  if(arg.loc.stSide != newDir){
    shift((width - 1) * 2,opp_oldDir);
  }
  else{
    shift((width - 1) * 2,oldDir);
  }
  ret.Tap2.loc.stSide = oppDir(ret.Tap2.loc.stSide);

  ret.Tap1.name = "Tap1";
  ret.Tap2.name = "Tap2";

  comment("endbussTap");
  return ret;
}

struct DESKeys{
  struct buss encrypt[16];
  struct buss decrypt[16];
};

struct DESKeys buildRoundKeyBuss(struct keySchedual keySchedual){
  comment("buildRoundKeyBuss");

  short block_space = 30;
  short row_space = 12;

  char round_Stside = keySchedual.roundKeys[0].loc.stSide;
  char round_Dir = keySchedual.roundKeys[0].loc.direction;
  unsigned char round_width = keySchedual.roundKeys[0].width;

  struct DESKeys ret;

  // printBussInfo(DESKeys[1],"beforeBussDown");

  unsigned char numKeys = 4;

  for(int i = 0; i < numKeys; i++){
    printf("1 key# %i \n",i);
    struct bussPair tapPair = bussTap(keySchedual.roundKeys[i],'l');
    ret.encrypt[i] = tapPair.Tap1;
    ret.decrypt[i] = tapPair.Tap2;
    ret.decrypt[i].name = "DECRYPT";
    ret.encrypt[i].name = "ENCRYPT";
    if(i != numKeys - 1)
    shift(row_space,'d');
  }

  for(int i = 0; i < 16; i++){
    freeBuss(keySchedual.roundKeys[i]);
  }

  shift(round_width * 2,oppDir(round_Stside));
  shift(2,'d');
  shift(1,round_Dir);

  for(int i = 0; i < numKeys; i++){
    printf("2 key# %i \n",i);
    bussStraight(ret.decrypt[i],(i * 2) + 4);
    struct buss outKey = bussUp(1,ret.decrypt[i],i * row_space * 2 + block_space,'d',1);
    shift(1,outKey.loc.direction);
    bussStraight(outKey,(i * 2));

    ret.decrypt[i] = turnBuss(outKey,'l',0,0);
    shift(outKey.width * 2 - 2,outKey.loc.stSide);
    shift(outKey.width * 2 - 2,oppDir(outKey.loc.direction));

    shift(3,outKey.loc.direction);
    if(i != numKeys - 1){
      shift(3,oppDir(outKey.loc.direction));
      shift(i*row_space*2 + block_space + row_space,'u');
    }
  }

  comment("endbuildRoundKeyBuss");
  return ret;
}


void runTest(){
  wire(7,4,4,40,'f');
  wire(7,4,4,40,'r');
  buildImmages();
}

int main(){

  //memory allocation
  map = malloc(sizeof(struct block));



  free(script);
  //end portion of main (freeing and closing pointers)
  freeBlockMap();

  return 0;
}
