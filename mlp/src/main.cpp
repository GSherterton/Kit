#include "Data.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

//o programa esta com o custo atualizando em cada funcao

double tempo = 0;

typedef struct Solucao{
    vector<int> sequencia;
    double valorObj;
}Solucao;

struct InsertionInfo{
    int noInserido; // no k a ser inserido
    int arestaRemovida; // aresta {i,j} na qual o no k sera inserido
    double custo; // delta ao inserir k na aresta {i,j}
};

vector<int> escolher4NosAleatorios(int& tamanho){
    vector<int> s(5);//mudando a inicializacao
    int num[4];
    bool achou;
    
    //srand(time(NULL));
    
    for(int i = 0; i < 4; i++){
        num[i] = rand() % tamanho + 1;
        
        achou = 1;
        while(achou){
            achou = 0;
            
            for(int j = 0; j < i; j++){
                if(num[i] == num[j]){
                    num[i] = rand() % tamanho + 1;    
                    achou = 1;
                    break;
                }
            }
        }
    }
    
    s[0] = num[0];
    s[1] = num[1];
    s[2] = num[2];
    s[3] = num[3];
    s[4] = num[0];

    /*
    s.push_back(num[0]);
    s.push_back(num[1]);
    s.push_back(num[2]);
    s.push_back(num[3]);
    s.push_back(num[0]);
    */

    return s;
}

vector<int> nosRestantes(vector<int>& sequencia, int& tamanho){//fiz especificamente para 4 membros
    vector<int> CL(tamanho - (sequencia.size() - 1));//mudei a inicializacao
    int cont = 0;

    for(int i = 1; i <= tamanho; i++){
        if((i != sequencia[0]) && (i != sequencia[1]) && (i != sequencia[2]) && (i != sequencia[3])){
            CL[cont++] = i;
        }
    }

    return CL;
}

void printarSequencia(vector<int>& sequencia){
    cout << "Sequencia debug: ";
    for(int x : sequencia){
        cout << x << " ";
    }
    puts("");
}

void printarCustoInsercao(vector<InsertionInfo>& custoInsercao){
    int i = 0;
    for(auto x : custoInsercao){
        printf("%d - %d %d %.2lf\n", i++, x.noInserido, x.arestaRemovida, x.custo);
    }
}

void calcularCustoAcumulado(Solucao& s, Data& data){
    size_t tamanhoS = s.sequencia.size() - 1;
    s.valorObj = 0;
    double aux = 0;

    for(int i = 0; i < tamanhoS; i++){
        aux += data.getDistance(s.sequencia[i], s.sequencia[i+1]);
        s.valorObj += aux;
    }
}

void calcularValorObj(Solucao& s, Data& data){
    size_t tamanhoS = s.sequencia.size() - 1;
    s.valorObj = 0;

    for(int i = 0; i < tamanhoS; i++){
        s.valorObj += data.getDistance(s.sequencia[i], s.sequencia[i+1]);
    }
}

vector<InsertionInfo> calcularCustoInsercao(Solucao& s, vector<int>& CL, Data& data){
    size_t tamanhoS = (s.sequencia.size() - 1);
    vector<InsertionInfo> custoInsercao(tamanhoS * CL.size());
    int l = 0;
    int a, i, j;

    for(a = 0; a < tamanhoS; a++){//percorrer todas as arestas
        i = s.sequencia[a];
        j = s.sequencia[a+1];
        for(auto k : CL){//percorrer todos os nos
            custoInsercao[l].custo = data.getDistance(i, k) + data.getDistance(j, k) - data.getDistance(i, j);
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }

    return custoInsercao;
}

void ordenarEmOrdemCrescente(vector<InsertionInfo>& custoInsercao){
    bool trocou = 1;
    int tamanhoCusto = custoInsercao.size() - 1;
    InsertionInfo aux;

    while(trocou){
        trocou = 0;

        for(int i = 0; i < tamanhoCusto; i++){
            if(custoInsercao[i].custo > custoInsercao[i+1].custo){
                aux = custoInsercao[i];
                custoInsercao[i] = custoInsercao[i+1];
                custoInsercao[i+1] = aux;

                trocou = 1;
            }
        }
    }

    return;
}

bool maisCaro(InsertionInfo custo1, InsertionInfo custo2){
    return (custo1.custo < custo2.custo);
}

void inserirNaSolucao(Solucao& s, vector<int>& CL, InsertionInfo& custoInsercao){
    //inserir na solucao s
    s.sequencia.insert(s.sequencia.begin() + custoInsercao.arestaRemovida + 1, custoInsercao.noInserido);
    s.valorObj += custoInsercao.custo;
    //tirar o no de CL
    auto fim = CL.end();
    for(auto it = CL.begin(); it != fim; it++){
        if(*it == custoInsercao.noInserido){
            CL.erase(it);    
            break;
        }
    }
    //usando as informacoes do custoInsercao
}

Solucao Construcao(int& dimension, Data& data){
    Solucao s = {{}, 0.0};


    s.sequencia = escolher4NosAleatorios(dimension);
    
    calcularValorObj(s, data);

    vector<int> CL = nosRestantes(s.sequencia, dimension);

    int selecionado, limite;
    double alpha;

    while(!CL.empty()){
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL, data);

        sort(custoInsercao.begin(), custoInsercao.end(), maisCaro);//quick sort

        alpha = (double)(rand() % RAND_MAX + 1) / RAND_MAX;//numero aleatorio de 0 a 1

        limite = (int)(alpha * custoInsercao.size());
        
        if(limite == 0){
            selecionado = 0;
        }else{
            selecionado = rand() % limite;
        }

        inserirNaSolucao(s, CL, custoInsercao[selecionado]);
    }

    return s;
}

void troca(vector<int>& sequencia, int& i, int& j){
    int aux;

    aux = sequencia[i];
    sequencia[i] = sequencia[j];
    sequencia[j] = aux;
}

bool bestImprovementSwap(Solucao& s, Data& data){
    double bestDelta = 0;
    int best_i, best_j, tamanhoS = s.sequencia.size() - 1;

    int vi, vi_next, vi_prev, vj, vj_next, vj_prev;
    double delta;
    int i, j;

    for(i = 1; i < tamanhoS; i++){
        vi = s.sequencia[i];
        vi_next = s.sequencia[i + 1];
        vi_prev = s.sequencia[i - 1];

        for(j = i + 1; j < tamanhoS; j++){
            vj = s.sequencia[j];
            vj_next = s.sequencia[j + 1];
            vj_prev = s.sequencia[j - 1];

            if(j-i == 1){
                delta = -data.getDistance(vi_prev,vi) -data.getDistance(vj,vj_next)
                + data.getDistance(vi_prev,vj) + data.getDistance(vi,vj_next);
            }else{
                delta = -data.getDistance(vi_prev,vi) -data.getDistance(vi,vi_next) + data.getDistance(vi_prev,vj)
                + data.getDistance(vj,vi_next) - data.getDistance(vj_prev,vj) - data.getDistance(vj,vj_next)
                + data.getDistance(vj_prev,vi) + data.getDistance(vi,vj_next);
            }

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(bestDelta < 0){
        troca(s.sequencia, best_i, best_j);
        s.valorObj += bestDelta;
        return true;
    }

    return false;
}

void inverte(vector<int>& sequencia, int& i, int& j){
    vector<int> vi;
    int x;
    for(x = j; x > i; x--){
        vi.push_back(sequencia[x]);
    }

    sequencia.erase(sequencia.begin() + i + 1, sequencia.begin() + j + 1);
    sequencia.insert(sequencia.begin() + i + 1, vi.begin(), vi.end());
}

bool bestImprovement2Opt(Solucao& s, Data& data){
    double bestDelta = 0;
    int best_i, best_j, tamanhoS = s.sequencia.size() - 1;

    int ai_prev, ai_next, aj_prev, aj_next;
    double delta;
    int i, j;

    for(i = 0; i < tamanhoS; i++){
        ai_prev = s.sequencia[i];
        ai_next = s.sequencia[i + 1];

        for(j = i + 2; j < tamanhoS; j++){
            if(i == 0 && j == tamanhoS - 1){
                continue;
            }
            aj_prev = s.sequencia[j];
            aj_next = s.sequencia[j + 1];
            delta = -data.getDistance(ai_prev,ai_next) -data.getDistance(aj_prev,aj_next)
            + data.getDistance(ai_prev,aj_prev) + data.getDistance(ai_next,aj_next);

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(bestDelta < 0){
        inverte(s.sequencia, best_i, best_j);
        s.valorObj += bestDelta;
        return true;
    }

    return false;
}

void insercao(vector<int>& sequencia, int& i, int& j, int& n){
    vector<int> vi;
    vi.insert(vi.begin(), sequencia.begin() + i, sequencia.begin() + i + n);

    if(i < j){
        sequencia.insert(sequencia.begin() + j+1, vi.begin(), vi.end());
        sequencia.erase(sequencia.begin() + i, sequencia.begin() + i + n);
    }else if(i > j){
        sequencia.erase(sequencia.begin() + i, sequencia.begin() + i + n);
        sequencia.insert(sequencia.begin() + j+1, vi.begin(), vi.end());
    }
}

bool bestImprovementOrOpt(Solucao& s, Data& data, int n){
    double bestDelta = 0;
    int best_i, best_j, tamanhoS = s.sequencia.size() - n;

    int vi, vi_fim, vi_next, vi_prev;
    int vj_next, vj_prev;
    double delta;
    int i, aj;

    for(i = 1; i < tamanhoS; i++){
        vi = s.sequencia[i];
        vi_fim = s.sequencia[i + n - 1];
        vi_next = s.sequencia[i + n];
        vi_prev = s.sequencia[i - 1];

        for(aj = 0; aj < tamanhoS; aj++){
            if(aj >= i-1 && aj <= i+n-1){
                continue;
            }
            vj_next = s.sequencia[aj + 1];
            vj_prev = s.sequencia[aj];
            delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi_fim, vi_next)
            + data.getDistance(vi_prev, vi_next) - data.getDistance(vj_prev, vj_next)
            + data.getDistance(vj_prev, vi) + data.getDistance(vi_fim, vj_next);

            if(delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = aj;
            }
        }
    }

    if(bestDelta < 0){
        insercao(s.sequencia, best_i, best_j, n);
        s.valorObj += bestDelta;
        return true;
    }


    return false;
}

void BuscaLocal(Solucao& s, Data& data){
    vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;
    int n;

    while (NL.empty() == false){
        n = rand() % NL.size();
        switch (NL[n]){
            case 1:
                improved = bestImprovementSwap(s, data);
                break;
            case 2:
                improved = bestImprovement2Opt(s, data);
                break;
            case 3:
                improved = bestImprovementOrOpt(s, data, 1); // Reinsertion
                break;
            case 4:
                improved = bestImprovementOrOpt(s, data, 2); // Or-opt2
                break;
            case 5:
                improved = bestImprovementOrOpt(s, data, 3); // Or-opt3
                break;
            }

            if(improved){
                NL = {1, 2, 3, 4, 5};
            }else{
                NL.erase(NL.begin() + n);
            }
    }
}

Solucao Perturbacao(Solucao s, Data& data, int dimension){
    int tamanho1 = 1, tamanho2 = 1, i = 0, j = 0;

    while(1){
        if((((i + tamanho1 - 1) < j) || ((j + tamanho2 - 1) < i)) && i+tamanho1-1 < dimension && j+tamanho2-1 < dimension){
            break;
        }
        tamanho1 = rand() % ((int)ceil(dimension/10)) + 2;
        tamanho2 = rand() % ((int)ceil(dimension/10)) + 2;
        i = rand() % (dimension-2) + 1;
        j = rand() % (dimension-2) + 1; 
    }
    
    int vi = s.sequencia[i];
    int vi_fim = s.sequencia[i+tamanho1-1];
    int vi_next = s.sequencia[i+tamanho1];
    int vi_prev = s.sequencia[i-1];

    int vj = s.sequencia[j];
    int vj_fim = s.sequencia[j+tamanho2-1];
    int vj_next = s.sequencia[j+tamanho2];
    int vj_prev = s.sequencia[j-1];

    if(j - (i+tamanho1-1) == 1){
        s.valorObj += - data.getDistance(vi_prev, vi) - data.getDistance(vj_prev, vj) - data.getDistance(vj_fim, vj_next)
                      + data.getDistance(vi_prev, vj) + data.getDistance(vj_fim, vi) + data.getDistance(vi_fim, vj_next);
    }else if(i - (j+tamanho2-1) == 1){
        s.valorObj += - data.getDistance(vi_prev, vi) - data.getDistance(vj_prev, vj) - data.getDistance(vi_fim, vi_next)
                      + data.getDistance(vj_prev, vi) + data.getDistance(vi_fim, vj) + data.getDistance(vj_fim, vi_next);
    }else{
        s.valorObj += - data.getDistance(vi_prev, vi) - data.getDistance(vi_fim, vi_next) - data.getDistance(vj_prev, vj) - data.getDistance(vj_fim, vj_next)
                      + data.getDistance(vi_prev, vj) + data.getDistance(vj_fim, vi_next) + data.getDistance(vj_prev, vi) + data.getDistance(vi_fim, vj_next); 
    }

    if(i < j){
        s.sequencia.insert(s.sequencia.begin() + j, s.sequencia.begin() + i, s.sequencia.begin() + i+tamanho1);//coloco no j
        s.sequencia.erase(s.sequencia.begin() + i, s.sequencia.begin() + i+tamanho1);//apago o i
        s.sequencia.insert(s.sequencia.begin() + i, s.sequencia.begin() + j+tamanho2, s.sequencia.begin() + j+tamanho2+tamanho2);//coloco no i//n sei explicar o pq do tamanho2 ai
        s.sequencia.erase(s.sequencia.begin() + j+tamanho2, s.sequencia.begin() + j+tamanho2+tamanho2);//apago no j
    }else if(i > j){
        s.sequencia.insert(s.sequencia.begin() + i, s.sequencia.begin() + j, s.sequencia.begin() + j+tamanho2);
        s.sequencia.erase(s.sequencia.begin() + j, s.sequencia.begin() + j+tamanho2);
        s.sequencia.insert(s.sequencia.begin() + j, s.sequencia.begin() + i+tamanho1, s.sequencia.begin() + i+tamanho1+tamanho1);
        s.sequencia.erase(s.sequencia.begin() + i+tamanho1, s.sequencia.begin() + i+tamanho1+tamanho1);
    } 

    return s;
}

Solucao ILS(int maxIter, int maxIterIls, int& dimension, Data& data){
    Solucao bestOfAll;
    bestOfAll.valorObj = INFINITY;

    int iterIls = 0;

    for(int i = 0; i < maxIter; i++){
        Solucao s = Construcao(dimension, data);
        
        Solucao best = s;

        iterIls = 0;

        while(iterIls <= maxIterIls){
            BuscaLocal(s, data);

            if(s.valorObj < best.valorObj){
                best = s;
                iterIls = 0;
            }

            s = Perturbacao(best, data, dimension);
            
            iterIls++;
        }
        if (best.valorObj < bestOfAll.valorObj){
            bestOfAll = best;
        }
    }

    return bestOfAll;
}

int main(int argc, char** argv) {
    srand(time(NULL));

    auto data = Data(argc, argv[1]);
    data.read();
    int dimension = data.getDimension();//quantidade de cidades
    /*int maxIter = 50, maxIterILS;
    clock_t start, end;
    double tempoTotal = 0, valorTotal = 0;

    if(n >= 150){
        maxIterILS = dimension/2;
    }else{
        maxIterILS = dimension;
    }

    for(int i = 0; i < 10; i++){
        start = clock();
        Solucao s = ILS(maxIter, maxIterILS, n, data);
        end = clock();

        tempoTotal += (double)(end - start) / (double)(CLOCKS_PER_SEC);
        valorTotal += s.valorObj;
    }

    cout << tempoTotal/10 << " " << valorTotal/10 << endl;
    //printf("%.3lf %.2lf\n", tempoTotal/10, valorTotal/10);
    */

    Solucao s;

    for(int i = 1; i <= 6; i++){
        s.sequencia.push_back(i);
    }
    s.sequencia.push_back(1);
    calcularCustoAcumulado(s, data);

    printarSequencia(s.sequencia);
    printf("%.2lf\n\n", s.valorObj);
    //cout << s.valorObj << endl;

    Solucao s1, s2;

    for(int i = 1; i <= 4; i++){
        s1.sequencia.push_back(i);
    }
    for(int i = 5; i <= 6; i++){
        s2.sequencia.push_back(i);
    }
    s2.sequencia.push_back(1);
    calcularCustoAcumulado(s1, data);
    calcularCustoAcumulado(s2, data);

    double delta = 0;

    int i;
    for(i = 0; i < 3; i++){
        delta += data.getDistance(s1.sequencia[i], s1.sequencia[i+1]);
    }
    delta += data.getDistance(s1.sequencia[i], s2.sequencia[0]);

    printarSequencia(s1.sequencia);
    printarSequencia(s2.sequencia);
    printf("Delta: %.2lf | tamanho2: %d\n", delta, s2.sequencia.size());
    printf("%.2lf\n\n", (s1.valorObj + s2.valorObj + (delta * s2.sequencia.size())));

    for(int j = 0; j < 6; j++){
        printf("%d - %d : %.2lf%s", s.sequencia[j], s.sequencia[j+1], data.getDistance(s.sequencia[j], s.sequencia[j+1]), j == 5 ? "\n" : " | ");
    }

    return 0;
}
