#include <stdio.h>
#include <stdlib.h>

#define QtNode 5

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


node * inicializarNode(){
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


int semConexao(node Node){
    if (Node.neighbor == NULL) {
        return 1;
    }else{
        return 0;
    }
}

edge * createconection(node *no, int id, int weight){
    edge *novo, *aux;
    novo = (edge*)malloc(sizeof(edge));
    novo->conexao = &no[id];
    novo->outro = NULL;
    novo->peso = weight;
    return novo;
}




int main(void){
    node *graph;
    int i = 0;
    graph = inicializarNode();



    while (i < QtNode) {
        printf("%d\n", graph[i].id);
        i++;
    }
}
