#include <iostream>
#include <vector>
#include <list>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

#include "data.h"
#include "Kruskal.h"
// #include "hungarian.h"

struct Node{
	vector<pair<int, int>> forbidden_arcs;	//lista de arcos proibidos do no
	vector<vector<int>> subtour;			//conjunto de subtours da solucao
	double lower_bound;						//custo total da solucao do algoritmo hungaro
	int chosen;								//indice do menor subtour
	bool feasible;							//indica se a solucao do AP_TSp e viavel
};

void printarSubtour(vector<int>& subtour){
	for(int i = 0; i < subtour.size(); i++){
		cout << subtour[i] << " ";
	}cout << endl;
}

/*void printarMatrizCustos(hungarian_problem_t p, int dimension){
	for(int i = 0; i < dimension; i++){
		for(int j = 0; j < dimension; j++){
			cout << p.cost[i][j] << " ";
		}
		cout << endl;
	}
}*/

void printNode(Node node){
	for(int i = 0; i < node.forbidden_arcs.size(); i++){
		cout << node.forbidden_arcs[i].first << " - " << node.forbidden_arcs[i].second << endl;
	}

	cout << endl;

	for(int i = 0; i < node.subtour.size(); i++){
		printarSubtour(node.subtour[i]);
	}

	cout << endl << "LB: " << node.lower_bound << endl;
	cout << "Index: " << node.chosen + 1;

	cout << (node.feasible ? " | " : " | not ") << "feasible" << endl;
}

/*void preencherSubtour(hungarian_problem_t& p, vector<vector<int>>& subtour, int dimension){
	int i, index;
	vector<bool> passou(dimension, 0);
	bool acabou = 0;

	while(!acabou){//debugar
		// for(int i = 0; i < passou.size(); i++){
		// 	cout << passou[i] << " ";
		// }cout << endl;

		vector<int> aux;
		for(i = 0; i < dimension; i++){//so para achar o primeiro que ele ainda n passou
			if(passou[i] == 0){//se ele ainda n passou por ele vou colocar na sequencia
				passou[i] = 1;
				aux.push_back(i+1);

				do{
					//printarSubtour(aux);
					for(index = 0; index < dimension; index++){
						if(p.assignment[i][index] == 1){
							aux.push_back(index+1);
							passou[i] = 1;
							i = index;
							break;
						}
					}
				}while((i+1) != aux[0]);

				break;
			}
		}
		//printarSubtour(aux);
		//scanf("%*c");

		subtour.push_back(aux);

		//ver se acabou
		acabou = 1;
		for(i = 0; i < dimension; i++){
			if(passou[i] == 0){
				acabou = 0;
				break;
			}
		}
	}
}*/

int menorSubtourIndex(vector<vector<int>>& subtour){
	pair<int, int> menor;//o primeiro vai ser o valor do menor e o segundo vai ser o index
	menor.first = subtour[0].size();
	menor.second = 0;

	//cout << "Primeiro menor: " << menor.first << endl;

	for(int i = 1; i < subtour.size(); i++){
		if(subtour[i].size() < menor.first){
			menor.first = subtour[i].size();
			menor.second = i;
			//cout << "Atualizei o menor para " << menor.first << endl;
		}
	}

	return menor.second;
}

list<Node>::iterator branchingStrategy(list<Node>& tree, string& estrategia){
	if(estrategia.compare("dfs") == 0){//profundidade
		return --tree.end();
	}else if(estrategia.compare("bfs") == 0){//largura
		return tree.begin();
	}else if(estrategia.compare("lb") == 0){//menor lb
		int menor = 0;
		auto itMenor = tree.begin();

		for(auto it = tree.begin(); it != tree.end(); it++){
			if(it->lower_bound < menor){
				menor = it->lower_bound;
				itMenor = it;
			}
		}

		return itMenor;
	}

	return tree.begin();
}

/*void proibindoArcos(, Node& node){
	for(int i = 0; i < node.forbidden_arcs.size(); i++){
		//cout << "Proibi um arco\n";
		p.cost[node.forbidden_arcs[i].first - 1][node.forbidden_arcs[i].second - 1] = 99999999;
		// cout << "Custo do arco " << node.forbidden_arcs[i].first << " - " << node.forbidden_arcs[i].second << ": "
		// << p.cost[node.forbidden_arcs[i].first - 1][node.forbidden_arcs[i].second - 1] << endl;
	}
}*/

void dualLagrangiano(Node& node, int dimension, double** cost){
	// hungarian_init(&p, cost, dimension, dimension, HUNGARIAN_MODE_MINIMIZE_COST);

	//printarMatrizCustos(p, dimension);

	proibindoArcos(p, node);//proibo os arcos da matriz
	
	//cout << "Lower Bound antes: " << node.lower_bound << endl;
	node.lower_bound = hungarian_solve(&p);
	//cout << "Lower Bound depois: " << node.lower_bound << endl;
	preencherSubtour(p, node.subtour, dimension);

	node.chosen = menorSubtourIndex(node.subtour);
	
	if(node.subtour.size() == 1){
		node.feasible = 1;
	}else{
		node.feasible = 0;
	}

	hungarian_free(&p);
}

/*Node branchAndBound(int dimension, Data& data, double** cost, string& estrategia){
	Node root, best;
	//iniciarRaiz(root, p, dimension);

	list<Node> tree;
	tree.push_back(root);

	double upper_bound = numeric_limits<double>::infinity();

	while(!tree.empty()){
		//cout << "Entrei aqui\n";
		auto node = branchingStrategy(tree, estrategia); //node apontara para algum no da arvore
		// getSolutionHungarian(*node, p, dimension, cost); //trocar essa solucao
		dualLagrangiano(*node, dimension, cost);


		//printNode(*node);

		if(node->lower_bound > upper_bound){
			tree.erase(node);
			//cout << "No invalido\n";
			continue;
		}
		
		if(node->feasible){
			//cout << "Possivel solucao encontrada\n";

			if(node->lower_bound < upper_bound){
				upper_bound = node->lower_bound;
				best = *node;
			}
		}else{
			// Adicionando os filhos
			for(int i = 0; i < node->subtour[node->chosen].size() - 1; i++){//iterar por todos os arcos do subtour escolhido
				//printNode(*node);

				//cout << "No adicionado\n";
				Node n;//criar no auxiliar

				n.lower_bound = node->lower_bound;
				n.forbidden_arcs = node->forbidden_arcs;//herdar os arcos passados

				pair<int, int> forbidden_arc = {//proibir um arco novo
					node->subtour[node->chosen][i],
					node->subtour[node->chosen][i+1]
				};

				n.forbidden_arcs.push_back(forbidden_arc);
				tree.push_back(n);//inserir novos nos na arvore
				//scanf("%*c");
			}
		}

		tree.erase(node);
		//cout << "Apaguei um no\n";
		//cout << tree.size() << endl;
	}

	return best;
}*/

int main(int argc, char** argv){
	Data * data = new Data(2, argv[1]);
	data->readData();

	string estrategia = argv[2];
	//cout << "Estrategia: " << estrategia << endl;

	int index;

	vector<vector<double>> cost;
	for (int i = 0; i < data->getDimension(); i++){
		vector<double> aux;
		for (int j = 0; j < data->getDimension(); j++){
			aux.push_back(data->getDistance(i,j));
		}
		cost.push_back(aux);
	}

	printf("Matriz de custos\n");
	for(int i = 0; i < data->getDimension(); i++){
		for(int j = 0; j < data->getDimension(); j++){
			if(cost[i][j] > 999999){
				printf(" inf ");
			}else{
				printf("%4.lf ", cost[i][j]);
			}
		}
		puts("");
	}

	Kruskal kruskal(cost);
	double kruskalCost = kruskal.MST(cost.size()-1);
	vector<pair<int, int>> edges = kruskal.getEdges();

	//debug do Kruskal
	// printf("Custo MST: %.2lf\n", kruskalCost);
	// printf("Edges:\n");
	// for(int i = 0; i < edges.size(); i++){
	// 	printf("%2.d - %2.d\n", edges[i].first, edges[i].second);
	// }

	Node node;
	dualLagrangiano(node, data->getDimension(), cost);

	//antes de tudo tem que implementar a ideia de resolver o dualLagrangiano


	//não vai precisar da solução do problema hungaro
	//e vai substituir ela pela resolução do MST

	clock_t start = clock();
	// Node best = branchAndBound(data->getDimension(), *data, cost, estrategia);
	clock_t end = clock();

	double tempoTotal = (double)(end - start) / (double)(CLOCKS_PER_SEC);
	double valorTotal = 0;//best.lower_bound;

	printf("%.3lf %.2lf\n", tempoTotal, valorTotal);

	delete data;

	//debug
	/*printarMatrizCustos(p, data->getDimension());
	cout << endl;

	//teste de usar iterator em uma lista
	/*list<int> lista;

	lista.push_back(1);
	auto x = lista.begin();

	for(int i = 2; i < 10; i++){
		lista.push_back(i);
	}

	cout << "P: " << *x << endl;

	for(auto it = lista.begin(); it != lista.end(); it++){
		cout << *it << " ";
	}cout << endl;*/

	return 0;
}
