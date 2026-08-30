#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include <time.h>

int altura;
int largura;
int max;
int num_threads;
FILE *fptr;
typedef struct {
  int x;
  int y;
  float intensidade;
} args;
args *ptr;
void *mandel(void *argumentos){
  args *arg = argumentos;
  float cx = 3 * arg->x / (float)largura - 2;
  float cy = 3 * arg->y / (float)altura - 1.5;
  float zx = 0;
  float zy = 0;
  int iter = 0;
  while(iter < max){
    double new = zx * zx + cx;
    zx = new;
    new = zy * zy + cy;
    zy = new;
    iter++;
  }
  arg->intensidade = (float)iter * 255 / (float)max;
  return NULL;
}
void *mandelp(void *argumentos){
  int pixels = altura * largura;
  int *x = argumentos;
  int section = (pixels - pixels % num_threads) / num_threads;
  for(int i = *x * section; i < *x * section + section; i++){
    mandel(&ptr[i]);
  }
  if(*x == num_threads - 1){
    for(int i = *x * section + section; i < pixels; i++){
      mandel(&ptr[i]);
    } 
  }
  return NULL;
}
int main(int argc, char **argv){
  FILE *time = fopen("time.txt", "w");
  if(time == NULL){
    printf("erro");
    return 1;
  }
  clock_t start, end;
  double e;
  if(argc != 5){
    printf("argumentos invalidos");
    return 1;
  }
  largura = atoi(argv[1]);
  altura = atoi(argv[2]);
  max = atoi(argv[3]);
  num_threads = atoi(argv[4]);
  if(largura < 1 || altura < 1 || max < 1 || num_threads < 0){
    printf("argumentos invalidos");
    return 1;
  }
  args argumentos[altura][largura];
  ptr = &argumentos[0][0];
  int pixels = altura * largura;
  if(num_threads > pixels){
    num_threads = pixels;
  }
  fptr = fopen("mandelbrot_vfmns_serial.txt", "w");
  if(fptr == NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      argumentos[i][j].x = j;
      argumentos[i][j].y = i;
      mandel(&argumentos[i][j]);
      fprintf(fptr, "intensidade do pixel[x: %d][y: %d]: %f\n", argumentos[i][j].x, argumentos[i][j].y, argumentos[i][j].intensidade);
    }
  }
  end = clock();
  e = (double)(end - start) / CLOCKS_PER_SEC;
  fprintf(time, "tempo de exec serial: %f segundos\n", e);
  if(fclose(fptr) != 0){
    printf("erro");
  }
  fptr = fopen("mandelbrot_vfmns_openmp.txt", "w");
  if(fptr == NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  int p[num_threads];
  #pragma omp parallel for
  for(int i = 0; i < num_threads; i++){
    p[i] = i;
    mandelp(&p[i]);
  }
  end = clock();
  e = (double)(end - start) / CLOCKS_PER_SEC;
  fprintf(time, "tempo de exec openmp: %f segundos\n", e);
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      fprintf(fptr, "intensidade do pixel[x: %d][y: %d]: %f\n", argumentos[i][j].x, argumentos[i][j].y, argumentos[i][j].intensidade);
    }
  }
  if(fclose(fptr) != 0){
    printf("erro");
  }
  fptr = fopen("mandelbrot_vfmns_pthread1.txt", "w");
  if(fptr == NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  pthread_t threads[num_threads];
  for(int i = 0; i < num_threads; i++){
    if(pthread_create(&threads[i], NULL, mandelp, &p[i]) != 0){
      printf("erro");
      return 1;
    }
  }
  for(int i = 0; i < num_threads; i++){
    if(pthread_join(threads[i], NULL) != 0){
      printf("erro");
      return 1;
    }
  }
  end = clock();
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      fprintf(fptr, "intensidade do pixel[x: %d][y: %d]: %f\n", argumentos[i][j].x, argumentos[i][j].y, argumentos[i][j].intensidade);
    }
  }
  e = (double)(end - start) / CLOCKS_PER_SEC;
  fprintf(time, "tempo de exec pthread1: %f segundos\n", e);
  if(fclose(fptr) != 0){
    printf("erro");
  }
  if(fclose(time) != 0){
    printf("erro");
    return 1;
  }
  return 0;
}
