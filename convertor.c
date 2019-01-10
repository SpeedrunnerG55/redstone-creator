
#include <stdio.h> //for console io
#include <time.h> //for console io
#include "tables.h"
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
    printf("%llu",(1 & print >> i));
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
unsigned long long permute(unsigned long long original, unsigned char *perm, int permLength, int originaLength) {

  unsigned long long ans = 0LL;
  for (int i=0; i< permLength; i++){
    //this monstrosity is one line
    ans |=
    (original >> (originaLength - perm[i]) & 1LL)  //move original one complete length then back to grap the perm[i]'th position' to grab that bit from it
    << (permLength - i - 1); //then move it back to the ith position of the result
  }
  return ans;
}

// Set up the keys for each round.key
void setKeys(unsigned long long roundkeys[16], unsigned long long key) {

  // Set the original key with PC1.
  //unsigned long long permute(unsigned long long original, int perm[], int permlength)
  key = permute(key,PC1,56,64);

  // Go through and set the round keys using the process by which they
  // are supposed to be computed.

  int keyshifts[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
  };

  for (int i=0; i<16; i++) {

    // Supposed to left-shift both halves by the appropriate amount,
    // based on the round.

    for(int j = 0; j < keyshifts[i]; j++){
      key = ((key << 1) & 0xFFFFFFEFFFFFFF) //shifts the bits left 1 and cuts off the bits on the left
      | (0x80000008000000 & key) >> 27; //saves the bits that got cut off and then transposes the saved bit back to the correct position
    }

    // Now, just copy in the (i+1)th round key.
    roundkeys[i] = permute(key,PC2,48,56);
  }
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
  // expand the right block
  // This is the XOR we want.
  // Run the s-boxes on all the appropriate "blocks".
  // permute the S-box output.
  // Then do the necessary XOR.
  // Copy the blocks back into their proper place! (swapped)

  //read ^ and not V

  *block = *block << 32 |
  (*block >> 32 ^ permute(Sboxes(permute(0xFFFFFFFF & *block,E,48,32) ^ roundKey),P,32,32));
}

// does not work
// Encrypts the current block.
void encrypt(unsigned long long roundKeys[16], unsigned long long *block, _Bool encrypting) {

  // permute the block with the initial permutation.
  *block = permute(*block,IP,64,64);

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

  *block = permute(*block,IPInv,64,64);

}

_Bool testBlock(unsigned long long block){
  return block % 0x10 == 0;
}

_Bool testBlock2(unsigned long long block){
  return (0xFFFF & block) == 0;
}


int main(){

  unsigned long long ciphertext = 0x0892E7E6F4AF00CA;
  unsigned long long roundKeys[16]; //declare round key variable


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
  setKeys(roundKeys,key); //make round keys

  // for(int i = 0; i < 16; i++){
  //   printKeyBin(roundKeys[i],48);
  // }

  // Read in each block and process... (for each block of cipher text (i hope))
  unsigned long long block = ciphertext;
  encrypt(roundKeys,&block,0);
  printf("RESULT OF ENCRYPT\n");
  printKey(block);

  return 0;
}
