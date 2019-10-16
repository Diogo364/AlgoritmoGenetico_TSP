#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QtNode 4    //Quantidades de nodes do grafo
#define path "graph.txt" //Caminho do arquivo txt que contem a estrutura do grafo

//Estrutura de cada node
typedef struct node{
    int id;
    int qntConexoes; //Quantas conexoes se originam nesse node
    struct edge *neighbor; //Endereco da primeira conexao
}node;

//Estrutura que representa a conexao
typedef struct edge{
    int peso; //Peso da conexao
    struct node *conexao; //Endereco do node de destino da conexao
    struct edge *outro; //Endereco de outra conexao para o mesmo node de origem
}edge;

/*EM IMPLEMENTACAO
typedef struct cromossomo{
    char caminho[QtNode];
    int peso;
}cromossomo;
*/

node * inicializarArrayNode(void);

int semConexao(node Node);

void createEdge(node **Node,int origin ,int destination, int weight);

int readFile(FILE *connections, int **vetor);


/*
**EM IMPLEMENTACAO
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
*/
int main(void){
    FILE *connections; //Estrutura das conexoes do grafo sera carregada por essa variavel
    node *graph;       //Array de nodes (grafo)
    int *OrDestPeso,   //Vetor contendo Node de origem, Node de destino e Peso de conexao lido
    fimArquivo;        //Representara o final do arquivo carregado em connections

    connections = fopen(path, "r"); //Inicializa variavel com o arquivo contendo a estrutura do grafo
    OrDestPeso = (int*)malloc(sizeof(int) * 3); //Inicializa vetor para armazenar a estrutura de connections
    graph = inicializarArrayNode(); //Inicializa os nodes do grafo

    //Configura todas as conexoes do grafo
    do {
        fimArquivo = readFile(connections, &OrDestPeso);
        // printf("%d, %d, %d\n", OrDestPeso[0], OrDestPeso[1], OrDestPeso[2]);
        createEdge(&graph, OrDestPeso[0], OrDestPeso[1], OrDestPeso[2]);
    } while(fimArquivo != 0);





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

/*
**Le um arquivo txt com padrao de linhas: "int,int,int\n"
**Aloca os int nas posicoes 0, 1, 2 de um vetor, respectivamente
**Retorna 0 se chegar ao final do arquivo ou não conseguir realizar a operacao
*/
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
