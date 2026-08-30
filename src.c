#include <stdio.h>
#include <stdlib.h>
#include <pthreads.h>
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
  int intensidade;
} args;
args *ptr;
void *mandel(void *argumentos){
  args *arg = argumentos;
  double cx = 3 * arg->x / largura - 2;
  double cy = 3 * arg->y / altura - 1.5;
  double zx = 0;
  double zy = 0;
  int iter = 0;
  while(iter < max){
    double new = zx * zx + cy;
    zx = new;
    new = zy * zy + cy;
    zy = new;
    iter++;
  }
  arg->intensidade = iter + 255 / max;
  fprintf(ptr, "intensidade do pixel[x: %d][y: %d]: %d\n", arg->x, arg->y, arg->intensidade);
  return NULL;
}
void *mandelp(void *argumentos){
  int pixels = altura * largura;
  int *x = argumentos;
  int section = (pixels - pixels % num_threads) / num_threads;
  for(int i = *x * section; i < *x * section + section; i++){
    mandel(ptr[i]);
  }
  return NULL;
}
int main(int argc, int **argv){
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
  args argumentos[altura][largura];
  ptr = &argumentos[0][0];
  fptr = fopen("mandelbrot_vfmns_serial.txt", "w");
  if(fptr == NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  for(int i = 0; i < altura; i++){
    for(int j = 0; 0 < largura; j++){
      argumentos[i][j].x = j;
      argumentos[i][j].y = i;
      mandel(&argumentos[i][j]);
    }
  }
  end = clock();
  e = (double)(end - start) / CLOCK_PER_SEC;
  fprintf(time, "tempo de exec serial: %f segundos\n", e);
  if(fclose(ptr) != 0){
    printf("erro");
  }
  fptr = fopen("mandelbrot_vfmns_openmp.txt", "w");
  if(fptr == NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  #pragma omp parallel for
  for(int i = 0; i < num_threads; i++){
    mandelp(i);
  }
  end = clock();
  e = (double)(end - start) / CLOCK_PER_SEC;
  fprintf(time, "tempo de exec openmp: %f segundos\n", e);
  if(fclose(ptr) != 0){
    printf("erro");
  }
  fptr = fopen("mandelbrot_vfmns_pthread1.txt", "w");
  if(fptr = NULL){
    printf("erro");
    return 1;
  }
  start = clock();
  pthread_t threads[num_threads];
  for(int i = 0; i < num_threads; i++){
    if(pthread_create(&threads[i], NULL, mandelp, &i) != 0){
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
  e = (double)(end - start) / CLOCK_PER_SEC;
  fprintf(time, "tempo de exec pthread1: %f segundos\n", e);
  if(fclose(ptr) != 0){
    printf("erro");
  }
  if(fclose(time) != 0){
    printf("erro");
    return 1;
  }
  return 0;
}
