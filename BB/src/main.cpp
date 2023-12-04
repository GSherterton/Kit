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
	int menor = subtour[0].size();

	for(int i = 1; i < subtour.size(); i++){
		if(subtour[i].size() < menor){
			menor = subtour[i].size();
		}
	}

	return menor;
}

void iniciarNo(Node node, hungarian_problem_t& p, int dimension){
	node.lower_bound = hungarian_solve(&p);
	preencherSubtour(p, node.subtour, dimension);

	node.chosen = menorSubtourIndex(node.subtour);
	
	if(node.subtour.size() > 1){
		node.feasible = 0;
	}else{
		node.feasible = 1;
	}
}

void branchAndBound(hungarian_problem_t& p, int dimension){
	Node root;
	iniciarNo(root, p, dimension);//fiz so ate a parte de achar os subtours e preencher o primeiro node

	list<Node> tree;
	tree.push_back(root);

	double upper_bound = numeric_limits<double>::infinity();

	/*while(!tree.empty()){
		auto node = branchingStrategy();
	}*/
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

	branchAndBound(p, data->getDimension());

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
