#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QtNode 4
#define path "graph.txt"

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
    char caminho[QtNode];
    int peso;
}cromossomo;

node * inicializarArrayNode(void);

int semConexao(node Node);

void createEdge(node **Node,int origin ,int destination, int weight);

int readFile(FILE *connections, int **vetor){
    if(connections == NULL){
        printf("ERRO NA LEITURA DO ARQUIVO!!\n");
    }else{
        if ((fscanf(connections,"%d,%d,%d\n", &(*vetor)[0], &(*vetor)[1], &(*vetor)[2]) != EOF)) {
            return 1;
        }else{
            return 0;
        }
    }
    return 0;
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

int main(void){
    FILE *connections;
    node *teste;
    int *vetor, i = 1;

    connections = fopen(path, "r");
    vetor = (int*)malloc(sizeof(int) * 3);

    teste = inicializarArrayNode();
    while(i != 0){
        i = readFile(connections, &vetor);
        printf("%d, %d, %d\n", vetor[0], vetor[1], vetor[2]);
    }

    createEdge(&teste, 1, 0, 4);


    return 0;
}

/*
**Inicializa array de nodes com configuracao padrao:
**id = contador
**qntConexoes = 0
**neighbor = NULL
*/
node * inicializarArrayNode(void){
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

/*
**Retorna 1 se o node nao possuir conexoes
**Retorna 0 se o node possuir conexoes
*/
int semConexao(node Node){
    if (Node.neighbor == NULL) {
        return 1;
    }else{
        return 0;
    }
}

/*
**Utiliza endereco de array de node para acrescentar conexoes com devido peso
**acrescenta o endereco do destino na proxima posicao disponivel da lista ligada da origem
*/
void createEdge(node **Node,int origin ,int destination, int weight){
    edge *novo, *aux;
    novo = (edge*)malloc(sizeof(edge));
    novo->conexao = &(*Node)[destination];
    novo->outro = NULL;
    novo->peso = weight;
    if (semConexao((*Node)[origin])) {
        (*Node)[origin].neighbor = novo;
    }else{
        aux = (*Node)[origin].neighbor;
        while (aux->outro != NULL) {
            aux =  aux->outro;
        }
        aux->outro = novo;
    }
    (*Node)[origin].qntConexoes = (*Node)[origin].qntConexoes + 1;
}
