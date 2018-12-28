
#include <stdio.h> //for console io
#include <time.h> //for console io
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

// bool getBit(unsigned long long original,int position){
//   return (bool)(1 & original >> (63 - position));
// }
//
// //tested
// void setBit(unsigned long long &original, bool bit, int position){
//   original =
//   (original & ~(1LL << (63 - position))) //mask out the bit to be changed
//   | bit << (63 - position); //or the new value into the place
// }

//what have i done...
unsigned long long permute(unsigned long long original, int *perm, int permLength, int originaLength) {

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

  int PC1[56] = {
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
  };

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

    int PC2[48] = {
      14,17,11,24,1, 5,
      3, 28,15,6, 21,10,
      23,19,12,4, 26,8,
      16,7, 27,20,13,2,
      41,52,31,37,47,55,
      30,40,51,45,33,48,
      44,49,39,56,34,53,
      46,42,50,36,29,32
    };

    // Now, just copy in the (i+1)th round key.
    roundkeys[i] = permute(key,PC2,48,56);
  }
}

// Returns the output of putting the 48 bit input through the
// 8 S-boxes.
unsigned long long Sboxes(unsigned long long input) {

  int stables[8][4][16] =
  {
    {
      {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
      {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
      {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
      {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
      {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
      {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
      {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
      {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
      {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
      {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
      {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
      {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
      {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
      {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
      {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
      {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
      {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
      {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
      {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
      {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
      {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
      {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
      {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
      {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
      {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
      {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
      {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
      {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
      {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
      {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
      {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
      {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
  };

  unsigned long long ans = 0;

  for (int i=0; i<8; i++) {
    ans |=
    stables
    [i] //what box
    [(0x2 & input >> (46-(6*i)))|(1 & input >> (42-(6*i)))] //what row
    [(0xF & input >> (43-(6*i)))] //what collumn
    << ((7 - i) * 4); //shifted to right place
  }

  return ans;
}

// Runs round num of DES.
void round2(unsigned long long roundKey, unsigned long long &block) {

  int E[48] = {
    32,1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
  };

  int P[32] = {
    16,7, 20,21,29,12,28,17,
    1, 15,23,26,5, 18,31,10,
    2, 8, 24,14,32,27,3, 9,
    19,13,30,6, 22,11,4, 25
  };

  // Copy in the left and right blocks
  // expand the right block
  // This is the XOR we want.
  // Run the s-boxes on all the appropriate "blocks".
  // permute the S-box output.
  // Then do the necessary XOR.
  // Copy the blocks back into their proper place! (swapped)

  //read ^ and not V

  block = block << 32 | (block >> 32 ^ permute(Sboxes(permute((0xFFFFFFFF & block),E,48,32) ^ roundKey),P,32,32));
}

// does not work
// Encrypts the current block.
void encrypt(unsigned long long roundKeys[16], unsigned long long &block, bool encrypting) {

  int IP[64] = {
    58,50,42,34,26,18,10,2,
    60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,
    64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9, 1,
    59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,
    63,55,47,39,31,23,15,7
  };

  // permute the block with the initial permutation.
  block = permute(block,IP,64,64);

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
  block = (block >> 32) | (block << 32);

  int IPInv[64] = {
    40,8,48,16,56,24,64,32,
    39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,
    37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,
    35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,
    33,1,41,9, 49,17,57,25
  };

  block = permute(block,IPInv,64,64);

}

bool testBlock(unsigned long long block){
  return block % 0x10 == 0;
}

bool testBlock2(unsigned long long block){
  return (0xFFFF & block) == 0;
}


int main(){

  unsigned long long ciphertext = 0x0892E7E6F4AF00CA;
  unsigned long long roundKeys[16]; //declare round key variable


  int start = time(NULL);
  int now = start;
  unsigned long long temp = 0x6BD5000000000000;
  setbuf(stdout, NULL); //disable stdout buffering

  // #pragma omp parallel for

  for(unsigned long long i = 0x6BD5000000000000; i <= 0x6BD5082664DF0426; i++){
    for(int j = 0; j < 8; j++){
      if(((0x01 << j * 8) & i) != 0){
        i = i + (0x01 << j * 8);
      }
    }

    if((time(NULL) - now) == 1){
      temp = i - temp;
      now = time(NULL);
      printf("\r Keys failed :");
      printKey(i);
      printf(" @ %llu KPS   ",temp);
      temp = i;
      // getchar();
    }

    setKeys(roundKeys,i); //make round keys
    unsigned long long test = ciphertext;
    encrypt(roundKeys,test,0);

    if(test == 0x59E95CA267BFB680){
      printf("FOUND KEY! ");
      printKey(i);
      break;
    }
  }

  printf("ellapsed time %ti seconds",(time(NULL) - now));
  getchar();


  unsigned long long key = 0x6BD5082664DF0426;
  setKeys(roundKeys,key); //make round keys

  // for(int i = 0; i < 16; i++){
  //   printKeyBin(roundKeys[i],48);
  // }

  // Read in each block and process... (for each block of cipher text (i hope))
  unsigned long long block = ciphertext;
  encrypt(roundKeys,block,0);
  printf("RESULT OF ENCRYPT\n");
  printKey(block);

  return 0;
}
