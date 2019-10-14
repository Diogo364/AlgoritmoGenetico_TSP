#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QtNos 5

typedef struct node{
    int id;
    int qntConexoes;
    struct edge *neighbor;
}node;

typedef struct edge{
    int peso;
    struct node *conexao;
    struct edge *outro;
}edge;

typedef struct cromossomo{
    char caminho[QtNos];
    int peso;
}cromossomo;


/*Inicializa array de nodes com configuracao padrao:
**id = contador
**qntConexoes = 0
**neighbor = NULL
*/
node * inicializarNode(void);

edge* createconection(node* no, int id, int weight){
    edge *novo, *aux;
    novo = (edge*)malloc(sizeof(edge));
    novo->conexao = &no[id];
    novo->outro = NULL;
    novo->peso = weight;
    return novo;
}

void inicializarcaminho(cromossomo *c1, node grafo){
    char *aux, strcopy[10000], delimitador[] = ",";
    int i = 0, array[10000];
    printf("\nEntre com um caminho: ");
    fgets(c1->caminho, 10000, stdin);
    strcpy(strcopy, c1->caminho);
    puts(strcopy);
    aux = strtok(strcopy, delimitador);
    while (aux != NULL) {
        array[i] = atoi(aux);
        aux = strtok(NULL, delimitador);
        i++;
    }
}

node* initializegraph(){
    node *aux;
    int i;
    aux = (node*)malloc(sizeof(node) * QtNos);
    for(i = 0; i < N; i++){
        printf("\nEntre com o ID do no: ");
        scanf("%d", &aux[i].id);
    }
    return aux;
}



int main(void){
    node *teste;
    edge *point;
    cromossomo caminho1;
    int i, qnt, auxconect, peso;




    teste = (node*)malloc(sizeof(node) * Qnos);
    inicializarcaminho(&caminho1,teste[0]);
    for(i = 0; i < N; i++){
        teste[i].id = i;
        teste[i].neighbor = NULL;
        printf("\nEntre com a quantidade de conexoes do no %d: ", i);
        scanf("%d", &qnt);
        teste[i].qntConexoes = qnt;
        while (qnt > 0) {
            printf("\nEntre com um no de conexao para o no %d e o peso dessa conexao: ", i);
            scanf("%d %d", &auxconect, &peso);
            if(teste[i].neighbor == NULL){
                teste[i].neighbor = createconection(teste, auxconect, peso);
            }else{
                point = teste[i].neighbor;
                while(point->outro != NULL){
                    point = point->outro;
                }
                point->outro = createconection(teste, auxconect, peso);
            }
            qnt--;
        }
    }



    for(i = 0;i < N; i++){
        printf("\nO no %d tem %d conexoes:",teste[i].id, teste[i].qntConexoes);
        point = teste[i].neighbor;
        do{
            printf("\nConexao com %d", (point->conexao)->id);
            point = point->outro;
        }while (point != NULL);
        printf("\n");
    }




    return 0;
}

node * inicializarNode(void){
    node *arrayNode;
    int init;
    arrayNode = (node*)malloc(sizeof(node) * QtNode);
    for (init = 0; init < QtNode; init++) {
        arrayNode[init].id = init;
        arrayNode[init].neighbor = NULL;
        arrayNode[init].qntConexoes = 0;
    }
    return arrayNode;
}
