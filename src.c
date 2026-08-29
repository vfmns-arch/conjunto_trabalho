#include <stdio.h>
#include <stdlib.h>

void mandel(int largure, intv altura, int max, int *arr){
  for(int y = 0; y < largura; y++){
    for(int x = 0; x < largura; x++){
      double cx = -2 + (3 * x / largura);
      double cy = -1.5 + (3 * y / altura);
      double zx = 0;
      double zy = 0;
      int iter = 0;
      while(iter < max){
        double new = zx * zx + cx;
        zx = new;
        new = zy * zy + cy;
        zy = new;
        iter++;
      }
      arr[y][x] = iter;
    }
  }
  return;
}

int main(int argc, int *argv[]){
  
  return 0;
}
