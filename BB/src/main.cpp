#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

#include "data.h"
#include "hungarian.h"

struct Node{
	vector<pair<int, int>> forbidden_arcs;//lista de arcos proibidos do no
	vector<vector<int>>& subtour;//conjunto de subtours da solucao
	double lower_bound;//custo total da solucao do algoritmo hungaro
	int chosen;//indice do menor subtour
	bool feasible;//indica se a solucao do AP_TSp e viavel
};

void printarSubtour(vector<int>& subtour){
	for(int i = 0; i < subtour.size(); i++){
		cout << subtour[i] << " ";
	}cout << endl;
}

void preencherSubtour(hungarian_problem_t& p, vector<vector<int>>& subtour, int tamanho){
	int i, index;
	vector<bool> passou(tamanho, 0);
	bool acabou = 0;

	while(!acabou){//debugar
		/*for(int i = 0; i < passou.size(); i++){
			cout << passou[i] << " ";
		}cout << endl;*/

		vector<int> aux;
		for(i = 0; i < tamanho; i++){//so para achar o primeiro que ele ainda n passou
			if(passou[i] == 0){//se ele ainda n passou por ele vou colocar na sequencia
				passou[i] = 1;
				aux.push_back(i+1);

				do{
					//printarSubtour(aux);
					for(index = 0; index < tamanho; index++){
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
		for(i = 0; i < tamanho; i++){
			if(passou[i] == 0){
				acabou = 0;
				break;
			}
		}
	}
}

int main(int argc, char** argv) {

	Data * data = new Data(argc, argv[1]);
	data->readData();

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

	double obj_value = hungarian_solve(&p);
	cout << "Obj. value: " << obj_value << endl;

	cout << "Assignment" << endl;
	hungarian_print_assignment(&p);

	vector<vector<int>> subtour;

	preencherSubtour(p, subtour, data->getDimension());

	/*for(int i = 0; i < subtour.size(); i++){
		printarSubtour(subtour[i]);
	}*/

	hungarian_free(&p);
	for (int i = 0; i < data->getDimension(); i++) delete [] cost[i];
	delete [] cost;
	delete data;

	return 0;
}
