#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define QtNode 4    //Quantidades de nodes do grafo
#define INF 4000    //Valor absurdo para conexao inexistente

/*
    **ANTES DE COMPILAR: Ajeitar caminho do arquivo -> graph.txt
*/
#define path "/Users/diogotelheirodonascimento/Desktop/PI Project/PI/graph.txt" //Caminho ABSOLUTO do arquivo txt que contem a estrutura do grafo
//#define path "graph.txt" //Caminho RELATIVO do arquivo txt que contem a estrutura do grafo

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

//Estrutura que representa um cromossomo
typedef struct cromossomo{
    int caminho[QtNode];
    int peso;
}cromossomo;

//Prototipagem das Funcoes
node * inicializarArrayNode(void);
int semConexao(node Node);
void createEdge(node **Node,int origin ,int destination, int weight);
int readFile(FILE *arquivoConexaoNode, int **origemDestinoPeso);
int pesoCromosomo(cromossomo c, node *Node);

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
    int *OrigemDestinoPeso;   //Vetor Auxiliar Para Transportar o Node de origem, Node de destino e Peso de uma Conexao
    int temArquivo;    //Representa o final do arquivo


    cromossomo c1;
    c1.caminho[0] = 0;
    c1.caminho[1] = 1;
    c1.caminho[2] = -1;
    c1.caminho[3] = -1;
    c1.peso = 0;

    //**BLOCO DE INICIALIZACAO DO GRAFO
    connections = fopen(path, "r"); //Inicializa variavel com o arquivo contendo a estrutura do grafo
    OrigemDestinoPeso = (int*)malloc(sizeof(int) * 3); //Inicializa vetor para armazenar a estrutura de connections
    graph = inicializarArrayNode(); //Inicializa os nodes do grafo
    //Configuracao de todas as conexoes do grafo
    do {
        temArquivo = readFile(connections, &OrigemDestinoPeso);
        createEdge(&graph, OrigemDestinoPeso[0], OrigemDestinoPeso[1], OrigemDestinoPeso[2]);
    } while(temArquivo);
    //**Grafo inicializado na variavel graph

    c1.peso = pesoCromosomo(c1, graph);
    printf("O peso do c1 e: %d\n", c1.peso);




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
**Aloca os int nas posicoes 0, 1, 2 de um vetor: Origem-Destino-Peso, respectivamente
**Retorna 0 se chegar ao final do arquivo ou não conseguir realizar a operacao
*/
int readFile(FILE *arquivoConexaoNode, int **origemDestinoPeso){
    if(arquivoConexaoNode == NULL){
        printf("ERRO NA LEITURA DO ARQUIVO!!\n");
    }else{
        if ((fscanf(arquivoConexaoNode,"%d,%d,%d\n", &(*origemDestinoPeso)[0], &(*origemDestinoPeso)[1], &(*origemDestinoPeso)[2]) != EOF)) {
            return 1;
        }else{
            return 0;
        }
    }
    return 0;
}


int pesoCromosomo(cromossomo c, node *Node){
    int origin = 0, destino = 1, peso = 0;
    edge *aux;
    while((c.caminho[destino] != -1) && (destino <= QtNode)){
//        printf("grafo: %d\n", Node[c.caminho[origin]].neighbor->conexao->id);
//        printf("caminho: %d\n", c.caminho[destino]);
//        printf("o destino vale: %d\n", destino);
        aux = Node[c.caminho[origin]].neighbor;
        while(aux->conexao->id != c.caminho[destino] && aux != NULL){
            aux = aux->outro;
        }
        aux == NULL? (peso += INF):(peso += aux->peso);
        origin++;
        destino++;
    }
    return peso;
}
