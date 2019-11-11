#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define QtNode 100000
#define INF 1000                //Valor absurdo para conexao inexistente
#define TamanhoPopulacao 100    //Quantidade de cromossomos em cada geracao
#define ORIGEM 5                //Node de origem dos cromossomos
#define DESTINO 2               //Node de destino dos cromossomos
#define TaxaMutacao 50
#define TaxaElitismo 10   //Referente a permanencia de percentual de individuos para a proxima geracao
#define TaxaMutagenicos 20   //Referente a mutacao de individuos antes de reproduzirem
#define QtGeracoes 10


/*
    **ANTES DE COMPILAR: Ajeitar caminho absoluto do arquivo -> graph.txt
*/
#define path "/Users/diogotelheirodonascimento/Desktop/PI/graph3.txt" //Caminho ABSOLUTO do arquivo txt que contem a estrutura do grafo

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
*/
typedef struct cromossomo{
    int fitness;
    int peso;
    struct gene *caminho;
}cromossomo;

/*
 * Estrutura que representa o gene
 * Cada gene pertencente a um cromossomo representa um node
 */
typedef struct gene{
    int id;
    struct gene *next;
}gene;

//Estrutura de arvore binaria para evitar caminhos ciclicos
typedef struct binaryTree{
    int value;
    struct binaryTree *maior;
    struct binaryTree *menor;
}binaryTree;


//Prototipagem das Funcoes

//Funcoes ARVORE BINARIA
binaryTree * initializeTree(int origem);
int isInTree(int value, binaryTree *tree);

//Funcoes Algoritmo Genetico
cromossomo * createArrayCromossomo(int origem, int destino, node *graph, int *geracao);
gene * initializeCaminhoCromossomo(int origem, int destino, node *Graph);
gene* initializeGene();
gene * createGeneNode(int Node);
int pesarPopulacao(cromossomo *populacao);
void generateFitness(cromossomo *populacao, int *somatorioFitness);
int * generateArrayProbabilidade(cromossomo *populacao, int somatorioFitness);
int * pickTop10(cromossomo *populacao, int *percentualElitismo);
int * chooseParents(int *arrayProbabilidade);
int avaliaCruzamento(cromossomo parent1, cromossomo parent2);
cromossomo cruzamento(cromossomo *populacao, int *arrayProbabilidade, node *graph);
cromossomo copyCromossomo(cromossomo individuo);
void crossingOver(cromossomo *individuo1, cromossomo *individuo2, int pontoCrossingOver);
int tamanhoCaminho(gene *caminho);
void mutar(gene *caminho, node *graph);
void corrigeLoop(gene *caminho);
int pesoCromossomo(gene *caminho, node *graph);
void showPopulacao(cromossomo *Cromossomo);
void showIndividuo(cromossomo *populacao, int individuo);
void showBest(cromossomo *populacao);



//Funcoes NODE
node * inicializarArrayNode(void);
int semConexao(node Node);
void createEdge(node **Node,int origin ,int destination, int weight);
int readFile(FILE *arquivoConexaoNode, int **origemDestinoPeso);



int main(void){
    FILE *connections; //Estrutura das conexoes do grafo sera carregada por essa variavel
    node *graph;       //Array de nodes (grafo)
    int *OrigemDestinoPeso;   //Vetor Auxiliar Para Transportar o Node de origem, Node de destino e Peso de uma Conexao
    int temArquivo = 0;    //Representa o final do arquivo
    int percentualElitismo = TamanhoPopulacao / TaxaElitismo;
    int percentualMutacaoAdultos = TamanhoPopulacao / TaxaMutagenicos;
    int geracao = 0;     //Identifica geracao atual
    int somaFitness = 0;    //Armazena o valor total do fitness da populacao para selecao dos melhores
    int *melhores;          //Armazena os melhores 10% da populacao
    int *arrayProbabilidade;    //Armazena a probabilidade dos individuos serem selecionados para cruzamento
    int individuo = 0;
    int contadorMutacao = 0;
    clock_t tempoExecucao;
    cromossomo *newPopulacao;
    cromossomo *populacao;
    srand(time(NULL));  //Define semente variavel para funcao rand()
    rand();             //Resolucao de problema de variabilidade na primeira chamada da funcao rand()

    //**BLOCO DE INICIALIZACAO DO GRAFO
    connections = fopen(path, "r"); //Inicializa variavel com o arquivo contendo a estrutura do grafo
    if(connections == NULL){
        printf("ERRO NA LEITURA DO ARQUIVO!!\n");
        return  1;
    }
    OrigemDestinoPeso = (int*)malloc(sizeof(int) * 3); //Inicializa vetor para armazenar a estrutura de connections
    graph = inicializarArrayNode(); //Inicializa os nodes do grafo
    //Configuracao de todas as conexoes do grafo
    do{
        temArquivo = readFile(connections, &OrigemDestinoPeso);
        if(temArquivo){
            createEdge(&graph, OrigemDestinoPeso[0], OrigemDestinoPeso[1], OrigemDestinoPeso[2]);
        }
    }while(temArquivo);
    //**Grafo inicializado na variavel graph

    //CODIGO GENETICO
    tempoExecucao = clock();
    populacao = createArrayCromossomo(ORIGEM, DESTINO, graph, &geracao);
    generateFitness(populacao, &somaFitness);
//    showPopulacao(populacao);
//    printf("\n\n");
//    printf("------------------------------------");
//    printf("\n\n");
    do{
        individuo = 0;
        arrayProbabilidade = generateArrayProbabilidade(populacao, somaFitness);
        melhores = pickTop10(populacao, &percentualElitismo);
        newPopulacao = createArrayCromossomo(ORIGEM, DESTINO, graph, &geracao);
        while(individuo < TamanhoPopulacao){
            newPopulacao[individuo] = cruzamento(populacao, arrayProbabilidade, graph);
            individuo++;
        }
        individuo = 0;
        while(individuo < percentualElitismo){
            newPopulacao[individuo] = copyCromossomo(populacao[melhores[individuo]]);
            individuo++;
        }
        int ultimos = TamanhoPopulacao-1;
        for (contadorMutacao = 0; contadorMutacao < percentualMutacaoAdultos; contadorMutacao++){
            mutar(newPopulacao[ultimos].caminho, graph);
            corrigeLoop(newPopulacao[ultimos].caminho);
            newPopulacao[ultimos].peso = pesoCromossomo(newPopulacao[ultimos].caminho, graph);
            ultimos--;
        }
        somaFitness = 0;
        generateFitness(newPopulacao, &somaFitness);
        populacao = newPopulacao;
//        printf("GERACAO: %d", geracao);
//        printf("--------------------------------\n\n");
//        showIndividuo(newPopulacao, 0);
    }while(geracao < QtGeracoes);
//    showPopulacao(newPopulacao);
    tempoExecucao = clock() - tempoExecucao;
    printf("\nTEMPO DE EXECUCAO: %f", ((double)tempoExecucao)/(CLOCKS_PER_SEC));
    showBest(newPopulacao);
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
        arrayNode[init].id = init;
        arrayNode[init].neighbor = NULL;
        arrayNode[init].qntConexoes = 0;
    }
    return arrayNode;
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
**Le um arquivo txt com padrao de linhas: "int,int,int\n"
**Aloca os int nas posicoes 0, 1, 2 de um vetor: Origem-Destino-Peso, respectivamente
**Retorna 0 se chegar ao final do arquivo ou não conseguir realizar a operacao
*/
int readFile(FILE *arquivoConexaoNode, int **origemDestinoPeso){
    if ((fscanf(arquivoConexaoNode,"%d,%d,%d\n", &(*origemDestinoPeso)[0], &(*origemDestinoPeso)[1], &(*origemDestinoPeso)[2]) != EOF)) {
        return 1;
    }else{
        return 0;
    }
}

/*
 * Inicializa um gene nas seguintes configuracoes:
 *  id: -1 //Valor invalido
 *  next: NULL
 */
gene* initializeGene(){
    gene *newGene;
    newGene = (gene*)malloc(sizeof(gene));
    newGene->id = -1;
    newGene->next = NULL;
    return newGene;
}

/*
 * Inicializa um gene nas seguintes configuracoes:
 *  id: Valor do node entrado como parametro    //Valor invalido
 *  next: NULL
 */
gene * createGeneNode(int Node){
    gene *newGene;
    newGene = (gene*)malloc(sizeof(gene));
    newGene->id = Node;
    newGene->next = NULL;
    return newGene;
}

/*
**Retorna o peso do cromossomo
 * Recebe como Parametro o caminho do cromossomo a ser pesado e o ponteiro contendo o grafo
*/
int pesoCromossomo(gene *caminho, node *graph){
    gene *origem = NULL, *destino = NULL;   //Variaveis que receberao a origem e o destino de cada conexao
    int PesoCromossomo = 0;
    edge *conexao;  //Variavel que recebera as conexoes a serem testadas
    origem = caminho;            //Definicao do node de origem da conexao
    destino = caminho->next;     //Definicao do node destino da conexao
    while(destino != NULL){   //Enquanto houver conexao valida
        conexao = graph[origem->id].neighbor; //Conexao aponta para o primeiro vizinho do node de origem
        while(conexao != NULL && (conexao->conexao->id != destino->id)){  //Enquanto ainda houverem vizinhos e o vizinho nao for o node de destino
            conexao = conexao->outro;   //Conexao apontara para o proximo vizinho
        }
        //Soma valor ABSURDO ao peso do cromossomo caso nao haja conexao entre os nodes
        //Soma peso da conexao ao peso do cromossomo caso exista conexao
        conexao == NULL? (PesoCromossomo += INF):(PesoCromossomo += conexao->peso);

        origem = origem->next;   //Origem aponta para o proximo node
        destino = destino->next; //Destino aponta para o proximo node
    }
    return PesoCromossomo;
}

/*
 * Recebe os nodes de origem e destino como parametro e a estrutura do grafo
 * Utiliza estrutura de arvore binaria para evitar a formacao de caminhos ciclicos
 * Cria um caminho randomico
 */
gene * initializeCaminhoCromossomo(int origem, int destino, node *Graph){
    gene *caminho;          //Aponta para o primeiro elemento da lista ligada de genes
    gene *aux, *novo;       //Aponta para ultimo gene adicionado a lista e para o novo gene a ser adicionado, respectivamente
    edge *possivelGene = NULL;   //Aponta para o node candidato a gene
    binaryTree *tree = initializeTree(origem);  //Criada estrutura de arvore binaria
    int contador = 1, node = 0;     //Variavel contadora e id do node candidato a gene
    //Distancia para chegar ao node candidato a gene e quantidade de conexoes a partir da origem
    int distanciaNodeEscolhido, quantidadeCaminhosOrigem;
    caminho = createGeneNode(origem);   //Atribui o node de origem ao inicio do caminho
    aux = caminho;  //aux aponta para o node de origem do caminho
    while((contador < (QtNode - 1)) && (node != destino)){   //Enquanto não chegar no node de destino e não tiver o numero maximo de nodes
        possivelGene = Graph[aux->id].neighbor;  //Atribuicao de vizinho possivel para possivel gene
        quantidadeCaminhosOrigem = Graph[aux->id].qntConexoes; //Atribuicao de quantidade de conexoes a partir do node origem
        if(quantidadeCaminhosOrigem == 0){ //Se nao houverem conexoes retorna NULL
            return NULL;
        }
        distanciaNodeEscolhido = (rand() % quantidadeCaminhosOrigem);   //Gera o indice de um elemento valido na lista de adjacencias
        while(distanciaNodeEscolhido > 0){  //Enquanto nao chegar no node escolhido
            possivelGene = possivelGene->outro; //Possivel gene aponta para o proximo candidato
            distanciaNodeEscolhido--;
        }
        node = possivelGene->conexao->id;   //Atribui valor do id ao node
        novo = initializeGene();    //Cria um gene padrao
        if(isInTree(node, tree) && (quantidadeCaminhosOrigem > 1)){ //Se o node escolhido ja estiver no caminho e houverem outras possibilidades de candidatos
            possivelGene = Graph[aux->id].neighbor; //Candidato aponta para o primeiro elemento da lista ligada
            while(isInTree(node, tree)){  //Enquanto o candidato escolhido estiver no caminho
                node = possivelGene->conexao->id;   //Atribuicao de novo id ao node
                possivelGene = possivelGene->outro; //possivelGene aponta para o candidato vizinho
                if(possivelGene == NULL){ //SE NAO HOUVEREM MAIS CANDIDATOS
                    return NULL;
                }
            }
        }
        novo->id = node;    //Adiciona id do node candidato ao gene
        aux->next = novo;   //Novo Node e adicionado ao caminho
        aux = aux->next;    //Aux aponta para o ultimo node do caminho
        contador++;
    }
    //Cria e adiciona o node de destino ao caminho caso nao haja
    if(node != destino){
        novo = initializeGene();
        novo->id = destino;
        aux->next = novo;
    }
    return caminho;
}

/*
 * Cria e inicializa a populacao de cromossomos
 * modoGeracao = 1: Nao realiza a criacao de caminhos
 * modoGeracao = 0: Realiza a criacao de caminhos
 */
cromossomo * createArrayCromossomo(int origem, int destino, node *graph, int *geracao){
    cromossomo *populacao;
    int init;   //Variavel iteravel iniciadora
    populacao = (cromossomo*)malloc(sizeof(cromossomo) * TamanhoPopulacao);  //Aloca a populacao
    for (init = 0; init < TamanhoPopulacao; init++) {  //Para os cromossomos da populacao
        populacao[init].caminho = NULL;
        populacao[init].fitness = 0;
        if(*geracao == 0){   //Se for a primeira geracao
            //Atribui um caminho valido e um peso ao cromossomo
            while (populacao[init].caminho == NULL){
                populacao[init].caminho = initializeCaminhoCromossomo(origem, destino, graph);
            }
            populacao[init].peso = pesoCromossomo(populacao[init].caminho, graph);
        }
    }
    *geracao = *geracao + 1;
    return populacao;
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
    return 0;
}

/*
 * Calcula o peso total da populacao
 */
int pesarPopulacao(cromossomo *populacao){
    int contador = 0;
    int pesoPopulacao = 0;
    while(contador < TamanhoPopulacao){
        pesoPopulacao += populacao[contador].peso;
        contador++;
    }
    return pesoPopulacao;
}

/*
 * Calcula o Fitness de cada individuo da populacao e o somatorio total de todos os Fitness
 * Formula do fitness: (Peso Total da Populacao)/(Peso do Cromossomo)
 */
void generateFitness(cromossomo *populacao, int *somatorioFitness){
    int individuo = 0;
    int pesoTotalPopulacao = pesarPopulacao(populacao);
    while(individuo < TamanhoPopulacao){
        //Utiliza o peso total da populacao e a parcela do peso do cromossomo para calculo do fitness
        populacao[individuo].fitness = (int)((float)pesoTotalPopulacao/(float)populacao[individuo].peso);
        *somatorioFitness += populacao[individuo].fitness;
        individuo++;
    }
}

/*
 * Gera um Array de tamanho 100 correspondente ao percentual de probabilidade de ser escolhido um individuo baseado no Fitness
 * Probabilidade: (FitnessIndividuo/SomatorioFitness)*100
 * Por conta das aproximacoes da conversao da probabilidade para inteiro, sao sorteados individuos aleatoriamente para o preenchimento do vetor
 */
int * generateArrayProbabilidade(cromossomo *populacao, int somatorioFitness){
    int individuo;
    int probabilidade;
    int contadorArrayProbabilidade = 0;
    int *arrayProbabilidade = (int*)malloc(sizeof(int) * 100);
    for (individuo = 0; individuo < TamanhoPopulacao; ++individuo) {
        //Avalia a probabilidade de cada individuo
        probabilidade = ((int)(((float)populacao[individuo].fitness/(float)somatorioFitness) * 100));
        //Acrescenta copias do individuo no array correspondendo a sua probabilidade
        while(probabilidade > 0){
            arrayProbabilidade[contadorArrayProbabilidade] = individuo;
            probabilidade--;
            contadorArrayProbabilidade++;
            if(contadorArrayProbabilidade == 100){
                return arrayProbabilidade;
            }
        }
    }
    //Caso nao seja preenchido completamente, sorteia cromossomos para preenchimento
    while(contadorArrayProbabilidade < 100){
        arrayProbabilidade[contadorArrayProbabilidade] = (rand() % TamanhoPopulacao);
        contadorArrayProbabilidade++;
    }
    return arrayProbabilidade;
}

/*
 * Seleciona os 10% mais bem adaptados da populacao (maior fitness)
 * Retorna um array com os individuos
 */
int * pickTop10(cromossomo *populacao, int *percentualElitismo){
    int contador = 0, individuo = 0;
    int fitnessCandidato = 0;
    int fitnessConsolidado = 0;
    int candidato = 0, aux = 0;
    //Garante que pelo menos um individuo esteja no array
    if(*percentualElitismo == 0){
        (*percentualElitismo) = 1;
    }
    //Aloca vetor de tamanho correspondente a 10% da populacao
    int *top10 = (int*)malloc(sizeof(int) * (*percentualElitismo));
    while(individuo < TamanhoPopulacao){    //Enquanto houverem individuos na populacao
        //Adiciona os primeiros 10% individuos da populacao no array
        if(individuo < (*percentualElitismo)){
            top10[individuo] = individuo;
        }
            //Teste para restante da populacao
        else{
            candidato = individuo;
            //Itera o array dos 10% comparando candidatos com os consolidados
            //Se o candidato for mais adaptado que o consolidado, este e acrescentado aos consolidados e o ex-consolidado se torna o novo candidato
            while(contador < (*percentualElitismo)){
                fitnessCandidato = populacao[candidato].fitness;
                fitnessConsolidado = populacao[top10[contador]].fitness;
                if(fitnessCandidato > fitnessConsolidado){
                    aux = top10[contador];
                    top10[contador] = individuo;
                    candidato = aux;
                }
                contador++;
            }
        }
        individuo++;
        contador = 0;
    }
    return top10;
}

int * chooseParents(int *arrayProbabilidade){
    int p1, p2, *parents;
    parents = (int*)malloc(sizeof(int) * 2);
    do{
        p1 = arrayProbabilidade[rand() % 100];
        p2 = arrayProbabilidade[rand() % 100];
        parents[0] = p1;
        parents[1] = p2;
    }while(parents[0] == parents[1]);
    return parents;
}

int avaliaCruzamento(cromossomo parent1, cromossomo parent2){
    gene* caminho1 = parent1.caminho->next;
    gene* caminho2 = parent2.caminho->next;
    binaryTree *tree1 = initializeTree(caminho1->id);
    int *id = (int*)malloc(sizeof(int) * QtNode);
    int contador = 0;
    while(caminho1->id != DESTINO){
        isInTree(caminho1->id, tree1);
        caminho1 = caminho1->next;
    }
    while(caminho2->id != DESTINO){
        if(isInTree(caminho2->id, tree1)){
            id[contador] = caminho2->id;
            contador++;
        }
        caminho2 = caminho2->next;
    }
    if(!contador){
        return -1;
    }
//    for (int i = 0; i < contador; ++i) {
//        printf("%d, ", id[i]);
//    }
    return id[rand() % contador];
}

cromossomo copyCromossomo(cromossomo individuo){
    cromossomo *copia = (cromossomo*)malloc(sizeof(cromossomo));
    gene *caminho = individuo.caminho;
    gene *novoCaminho = createGeneNode(caminho->id);
    copia->peso = individuo.peso;
    copia->fitness = 0;
    copia->caminho = novoCaminho;
    while(1){
        caminho = caminho->next;
        if(caminho == NULL){
            break;
        }else{
            novoCaminho->next = createGeneNode(caminho->id);
            novoCaminho = novoCaminho->next;
        }
    }
    return *copia;

}

void crossingOver(cromossomo *individuo1, cromossomo *individuo2, int pontoCrossingOver){
    gene *caminhoIndividuo1, *caminhoIndividuo2, *aux;
    caminhoIndividuo1 = individuo1->caminho;
    caminhoIndividuo2 = individuo2->caminho;
    while ((caminhoIndividuo1->id != pontoCrossingOver) || (caminhoIndividuo2->id != pontoCrossingOver)){
        if(caminhoIndividuo1->id != pontoCrossingOver){
            caminhoIndividuo1 = caminhoIndividuo1->next;
        }
        if(caminhoIndividuo2->id != pontoCrossingOver){
            caminhoIndividuo2 = caminhoIndividuo2->next;
        }
    }
    aux = caminhoIndividuo1->next;
    caminhoIndividuo1->next = caminhoIndividuo2->next;
    caminhoIndividuo2->next = aux;
}

void corrigeLoop(gene *caminho){
    binaryTree *tree = initializeTree(caminho->id);;
    gene *loop = caminho->next, *proximo = caminho->next;
    while(loop->next != NULL){
        if(isInTree(loop->id, tree)){
            while(proximo->id != loop->id){
                proximo = proximo->next;
            }
            proximo->next = loop->next;
        }
        loop = loop->next;
        proximo = caminho->next;
    }
}

int tamanhoCaminho(gene *caminho){
    int tamanho = 0;
    while(caminho != NULL){
        caminho = caminho->next;
        tamanho++;
    }
    return tamanho;
}
void mutar(gene *caminho, node *graph){
    gene *ponteiro = NULL;
    int tamanho = tamanhoCaminho(caminho);
    int pontoAleatorio;
    do{
        ponteiro = caminho;
        pontoAleatorio = rand() % tamanho;
            while (pontoAleatorio){
                ponteiro = ponteiro->next;
                pontoAleatorio--;
            }
    }while(ponteiro->id == DESTINO && ponteiro->id != ORIGEM);
    if(ponteiro->next->id == DESTINO){
        return;
    } else{
        do{
            ponteiro->next = initializeCaminhoCromossomo(ponteiro->id, DESTINO, graph);
        }while(ponteiro->next == NULL);
    }
}

cromossomo cruzamento(cromossomo *populacao, int *arrayProbabilidade, node *graph) {
//    for (int i = 0; i < 100; ++i) {
//        printf("\n");
//    }
    cromossomo *offspring1 = (cromossomo*) malloc(sizeof(cromossomo));
    cromossomo *offspring2 = (cromossomo*) malloc(sizeof(cromossomo));
    int *parents = NULL, pontoCrossingOver;
    do {
        parents = chooseParents(arrayProbabilidade);
        pontoCrossingOver = avaliaCruzamento(populacao[parents[0]], populacao[parents[1]]);
    } while (pontoCrossingOver == -1);
//    printf("\n%d\n", pontoCrossingOver);
//    showIndividuo(populacao, parents[0]);
//    printf("\n-------------------------------\n");
//    showIndividuo(populacao, parents[1]);
    *offspring1 = copyCromossomo(populacao[parents[0]]);
    *offspring2 = copyCromossomo(populacao[parents[1]]);
    crossingOver(offspring1, offspring2, pontoCrossingOver);
    corrigeLoop(offspring1->caminho);
    corrigeLoop(offspring2->caminho);

    if (rand() % 101 <= TaxaMutacao){
        mutar(offspring1->caminho, graph);
        corrigeLoop(offspring1->caminho);
}
    if (rand() % 101 <= TaxaMutacao){
        mutar(offspring2->caminho, graph);
        corrigeLoop(offspring2->caminho);
    }
    offspring1->peso = pesoCromossomo(offspring1->caminho, graph);
    offspring2->peso = pesoCromossomo(offspring2->caminho, graph);
    return (offspring1->peso > offspring2->peso)? *offspring2: *offspring1;
}

//Apresentacao de cromossomos
void showPopulacao(cromossomo *populacao){
    int atual;      //Variavel para indice do cromossomo
    gene *pointer;  //Aponta para os nodes do caminho do cromossomo
    for (atual = 0; atual < TamanhoPopulacao; ++atual) { //Para todos os cromossomos da populacao
        pointer = populacao[atual].caminho;    //Aponta para o primeiro do caminho
        printf("populacao %d\n", atual); //Impressao do peso
        while(pointer != NULL){     //Enquanto nao acabar o caminho
            printf("Node: %d\n", pointer->id);     //Imprime o node correspondente
            pointer = pointer->next;    //Aponta para o proximo node do caminho
        }
        printf("----------------------\n");
        printf("PESO: %d\nFITNESS %d\n", populacao[atual].peso, populacao[atual].fitness);
        printf("----------------------\n");
    }
}
//Apresentacao de cromossomos
void showIndividuo(cromossomo *populacao, int individuo){
        gene *caminho;
        caminho = populacao[individuo].caminho;
        printf("\nIndividuo %d\n", individuo); //Impressao do peso
        while(caminho != NULL){     //Enquanto nao acabar o caminho
            printf("Node: %d\n", caminho->id);     //Imprime o node correspondente
            caminho = caminho->next;    //Aponta para o proximo node do caminho
        }
        printf("FITNESS %d\n", populacao[individuo].fitness);
        printf("----------------------\n");
        printf("PESO %d\n", populacao[individuo].peso);
        printf("----------------------\n");

}

void showBest(cromossomo *populacao){
    int  current = 0, best = 0;
    for(current = 1; current < TamanhoPopulacao; current++){
        if(populacao[current].peso < populacao[best].peso){
            best = current;
        }
    }
        showIndividuo(populacao, best);
}
