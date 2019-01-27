int length = 15;
int strength = 5;
for(int i = 1; i < length; i++){
  for(int j = 0; j < length; j++){
    for(int k = length - 1; k >- 0; k--){
      strength = wire(strength,j,"wool",3,4*k,'f');
      strength = wire(strength,j,"wool",3,2,'r');
      strength = wire(strength,j,"wool",3,4*k,'b');
      strength = wire(strength,j,"wool",3,2,'r');

    }
    for(int k = 0; k < length; k++){
      strength = wire(strength,j,"wool",3,2,'f');
      strength = wire(strength,j,"wool",3,k*4,'l');
      strength = wire(strength,j,"wool",3,2,'f');
      strength = wire(strength,j,"wool",3,k*4,'r');
    }
    strength = wire(strength,j,"wool",3,2,'r');
    strength = wire(strength,j,"wool",3,4*length,'b');
    strength = wire(strength,j,"wool",3,2,'r');
  }
  printf("%i \n",i);
}

unsigned char inStrength = 3;
for(int i = 0; i < 400; i++){
  wire(inStrength,3,"wool",0,i,'r');
  shift(i,'l');
  shift(2,'f');
}

wire(1,1,"wool",4,33,'l');
shift(33,'r');
shift(2,'f');

for(short i = 2; i < 5; i++){
  tower(i,'d',"wool",3);
  flipFlop('f');
  shift(1,'f');
  wire(14,5,"wool",8,i,'f');
  shift(i,'u');
  shift(2,'l');
  shift(i + 4,'b');
}
