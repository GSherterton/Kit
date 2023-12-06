#include <iostream>
#include <vector>
#include <list>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

#include "data.h"
#include "hungarian.h"

struct Node{
	vector<pair<int, int>> forbidden_arcs;//lista de arcos proibidos do no
	vector<vector<int>> subtour;//conjunto de subtours da solucao
	double lower_bound;//custo total da solucao do algoritmo hungaro
	int chosen;//indice do menor subtour
	bool feasible;//indica se a solucao do AP_TSp e viavel
};

void printarSubtour(vector<int>& subtour){
	for(int i = 0; i < subtour.size(); i++){
		cout << subtour[i] << " ";
	}cout << endl;
}

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

void preencherSubtour(hungarian_problem_t& p, vector<vector<int>>& subtour, int dimension){
	int i, index;
	vector<bool> passou(dimension, 0);
	bool acabou = 0;

	while(!acabou){//debugar
		/*for(int i = 0; i < passou.size(); i++){
			cout << passou[i] << " ";
		}cout << endl;*/

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
}

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

void iniciarRaiz(Node node, hungarian_problem_t& p, int dimension){//talvez tirar isso
	node.lower_bound = hungarian_solve(&p);
	preencherSubtour(p, node.subtour, dimension);

	node.chosen = menorSubtourIndex(node.subtour);
	
	if(node.subtour.size() == 1){
		node.feasible = 1;
	}else{
		node.feasible = 0;
	}
}

list<Node>::iterator branchingStrategy(list<Node> tree){//fazer por profundidade
//Node* branchingStrategy(list<Node> tree){//fazer por profundidade
	//a principio, so para a primeira iteracao
	//Node* p = tree.begin();
	//return p;
	return tree.begin();
}

void proibindoArcos(hungarian_problem_t& p, Node& node){
	for(int i = 0; i < node.forbidden_arcs.size(); i++){
		p.cost[node.forbidden_arcs[i].first][node.forbidden_arcs[i].second] = 99999999;
	}
}	

void getSolutionHungarian(Node& node, hungarian_problem_t p, int dimension){
	proibindoArcos(p, node);
	
	node.lower_bound = hungarian_solve(&p);
	preencherSubtour(p, node.subtour, dimension);

	node.chosen = menorSubtourIndex(node.subtour);
	
	if(node.subtour.size() == 1){
		node.feasible = 1;
	}else{
		node.feasible = 0;
	}
}

void branchAndBound(hungarian_problem_t& p, int dimension, Data& data){
	Node root;
	//iniciarRaiz(root, p, dimension);

	list<Node> tree;
	tree.push_back(root);

	double upper_bound = numeric_limits<double>::infinity();

	while(!tree.empty()){
		cout << "Entrei aqui\n";
		auto node = branchingStrategy(tree); //node apontara para algum no da arvore
		getSolutionHungarian(*node, p, dimension);

		//printNode(*node);

		if(node->lower_bound > upper_bound){
			tree.erase(node);
			cout << "No invalido\n";
			continue;
		}
		
		if(node->feasible){
			cout << "Possivel solucao encontrada\n";
			upper_bound = min(upper_bound, node->lower_bound);
		}else{
			/*Adicionando os filhos*/
			for(int i = 0; i < node->subtour[node->chosen].size() - 1; i++){//iterar por todos os arcos do subtour escolhido
			//for(int i = 0; i < 3 - 1; i++){//iterar por todos os arcos do subtour escolhido
				printNode(*node);

				cout << "No adicionado\n";
				Node n;//criar no auxiliar

				n.forbidden_arcs = node->forbidden_arcs;//herdar os arcos passados

				pair<int, int> forbidden_arc = {//proibir um arco novo
					node->subtour[node->chosen][i],
					node->subtour[node->chosen][i+1]
				};

				n.forbidden_arcs.push_back(forbidden_arc);
				tree.push_back(n);//inserir novos nos na arvore
			}
		}

		tree.erase(node);
	}
}

int main(int argc, char** argv){
	Data * data = new Data(argc, argv[1]);
	data->readData();
	int index;

	double **cost = new double*[data->getDimension()];
	for (int i = 0; i < data->getDimension(); i++){
		cost[i] = new double[data->getDimension()];
		for (int j = 0; j < data->getDimension(); j++){
			cost[i][j] = data->getDistance(i,j);
		}
	}

	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost, data->getDimension(), data->getDimension(), mode); // Carregando o problema

	//double obj_value = hungarian_solve(&p);
	/*cout << "Obj. value: " << obj_value << endl;

	cout << "Assignment" << endl;
	hungarian_print_assignment(&p);*/

	list<int> lista;

	lista.push_back(1);
	auto x = lista.begin();

	for(int i = 2; i < 10; i++){
		lista.push_back(i);
	}

	cout << "P: " << *x << endl;

	for(auto it = lista.begin(); it != lista.end(); it++){
		cout << *it << " ";
	}cout << endl;

	//branchAndBound(p, data->getDimension(), *data);

	//vector<vector<int>> subtour;

	//preencherSubtour(p, subtour, data->getDimension());

	/*for(int i = 0; i < subtour.size(); i++){
		printarSubtour(subtour[i]);
	}*/

	//index = menorSubtourIndex(subtour);
	
	hungarian_free(&p);
	for (int i = 0; i < data->getDimension(); i++) delete [] cost[i];
	delete [] cost;
	delete data;

	return 0;
}
