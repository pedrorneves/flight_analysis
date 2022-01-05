/*
Pedro Neves - 92713
Rafaela Seguro - 92718
*/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

SDL_Window* g_pWindow = NULL;                                                          
SDL_Renderer* g_pRenderer = NULL;   

#define DIM 100
#define NL 1000
#define PI 3.14159

/*Lê os dados do ficheiro de configuração e guarda-os em dois vetores*/
void le_dados(float parametros[10], float variaveis_estado[4]){
  FILE *fp;
  char linha[DIM];
  int i=0, p=0, d=0;

  fp = fopen("config_model.txt", "r");

  if(fp == NULL){
    printf("Erro na abertura do ficheiro\n");
    return;
  }

  while(fgets(linha, DIM, fp) != NULL){
    if(linha[0] != '%'){
      if(i<10){
        parametros[p] = atof(linha);
        ++p;
      }
      if(i>=10){
        variaveis_estado[d] = atof(linha);
        ++d;
      }
      i++;
      }
  }    
  fclose(fp);
}


/*Calcula os coeficientes de sustentação e de atrito*/
void calcula_coeficientes(float p[10],float c[2]){
/*c[0] = Cl, c[1] = Cd */
  c[0] = p[9] * ((PI*((p[3]*p[3])/p[2])) / (1 + sqrt(1 + (((p[3]*p[3])/p[2])/2) * (((p[3]*p[3])/p[2])/2))));
  c[1] = 0.02 + (1/(PI*((p[3]*p[3])/p[2])*p[8])) * c[0]*c[0];
}


/*Executa os cálculos das equações diferenciais e imprime-os num ficheiro e no ecrã, de modo a demonstrar o progresso*/
void simulacao(char nome[DIM], float p[10], float va[4], float c[2]){

  FILE *fp;
  float tempo = 0.0;
  float drag, lift;
  float a;

  fp = fopen(nome, "w");
  if(fp == NULL){
    printf("Erro na abertura do ficheiro\n");
  }

  fprintf(fp, "(%.6e %.6e %.6e %.6e %.6e %.6e %.6e %.6e %.6e %.6e)\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9]);

  fprintf(fp, "%.6e  %.6e  %.6e  %.6e  %.6e\n", tempo, va[0], va[1], va[2], va[3]);

  while ((va[3] >= 0) && (tempo <= p[0])){

    va[2] = va[2] + va[0] * cos (va[1]) * p[1];
    va[3] = va[3] + va[0] * sin (va[1]) * p[1];
    drag = c[1] * 1/2 * p[6] * (va[0] * va[0]) * p[2];
    lift = c[0] * 1/2 * p[6] * (va[0] * va[0]) * p[2];
    a = 1/(p[4] * va[0]);
    va[0] = va[0] + 1/p[4] * (- drag - p[4] * p[5] * sin (va[1])) * p[1];
    va[1] = va[1] + a * (lift - p[4] * p[5] * cos (va[1])) * p[1];


    tempo = tempo + p[1];

    fprintf(fp, "%.6e  %.6e  %.6e  %.6e  %.6e\n", tempo, va[0], va[1], va[2], va[3]);
    printf("%.6e  %.6e  %.6e  %.6e  %.6e\n", tempo, va[0], va[1], va[2], va[3]);
  }
  return;
}


/*Encontra o valor mínimo de cada coluna do ficheiro criado*/
float encontra_min(char nome[50], int a){
  FILE *fp;
  float te, ve, ga, xo, ha;
  float *t, *v, *g, *x, *h;
  float min[5];
  int i=0;
  char linha[NL];

  t = &te;
  v = &ve;
  g = &ga;
  x = &xo;
  h = &ha;

  fp = fopen(nome, "r");

  while(fgets(linha, NL, fp)!=NULL){
    sscanf(linha, "%f  %f  %f  %f  %f\n", t, v, g, x, h);
    if(i == 1){
      min[0] = *t;
      min[1] = *v;
      min[2] = *g;
      min[3] = *x;
      min[4] = *h;
    }

    else{    
      if(*t<min[0])
        min[0]=*t;
      if(*v<min[1])
        min[1]=*v;
      if(*g<min[2])
        min[2]=*g;
      if(*x<min[3])
        min[3]=*x;
      if(*h<min[4])
        min[4]=*h;
    }
    ++i;
  }

  fclose(fp);
  return min[a];
}


/*Encontra o máximo de cada coluna do ficheiro criado*/
float encontra_max(char nome[50], int a){
  FILE *fp;
  float te, ve, ga, xo, ha;
  float *t, *v, *g, *x, *h;
  float max[5];
  int i=0;
  char linha[NL];

  t = &te;
  v = &ve;
  g = &ga;
  x = &xo;
  h = &ha;

  fp = fopen(nome, "r");

  while(fgets(linha, NL, fp)!=NULL){
    sscanf(linha, "%f  %f  %f  %f  %f\n", t, v, g, x, h);
    if(i == 1){
      max[0] = *t;
      max[1] = *v;
      max[2] = *g;
      max[3] = *x;
      max[4] = *h;
    }

    else{    
      if(*t>max[0])
        max[0]=*t;
      if(*v>max[1])
        max[1]=*v;
      if(*g>max[2])
        max[2]=*g;
      if(*x>max[3])
        max[3]=*x;
      if(*h>max[4])
        max[4]=*h;
    }
    ++i;
  }

  fclose(fp);
  return max[a];
}


/*Pergunta ao utilizador qual o gráfico a criar e constrói-o com base nos valores do ficheiro criado na opção 1*/
int cria_grafico(char nome[50]){
  int a1, b1, a, b;
  int i = 0;
  char linha[NL];  
  double x_min, x_max, y_min, y_max;
  double px0, py0, x1, y1; 
  double *x, *y;
  double v[5];

  FILE *fp;

  printf("\n");
  printf("Escolha a variavel para o eixo dos xx e dos yy:\n 1 - Tempo\n 2 - Velocidade\n 3 - Angulo de ataque\n 4 - Posicao\n 5 - Altura\n");
  scanf("%d %d", &a1, &b1);
  printf("\n");
  
  a = a1 - 1;
  b = b1 - 1;

  fp = fopen(nome, "r");
  if(fp == NULL){
    printf("Primeiro crie o ficheiro na opcao 1\n");
    return 0;
  }


  if(SDL_Init(SDL_INIT_EVERYTHING)>=0){
    g_pWindow = SDL_CreateWindow("Gráfico que relaciona as variáveis escolhidas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if(g_pWindow != 0){                                                                       
      g_pRenderer = SDL_CreateRenderer(g_pWindow, -1,0);   
      if(g_pRenderer == 0){
        printf("Erro na abertura da janela\n");  
        return 1;                
      }                                                                                  
    }                                                                                    
    else{
      printf("Erro na abertura da janela\n");
      return 1;
    }
  }

  x = &x1;
  y = &y1;

  x_min = encontra_min(nome, a);
  y_min = encontra_min(nome, b);
  x_max = encontra_max(nome, a);
  y_max = encontra_max(nome, b);

  SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 255);
  SDL_RenderClear(g_pRenderer);

  px0 = (640/(x_max - x_min))*(0 - x_min);
  py0 = (480/(y_max - y_min))*(0 - y_min);

  SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
  SDL_RenderDrawLine(g_pRenderer, px0, 0, px0, 480);
  SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
  SDL_RenderDrawLine(g_pRenderer, 0, py0, 640, py0); 


  while((fgets(linha, NL, fp)!=NULL)){
    sscanf(linha, "%f  %f  %f  %f  %f\n", v[0], v[1], v[2], v[3], v[4]);
    if(linha[0]!='(' && linha[0]!= EOF){
      *x = (640/(x_max - x_min))*(v[a] - x_min);
      *y = (480/(y_max - y_min))*(v[b] - y_min);
      SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
      SDL_RenderDrawPoint(g_pRenderer, *x, *y);
    }
    i = i + 1;
  }
  
  SDL_RenderPresent(g_pRenderer);
  SDL_Delay(60000);
  SDL_Quit();
  fclose(fp);
  return 0;
}


/*Aparece após a realização da opção 2 para que o utilizador escolha o que quer fazer*/
int menu(){
  int a;
  printf("1 - Criar novo gráfico\n2 - Voltar ao menu principal\n");
  scanf("%d", &a);
  return a;
}



/*Executa os cálculos das equações diferenciais*/
float simulacao2(float p[10], float va[4], float c[2]){

  float tempo = 0.0;
  float drag, lift;
  float a;

  while ((va[3] >= 0) && (tempo <= p[0])){

    va[2] = va[2] + va[0] * cos (va[1]) * p[1];
    va[3] = va[3] + va[0] * sin (va[1]) * p[1];
    drag = c[1] * 1/2 * p[6] * (va[0] * va[0]) * p[2];
    lift = c[0] * 1/2 * p[6] * (va[0] * va[0]) * p[2];
    a = 1/(p[4] * va[0]);
    va[0] = va[0] + 1/p[4] * (- drag - p[4] * p[5] * sin (va[1])) * p[1];
    va[1] = va[1] + a * (lift - p[4] * p[5] * cos (va[1])) * p[1];

    tempo = tempo + p[1];
  }
  return va[2];
}


/*Calcula o ângulo de ataque ótimo*/
void otimiza(float p[10], float va[4], float c[2]){

  float max[2];
  float x1, *x, *a;
  
  x = &x1;
  a = &p[9];
  max[0] = -(PI/2);
  max[1] = 0;

  le_dados (p, va);  

  for(*a = -(PI/2); *a < (PI/2); *a = *a + 0.01){
    calcula_coeficientes(p, c);
    *x = simulacao2(p, va, c);
    if(*x > max[1]){
      max[0] = *a;
      max[1] = *x;
    }
  }
  
  printf("O angulo de ataque otimo e %f e a distancia percorrida e %f", max[0], max[1]);
  printf("\n");
}

/*Apresenta o menu inicial e invoca as restantes funções*/
int main() {
  char n;
  float parametros[10];
  float variaveis_estado[4];
  float coeficientes[2];
  char nome[50];
  int a;
 
    printf("\n");
    printf("  **************************************\n \n  *    BEM VINDO AO FLIGHT ANALYSIS    *\n \n  **************************************\n");
    printf("\n");

  do{
    printf("\n");
    printf("Escolha uma opção:\n 0 - Terminar o programa\n 1 - Simular o movimento da aeronave\n 2 - Visualizar resultados (graficos)\n 3 - Calcula o angulo de ataque otimo\n");
    printf("\n");
    scanf(" %c", &n);
    getchar();
    
    if(n=='1'){
      le_dados(parametros, variaveis_estado);
      calcula_coeficientes(parametros, coeficientes);
      printf("Insira um nome para o ficheiro que sera criado (termine com .txt): ");
      scanf("%s", nome);     
      simulacao(nome, parametros, variaveis_estado, coeficientes);   
    }

    if(n=='2'){
      do{
        cria_grafico(nome);
        a = menu();
      }while(a==1);
    }

    if(n=='3'){
       otimiza(parametros, variaveis_estado, coeficientes);
    }

    if((n!='1') && (n!='0') && (n!='2') && (n!='3')){
      printf("Opcao invalida\n");
    } 
  } while (n!='0');
 
  if(n=='0'){
    printf("Fim do programa!\n");
    return 0;
  }
  
  return 0;
}

