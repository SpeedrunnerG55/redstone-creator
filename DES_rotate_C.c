
#include <stdio.h> //for console io
#include <time.h> //for console io
#include "tables.h"
#include <stdlib.h>
#include <math.h>
// #include <fstream>

// //for sleep
// #include <chrono>
// #include <thread>


// void sleep(int amount){
//   this_thread::sleep_for(chrono::milliseconds(amount));
// }

//tested

//prints key in hex block format thingy
void printKey(unsigned long long key){
  for(int i = 7; i >= 0; i--){
    printf("%02hhX ",(char)(key >> i * 8));
  }
}

//prints key in bin
void printKeyBin(unsigned long long print){
  for(int i = 63; i >= 0; i--){
    printf("%llu",1 & print >> i);
  }
  printf("\n");
}

//not needed but works
unsigned long long charstodecimal(char key[8]){
  unsigned long long retval = 0;
  for(int i = 0; i < 8; i++){
    retval |= (0xFF & key[i]); //for some reson the left most bits turn into 1's from the char so i mask them out
    if(i != 7)
    retval = retval << 8; //shift over for the next value untill it is the last value then dont shift
  }
  return retval;
}

//these 2 functions are amazing

// _Bool getBit(unsigned long long original,int position){
//   return (_Bool)(1 & original >> (63 - position));
// }
//
// //tested
// void setBit(unsigned long long &original, _Bool bit, int position){
//   original =
//   (original & ~(1LL << (63 - position))) //mask out the bit to be changed
//   | bit << (63 - position); //or the new value into the place
// }

//what have i done...
unsigned long long permute(unsigned long long original, unsigned char rows, unsigned char cols, unsigned char perm[rows][cols], unsigned char originaLength) {
  unsigned char permLength = rows * cols;
  unsigned long long ans = 0LL;
  for (unsigned char i=0; i< rows; i++){
    for (unsigned char j=0; j< cols; j++){
      //this monstrosity is one line
      ans |=
      (original >> ((originaLength - perm[i][j]) % originaLength) & 1LL)  //move original one complete length then back to grap the perm[i]'th position' to grab that bit from it
      << (permLength - (i * cols + j) - 1); //then move it back to the ith position of the result
    }
  }
  return ans;
}

void showTables(unsigned char rows, unsigned char cols,unsigned char num_tables, unsigned char table[num_tables][rows][cols],char* name){

  unsigned char boxWidth = sqrt(num_tables);

  getchar();

  for(int k = 0; k < boxWidth; k++){
    for(int l = 0; l < boxWidth; l++){
      printf("%-14s%-3i%6s",name,k * boxWidth + l,"");
    }
    printf("\n");
    for(unsigned char i = 0; i < rows; i++){
      for(int l = 0; l < boxWidth; l++){
        for(unsigned char j = 0; j < cols; j++){
          printf("%2i",table[k * boxWidth + l][i][j]);
          // printf("0");
          if(j < cols - 1)
          printf(",");
        }
        if(i < rows - 1)
        printf(",");
        else
        printf(" ");
        printf("%5s","");
      }
      printf("\n");
    }
    printf("\n");
  }
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

struct roundKeys{
  unsigned long long rotate[16];
  unsigned long long shift[16];
};

void compareKeys(struct roundKeys keysSet, int index){

  _Bool copies[2][49];
  for(int j = 47; j >= 0; j--){
    copies[0][j] = 1 & keysSet.rotate[index] >> j;
    copies[1][j] = 1 & keysSet.shift[index] >> j;
  }

  printf("\n%11s","");
  for(int j = 47; j >= 0; j--){
    if(copies[0][j] != copies[1][j]){
      printf("%i",48 - j);
    }
    else{
      printf(" ");
    }
  }

  printf("\n%11s","");
  for(int j = 47; j >= 0; j--){
    if(copies[0][j] != copies[1][j]){
      printf("V");
    }
    else{
      printf(" ");
    }
  }

  printf("\n %-10s","Correct");
  for(int j = 47; j >= 0; j--){
    printf("%i",copies[1][j]);
  }
  printf("\n %-10s","Incorrect");
  for(int j = 47; j >= 0; j--){
    printf("%i",copies[0][j]);
  }
  printf("\n");
}

// Set up the keys for each round.key
struct roundKeys setKeys(unsigned long long key) {

  struct roundKeys keys;

  // Set the original key with PC1.
  //unsigned long long permute(unsigned long long original, int perm[], int permlength)
  unsigned char PC1rows = sizeof(PC1) / sizeof(PC1[0]);
  unsigned char PC1cols = sizeof(PC1[0]) / sizeof(PC1[0][0]);
  key = permute(key,PC1rows,PC1cols,PC1,64);

  // Go through and set the round keys using the process by which they
  // are supposed to be computed.

  unsigned char keyshifts[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
  };

  unsigned char rotations = 0;

  unsigned long long shift = key;
  unsigned long long rotate = key;

  unsigned char PC2rows = sizeof(PC2) / sizeof(PC2[0]);
  unsigned char PC2cols = sizeof(PC2[0]) / sizeof(PC2[0][0]);

  unsigned char tableBox[16][PC2rows][PC2cols];

  for (int i=0; i<16; i++) {

    rotations += keyshifts[i];

    for(int j = 0; j < keyshifts[i]; j++){
      shift = ((shift << 1) & 0xFFFFFFEFFFFFFF) //shifts the bits left 1 and cuts off the bits on the left
      | (0x80000008000000 & shift) >> 27; //saves the bits that got cut off and then transposes the saved bit back to the correct position
    }

    unsigned char ** heapRotated = rotateTable(PC2rows,PC2cols,PC2,rotations,56);
    unsigned char rotated[PC2rows][PC2cols];
    for(unsigned char j = 0; j < PC2rows; j++){
      for(unsigned char k = 0; k < PC2cols; k++){
        rotated[j][k] = heapRotated[j][k];
        tableBox[i][j][k] = rotated[j][k];
      }
      free(heapRotated[j]);
    }
    free(heapRotated);

    // showTable(PC2rows,PC2cols,PC2,"original",0);

    keys.shift[i] = permute(shift,PC2rows,PC2cols,PC2,56);
    keys.rotate[i] = permute(rotate,PC2rows,PC2cols,rotated,56);
    // compareKeys(keys,i);

  }

  showTables(PC2rows,PC2cols,16,tableBox,"table for key");

  return keys;
}

// Returns the output of putting the 48 bit input through the
// 8 S-boxes.
unsigned long long Sboxes(unsigned long long input) {

  unsigned long long ans = 0;

  for (int i=0; i<8; i++) {

    // printf("stables[%i][%llu][%llu]\n",i,(2 & input >> (46-(6*i)))|(1 & input >> (42-(6*i))),(0xF & input >> (43-(6*i))));

    ans |=
    stables
    [i] //what box
    [(2 & input >> (46-(6*i)))|(1 & input >> (42-(6*i)))] //what row
    [(0xF & input >> (43-(6*i)))] //what collumn
    << ((7 - i) << 2); //shifted to right place
  }
  return ans;
}

// Runs round num of DES.
void round2(unsigned long long roundKey, unsigned long long *block) {

  // Copy in the left and right blocks
  // expand the right blockrotated
  // This is the XOR we want.
  // Run the s-boxes on all the appropriate "blocks".
  // permute the S-box output.
  // Then do the necessary XOR.
  // Copy the blocks back into their proper place! (swapped)

  //read ^ and not V

  unsigned char Erows = sizeof(E) / sizeof(E[0]);
  unsigned char Ecols = sizeof(E[0]) / sizeof(E[0][0]);

  unsigned char Prows = sizeof(P) / sizeof(P[0]);
  unsigned char Pcols = sizeof(P[0]) / sizeof(P[0][0]);

  *block = *block << 32 |
  (*block >> 32 ^ permute(Sboxes(permute(0xFFFFFFFF & *block,Erows,Ecols,E,32) ^ roundKey),Prows,Pcols,P,32));
}

// does not work
// Encrypts the current block.
void encrypt(unsigned long long roundKeys[16], unsigned long long *block, _Bool encrypting) {

  // permute the block with the initial permutation.
  unsigned char IProws = sizeof(IP) / sizeof(IP[0]);
  unsigned char IPcols = sizeof(IP[0]) / sizeof(IP[0][0]);
  *block = permute(*block,IProws,IPcols,IP,64);

  //difference between encrypting and decrypting
  if(encrypting){
    // Run 16 rounds.
    for (int i=0; i<16; i++) {
      round2(roundKeys[i],block);
    }
  }
  else{
    // Run 16 rounds. in reverse order
    for (int i=15; i>=0; i--) {
      round2(roundKeys[i],block);
    }
  }

  // Supposed to switch halves at the end and invert the initial
  // permutation.
  *block = (*block >> 32) | (*block << 32);


  unsigned char IPInvrows = sizeof(IPInv) / sizeof(IPInv[0]);
  unsigned char IPInvcols = sizeof(IPInv[0]) / sizeof(IPInv[0][0]);
  *block = permute(*block,IPInvrows,IPInvcols,IPInv,64);

}

_Bool testBlock(unsigned long long block){
  return block % 0x10 == 0;
}

_Bool testBlock2(unsigned long long block){
  return (0xFFFF & block) == 0;
}


int main(){

  unsigned long long ciphertext = 0x0892E7E6F4AF00CA;



  // int start = time(NULL);
  // int now = start;
  // unsigned long long temp = 0x6BD5000000000000;
  // setbuf(stdout, NULL); //disable stdout buffering
  //
  //
  // for(unsigned long long i = 0x6BD5000000000000; i <= 0x6BD5082664DF0426; i++){
  //   for(int j = 0; j < 8; j++){
  //     if(((0x01 << j * 8) & i) != 0){
  //       i = i + (0x01 << j * 8);
  //     }
  //   }
  //
  //   if((time(NULL) - now) == 1){
  //     temp = i - temp;
  //     now = time(NULL);
  //     printf("\r Keys failed :");
  //     printKey(i);
  //     printf(" @ %llu KPS   ",temp);
  //     temp = i;
  //     // getchar();
  //   }
  //
  //   setKeys(roundKeys,i); //make round keys
  //   unsigned long long test = ciphertext;
  //   encrypt(roundKeys,&test,0);
  //
  //   if(test == 0x59E95CA267BFB680){
  //     printf("FOUND KEY! ");
  //     printKey(i);
  //     // break;
  //   }
  // }
  //
  //
  // printf("ellapsed time %ti seconds",time(NULL) - start);
  // getchar();


  unsigned long long key = 0x6BD5082664DF0426;
  struct roundKeys keys = setKeys(key); //make round keys

  // for(int i = 0; i < 16; i++){
  //   printKeyBin(keys.shift[i]);
  // }
  //
  // for(int i = 0; i < 16; i++){
  //   printKeyBin(keys.rotate[i]);
  // }

  // Read in each block and process... (for each block of cipher text (i hope))
  unsigned long long block = ciphertext;
  encrypt(keys.rotate,&block,0);
  printf("RESULT OF ENCRYPT\n");
  printKey(block);

  return 0;
}
