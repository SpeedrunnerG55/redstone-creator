#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char * main_Script;
unsigned long main_Length[1] = {0};

unsigned int longwaits = 0;

void printFileBuffer(char* buffer, char* name){

  char fileName[30];
  snprintf(fileName,30,"testing/%s.txt",name);

  FILE * mPtr = fopen(fileName,"w");
  // printf("%p > %s\n",(void*)mPtr,script);
  fprintf(mPtr,"$${\n");

  fprintf(mPtr,"%s\n",buffer);

  fprintf(mPtr,"UNSET(@done);\n");
  fprintf(mPtr,"}$$\n");
  fclose(mPtr);

}

short commandBuffer = 64;
unsigned short max_command = 0;

void toBuffer(char* buffer,unsigned long *bufferLength, char* message){
  if(strlen(message) > max_command && strlen(message) != 80){
    max_command = strlen(message);
    printf("newMax = %i\n",max_command);
  }

  _Bool printing = 1;
  if(printing){
    short length = strlen(message);
    printf("%s old buffLen = %lu\n",message,bufferLength[0]);
    buffer[bufferLength[0]] = 0; //place null terminator at end
    bufferLength[0] += length;
    printf("new buffLen = %lu\n",bufferLength[0]);
    buffer = realloc(buffer,(bufferLength[0]) + 1);
    strncat(buffer,message,bufferLength[0]);
  }
}

void comment(char* buffer, unsigned long *bufferLength,char * message){
  char comment[80] = "// ";
  strncat(comment,message,80);
  strncat(comment," \n",80);
  toBuffer(buffer,bufferLength,comment);
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

void waitUntlDone(){
  char *command = "UNSET(@done); log(waiting for done); DO; WAIT(20ms); UNTIL(@done); log(Done!); \n";
  toBuffer(main_Script,main_Length,command);
  longwaits++;
}

struct block{
  unsigned char type;
  unsigned char value;
  char relDir;
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

_Bool inSel(short x,short z,short y){
  struct extents sel = getAllSelExtents();
  return (x <= sel.l && x >= sel.r && z <= sel.u && z >= sel.d && y <= sel.f && y >= sel.b);
}

void memory_setBlock(struct block B){

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


//commands i can do
void script_setBlock(char* buffer, struct block B, _Bool wait){

  //unpack B
  char* type = reverseBlockLookup(B.type);
  unsigned char value = B.value;

  //in game
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//set %s:%i);\n",type,value);
  toBuffer(buffer,main_Length,command);
  if(wait)
  waitUntlDone();

  memory_setBlock(B);
}

void memory_Replace(char* typeA, _Bool specificA,char valueA, char* typeB,_Bool specificB, char valueB){

  //in memory
  struct extents selExt = getAllSelExtents();

  for(short x = selExt.r; x <= selExt.l; x++){
    for(short z = selExt.d; z <= selExt.u; z++){
      for(short y = selExt.b; y <= selExt.f; y++){
        int index = getIndex(x,z,y);
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

void script_Replace(char* buffer, char* typeA, _Bool specificA, char valueA, char* typeB,_Bool specificB, char valueB){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//replace %s",typeA);
  toBuffer(buffer,main_Length,command);
  if(specificA){
    snprintf(command,commandBuffer,":%i",valueA);
    toBuffer(buffer,main_Length,command);
  }
  snprintf(command,commandBuffer," %s",typeB);
  toBuffer(buffer,main_Length,command);
  if(specificB){
    snprintf(command,commandBuffer,":%i",valueB);
    toBuffer(buffer,main_Length,command);
  }
  toBuffer(buffer,main_Length,");\n");
  memory_Replace(typeA,specificA,valueA,typeB,specificB,valueB);
}

_Bool mask(unsigned char type){
  if(type == blockLookup("wool")) return 0;
  return 0;
}

//RGB masks
unsigned int R = 0xFF0000;
unsigned int G = 0x00FF00;
unsigned int B = 0x0000FF;

void darken(unsigned int* whole,float scaler){
  // remove old part then get new part ... getting the new part was hard...
  *whole = (*whole & ~R) | (((int)((*whole & R) * (scaler / 2)) & R) + (((*whole & R)/3) & R));
  *whole = (*whole & ~G) | (((int)((*whole & G) * (scaler / 2)) & G) + (((*whole & G)/3) & G));
  *whole = (*whole & ~B) | (((int)((*whole & B) * (scaler / 2)) & B) + (((*whole & B)/3) & B));
}

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
  printf("\r");

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
  FILE * Top = fopen("testing/topView.pnm","w");
  fprintf(Top,"P3\n");
  fprintf(Top,"%i %i\n",mapW,mapD);
  printf("%i %i\n",mapW,mapD);
  fprintf(Top,"255\n");
  for(short y = mapD - 1; y >=0; y--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int collor = backround;
      for(short z = 0; z < mapH; z++){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor > 0){
            collor = blockCollor;
            float scaler = (float)z / (mapH);
            darken(&collor,scaler);
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

  FILE * Front = fopen("testing/frontview.pnm","w");
  fprintf(Front,"P3\n");
  fprintf(Front,"%i %i\n",mapW,mapH);
  printf("%i %i\n",mapW,mapH);
  fprintf(Front,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short x = mapW - 1; x >= 0; x--){
      unsigned int  collor = backround;
      for(short y = mapD - 1; y >= 0; y--){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapD - y) / mapD;
            darken(&collor,scaler);
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

  FILE * Left = fopen("testing/leftview.pnm","w");
  fprintf(Left,"P3\n");
  fprintf(Left,"%i %i\n",mapD,mapH);
  printf("%i %i\n",mapD,mapH);
  fprintf(Left,"255\n");

  for(short z = mapH - 1; z >= 0; z--){
    for(short y = mapD - 1; y >= 0; y--){
      unsigned int  collor = backround;
      for(short x = 0; x < mapW; x++){
        int index = getIndex(x,z,y);
        if(!mask(map[index].type)){
          int blockCollor = collorLookup(map[index].value,partLookup(getPartnameForBlock(map[index].type,0)));
          if(blockCollor != 0){
            collor = blockCollor;
            float scaler = (float)(mapW - x) / mapW;
            darken(&collor,scaler);
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

//can be placed anywhere and wil show the selection
void showMap(char* state){
  printf("MAP:%20s H:%-5iW:%-5iD:%-5i A X:%-5iZ:%-5iY:%-5i B X:%-5iZ:%-5iY:%-5i\n",state,mapH,mapW,mapD,mapXA,mapZA,mapYA,mapXB,mapZB,mapYB);
}

//can be placed anywhere and wil show the selection
void showSelection(char* state){
  printf("SEL:%20s H:%-5iW:%-5iD:%-5i A X:%-5iZ:%-5iY:%-5i B X:%-5iZ:%-5iY:%-5i\n",state,selH,selW,selD,selXA,selZA,selYA,selXB,selZB,selYB);
}

//allocate more memory
void expandBlockMap(unsigned short change, char Dir){

  if(mapW > 700 || mapH > 700 || mapD > 700){
    printf("WARNING LARGE MAP!\n");
    showMap("MAP TOO BIG");
    showSelection("MAP TOO BIG");
    getchar();
    buildImmages();
  }

  if(Dir == 'l' || Dir == 'u' || Dir == 'f') *getMapExtent(Dir) += change; // twards positive numbers get bigger
  if(Dir == 'r' || Dir == 'd' || Dir == 'b') *getMapExtent(Dir) -= change; // twards negitive numbers get smaller

  unsigned short tempW = mapW;
  unsigned short tempH = mapH;
  unsigned short tempD = mapD;

  //adjust selection and map set points for shift
  if     (Dir == 'r'){ selXA += change; selXB += change; mapXA += change; mapXB += change; }
  else if(Dir == 'd'){ selZA += change; selZB += change; mapZA += change; mapZB += change; }
  else if(Dir == 'b'){ selYA += change; selYB += change; mapYA += change; mapYB += change; }

  struct block*** cuboid = rollMap(map);

  //calculate the highth the width and the breadth...
  if     (Dir == 'r' || Dir == 'l') mapW = abs(mapXB - mapXA) + 1;//if the direction is left or right the map will get thinner
  else if(Dir == 'u' || Dir == 'd') mapH = abs(mapZB - mapZA) + 1;//if the direction is up or down the map will get shorter
  else if(Dir == 'f' || Dir == 'b') mapD = abs(mapYB - mapYA) + 1;//if the direction is forward or backward the map will get shallower

  //memory allocation
  if(Dir == 'r' || Dir == 'l'){
    cuboid = realloc(cuboid,mapW * sizeof(struct block**));
    for(short x = tempW; x < mapW; x++){
      cuboid[x] = malloc(mapH * sizeof(struct block*));  // <here
      for(short z = 0; z < mapH; z++){
        cuboid[x][z] = malloc(mapD * sizeof(struct block)); // <here
        for(short y = 0; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].relDir = 0;
        }
      }
    }
    //shift data
    if(Dir == 'r'){
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
            cuboid[x][z][y].relDir = 0;
          }
        }
      }
    }
  }

  else if(Dir == 'u' || Dir == 'd'){
    for(short x = 0; x < mapW; x++){
      cuboid[x] = realloc(cuboid[x],mapH * sizeof(struct block*));
      for(short z = tempH; z < mapH; z++){
        cuboid[x][z] = malloc(mapD * sizeof(struct block));  // <here
        for(short y = 0; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].relDir = 0;
        }
      }
      //shift data
      if(Dir == 'd'){
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
            cuboid[x][z][y].relDir = 0;
          }
        }
      }
    }
  }

  else if(Dir == 'f' || Dir == 'b'){
    for(short x = 0; x < mapW; x++){
      for(short z = 0; z < mapH; z++){
        cuboid[x][z] = realloc(cuboid[x][z],mapD * sizeof(struct block));
        for(short y = tempD; y < mapD; y++){
          cuboid[x][z][y].type = 0;
          cuboid[x][z][y].value = 0;
          cuboid[x][z][y].relDir = 0;
        }
        //shift data
        if(Dir == 'b'){
          unsigned short shift_Amt = mapD - tempD;
          for(short y = mapD - 1; y >= shift_Amt; y--){
            cuboid[x][z][y].type = cuboid[x][z][y-shift_Amt].type;
            cuboid[x][z][y].value = cuboid[x][z][y-shift_Amt].value;
            cuboid[x][z][y].relDir = cuboid[x][z][y-shift_Amt].relDir;
          }
          //0 the data unshifted
          for(short y = shift_Amt - 1; y >= 0; y--){
            cuboid[x][z][y].type = 0;
            cuboid[x][z][y].value = 0;
            cuboid[x][z][y].relDir = 0;
          }
        }
      }
    }
  }
  map = unRollMap(cuboid,map);
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

void updateSelDementions(char Dir){
  if(Dir == 'u' || Dir == 'd')//if the direction is up or down the selection will get shorter
  selH = (*getSelExtent('u') - *getSelExtent('d')) + 1;
  else if(Dir == 'l' || Dir == 'r')//if the direction is left or right the selection will get thinner
  selW = (*getSelExtent('l') - *getSelExtent('r')) + 1;
  else if(Dir == 'f' || Dir == 'b')//if the direction is forward or backward the selection will get shallower
  selD = (*getSelExtent('f') - *getSelExtent('b')) + 1;
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


void memory_Expand(unsigned short amount,char Dir){
  if(amount > 0){
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
}

void Script_Expand(char* buffer, unsigned short amount,char Dir){
  if(amount > 0){
    char command[commandBuffer];
    snprintf(command,commandBuffer,"echo(//expand %i %c);\n",amount,Dir);
    toBuffer(buffer,main_Length,command);
    memory_Expand(amount,Dir);
  }
}

void memory_Contract(unsigned short amount,char Dir){

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

void script_Contract(char* buffer, unsigned short amount,char Dir){

  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//contract %i %c);\n",amount,Dir);
  toBuffer(buffer,main_Length,command);

  memory_Contract(amount,Dir);
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

void memory_Shift(unsigned short amount,char Dir){
  if(amount > 0){
    selectionShift(Dir,amount);
  }
}

void ScriptShift(unsigned short amount,char Dir){
  if(amount > 0){
    char command[commandBuffer];
    snprintf(command,commandBuffer,"echo(//shift %i %c);\n",amount,Dir);
    toBuffer(main_Script,main_Length,command);
    memory_Shift(Dir,amount);
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
        int orig_index = getSubIndex(x-selExt.r,z-selExt.d,y-selExt.b,selD,selH);
        int dest_index = getIndex(blockBX,blockBZ,blockBY);

        //place block
        map[dest_index] = arg[orig_index];
      }
    }
  }
}

void move(unsigned short amount,_Bool shift, char Dir){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"echo(//move %i %c",amount,Dir);
  toBuffer(main_Script,main_Length,command);
  //preemtivle test the area im moving into b/c its not in the selection
  checkAndExpand(amount,Dir); //wow that was easy

  if (shift){
    toBuffer(main_Script,main_Length," -s);\n");
  }
  else{
    toBuffer(main_Script,main_Length,");\n");
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
    toBuffer(main_Script,main_Length,command);

    _Bool preserveAir = 0;
    _Bool sel_shift = 0;

    for(unsigned int i = 0; i < strlen(options); i++){
      snprintf(command,commandBuffer," -%c",options[i]);
      toBuffer(main_Script,main_Length,command);
      preserveAir |= options[i] == 'a';
      sel_shift |= options[i] == 's';
    }
    toBuffer(main_Script,main_Length,");\n");
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
      selectionShift(Dir,shift);
    }
  }
}

void scriptedSetRepeater(){

  FILE* setR = fopen("testing/setR.txt","w");

  fprintf(setR,"$${\n");

  fprintf(setR,"SET(@#complete,0);\n");

  fprintf(setR,"&dirs[0] = \"N\"\n");
  fprintf(setR,"&dirs[1] = \"E\"\n");
  fprintf(setR,"&dirs[2] = \"S\"\n");
  fprintf(setR,"&dirs[3] = \"W\"\n");

  fprintf(setR,"&rel[0] = \"f\"\n");
  fprintf(setR,"&rel[1] = \"r\"\n");
  fprintf(setR,"&rel[2] = \"b\"\n");
  fprintf(setR,"&rel[3] = \"l\"\n");

  fprintf(setR,"indexof(&dirs[],#currentdir,%%DIRECTION%%)\n");
  fprintf(setR,"indexof(&rel[],#currentrelativedir,$$[1])\n");
  fprintf(setR,"#data = #currentdir + #currentrelativedir\n");
  fprintf(setR,"if(#data >= 4)\n");
  fprintf(setR,"    #data = #data - 4\n");
  fprintf(setR,"endif\n");

  fprintf(setR,"#value = #data + ($$[2] * 4); echo(//set redstone_repeater:%%#value%%);\n");
  fprintf(setR,"log(//set redstone_repeater:%%#value%%));\n");
  fprintf(setR,"UNSET(@done); log(waiting for done); DO; WAIT(20ms); UNTIL(@done); log(Done!); \n");

  fprintf(setR,"SET(@#complete,1);\n");

  fprintf(setR,"}$$\n");

  fclose(setR);
}

void memory_SetRepeater(char Dir,unsigned char m){

  //in memory
  unsigned char offset = 0;
  switch (Dir) {
    case 'f': offset = 0; break;
    case 'r': offset = 1; break;
    case 'b': offset = 2; break;
    case 'l': offset = 3; break;
  }

  //pack B
  struct block B;
  B.type = blockLookup("redstone_repeater");
  B.value = ((3 + offset) % 4) + (m * 4);
  B.relDir = Dir;

  memory_setBlock(B);

  //place block depenting on dirrection and offset on actual Dir

}

void setRepeater(char Dir,unsigned char m){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"EXEC(setR.txt,setR,%c,%i); DO; UNTIL(@#complete == 1);\n",Dir,m);
  toBuffer(main_Script,main_Length,command);
  memory_SetRepeater(Dir,m);
}

//bare bones scripted file i can execute rather than generaing its output (greatly reduces file sizes so i can have just the repeated script call rthther t han its contents)
void scriptedWire(){

  //short inStrength,unsigned char outToll, unsigned char value, unsigned short amount, char Dir

  FILE * wire = fopen("testing/wire.txt","w");
  fprintf(wire,"$${\n");

  //unpack parammeters

  fprintf(wire,"#inStrength = $$[1];\n");
  fprintf(wire,"#outToll    = $$[2];\n");
  fprintf(wire,"#value      = $$[3];\n");
  fprintf(wire,"#amount     = $$[4];\n");
  fprintf(wire,"#Dir        = $$[5];\n");

  fprintf(wire,"#oppDir;\n");

  //calculate the opposite direction from Dir
  fprintf(wire,"if      (#Dir == f) #oppDir = b;\n");
  fprintf(wire,"  elseif(#Dir == b) #oppDir = f;\n");
  fprintf(wire,"  elseif(#Dir == l) #oppDir = r;\n");
  fprintf(wire,"  elseif(#Dir == r) #oppDir = l;\n");
  fprintf(wire,"  else  log(Invalid Direction received :%%#Dir%%\n");
  fprintf(wire,"endif\n");

  //current position of end of selection
  fprintf(wire,"#pos = 0;\n");

  //current state of the output strength
  fprintf(wire,"#outStrength = #inStrength - #amount;\n");

  //support structure for redstone
  fprintf(wire,"echo(//set wool:%%#value%%);\n");
  fprintf(wire,"echo(//stack %%#amount%% %%#Dir%%");

  //shift up to do redstone
  fprintf(wire,"echo(//shift 1 u);\n");

  //check to see if the wire is longer than the input strength
  //if so it is safe to place the first repeater where the signal ends
  fprintf(wire,"if(#inStrength < #amount && #outStrength < #outToll)\n");
  fprintf(wire,"  echo(//shift %%#inStrength&& %%#Dir%%);\n");
  fprintf(wire,"  #pos = #pos + #inStrength;\n");
  fprintf(wire,"  EXEC(setR.txt,setR,#Dir,0);\n");
  fprintf(wire,"  DO; UNTIL(@#complete == 1)\n");
  fprintf(wire,"  #outStrength = 16 + #pos - %%#amount%%;");
  fprintf(wire,"endif\n");

  //check to see if there is enough room for at least 1 full 16 line
  fprintf(wire,"if((#amount - #inStrength - 1) / 16 > 0)\n");
  fprintf(wire,"echo(//shift 16 %%#Dir%%);\n");
  fprintf(wire,"  EXEC(setR.txt,setR,#Dir,0);\n");
  fprintf(wire,"  DO; UNTIL(@#complete == 1)\n");

  fprintf(wire,"  echo(//expand 15 %%#oppDir%%);\n"); //opposite
  //check to see if there is at least one more if so use stack to fill in the rest
  //-s option ensures im at the end of the lines and in position for the final line if needed
  fprintf(wire,"  echo(//stack ((#amount - #inStrength) / 16) - 1 %%#Dir%% -s");
  fprintf(wire,"  echo(//contract 15 %%#oppDir%%);\n"); //opposite
  fprintf(wire,"  #pos = #pos + ((#amount - #inStrength) / 16) * 16;\n");
  fprintf(wire,"  #outStrength = 16 + #pos - #amount;\n");
  fprintf(wire,"  if((#amount - #inStrength) / 16 == 0)\n");
  fprintf(wire,"    echo(//move 1 %%#oppDir%%");
  fprintf(wire,"    #outStrength = #outStrength - 1");
  fprintf(wire,"  endif\n");
  fprintf(wire,"endif\n");


  //if the output is still too weak go to the end and place a repeater just behind it
  //this will boost the output to the proper strength reguardless of previous actions
  fprintf(wire,"if((#outStrength < #outToll)\n");
  fprintf(wire,"  echo(//shift %%#amount%% - %%#pos%% - 1 %%#Dir%%);\n");
  fprintf(wire,"  #pos = #pos + amount - #pos - 1;\n");
  fprintf(wire,"  EXEC(setR.txt,setR,#Dir,0);\n");
  fprintf(wire,"  DO; UNTIL(@#complete == 1)\n");
  fprintf(wire,"  #outStrength = 16 + #pos - %%#amount%%;");
  fprintf(wire,"  echo(//shift 1 %%#Dir%%);\n");
  fprintf(wire,"  #outStrength = 15;");
  //if the output is proper strength go to end
  fprintf(wire,"else\n");
  fprintf(wire,"  echo(//shift (%%#amount%% - %%#pos%%) %%#Dir%%);\n");
  fprintf(wire,"  #pos = #pos + #amount - #pos;;\n");
  fprintf(wire,"endif\n");

  //place redstone
  fprintf(wire,"echo(//expand %%#amount%% %%#oppDir%%);\n"); //opposite
  fprintf(wire,"echo(//replace air redstone);\n");
  fprintf(wire,"echo(//contract %%#amount%% %%#Dir%%);\n");
  //go back down into end position
  fprintf(wire,"echo(//shift 1 d);\n");

  fprintf(wire,"}$$\n");
  fclose(wire);

}


unsigned char memoryWire(short inStrength,unsigned char outToll, unsigned char value, unsigned short amount, char Dir){

  //pack B
  struct block B;
  B.type = blockLookup("wool");
  B.value = value;
  B.relDir = '\0';

  comment(main_Script,main_Length,"wire");
  //current position of end of selection
  unsigned short pos = 0;
  //current state of the output strength
  short outStrength = inStrength - amount;
  //support structure for redstone
  memory_setBlock(B);
  stack(amount,Dir,"");
  //shift up to do redstone
  memory_Shift(1,'u');

  //check to see if the wire is longer than the input strength
  //if so it is safe to place the first repeater where the signal ends
  if(inStrength < amount && outStrength < outToll){
    memory_Shift(inStrength,Dir);
    pos += inStrength;
    memory_SetRepeater(Dir,0);
    outStrength = 16 + pos - amount;
  }
  //check to see if there is enough room for at least 1 full 16 line
  if((amount - inStrength - 1) / 16 > 0){
    memory_Shift(16,Dir);
    memory_SetRepeater(Dir,0);
    //check to see if there is at least one more if so use stack to fill in the rest
    if(((amount - inStrength - 1) / 16 > 0)){
      memory_Expand(15,oppDir(Dir));
      //-s option ensures im at the end of the lines and in position for the final line if needed
      stack(((amount - inStrength) / 16) - 1,Dir,"s");
      memory_Contract(15,Dir);
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
    memory_Shift(amount - pos - 1,Dir);
    pos += amount - pos - 1;
    memory_SetRepeater(Dir,0);
    outStrength = 16 + pos - amount;
    memory_Shift(1,Dir);
    outStrength = 15;
  }
  //if the output is proper strength go to end
  else{
    memory_Shift(amount - pos,Dir);
    pos += amount - pos;
  }
  //place redstone
  memory_Expand(amount,oppDir(Dir));
  memory_Replace("air",0,0,"redstone",0,0);
  memory_Contract(amount,Dir);
  //go back down into end position
  memory_Shift(1,'d');

  comment(main_Script,main_Length,"endwire");

  return outStrength;
}

unsigned char wire(short inStrength,unsigned char outToll, unsigned char value, unsigned short amount, char Dir){
  char command[commandBuffer];
  snprintf(command,commandBuffer,"EXEC(wire.txt,wire,%i,%i,%i,%i,%c); DO; UNTIL(@#complete == 1);\n",inStrength,outToll,value,amount,Dir);
  toBuffer(main_Script,main_Length,command);
  return memoryWire(inStrength,outToll,value,amount,Dir);
}

void freeBlockMap(){
  printf("free W%i D%i H%i %lu \n",mapW,mapD,mapH,mapW*mapD*mapH * sizeof(struct block));
  free(map);
  printf("free(bMap.map);\n");
}

int main() {

  scriptedSetRepeater();
  scriptedWire();
  getchar();

  //memory allocation
  map = malloc(sizeof(struct block));
  main_Script = malloc(1);


  wire(4,4,2,20,'r');

  // for(int i = 0; i < 4; i++){
  //   setRepeater('f',i);shift(2,'f');
  //   setRepeater('r',i);shift(2,'f');
  //   setRepeater('b',i);shift(2,'f');
  //   setRepeater('l',i);shift(6,'b');
  //   shift(2,'l');
  // }

  printFileBuffer(main_Script,"main");

  buildImmages();

  freeBlockMap();

  return 0;
}
