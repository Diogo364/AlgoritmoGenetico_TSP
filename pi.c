#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QtNode 30            //Quantidade de nodes do grafo
#define INF 4000            //Valor absurdo para conexao inexistente
#define QtCromossomos 15     //Quantidade de cromossomos em cada geracao

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

/*Estrutura que representa um cromossomo
**Cromossomo representa um caminho/sequencia de nodes
**O peso de cromossomo e a soma dos pesos das conexoes dos seus nodes
**Por definicao o cromossomo pode ter no maximo a mesma quantidade de nodes do grafo
*/
typedef struct cromossomo{
    int *caminho;
    int peso;
}cromossomo;

//Estrutura de arvore binaria para evitar caminhos ciclicos
typedef struct binaryTree{
    int value;
    struct binaryTree *maior;
    struct binaryTree *menor;
}binaryTree;


//Prototipagem das Funcoes
node * inicializarArrayNode(void);
int semConexao(node Node);
void createEdge(node **Node,int origin ,int destination, int weight);
int readFile(FILE *arquivoConexaoNode, int **origemDestinoPeso);
cromossomo * createArrayCromossomo(int origem, int destino, node *graph);
int * initializeCaminhoCromossomo(int origem, int destino);
void fillTheRest(int *caminho, int *contador);
int pesoCromossomo(cromossomo Cromossomo, node *graph);
binaryTree * initializeTree(int origem);
int isInTree(int value, binaryTree *tree);
void showCromossomos(cromossomo *Cromossomo);

int main(void){
    FILE *connections; //Estrutura das conexoes do grafo sera carregada por essa variavel
    node *graph;       //Array de nodes (grafo)
    int *OrigemDestinoPeso;   //Vetor Auxiliar Para Transportar o Node de origem, Node de destino e Peso de uma Conexao
    int temArquivo;    //Representa o final do arquivo
    cromossomo *Cromossomo;
    srand(time(NULL));  //Define semente variavel para funcao rand()
    rand();             //Resolucao de problema de variabilidade na primeira chamada da funcao rand()


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

    //BLOCO DE INICIALIZACAO DOS CROMOSSOMOS
    Cromossomo = createArrayCromossomo(1, 2, graph);
    showCromossomos(Cromossomo);





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

//Preenche o restante de um caminho com o valor -1
void fillTheRest(int *caminho, int *contador){
    while(*contador < QtNode){
        caminho[*contador] = -1;
        *contador = *contador + 1;
    }
}

/*
 * Recebe os nodes de origem e destino como parametro
 * Utiliza estrutura de arvore binaria para evitar a formacao de caminhos ciclicos
 * Cria um caminho randomico
 */
int * initializeCaminhoCromossomo(int origem, int destino){
    int *caminho, contador = 1, node;
    binaryTree *tree = initializeTree(origem);  //Criada estrutura de arvore binaria
    caminho = (int*)malloc(sizeof(int) * QtNode);   //Aloca espaco para caminho com o numero maximo de nodes
    caminho[0] = origem;    //Atribui o node de origem ao inicio do caminho
    while(contador < QtNode){
        node = (rand() % QtNode);   //Atribui um node aleatorio a variavel node
        if(node == destino){        //Caso o node seja o destino
            caminho[contador] = destino;
            contador++;
            fillTheRest(caminho, &contador);
            break;
        }
        if(isInTree(node, tree)){   //Caso o node ja tenha sido adicionado
            continue;
        } else if(contador == (QtNode - 1)){ //Garante que o node destino sera adicionado ao caminho
            caminho[contador] = destino;
        }else{
            caminho[contador] = node; //Adiciona o node ao caminho
            contador++;
        }
    }
    return caminho;
}

/*
**Retorna o peso do cromossomo
*/
int pesoCromossomo(cromossomo Cromossomo, node *graph){
    int origem = 0, destino = 1, PesoCromossomo = 0;
    edge *conexao;
    while((destino < QtNode) && (Cromossomo.caminho[destino] != -1)){   //Enquanto houver conexao valida
        conexao = graph[Cromossomo.caminho[origem]].neighbor;   //Conexao aponta para o primeiro vizinho do node de origem
        while(conexao != NULL && conexao->conexao->id != Cromossomo.caminho[destino]){  //Enquanto ainda houverem vizinhos e o vizinho nao for o node de destino
            conexao = conexao->outro;   //Conexao apontara para o proximo vizinho
        }

        //Soma valor absurdo ao peso do cromossomo caso nao haja conexao entre os nodes
        //Soma peso da conexao ao peso do cromossomo caso exista conexao
        conexao == NULL? (PesoCromossomo += INF):(PesoCromossomo += conexao->peso);

        origem++;   //Proximo node passa a ser a origem
        destino++;  //Proximo node passa a ser o destino
    }
    return PesoCromossomo;
}

/*
 * Cria e inicializa a populacao de cromossomos
 */
cromossomo * createArrayCromossomo(int origem, int destino, node *graph){
    cromossomo *arrayCromossomo;
    int init;   //Variavel iteravel iniciadora
    arrayCromossomo = (cromossomo*)malloc(sizeof(cromossomo) * QtCromossomos);
    for (init = 0; init < QtCromossomos; init++) {
        arrayCromossomo[init].caminho = initializeCaminhoCromossomo(origem, destino);
        arrayCromossomo[init].peso = pesoCromossomo(arrayCromossomo[init], graph);
    }
    return arrayCromossomo;
}

/*
 * Inicializa uma arvore binaria com configuracao padrao:
 * value = origem
 * menor = NULL
 * maior = NULL
 */
binaryTree * initializeTree(int origem){
    binaryTree *novaArvore;
    novaArvore = (binaryTree*)malloc(sizeof(binaryTree));
    novaArvore->value = origem;
    novaArvore->menor = NULL;
    novaArvore->maior = NULL;
    return novaArvore;
}

/*
 * Busca um valor dentro de uma estrutura de arvore binaria
 * retorna 1 caso encontre
 * retorna 0 e acrescenta o valor na estrutura caso nao encontre
 */
int isInTree(int value, binaryTree *tree){
    binaryTree *lastBranch = NULL;      //Guarda o endereco do ultimo ramo visitado
    binaryTree *currentBranch = tree;   //Guarda o endereco ramo atual
    int Bigger;                         //Recebe 1 se o value for maior do que o valor da posicao atual
    while(currentBranch != NULL){
        if (value == currentBranch->value){
            return 1;
        } else if(value > currentBranch->value){
            Bigger = 1;
            lastBranch = currentBranch;
            currentBranch = currentBranch->maior;
        }else{
            Bigger = 0;
            lastBranch = currentBranch;
            currentBranch = currentBranch->menor;
        }
    }
    //Caso nao encontre o valor ele sera adicionado na estrutura
    currentBranch = initializeTree(value);
    if(Bigger){
        lastBranch->maior = currentBranch;
    }else{
        lastBranch->menor = currentBranch;
    }
}


//Apresentacao de cromossomos
void showCromossomos(cromossomo *Cromossomo){
    int i, j;
    for (i = 0; i < QtCromossomos; ++i) {
        printf("Cromossomo %d\n", i);
        for (j = 0; j < QtNode; ++j) {
            if(Cromossomo[i].caminho[j] != -1){
                printf("Node %d: %d\n",j,Cromossomo[i].caminho[j]);
            }else{
                break;
            }
        }
        printf("PESO: %d\n", Cromossomo[i].peso);
        printf("----------------------\n");
    }
}