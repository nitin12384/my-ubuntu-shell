#include <stdio.h>

void delay(int n){
  int i,j,x=0;
  for(i=0; i<n; i++){
    for(j=0; j<n; j++){
      x ++;
    }
  }
}


int main(){
  int n=10000;
  while(1){
    delay(n);
    printf("Iter - ");
  }
}
