#include <stdio.h> 
#include <stdlib.h>

#define FILE_PATH "inputs/e2.in"

int total = 0;
int numbers_size = 0;

int* read_file();

int main(void) { 

  // chama a função read_file() para ler o arquivo
  // recebe um pointeiro para um array contendo os números
  int *numbers = read_file();

  for(int i=0; i<numbers_size; i++) {
    int result = 0;
    for(int j=1; j<=numbers[i]; j++) {
        if (numbers[i] % j == 0) {
            result++;
        }
    }
    printf("Total de divisores para %d = %d\n", numbers[i], result);
    total += result;
  }

  printf("\nTotal de divisores: %d\n", total);
  
  return 0; 
}

int* read_file() {
  FILE *file;
  static char line[20];
  
  // abre o arquivo
  file = fopen(FILE_PATH, "r");

  // lê a primeira linha do arquivo, contendo a quantidade de números
  // adiciona esse numero para a variavel number_size
  fgets(line, 20, file);
  numbers_size = atoi(line);

  // aloca memoria para o array dos numeros
  int *numbers = (int *)malloc(sizeof(int) * numbers_size);

  // continua a leitura do arquivo
  // e adiciona cada número no array
  int count = 0;
  while(fgets(line, 20, file)!= NULL) {
    numbers[count] = atoi(line);
    count ++;
  }
  
  // fecha o arquivo
  fclose(file);

  return numbers;
}
