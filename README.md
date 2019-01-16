# redstone-creator
a system to design and create redstone circuitry in minecraft while implementing macro keybinding mod and world edit

code and implimentiation found in commandemulator.c, tables.h is needed for implimentation

there are several different hieracrchys of functoins that are defigned in commandemulator that will be defigned here


### type - 0 memory
  #### these are the functions that deal with block map memory managment
  #### and do not directly make any changes to the output render files or script

   #### diagnostic functions
    #### void showSelection(unsigned int counter) shows the current dimentions and setpoints of the selection
    #### void showMap(unsigned int counter) shows the current dimentions and setpoints of the map
    #### _Bool outofBounds() tells weather the selection if outside of the map
    #### void showFloat(float progress) shows the value of a float from 0-1
    #### _Bool inSel(short x,short z,short y) tells weather an XZY coodrinate is whithin the selection or not
    #### void printBussInfo(struct buss arg,char* state) prints basic information about a buss structure
    #### void showReferenceTable(unsigned char referenceTable[6][4],_Bool facePresent[6],_Bool schedualVirtex[8]) shows the reference table for a voxel
    #### struct buss createTestBuss(char* name,unsigned char width,char direction, char stSide) creates a test buss for testing purposes
    void showProgress(short ARG,short MAX_ARG, char* message) shows progress of a function that might take a while

   #### utility functions   
    #### short* getSelExtent(char Dir) gets the location of the edge of the map at a certain direction and returns the set marker itself
    #### short* getMapExtent(char Dir) gets the location of the edge of the selection at a certain direction and returns the set marker itself
    #### struct extents getAllSelExtents() returns all setpoint values for the selection assigned to their extents
    #### struct extents getAllMapExtents() returns all setpoint values for the map assigned to their extents
    #### void updateSelDementions(char Dir) updates the selection dementions given the updated setpoints
    #### void expandBlockMap(unsigned short change, char Dir) will expand the block map a specific distance in a specific direction
    #### void checkAndExpand(unsigned short distance,char Dir) will check the block map agents the selection and expand the map if the map is too small
    #### void selectionShift(char Dir, unsigned short amount) will shift the selection in a specific direction
    #### struct block*** copySel() will grab a subset of the block map defigned by the selection and return it
    #### void pasteSel(struct block*** arg,short amount, char Dir) will write in a predifigned blockmap subset (copy) and write it into the block map with a distance offset and a direction
    #### void reflowTable(unsigned char referenceTable[6][4],_Bool facePresent[6],_Bool schedualVirtex[8]) an attempt to remove unused virtexes fromt he virtex index table to reduce file size
    #### struct buss* allocateBlock() a short hand to allocate the block buss

   #### math/logic functions
    #### struct vector getPointVector(struct point a, struct point b) get the vector from point a to point b by using subtraction
    #### float getDistance(struct vector arg) gets the length of a vector (the distance between 2 points) using pythagrans therom
    #### struct redShape getRedStoneShape(short x, short z, short y) tells the shape of a redstone piece using the blocks around it
    #### struct subVoxelPack getsubVoxels(short x, short z, short y) gets the subvoxel pack using the block coordinates to look up block information


   #### free
    #### void freeBuss(struct buss arg) frees a buss (contains heap objects)
    #### void freeBlockMap() frees the block map (is a heap object)


 ### type - 1 dictionary lookup
  #### these are the functions that deal with looking up information from a table or dictionary

   #### block
    #### _Bool mask(unsigned char type) tells what materials i wans to mask out from the renders
    #### unsigned char blockLookup(char* type) tells what the block id is givent he block name as a string (using the block table)
    #### char* reverseBlockLookup(unsigned char type) returns the block name as a string given the block id (using the block table)
    #### _Bool redType(char type) tells weather a block type is a type that would interact with redstone or not
    #### _Bool solid(char type) tells weather the block type is solid or not
    #### _Bool smallBlock(char type) tells weather the block type is smaller than 1x1 cube (air included)

   #### part      
    #### size_t partCount(unsigned char type) returns the number of parts a block type uses in the render
    #### char* getPartnameForBlock(unsigned char type, unsigned char part) returns the name string viven the type and part id for the type (using the block table)
    #### unsigned char PartLookup(char* partName) returns the part id given the part name string (using part table)
    #### char* reversePartLookup(unsigned char part) returns the part name string given the part id (using part table)

   #### direction
    #### char oppDir(char Dir) returns the oposite direction char of the given direction char
    #### char turnDir(char Dir, char turnDir) returns the direction char turned in a specific direction relitive to itself
    #### unsigned char numerateDir(char Dir) enumerates a direction to a direction id

   #### misc
    #### unsigned int collorLookup(int value, unsigned char part) gets the collor of a specific part and data value
    #### unsigned int KaLookup(unsigned char part) returns the ambiant reflection collor of a materrial type for a part
    #### unsigned int KsLookup(unsigned char part) returns the specualr reflection collor of a materrial type for a part
    #### unsigned int TfLookup(unsigned char part) i dont know something to do with .mtl files
    #### char illumLookup(unsigned char part) returns the illumination moodel for  a materrial type for a part
    #### float dLookup(unsigned char part) retuens the diffusion value for a materrial type for a part



 ### type - 2 world edit
  #### these are the functions that deal with dirtectly emulatiing the effects of each relivant world edit command

   #### block placement      
     #### void replace(char* typeA, _Bool specificA,char valueA, char* typeB,_Bool specificB, char valueB) //replace
     #### void setBlock(struct block B, _Bool wait) //set
     #### void overlay(char* type, unsigned char value) //overlay
     #### void move(unsigned short amount,_Bool shift, char Dir) //move
     #### void stack(short amount,char Dir, char* options) //stack
     #### void line(char* type, unsigned char value) //line
     #### void delete() short hand for //set 0

   #### selection managment
     #### void shift(unsigned short amount,char Dir) //shift
     #### void expand(unsigned short amount,char Dir) //expand
     #### void contract(unsigned short amount,char Dir) //contract


 ### type - 2.5 specialised world edit
  #### these are the functions are a partial step up from world edit commands
  #### such that they use the world edit commands directly but also use
  #### direction context to place certain blocks correctly

   #### variations of setBlock()
      #### void setRepeater(char Dir,unsigned char m) places repeater depending on direction
      #### void setredTorch(char Dir) places Redstone torch depending on direction
      #### void setObservor(char Dir) places Observor depending on direction
      #### void setPiston(char Dir, _Bool sticky) places Piston depending on direction


 ### type - 3 micro-Structures
  #### these are the functions that are created by implimenting
  #### different Tier 2 and/or tier2.5 functions to create specific structures

      #### unsigned char wire(short inStrength,unsigned char outToll, unsigned char value, unsigned short amount, char Dir) creates a wire givent the redstone input strength, will guanantee the output strength will be at least as high as the output tollerance with a specific collor used for the support wool and length
      #### void stairs(char* type, unsigned char value, char Dir, char up, char distance) will create a stair step pattern to elevate a redstone signal upwards
      #### void tower(unsigned char amount,char up, unsigned char value) sends a signal directly up or down using observors
      #### void tower2(unsigned char value,unsigned char levels) sends a signal directly up using torch ladders
      #### void flipFlop(char Dir) creates a piston flip flop to debounce a signame after using observers
      #### void buildDecoderTop(short inBits, char Dir, char stSide, unsigned char rows, unsigned char box) creates the decoder top structure
      #### void fillBinPtrn(short inBits, unsigned char significance[], char Dir, char stSide) created the binary pattern for the decoder
      #### void buildOutLines(unsigned char outBits, short inBits,char inSide, char outSide, char stSide) creates the putput lines for the encoder
      #### void buildandSupport(short inBits, unsigned char outBits, short inSide, char stSide) creates the support structure for the and gates on the decoder
      #### void dataEntry(char* type, unsigned char value, unsigned char entry,char inSide,char stSide, unsigned char max) preforms a data entry of the encoder
      #### void lvlDown(char stSide) goes down 1 level of the encoder for use while encoding
      #### void buildSegment(char Dir) builds a segment for the 7 segment display in a specific oriantation
      #### void seperateNibble(char* type, unsigned char value, char inSide, char stSide) build a sepperating line to sepperate specific nibbles of a 7 segment display cluster
      #### void breakout(unsigned char value,char outSide) preforms a breakout fromt he keyshift structure
      #### void invertor(char value, char dir) inverts a signal in place
      #### void bitXOR1(char Dir) build a type of XOR gate
      #### void bitXOR2(char Dir) build a different type of XOR gate (the logic is the same but the stucture is different)


 ### type - 4 Sub-Structures
  #### these are the functions that are created by implimenting
  #### different Tier 3 functions to create specific structures that are not unique      

         #### void LWire(char Dir, char inSide, _Bool move) short hand for a specific kind of wire for the 7 segment display
         #### void stairWire(char Dir, char up, char inSide, _Bool move)  short hand for a specific kind of wire for the 7 segment display
         #### struct buss bussUp(_Bool type, struct buss arg, short distance, char up, _Bool flip) runs the buss directly up using one of the the tower functions
         #### struct buss bussStraight(struct buss arg, short distance) runs the buss straight using the wire function  
         #### struct buss bussStairs(struct buss arg, short distance, char up) elevates the buss using hte stairs function
         #### struct buss XOR(_Bool XORType,struct buss inBitsA, struct buss inBitsB) XORS 2 busses together using one one of the BITXOR functions
         #### struct buss turnBuss(struct buss arg, char direction,_Bool flip, unsigned char depth) turns the buss stuctuure in a specific direction relitive to itself
         #### struct buss bussFlipFlop(struct buss arg) created flip flops structures for every bit in the buss
         #### struct buss* bussTap(struct buss arg,char direction) taps a buss of from a buss creating 2 busses


 ### type - 5 Structures
  #### these are the functions that are created by implimenting
  #### different Tier 4 functions to create specific structures that are unique

          #### void shiftBit(unsigned char value, char shiftDir, char outSide, unsigned char shift_amt) shifts a bit tot he left
          #### void cycleBit(unsigned char value,char shiftDir, char inSide, char outSide, unsigned char shift_amt, unsigned char shiftLength, unsigned char bitIndex) shift a bitt tot he opposte side
          #### void build7segLights(char* type, unsigned char value, char stSide, char inSide) build the lights for a 7 segment display
          #### void build7segWires(char inSide,char stSide) created the 7 segment wire support stucture
          #### void buildDecoder(short inBits,char Dir,char stSide,unsigned char rows,unsigned char significance[],unsigned char box) createsd the decoder used for decoding certain encoding schemes to a selection signal
          #### void buildEncoder(unsigned char rows, unsigned char collumns,unsigned char Table[rows][collumns],short inBits, unsigned char outBits, char stSide,char inSide, char outSide) takes a selection signal and encodes a buss with a certain encodiing scheme
          #### struct buss keyShiftSide(struct buss key,_Bool half,unsigned char shiftIndex) shifts the key to the left using bitshift and cycle shift
          #### struct buss buildPermute(char* name,struct buss inBuss, unsigned char permTable[], unsigned char permLength, unsigned char groupSize, unsigned char groupSpacing, char forward, char stSide, char inSide,_Bool startAtIn, _Bool stayAtOut) build the permute function using wire and a permute table
          #### struct buss halfSwapSides(struct buss inBuss,short amount,char Dir) preforms hlf of swap sides used to takes a number of bits and shift them all the way over to one side


 ### type - 6 process
  #### these are the functions that are created by implimenting
  #### different Tier 5 structures execute specific processes

          #### struct buss* buildRoundKeyBuss(struct buss roundKey) is used to create the reverse order of the round keys on a seperate set of buss lines
          #### struct buss buildStables(struct buss roundKey, char Dir,char stSide) builds the substitution boxes using the substitution box tables
          #### struct buss setKeySchedual() sets the key schedual
          #### void buildOutput(unsigned char nibbles, char stSide, char backward) builds an output for a specific number of nibbles
          #### struct buss Keyshift(struct buss key,unsigned char shiftIndex) preforms a keyshift
          #### struct buss* buildRound(struct buss roundKey, struct buss* block) builds a round of DES


 ### type - 7 output
  #### these are the functions that output data to output files
   #### immages
          #### void buildImmages() created 3 orthographic 2d projections of the map from 3 different sides
   #### material library
          #### void printMaterial(FILE * Mtl,unsigned char value,unsigned char part) prints the material attrabutes to .mtl file
          #### void buildMaterialLibrary() prints all material attrabutes to .mtl file using printMaterial()
   #### wavefront object          
          #### void printFaceQuad(FILE * Obj,unsigned char v_index[4], int vc) prints the face information of the voxel to the .obj file
          #### void printVirtex(FILE * Obj,struct virtex v) prints the virtex informatiion to the .obj file
          #### void printsubVoxel(FILE * Obj, struct subVoxel vox, int vc, short x, short z, short y) prints the virtexes and faces used by a voxel to the .obj file using printFaceQuad() and printVirtex()
          #### void buildWaveFront() prints the entiire wavefront object using getsubVoxels() and printsubVoxel() to get and print each voxel for each block int he map
