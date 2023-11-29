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

struct Subsequencia{
    double T, C;
    int W;
    int first, last; //primeiro e ultimo nos da subsequencia
    inline static Subsequencia Concatenate(Subsequencia& sigma1, Subsequencia& sigma2, Data& data){
        Subsequencia sigma;
        double temp = data.getDistance(sigma1.last, sigma2.first);
        sigma.W = sigma1.W + sigma2.W;
        sigma.T = sigma1.T + temp + sigma2.T;
        sigma.C = sigma1.C + sigma2.W * (sigma1.T + temp) + sigma2.C;
        sigma.first = sigma1.first;
        sigma.last = sigma2.last;

        return sigma;
    }
};

void UpdateAllSubseq(Solucao& s, vector<vector<Subsequencia>>& subseqMatrix, Data& data){
    int n = s.sequencia.size();

    //subsequencias de um unico no
    for(int i = 0; i < n; i++){
        int v = s.sequencia[i];
        subseqMatrix[i][i].W = (i > 0);
        subseqMatrix[i][i].C = 0;
        subseqMatrix[i][i].T = 0;
        subseqMatrix[i][i].first = s.sequencia[i];
        subseqMatrix[i][i].last = s.sequencia[i];
    }
    
    /*//teste
    subseqMatrix[0][1] = Subsequencia::Concatenate(subseqMatrix[0][0], subseqMatrix[1][1], data);*/

    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            //cout << "Entrei aqui\n";
            subseqMatrix[i][j] = Subsequencia::Concatenate(subseqMatrix[i][j-1], subseqMatrix[j][j], data);
            //cout << "Concatenei uma subsequencia\n";
        }
    }

    //subsequencias invertidas
    //(necessarias para o 2-opt)
    for(int i = n - 1; i >= 0; i--){
        for(int j = i - 1; j >= 0; j--){
            subseqMatrix[i][j] = Subsequencia::Concatenate(subseqMatrix[i][j+1], subseqMatrix[j][j], data);
        }
    }
}

void mostrarSubsequencia(Subsequencia& subsequencia){
    cout << "Tempo Total: " << subsequencia.T << endl;
    cout << "Custo Acumulado: " << subsequencia.C << endl;
    cout << "Custo de Atraso: " << subsequencia.W << endl;
}

vector<int> escolher1NoAleatorio(int& tamanho){
    vector<int> s(1);

    s[0] =  rand() % tamanho + 1;

    return s;
}

vector<int> nosRestantes(vector<int>& sequencia, int& tamanho){//fiz especificamente para 4 membros
    vector<int> CL(tamanho - 1);//mudei a inicializacao
    int cont = 0;

    for(int i = 1; i <= tamanho; i++){
        if(i != sequencia[0]){
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

void ordenarEmOrdemCrescente(vector<InsertionInfo>& custoInsercao){//bubble sort
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

void troca(int& a, int& b){
    int aux;

    aux = a;
    a = b;
    b = aux;
}

int partition(vector<int>& CL, int inicio, int fim, int noAvaliado, Data& data){//fim e inicio sao indices
    int pivo = data.getDistance(CL[fim], noAvaliado);
    int i = inicio;
    int j; 

    for(j = inicio; j < fim; j++){
        if(data.getDistance(CL[j], noAvaliado) <= pivo){
            troca(CL[j], CL[i]);
            i++;
        }
    }
    troca(CL[j], CL[i]);

    return i;
}

void quickSort(vector<int>& CL, int inicio, int fim, int noAvaliado, Data& data){
    int pivo;

    if(inicio < fim){
        pivo = partition(CL, inicio, fim, noAvaliado, data);
        quickSort(CL, inicio, pivo-1, noAvaliado, data);
        quickSort(CL, pivo+1, fim, noAvaliado, data);
    }
}

bool maisLonge(int no1, int no2, int noAvaliado, Data& data){
    return (data.getDistance(no1, noAvaliado) < data.getDistance(no2, noAvaliado));
}

void inserirNaSolucao(Solucao& s, vector<int>& CL, int& noInserido){
    //inserir na solucao s
    s.sequencia.push_back(noInserido);
    
    //tirei a atualizacao do custo da sequencia s devido talvez n ser mais importante;
    //tirar o no de CL
    auto fim = CL.end();
    for(auto it = CL.begin(); it != fim; it++){
        if(*it == noInserido){
            CL.erase(it);    
            break;
        }
    }
    //usando as informacoes do custoInsercao
}

Solucao Construcao(int& dimension, Data& data){
    Solucao s = {{}, 0.0};


    s.sequencia = escolher1NoAleatorio(dimension);
    
    //calcularValorObj(s, data);

    vector<int> CL = nosRestantes(s.sequencia, dimension);

    //printarSequencia(s.sequencia);
    //printarSequencia(CL);

    int selecionado, limite, noAvaliado;
    double alpha;

    while(!CL.empty()){
        noAvaliado = s.sequencia[s.sequencia.size()-1];

        quickSort(CL, 0, CL.size()-1, noAvaliado, data);

        alpha = (double)(rand() % RAND_MAX + 1) / RAND_MAX;//numero aleatorio de 0 a 1
        alpha /= 4;

        limite = (int)(alpha * CL.size());
        
        if(limite == 0){
            selecionado = 0;
        }else{
            selecionado = rand() % limite;
        }

        inserirNaSolucao(s, CL, CL[selecionado]);
    }

    s.sequencia.push_back(s.sequencia[0]);//fechar o ciclo

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

Solucao Perturbacao(Solucao s, vector<vector<Subsequencia>>& subseqMatrix, Data& data, int dimension){
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

    Subsequencia subAux, sub1;

    if(i < j){
        subAux = subseqMatrix[0][i-1];
        sub1 = subseqMatrix[j][j+tamanho2-1];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);

        if(j - (i+tamanho1-1) != 1){//elas nao estao coladas
            sub1 = subseqMatrix[i+tamanho1][j-1];
            subAux = Subsequencia::Concatenate(subAux, sub1, data);
        }

        sub1 = subseqMatrix[i][i+tamanho1-1];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);
        sub1 = subseqMatrix[j+tamanho2][dimension];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);
        

        s.sequencia.insert(s.sequencia.begin() + j, s.sequencia.begin() + i, s.sequencia.begin() + i+tamanho1);//coloco no j
        s.sequencia.erase(s.sequencia.begin() + i, s.sequencia.begin() + i+tamanho1);//apago o i
        s.sequencia.insert(s.sequencia.begin() + i, s.sequencia.begin() + j+tamanho2, s.sequencia.begin() + j+tamanho2+tamanho2);//coloco no i//n sei explicar o pq do tamanho2 ai
        s.sequencia.erase(s.sequencia.begin() + j+tamanho2, s.sequencia.begin() + j+tamanho2+tamanho2);//apago no j
    }else if(i > j){
        subAux = subseqMatrix[0][j-1];
        sub1 = subseqMatrix[i][i+tamanho1-1];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);

        if(i - (j+tamanho2-1) != 1){//elas nao estao coladas
            sub1 = subseqMatrix[j+tamanho2][i-1];
            subAux = Subsequencia::Concatenate(subAux, sub1, data);
        }

        sub1 = subseqMatrix[j][j+tamanho2-1];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);
        sub1 = subseqMatrix[i+tamanho1][dimension];
        subAux = Subsequencia::Concatenate(subAux, sub1, data);
        
        s.sequencia.insert(s.sequencia.begin() + i, s.sequencia.begin() + j, s.sequencia.begin() + j+tamanho2);
        s.sequencia.erase(s.sequencia.begin() + j, s.sequencia.begin() + j+tamanho2);
        s.sequencia.insert(s.sequencia.begin() + j, s.sequencia.begin() + i+tamanho1, s.sequencia.begin() + i+tamanho1+tamanho1);
        s.sequencia.erase(s.sequencia.begin() + i+tamanho1, s.sequencia.begin() + i+tamanho1+tamanho1);
    } 

    s.valorObj = subAux.C;

    return s;
}

Solucao ILS(int maxIter, int maxIterIls, int& dimension, Data& data){
    Solucao bestOfAll;
    bestOfAll.valorObj = INFINITY;

    int iterIls = 0;

    for(int i = 0; i < maxIter; i++){
        Solucao s = Construcao(dimension, data);//crei uma solucao inicial e defini o custo acumulado dela
        calcularCustoAcumulado(s, data);

        vector<vector<Subsequencia>> subseqMatrix(s.sequencia.size(), vector<Subsequencia>(s.sequencia.size()));

        UpdateAllSubseq(s, subseqMatrix, data);

        printarSequencia(s.sequencia);
        cout << s.valorObj << endl << endl;

        Solucao best = s;

        iterIls = 0;

        while(iterIls <= maxIterIls){
            /*BuscaLocal(s, data);

            if(s.valorObj < best.valorObj){
                best = s;
                iterIls = 0;
            }*/

            s = Perturbacao(best, subseqMatrix, data, dimension);

            /*printarSequencia(s.sequencia);
            cout << s.valorObj << endl;
            calcularCustoAcumulado(s, data);
            cout << s.valorObj << endl << endl;

            scanf("%*c");*/
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
    int maxIter = 10, maxIterILS;
    clock_t start, end;
    double tempoTotal = 0, valorTotal = 0;

    if(dimension >= 100){
        maxIterILS = 100;
    }else{
        maxIterILS = dimension;
    }

    for(int i = 0; i < 10; i++){
        start = clock();
        Solucao s = ILS(maxIter, maxIterILS, dimension, data);
        end = clock();

        tempoTotal += (double)(end - start) / (double)(CLOCKS_PER_SEC);
        valorTotal += s.valorObj;
    }

    cout << tempoTotal/10 << " " << valorTotal/10 << endl;
    //printf("%.3lf %.2lf\n", tempoTotal/10, valorTotal/10);
    

    return 0;
}

/*int i;
    Solucao s = Construcao(dimension, data);

    calcularValorObj(s, data);
    double custo = s.valorObj;
    calcularCustoAcumulado(s, data);
    

    printarSequencia(s.sequencia);
    printf("Custo Acumulado: %.2lf\nTempo Total: %.2lf\n\n", s.valorObj, custo);
    //cout << s.valorObj << endl;

    vector<vector<Subsequencia>> subseqMatrix(s.sequencia.size(), vector<Subsequencia>(s.sequencia.size()));

    UpdateAllSubseq(s, subseqMatrix, data);

    int a = 0, b = 0;
    while(1){//procurando por indice comecando em 1
        cout << "Digite o indice inicial da subsequencia: ";
        cin >> a;

        if(a == 0){
            break;
        }

        cout << "Digite o indice final da subsequencia: ";
        cin >> b;

        mostrarSubsequencia(subseqMatrix[a-1][b-1]);
    }

    /*for(i = 0; i < 4; i++){
        printf("%d - %d: %.2lf%s", s.sequencia[i], s.sequencia[i+1], data.getDistance(s.sequencia[i], s.sequencia[i+1]), (i == 3 ? "\n" : " | "));
    }*/


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*Solucao s1, s2;

for(i = 1; i <= 21; i++){
    s1.sequencia.push_back(i);
}
for(i = 22; i <= dimension; i++){
    s2.sequencia.push_back(i);
}

s2.sequencia.push_back(1);
calcularCustoAcumulado(s1, data);
calcularCustoAcumulado(s2, data);

printarSequencia(s1.sequencia);
printarSequencia(s2.sequencia);

double delta = 0;

for(i = 0; i < s1.sequencia.size()-1; i++){
    delta += data.getDistance(s1.sequencia[i], s1.sequencia[i+1]);
}
delta += data.getDistance(s1.sequencia[i], s2.sequencia[0]);

printf("%.2lf\n\n", (s1.valorObj + s2.valorObj + (delta * s2.sequencia.size())));*/
