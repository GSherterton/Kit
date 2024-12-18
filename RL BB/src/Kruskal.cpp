#include "Kruskal.h"

//changing the algorithm to disconsider the vertex 0
Kruskal::Kruskal(vvi dist){
	//basicamente é só iniciar os dois for em 1 em vez de 0
	for(int i = 1; i < dist.size(); ++i){
		for(int j = 1; j < dist[i].size(); ++j){
			graph.push( make_pair(-dist[i][j], make_pair(i-1, j-1)) );
		}	
	}
}

void Kruskal::initDisjoint(int n){
	pset.resize(n);
	for (int i = 0; i < n; ++i){
		pset[i] = i;
	}
}

int Kruskal::findSet(int i){
	return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));
}

void Kruskal::unionSet(int i, int j){
	pset[findSet(i)] = findSet(j);
}

bool Kruskal::isSameSet(int i, int j){
	return (findSet(i) == findSet(j))? true:false;
}

vii Kruskal::getEdges(){
	return edges;
}

double Kruskal::MST(int nodes){
	initDisjoint(nodes);

	double cost = 0;

	while(!graph.empty()){
		pair<double, ii> p = graph.top();
		graph.pop();

		if(!isSameSet(p.second.first, p.second.second)){
			edges.push_back(make_pair(p.second.first+1, p.second.second+1)); //alterando para poder pegar os indices correspondentes certos
			cost += (-p.first);
			unionSet(p.second.first, p.second.second);
		}
	}

	return cost;
}