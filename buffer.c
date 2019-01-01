#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ** string = NULL;

void addToString(char ** string,int index,char* arg){
  unsigned int new_string_len = strlen(string[index]) + strlen(arg) + 1;
  if(strlen(string[index]) == 0){
    string[0] = malloc(strlen(arg) + 1);
    strncat(string[0],arg,strlen(arg) + 1);
    printf("%lu %s \n",strlen(string[index]),string[index]);
  }
  else{
    string[0] = realloc(string[index],new_string_len);
    strncat(string[index],arg,new_string_len);
  }
}

int main(){
  string = malloc(2 * sizeof(char*));
  string[0] = "";
  for(int i = 0; i < 3; i++){
    addToString(string,0,"boop");
    printf("%s",string[0]);
  }
  printf("%lu %s \n",strlen(string[0]),string[0]);
  return 0;
}
