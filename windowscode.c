#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int altura;
int largura;
int max;
int num_threads;
FILE *fptr;
int next_pixel = 0;
int pixels;
typedef struct {
  int x;
  int y;
  double intensidade;
} args;
args *ptr;
void *mandel(void *argumentos) {
    args *arg = argumentos;
    double cx = 3 * arg->x / (double)largura - 2.0;
    double cy = 3 * arg->y / (double)altura - 1.5;
    double zx = 0;
    double zy = 0;
    int iter = 0;
    while (iter < max && zx * zx + zy * zy <= 4) {
        double new_zx = zx * zx - zy * zy + cx;
        double new_zy = 2 * zx * zy + cy;
        zx = new_zx;
        zy = new_zy;
        iter++;
    }
    arg->intensidade = (double)iter * 255 / (double)max;
    return NULL;
}
int main(int argc, char **argv){
  FILE *time = fopen("time.txt", "w");
  if(time == NULL){
    printf("erro ao criar arquivo time");
    return 1;
  }
  double start, end;
  if(argc != 5){
    printf("argumentos invalidos");
    return 1;
  }
  largura = atoi(argv[1]);
  altura = atoi(argv[2]);
  max = atoi(argv[3]);
  num_threads = atoi(argv[4]);
  if(largura < 1 || altura < 1 || max < 1 || num_threads < 1){
    printf("argumentos invalidos");
    return 1;
  }
  pixels = altura * largura;
  args *argumentos = malloc(pixels * sizeof(args));
  if(!argumentos){
    printf("erro alocando memoria");
    return 1;
  }
  ptr = argumentos;
  if(num_threads > pixels){
    num_threads = pixels;
  }
  fptr = fopen("mandelbrot_vfmns_serial.txt", "w");
  if(fptr == NULL){
    printf("erro ao criar arquivo serial");
    return 1;
  }
  start = omp_get_wtime();
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      argumentos[i * largura + j].x = j;
      argumentos[i * largura + j].y = i;
      mandel(&argumentos[i * largura + j]);
    }
  }
  end = omp_get_wtime();
  fprintf(time, "tempo de exec serial: %f segundos\n", end - start);
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      fprintf(fptr, "intensidade do pixel[x: %d][y: %d]: %f\n", argumentos[i* largura + j].x, argumentos[i * largura + j].y, argumentos[i * largura + j].intensidade);
    }
  }
  if(fclose(fptr) != 0){
    printf("erro ao fechar arquivo serial");
  }
  fptr = fopen("mandelbrot_vfmns_openmp.txt", "w");
  if(fptr == NULL){
    printf("erro ao abrir arquivo openmp");
    return 1;
  }
  start = omp_get_wtime();
  #pragma omp parallel for num_threads(num_threads)
  for(int i = 0; i < pixels; i++){
    mandel(&ptr[i]);
  }
  end = omp_get_wtime();
  fprintf(time, "tempo de exec openmp: %f segundos\n", end - start);
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      fprintf(fptr, "intensidade do pixel[x: %d][y: %d]: %f\n", argumentos[i* largura + j].x, argumentos[i * largura + j].y, argumentos[i * largura + j].intensidade);
    }
  }
  if(fclose(fptr) != 0){
    printf("erro ao fechar arquivo openmp");
  }
  if(fclose(time) != 0){
    printf("erro ao fechar arquivo de tempo");
    return 1;
  }
  free(argumentos);
  return 0;
}
