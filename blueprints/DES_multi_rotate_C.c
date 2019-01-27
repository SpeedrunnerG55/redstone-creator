
#include <stdio.h> //for console io
#include <time.h> //for console io
#include "DES_tables.h"
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
unsigned long long permute(unsigned char keyLength, unsigned long long key, unsigned char tableLength, unsigned char table[tableLength]) {
  unsigned long long ans = 0LL;
  for (unsigned char i=0; i< tableLength; i++){
    //this monstrosity is one line
    ans |=
    (key >> ((keyLength - table[i]) % keyLength) & 1LL)  //move original one complete length then back to grap the perm[i]'th position' to grab that bit from it
    << (tableLength - i - 1); //then move it back to the ith position of the result
  }
  return ans;
}

struct roundKeys{
  unsigned long long encrypt[16];
  unsigned long long decrypt[16];
};

void compareKeys(struct roundKeys keysSet, int index){

  int keyLength = 64;

  _Bool copies[2][keyLength];
  for(int j = keyLength - 1; j >= 0; j--){
    copies[0][j] = 1 & keysSet.encrypt[index] >> j;
    copies[1][j] = 1 & keysSet.decrypt[index] >> j;
  }

  printf("\n%11s","");
  for(int j = keyLength - 1; j >= 0; j--){
    if(copies[0][j] != copies[1][j]){
      printf("%i",keyLength - j);
    }
    else{
      printf(" ");
    }
  }

  printf("\n%11s","");
  for(int j = keyLength - 1; j >= 0; j--){
    if(copies[0][j] != copies[1][j]){
      printf("V");
    }
    else{
      printf(" ");
    }
  }

  printf("\n %-10s","original");
  for(int j = keyLength - 1; j >= 0; j--){
    printf("%i",copies[1][j]);
  }
  printf("\n %-10s","test");
  for(int j = keyLength - 1; j >= 0; j--){
    printf("%i",copies[0][j]);
  }
  printf("\n");
}

// Set up the keys for each round.key
struct roundKeys setKeys(unsigned long long key) {

  struct roundKeys keys;

  // Set the original key with PC1.
  //unsigned long long permute(unsigned long long original, int perm[], int permlength)
  unsigned char PC1Length = sizeof(PC1) / sizeof(PC1[0]);
  key = permute(64,key,PC1Length,PC1);

  // Go through and set the round keys using the process by which they
  // are supposed to be computed.


  unsigned long long rotate = key;

  unsigned char PC2Length = sizeof(PC2) / sizeof(PC2[0]);
  unsigned char rotated[PC2Length];

  for(unsigned char j = 0; j < PC2Length; j++){
    rotated[j] = PC2[j];
  }

  for(int s = 0; s < 2;s++){
    for (int i=0; i<16; i++){

      int rotate_Amt;

      if(s){
        keys.encrypt[i] = permute(56,rotate,PC2Length,rotated);
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
        keys.decrypt[i] = permute(56,rotate,PC2Length,rotated);
      }
    }
  }
  return keys;
}

// Returns the output of putting the 48 bit input through the
// 8 S-boxes.
unsigned long long Sboxes(unsigned long long input) {

  unsigned long long ans = 0;

  for (unsigned char i=0; i<8; i++) {

    // printf("stables[%i][%llu][%llu]\n",i,(2 & input >> (46-(6*i)))|(1 & input >> (42-(6*i))),(0xF & input >> (43-(6*i))));

    ans |=
    (unsigned long long) //cast to unsigned long long because the bits will get shifted a lot and the container needs to be big enough
    stables
    [i] //what box
    [(2 & input >> (46-(6*i)))|(1 & input >> (42-(6*i)))] //what row
    [(0xFLLU & input >> (43-(6*i)))] //what collumn
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

  unsigned char ELength = sizeof(E) / sizeof(E[0]);
  unsigned char PLength = sizeof(P) / sizeof(P[0]);

  *block = *block << 32 |
  (*block >> 32 ^ permute(32,Sboxes(permute(32,0xFFFFFFFF & *block,ELength,E) ^ roundKey),PLength,P));
}

// does not work
// Encrypts the current block.
void encrypt(unsigned long long roundKeys[16], unsigned long long *block, _Bool encrypting) {

  // permute the block with the initial permutation.
  unsigned char IPLength = sizeof(IP) / sizeof(IP[0]);
  *block = permute(64,*block,IPLength,IP);

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


  unsigned char IPInvLength = sizeof(IPInv) / sizeof(IPInv[0]);
  *block = permute(64,*block,IPInvLength,IPInv);

}

void test(){

  unsigned char rotations = 0;

  for (int i=0; i<16; i++) {
    rotations += keyshifts[i];
    printf("%i << %i\n",rotations,keyshifts[i]);
  }

  for (int i=15; i>=0; i--) {
    printf("%i >> %i\n",rotations,keyshifts[i]);
    rotations -= keyshifts[i];
  }
}

int main(){

  // test();

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

  unsigned long long block;

  printf("CIPHER TEXT\n");
  printKey(ciphertext);
  printf("\n");

  // Read in each block and process...
  block = ciphertext;
  encrypt(keys.decrypt,&block,0);
  printf("RESULT OF DECRYPT\n");
  printKey(block);
  printf("\n");

  // Read in each block and process...
  encrypt(keys.encrypt,&block,0);
  printf("RESULT OF ENCRYPT\n");
  printKey(block);
  printf("\n");

  return 0;
}
