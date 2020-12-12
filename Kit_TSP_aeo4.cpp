/* 10-11-2020 Andersoneo: Esta 3ª versão mescla meus primeiros desenvolvimento que chegavam apenas a 2% do resultado ótimo,
com o desenvolvimento feito pelo Chico, colega da área de TI da UFPB.
Basicamente uso sua função "reinsertion" ao invés do meu emaranhado de código inicial.
*/
/* Anderson Ernani de Oliveira - Implementação do Kit Abordagem meta-heur´ıstica para o TSP
referente ao artigo do Prof. Anand */
/* classes leitura das instâncias */
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <sstream>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <limits>
/* classes método construção */
#include <vector>
/* Biblioteca de funções auxiliares*/
#include "biblio.h"




using namespace std;

enum NL { N1, N2, N3, N4, N5 };

void readData(int, string, int*, double ***);
double CalcDistEuc(double *X, double *Y, int I, int J);
double CalcDistAtt(double *X, double *Y, int I, int J);
void CalcLatLong(double *X, double *Y, int n, double *latit, double* longit);
double CalcDistGeo(double *latit, double *longit, int I, int J);
void printData();

void construction(vector<int> &s, double **c); //procedimento de construção - que inicia com um subtour

void printS(vector<int> &s);
void printM_viz(int m_viz[5][1000], int tamanho);

/*procedure para preencher a matriz vizinhança*/
int PreencheVizinhanca(vector<int> &novo_vetor_s, int m_viz[5][1000], bool teste_manual, int total_inserido, int num_vertices, int p, int comp_m_viz);

//10-11-2020 Andersoneo: função copiada de Chico
vector<int> reinsertion(vector<int> s, double *bestDelta, int subsegSize);

vector<int> RVND2(vector<int> s, double *mainCost);
void Permutacao(vector<int> &s1, vector<int> &s2); //pertubation , pertubação

double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices
FILE *file;




int main()
{
	/* !!!!!
	ATENCAO: O MODELO DE DADOS A SER TESTADO DEVE TER O NOME DO ARQUIVO TXT INFORMADO NO readData E O NÚMERO DE VÉRTICE INFORMADO
	NA VARIAVEL dimension ABAIXO !!!!!!!!
	*/
	dimension = 51; //número de vértices |V|
	//readData(2, "Aeo6.tsp", &dimension, &matrizAdj); 
	//readData(2, "bayg29.tsp", &dimension, &matrizAdj);
	//readData(2, "burma14.tsp", &dimension, &matrizAdj); //melhor solução 3323
	//readData(2, "fri26.tsp", &dimension, &matrizAdj); //melhor solução 937
	readData(2, "eil51.tsp", &dimension, &matrizAdj); //melhor solução 426 
	//readData(2, "bayg29.tsp", &dimension, &matrizAdj); //melhor solução 1610
	//readData(2, "bays29.tsp", &dimension, &matrizAdj); //melhor solução 2020
	//readData(2, "dantzig42.tsp", &dimension, &matrizAdj); //melhor solução 699
// neste versão vetor máximo 1000	readData(2, "fl1400.tsp", &dimension, &matrizAdj); //melhor solução 5046
//	readData(2, "gr17.tsp", &dimension, &matrizAdj); //melhor solução 2085
	//readData(2, "gr21.tsp", &dimension, &matrizAdj); //melhor solução 2707
	//readData(2, "gr24.tsp", &dimension, &matrizAdj); //melhor solução 1272
	//readData(2, "gr48.tsp", &dimension, &matrizAdj); //melhor solução 5046
	//readData(2, "brazil58.tsp", &dimension, &matrizAdj); //melhor solução 49135
	//readData(2, "berlin52.tsp", &dimension, &matrizAdj); //melhor solução 7542
	//readData(2, "ulysses16.tsp", &dimension, &matrizAdj); //melhor solução 6859
	//readData(2, "ulysses22.tsp", &dimension, &matrizAdj); //melhor solução 7013
	//impressão do matriz / grafo após leitura do arquivo
	//printData();
	auto timerStart = chrono::system_clock::now();
	//Início --> Procedure GILS-RVND (IMax, IILS, R)
	int IMax = 50; //valor recomendado 50
	int IILS; /*valores recomendados:  |V|/2 se |V| >= 150 ou |V|*/
	if (dimension >= 150)
		IILS = dimension / 2;
	else
		IILS = dimension;
	// outra forma de se escrever o if acima: IILS = (dimension >= 150) ? (dimension / 2) : (dimension);
	vector <int> Se = { 1 , 1 }; //solução s estrela, que conterá o menor resultado até o final do procedimento
	vector <int> Sx = { 1 , 1 }; //solução s, que será comparada internamente no bloco For, após saída do bloco while
	vector <int> s; // = { 1 , 1 }; //solução s
	vector <int> s1; // = { 1 , 1 }; //solução s' ou s1 - auxiliar
	double dist_S = 0;
	double dist_Sx = 0; //conterá as distâncias totais de cada solução obtida durante o procedimento
	double dist_Se = 2147483647;
//	*dist_Se = 2147483647; //distância inicial da solução estrela, irá conter a menor distância e a mánterá até o fim 
	for (int contador = 0; contador < IMax; contador++)
	{
		//impressão de teste
		/*std::cout << std::endl;
		std::cout << "<<< IMax .......> " << contador << endl;
		std::cout << std::endl;*/
		/* ----  Início Bloco Construção / Construction ---- */
		s = { 1 , 1 };
		s1 = { 1 , 1 };
		construction(s, matrizAdj); // procedimento de construção
		
		//calculo a distância de S
	//	*dist_S = 0;
		for (int j = 0; j <= dimension; j++)
		{
			if ((j < dimension - 1) & (s[j] >= 0))
				dist_S += matrizAdj[s[j]][s[j + 1]];
			/* se for o último vértice, somo o valor do ultimo looping*/
			if (j == dimension)
				dist_S += matrizAdj[s[j - 1]][s[0]];

		}
		//impressão de teste
		//std::cout << "Distancia Total de S= " << dist_S << endl << endl;
	//}
	/* ----  Término Bloco Construção / Construction ---- */
	//guardo a menor distâncie e menor solução s - será a solução s*
		dist_Sx = dist_S; //guardo a distancia total de Sx, equivalente a f(s')
		Sx = s; //equivalente à linha 6 s' = s;
		int iterILS = 0;
		while (iterILS < IILS)
		{
			//impressão de teste
			/*std::cout << std::endl;
			std::cout << "<<< iterILS .......> " << iterILS << endl;
			std::cout << std::endl;*/
			/*--- chamada ao procedimento RVND - passar como parâmetros o vetor s e a dist_s, evitando futuros cálculos ---*/
			s = RVND2(s, &dist_S);
			//verifico se a melhor solução da vizinhança é menor que a solução s'
			if (dist_S < dist_Sx) //f(s) < f(s')
			{
				dist_Sx = dist_S; //para economizar cálculo da distância f(s') = f(s)
				Sx = s; //equivalente à linha 11 s' = s;
				iterILS = 0;
				//andersoneo 12-11-2020
				if (dist_S < dist_Se)
				{
					dist_Se = dist_S;
					Se = s;
				}
			}
			vector<int> s2; //vetor que receberá a ação de permutação
			Permutacao(s, s2);
			s = s2; //atualizo parametro usado na procedure RVND
			//calculo a distância da nova solução obtida
			double dist_totals = 0;
			for (int c = 0; c < dimension; c++)
			{
				if ((c < dimension - 1) & (s[c] >= 0))
					dist_totals += matrizAdj[s[c]][s[c + 1]];
			}
			/* ao sair do For, tenho o último vértice, somo o valor do ultimo looping*/
			dist_totals += matrizAdj[s[dimension - 1]][s[0]];
			dist_S = dist_totals; //atualizo parametro usado na procedure RVND
			iterILS++;
		}
		if (dist_Sx < dist_S) //f(s') < f(s)
		{
			dist_S = dist_Sx; //para economizar cálculo da distância f(s) = f(s')
			s = Sx; //equivalente à linha 19 s = s';
		}
		//guardo a menor distâncie e menor solução s - será a solução s*
		if (dist_S < dist_Se)
		{
			dist_Se = dist_S;
			Se = s;
		}
		//std::cout << " " << contador << "/" << IMax;
	}
	auto timerEnd = chrono::system_clock::now();
	chrono::duration<double> elapsedSeconds = timerEnd - timerStart;
	//Impressão do resultado final
	std::cout << endl;
	std::cout << "S*: " << endl;
	printS(Se);
	std::cout << "E distancia total= " << dist_Se << endl;
	std::cout << "TIME: " << elapsedSeconds.count() << "\n";

	return 0;
}


void readData(int argc, string arq_instancia, int *Dimension, double ***Mdist)
{

	if (argc < 2) {
		std::cout << "\nFaltando parametros\n";
		std::cout << " ./exec [Instancia] " << endl;
		exit(1);
	}

	if (argc > 2) {
		std::cout << "\nMuitos parametros\n";
		std::cout << " ./exec [Instancia] " << endl;
		exit(1);
	}

	int N;
	//string arquivo, ewt;
	std::string arquivo, ewt;

	char *instancia;
	//instancia = argv[1];


	//std::ifstream in(instancia, ios::in);
	std::ifstream in(arq_instancia, ios::in); //"dantzig42.tsp"; //nome do arquivo txt referente à instância

	if (!in) {
		std::cout << "arquivo nao pode ser aberto\n";
		exit(1);
	}

	while (arquivo.compare("DIMENSION:") != 0 && arquivo.compare("DIMENSION") != 0) {
		//std::istringstream iss(arquivo);
		in >> arquivo;
		//iss >> arquivo;
	}

	if (arquivo.compare("DIMENSION") == 0)  in >> arquivo;

	in >> N;

	while (arquivo.compare("EDGE_WEIGHT_TYPE:") != 0 && arquivo.compare("EDGE_WEIGHT_TYPE") != 0) {
		in >> arquivo;
	}
	if (arquivo.compare("EDGE_WEIGHT_TYPE") == 0)  in >> arquivo;

	in >> ewt;

	double *x = new double[N + 1];
	double *y = new double[N + 1];

	// Alocar matriz 2D
	double **dist = new double*[N + 1];

	for (int i = 0; i < N + 1; i++) {
		dist[i] = new double[N + 1];
	}

	if (ewt == "EXPLICIT") {

		while (arquivo.compare("EDGE_WEIGHT_FORMAT:") != 0 && arquivo.compare("EDGE_WEIGHT_FORMAT") != 0) {
			in >> arquivo;
		}

		string ewf;
		if (arquivo.compare("EDGE_WEIGHT_FORMAT") == 0)  in >> arquivo;
		in >> ewf;

		if (ewf == "FUNCTION") {
			std::cout << "FUNCTION - Nao suportado!" << endl;
		}

		else if (ewf == "FULL_MATRIX") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int i = 1; i < N + 1; i++) {
				for (int j = 1; j < N + 1; j++) {
					in >> dist[i][j];
				}
			}
		}

		else if (ewf == "UPPER_ROW") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int i = 1; i < N; i++) {
				for (int j = i + 1; j < N + 1; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}

			for (int i = 1; i < N + 1; i++) {
				dist[i][i] = 0;
			}

		}

		else if (ewf == "LOWER_ROW") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int i = 2; i < N + 1; i++) {
				for (int j = 1; j < i; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}

			for (int i = 1; i < N + 1; i++) {
				dist[i][i] = 0;
			}
		}

		else if (ewf == "UPPER_DIAG_ROW") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int i = 1; i < N + 1; i++) {
				for (int j = i; j < N + 1; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}
		}

		else if (ewf == "LOWER_DIAG_ROW") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int i = 1; i < N + 1; i++) {
				for (int j = 1; j <= i; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}
		}

		else if (ewf == "UPPER_COL") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int j = 2; j < N + 1; j++) {
				for (int i = 1; i < j; i++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}

			for (int i = 1; i < N + 1; i++) {
				dist[i][i] = 0;
			}

		}

		else if (ewf == "LOWER_COL") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int j = 1; j < N; j++) {
				for (int i = j + 1; i < N + 1; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}

			for (int i = 1; i < N + 1; i++) {
				dist[i][i] = 0;
			}

		}

		else if (ewf == "UPPER_DIAG_COL") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int j = 1; j < N + 1; j++) {
				for (int i = 1; i <= j; i++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}
		}

		else if (ewf == "LOWER_DIAG_COL") {

			while (arquivo.compare("EDGE_WEIGHT_SECTION") != 0) {
				in >> arquivo;
			}

			// Preencher Matriz Distancia
			for (int j = 1; j < N + 1; j++) {
				for (int i = j; i < N + 1; j++) {
					in >> dist[i][j];
					dist[j][i] = dist[i][j];
				}
			}
		}

	}

	else if (ewt == "EUC_2D") {

		while (arquivo.compare("NODE_COORD_SECTION") != 0) {
			in >> arquivo;
		}
		// ler coordenadas
		int tempCity;
		for (int i = 1; i < N + 1; i++) {
			in >> tempCity >> x[i] >> y[i];
		}

		// Calcular Matriz Distancia (Euclidiana)
		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				dist[i][j] = floor(CalcDistEuc(x, y, i, j) + 0.5);
			}
		}
	}

	else if (ewt == "EUD_3D") {
		std::cout << "EUC_3D - Nao suportado!" << endl;
	}

	else if (ewt == "MAX_2D") {
		std::cout << "MAX_2D - Nao suportado!" << endl;
	}

	else if (ewt == "MAX_3D") {
		std::cout << "MAX_3D - Nao Suportado!" << endl;
	}

	else if (ewt == "MAN_2D") {
		std::cout << "MAN_2D - Nao suportado!" << endl;
	}

	else if (ewt == "MAN_3D") {
		std::cout << "MAN_3D - Nao Suportado!" << endl;
	}

	else if (ewt == "CEIL_2D") {

		while (arquivo.compare("NODE_COORD_SECTION") != 0) {
			in >> arquivo;
		}
		// ler coordenadas
		int tempCity;
		for (int i = 1; i < N + 1; i++) {
			in >> tempCity >> x[i] >> y[i];
		}

		// Calcular Matriz Distancia (Euclidiana)
		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				dist[i][j] = ceil(CalcDistEuc(x, y, i, j));
			}
		}
	}

	else if (ewt == "GEO") {

		while (arquivo.compare("NODE_COORD_SECTION") != 0) {
			in >> arquivo;
		}
		// ler coordenadas
		int tempCity;
		for (int i = 1; i < N + 1; i++) {
			in >> tempCity >> x[i] >> y[i];
		}

		double *latitude = new double[N + 1];
		double *longitude = new double[N + 1];

		CalcLatLong(x, y, N, latitude, longitude);

		// Calcular Matriz Distancia
		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				dist[i][j] = CalcDistGeo(latitude, longitude, i, j);
			}
		}

	}

	else if (ewt == "ATT") {

		while (arquivo.compare("NODE_COORD_SECTION") != 0) {
			in >> arquivo;
		}

		// ler coordenadas
		int tempCity;
		int *tempX = new int[N + 1];
		int *tempY = new int[N + 1];

		for (int i = 1; i < N + 1; i++) {
			in >> tempCity >> tempX[i] >> tempY[i];
			x[i] = tempX[i];
			y[i] = tempY[i];
		}

		// Calcular Matriz Distancia (Pesudo-Euclidiana)
		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				dist[i][j] = CalcDistAtt(x, y, i, j);
			}
		}

	}

	else if (ewt == "XRAY1") {
		std::cout << "XRAY1 - Nao suportado!" << endl;
	}

	else if (ewt == "XRAY2") {
		std::cout << "XRAY2 - Nao suportado!" << endl;
	}

	else if (ewt == "SPECIAL") {
		std::cout << "SPECIAL - Nao suportado!" << endl;
	}

	*Dimension = N;
	*Mdist = dist;
}

double CalcDistEuc(double *X, double *Y, int I, int J)
{
	return
		sqrt(pow(X[I] - X[J], 2) + pow(Y[I] - Y[J], 2));
}

double CalcDistAtt(double *X, double *Y, int I, int J)
{
	// Calcula Pseudo Distancia Euclidiana
	double rij, tij, dij;

	rij = sqrt((pow(X[I] - X[J], 2) + pow(Y[I] - Y[J], 2)) / 10);
	tij = floor(rij + 0.5);

	if (tij < rij)
		dij = tij + 1;
	else
		dij = tij;

	return dij;
}

void CalcLatLong(double *X, double *Y, int n, double *latit, double* longit)
{
	double PI = 3.141592, min;
	int deg;

	for (int i = 1; i < n + 1; i++) {
		deg = (int)X[i];
		min = X[i] - deg;
		latit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
	}

	for (int i = 1; i < n + 1; i++) {
		deg = (int)Y[i];
		min = Y[i] - deg;
		longit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
	}
}

double CalcDistGeo(double *latit, double *longit, int I, int J)
{
	double q1, q2, q3, RRR = 6378.388;

	q1 = cos(longit[I] - longit[J]);
	q2 = cos(latit[I] - latit[J]);
	q3 = cos(latit[I] + latit[J]);

	return
		(int)(RRR * acos(0.5*((1.0 + q1)*q2 - (1.0 - q1)*q3)) + 1.0);
}

void printData() {
	std::cout << "dimension: " << dimension << endl;
	for (size_t i = 1; i <= dimension; i++) {
		for (size_t j = 1; j <= dimension; j++) {
			std::cout << matrizAdj[i][j] << " ";
		}
		std::cout << endl;
	}
}

void construction(vector<int> &s, double **c)
{
	/*--- Agosto 2020 Aeo: início declaração estrutura de informação do custo de inserção ---*/
	struct InsertionInfo
	{
		int noInserido; // nó K a ser inserido
		int arestaRemovida; // aresta {i,j} onde o nó K será inserido
		double custo; // delta ao inserir K na aresta {i,j}
	};
	/*--- Agosto 2020 Aeo: término declaração estrutura de informação do custo de inserção ---*/
	vector <int> listaDeCandidatos;// = { 2,3,4,5,6 };
	for (int i = 0; i < dimension - 1; i++) //-1 pois o vértice 1 é minha origem
		listaDeCandidatos.insert(listaDeCandidatos.begin() + i, i + 2);
	

	srand(time(NULL)); //semente para fazer com que o rand não se inicie sempre por 1
	int tamanhoSubtourInicial = 3;
	for (int i = 0; i < tamanhoSubtourInicial; i++)
	{
		int j = rand() % listaDeCandidatos.size();
		s.insert(s.begin() + 1, listaDeCandidatos[j]);
		listaDeCandidatos.erase(listaDeCandidatos.begin() + j);
	}
	/*--- Agosto 2020 Aeo: ao sair do subtour tenho uma pré-solução S contendo 3 vértices, a seguir realizo o cálculo de inserção dos demais vértices,
	   para ao término, ordenar o vetor de resultados e escolher aleatóriamente (alfa = {0,1}) um dos menores valores.
	   E repetito o processo até ter a lista de candidatos vazia ---*/
	   //impressão de teste
	   /*std::cout << endl;
		std::cout << "--------- Construcao: pre-solucao S contendo 3 vertices ---- " << endl;
		printS(s);*/

		/*--- Agosto 2020 Aeo:
		 1) preciso de um looping limitado ao número de elementos de minha lista de candidatos. Pois irei a cada elemento calcular seu
		 custo de inserção entre cada um dos vértices de minha pré-solução S. Mas não irei esvaziar a listaDeCandidatos pois irei usá-la
		 novamente mais à frente quando da escolha aleatória (alfa = {0,1}) ---*/
	while (listaDeCandidatos.size() > 0)
	{
		//instancio vetor custoInserção para guardar as informações
		vector<InsertionInfo> custoInsercao((s.size() - 1) * listaDeCandidatos.size());
		//entro nos sub-loopings para cálculo dos custos de inserções
		//for (int i = 0, j = 1, l = 0; i < s.size() - 2; i++, j++)
		for (int i = 0, j = 1, l = 0; i < s.size() - 1; i++, j++)
		{
			for (auto k : listaDeCandidatos)
			{
				custoInsercao[l].custo = c[s[i]][k] + c[s[j]][k] - c[s[i]][s[j]];
				custoInsercao[l].noInserido = k;
				custoInsercao[l].arestaRemovida = i;
				l++;
			}
		}

		//imprimo resultado do cálculo do custo de inserção para teste
		/*std::cout << endl;
		std::cout << "--------- resultado do calculo do custo de insercao ---- " << endl;
		for (int j = 0; j < custoInsercao.size() - 2; j++) {
			std::cout << "Linha=" << j << " custoInsercao[l].custo= " << custoInsercao[j].custo << " | custoInsercao[l].noInserido= " << custoInsercao[j].noInserido
				<< " | custoInsercao[l].arestaRemovida= " << custoInsercao[j].arestaRemovida << endl;
		}
		std::cout << endl;*/
		/*--- Agosto 2020 Aeo:
		 Classe de funções considerando a ordem de seu crescimento
		 constante[a=1] < logaritmo[log n] < linear[n] < nlogn < quadrática[n^2] < cúbica[n^3] < exponencial [a^n]
		 -------------------------
		 Para ordenação do vetor escolhi o método Quick Sort, criado por C. A. R. Hoare em 1960, por ser o mais rápido e possuir complexidade C(n) = O(n²) no pior caso e
		 C(n) = O(n log n) no melhor e médio caso.
		 Mas por ser valores inteiros, acredito só cairmos na complexidade C(n) = O(n log n)
		 ---*/
		 /*crio um vetor auxiliar chamado vet_custoInsercao que conterá apenas os valores do custo de inserção a serem ordenados ao passar
		  como parâmetro da função quickSort() */
		  //int tamanho = sizeof(custoInsercao.size()) / sizeof(int);
		int tamanho = custoInsercao.size() - 2;
		int *vet_custoInsercao;
		//forma correta e padrao de reservar um vetor dinamico
		vet_custoInsercao = (int*)malloc(sizeof(int*) * tamanho);
		//atribuo os valores obtidos como custos de inserções ao novo vetor
		for (int i = 0; i < tamanho; i++)
		{
			vet_custoInsercao[i] = custoInsercao[i].custo;
		}
		quickSort(vet_custoInsercao, 0, tamanho - 1);
		//imprimo resultado do cálculo do custo de inserção para teste
		/*std::cout << endl;
		std::cout << "--------- vet_custoInsercao, resultado custo de insercao apos quickSort ---- " << endl;
		for (int j = 0; j < tamanho; j++) {
			std::cout << "Linha=" << j << " vet_custoInsercao[j]= " << vet_custoInsercao[j] <<
				" custoInsercao[j].custo= " << custoInsercao[j].custo << endl;
		}
		std::cout << endl;*/
		//faço a escolha aleatório de alfa(0,1), para pegar um dos primeiros elementos
		//int alfa = rand() % (0, 1);
		int alfa = (rand() % 100) / 100;
		//quando eu chegar a ter apenas 1 elemento no vetor, alfa não pode assumir o valor 1 pois causará erro de acesso fora do range
		/*if (listaDeCandidatos.size() < 2)
			alfa = 0;*/
			//std::cout << "alfa=" << alfa << endl;
			//identifico o vértice e em qual arestra ele será inserido
		int vertice_escolhido = 0;
		int aresta_removida = 0;
		for (int i = 0; i < tamanho; i++)
		{
			if (custoInsercao[i].custo == vet_custoInsercao[alfa])
			{
				vertice_escolhido = custoInsercao[i].noInserido;
				aresta_removida = custoInsercao[i].arestaRemovida;
				break; //interrompo o looping
			}
		}
		//caminho até a aresta predefinida para remoção e insiro o vértice. Tamanho - 1, pois o número de arestas é o número de vértices - 1
		//for (int i = 0; i < tamanho - 1; i++) //não sempre, mas às vezes ficava preso ex: tamanho=3 e aresta_removida=2
		for (int i = 0; i < tamanho; i++)
		{
			if (i == aresta_removida) //encontrei a aresta removida
			{
				s.insert(s.begin() + i + 1, vertice_escolhido);
				//e para remover o candidato da lista, preciso saber sua posição
				for (int j = 0; j < listaDeCandidatos.size(); j++)
				{
					if (listaDeCandidatos[j] == vertice_escolhido)
					{
						listaDeCandidatos.erase(listaDeCandidatos.begin() + j);
						break;
					}
				}
				break;
			}
		}
		/*std::cout << endl;
		std::cout << "--------- listaDeCandidatos, apos remover vertice inserido ---- " << endl;
		printS(listaDeCandidatos);*/
	}

}

void printS(vector<int> &s)
{
	for (int j = 0; j < s.size(); j++) {
		std::cout << s[j] << " ";
	}
	std::cout << endl;
}

void printM_viz(int m_viz[5][1000], int tamanho)
{
	std::cout << endl;
	std::cout << "Matriz de Vizinhanca" << endl;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < tamanho; j++)
			std::cout << m_viz[i][j] << " ";
		std::cout << endl;
	}
}

int PreencheVizinhanca(vector<int> &novo_vetor_s, int m_viz[5][1000], bool teste_manual, int total_inserido, int num_vertices, int p, int comp_m_viz)
{
	/* --> novo_vetor_s - terá a solução capturada que fará parte da vizinhança
	   --> m_viz - matriz de vizinança que receberá a solução capturada
	   --> teste_manual - flag que dirá se uso os dados fixos para testes unitários ou se adiciono novas soluções à matriz vizinhança
	   --> total_inserido - variável que indica quantas soluções S já constam na matriz vizinhança: m_viz
	   --> num_vertices - indica qual o modelo que foi carregado via arquivo e está em teste. dij6.txt num_vertices = 6
	   --> comp_m_viz - será o comprimento da matriz m_viz, com esta informação posso calcular o número de Ks de minha estrutura de vizinhança: k = comp_m_viz / num_vertice

	*/
	/* como testes apenas, criei manualmente a matriz vizinhança
	  K:	1						2						3
	j:		0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
S(i)	0	4	2	0	5	3	1	0	2	4	5	3	1	0	4	5	3	2	1
		1	4	2	5	0	3	1	0	2	5	4	3	1	0	4	5	3	1	2
		2	4	0	2	5	3	1	0	2	5	3	4	1	2	0	4	5	3	1

	*/
	if (teste_manual) //vizinhança criada com base na simulação feita em Excel, testes unitários grafo 6 x 6 e aqui matriz i=3 x k=3 [j=18]
	{
		//linha 0 k =1
		m_viz[0][0] = 4; m_viz[0][1] = 2; m_viz[0][2] = 0; m_viz[0][3] = 5; m_viz[0][4] = 3; m_viz[0][5] = 1;
		//linha 0 k =2
		m_viz[0][6] = 0; m_viz[0][7] = 2; m_viz[0][8] = 4; m_viz[0][9] = 5; m_viz[0][10] = 3; m_viz[0][11] = 1;
		//linha 0 k =3
		m_viz[0][12] = 0; m_viz[0][13] = 4; m_viz[0][14] = 5; m_viz[0][15] = 3; m_viz[0][16] = 2; m_viz[0][17] = 1;
		//linha 1 k =1
		m_viz[1][0] = 4; m_viz[1][1] = 2; m_viz[1][2] = 5; m_viz[1][3] = 0; m_viz[1][4] = 3; m_viz[1][5] = 1;
		//linha 1 k =2
		m_viz[1][6] = 0; m_viz[1][7] = 2; m_viz[1][8] = 5; m_viz[1][9] = 4; m_viz[1][10] = 3; m_viz[1][11] = 1;
		//linha 1 k =3
		m_viz[1][12] = 0; m_viz[1][13] = 4; m_viz[1][14] = 5; m_viz[1][15] = 3; m_viz[1][16] = 1; m_viz[1][17] = 2;
		//linha 2 k =1
		m_viz[2][0] = 4; m_viz[2][1] = 0; m_viz[2][2] = 2; m_viz[2][3] = 5; m_viz[2][4] = 3; m_viz[2][5] = 1;
		//linha 2 k =2
		m_viz[2][6] = 0; m_viz[2][7] = 2; m_viz[2][8] = 5; m_viz[2][9] = 3; m_viz[2][10] = 4; m_viz[2][11] = 1;
		//linha 2 k =3
		m_viz[2][12] = 2; m_viz[2][13] = 0; m_viz[2][14] = 4; m_viz[2][15] = 5; m_viz[2][16] = 3; m_viz[2][17] = 1;

		return 0;
	}
	else
	{
		//int total_inserido, int num_vertices, int r, int p, int comp_m_viz
		// K é o número de estruturas que possuo em minha vizinhança 
		int k = comp_m_viz / num_vertices;
		//1o preciso saber se a matriz de vizinhança já não está cheia. Assim, calculo o tamanho e verifico sua capacidade: Tam.Viz = K * p
		int tam_viz = k * p;
		if (total_inserido + 1 > tam_viz)
		{
			return 999; //vizinhança já cheia
		}
		else
		{
			/* OK, tenho espaço, agora preciso saber onde inserir esta nova Solução S (novo_vetor_s). Pela parâmetro total_inserido sei
			até onde chegar, assim, vou definir que primeiro irei preencher K por K, partindo do topo até sua base (profundidade p). */
			int num_iteracoes = 1; //quando for igual a total_inserido + 1, é onde devo inserir
			for (int j = 1; j <= k; j++) //irei percorrer cada K da estrutura
			{
				/*entro no looping de i: fixando as linhas da matriz de vizinhança até à profundidade informada pelo parâmetro p */
				for (int i = 0; i < p; i++)
				{
					if (num_iteracoes == total_inserido + 1) //é o ponto para adição do novo S
					{
						/*preciso saber a coluna de início para adição, que varia conforme K (aqui é meu j) que são as colunas, a largura da matriz de
							vizinhança delimitadas pelos múltiplos do número de vértices. j_inicial = abs((num_vertices / k - num_vertices) * k);
							Ex: num_vertices=6, temos que para k=1 j_inicial=0 (num_vertices * k)=6, k=2 j_inicial=6 (num_vertices * k)=12, k=3 j_inicial=12 (num_vertices * k)=18, k=4 j_inicial=18 (num_vertices * k)=24 etc*/
						int j_inicial = abs((num_vertices / j - num_vertices) * j);
						int j2 = 0; //este j é do vetor_aux que sempre irá variar de 0 até o num_vertice
						int j3 = j_inicial; //indica a coluna para início da inserção na matriz
						for (int z = 0; z < num_vertices; z++)
						{
							m_viz[i][j3] = novo_vetor_s[j2];
							j2++; j3++;
						}
						//ao término da inserção, incremento o total inserido e forço a saída de todos os loopings
						total_inserido++;
						i = p;
						j = k;
					}
					num_iteracoes++;
				}
			}
			return 0;
		}
	}
}

vector<int> reinsertion(vector<int> s, double *bestDelta, int subsegSize) {
	int best_i = 0, best_j = 0;
	*bestDelta = 0; //0 = no change
	double delta = 0, semiDelta = 0;

	for (int i = 1; i < s.size() - subsegSize; i++)
	{
		semiDelta = matrizAdj[s[i - 1]][s[i + subsegSize]] - matrizAdj[s[i]][s[i - 1]] - matrizAdj[s[i + subsegSize - 1]][s[i + subsegSize]];

		for (int j = 1; j < s.size(); j++)
		{
			if (i <= j && j <= i + subsegSize) continue;

			delta = matrizAdj[s[j - 1]][s[i]] + matrizAdj[s[i + subsegSize - 1]][s[j]] - matrizAdj[s[j]][s[j - 1]] + semiDelta;

			if (delta < *bestDelta - std::numeric_limits<double>::epsilon()) {
				*bestDelta = delta;

				best_i = i;
				best_j = j;
			}

		}
	}

	if (*bestDelta < std::numeric_limits<double>::epsilon())
	{
		vector<int> subseg(s.begin() + best_i, s.begin() + best_i + subsegSize);

		if (best_i < best_j) {
			s.insert(s.begin() + best_j, subseg.begin(), subseg.end());
			s.erase(s.begin() + best_i, s.begin() + best_i + subsegSize);
		}
		else {
			s.erase(s.begin() + best_i, s.begin() + best_i + subsegSize);
			s.insert(s.begin() + best_j, subseg.begin(), subseg.end());
		}

	}
	else {
		*bestDelta = 0;
	}

	return s;
}

vector<int> swap(vector<int> s, double *bestDelta) {
	int best_i = 0, best_j = 0;
	*bestDelta = 0; //0 = no change
	double delta = 0, semiDelta = 0;

	for (int j = 2; j < s.size() - 1; j++)
	{
		semiDelta = -matrizAdj[s[j]][s[j - 1]] - matrizAdj[s[j]][s[j + 1]]; //relies only on j

		for (int i = 1; i < j; i++)
		{
			if (i == j - 1) { // If they're adjacent
				delta = matrizAdj[s[j]][s[i - 1]] + matrizAdj[s[i]][s[j + 1]] - matrizAdj[s[i]][s[i - 1]] - matrizAdj[s[j]][s[j + 1]];
			}
			else {
				delta = matrizAdj[s[i]][s[j - 1]] + matrizAdj[s[i]][s[j + 1]] + matrizAdj[s[j]][s[i - 1]] + matrizAdj[s[j]][s[i + 1]]
					- matrizAdj[s[i]][s[i - 1]] - matrizAdj[s[i]][s[i + 1]] + semiDelta;
			}

			if (delta < *bestDelta) {
				*bestDelta = delta;

				best_i = i;
				best_j = j;
			}
		}
	}

	if (*bestDelta < 0) {
		std::swap(s[best_j], s[best_i]);
	}
	else {
		*bestDelta = 0;
	}

	return s;
}

vector<int> flip(vector<int> s, double *bestDelta) {
	int best_i = 0, best_j = 0;
	*bestDelta = 0; //0 = no change
	double delta = 0, semiDelta = 0;

	for (int j = 3; j < s.size() - 1; j++)
	{
		semiDelta = -matrizAdj[s[j]][s[j + 1]]; //relies only on j

		for (int i = 1; i < j - 1; i++)
		{
			delta = -matrizAdj[s[i - 1]][s[i]] + semiDelta + matrizAdj[s[i]][s[j + 1]] + matrizAdj[s[j]][s[i - 1]];

			if (delta < *bestDelta - std::numeric_limits<double>::epsilon())
			{
				*bestDelta = delta;

				best_i = i;
				best_j = j;
			}
		}
	}

	if (*bestDelta < std::numeric_limits<double>::epsilon()) {
		std::reverse(s.begin() + best_i, s.begin() + best_j + 1);
	}
	else {
		*bestDelta = 0;
	}

	return s;
}

vector<int> RVND2(vector<int> s, double *mainCost) {
	vector<int> ngbhList = { N1, N2, N3, N4, N5 };
	int ngbh_n;

	vector<int> neighbour_s = s;
	double neighbourCost = numeric_limits<double>::max();
	double delta = 0; //Receives delta from neighbouthood movements

	while (!ngbhList.empty())
	{
		ngbh_n = ngbhList[rand() % ngbhList.size()];

		switch (ngbh_n) {
		case N1:
			neighbour_s = swap(s, &delta);
			break;
		case N2:
			neighbour_s = flip(s, &delta);
			break;
		case N3:
			neighbour_s = reinsertion(s, &delta, 1);
			break;
		case N4:
			neighbour_s = reinsertion(s, &delta, 2);
			break;
		case N5:
			neighbour_s = reinsertion(s, &delta, 3);
			break;
		}

		neighbourCost = *mainCost + delta;

		if (neighbourCost < *mainCost) {
			s = neighbour_s;
			*mainCost = neighbourCost;

			ngbhList = { N1, N2, N3, N4, N5 };
		}
		else {
			ngbhList.erase(std::remove(ngbhList.begin(), ngbhList.end(), ngbh_n), ngbhList.end());
		}
	}

	return s;
}

void Permutacao(vector<int> &s1, vector<int> &s2)
{
	/*==>> início bloco Permutação - procedimento criado com base no método 2-opt—Two non-adjacent arcs are removed and
		another two are inserted in order to build a new feasible tour.
		A diferença é que aqui poderá variar entre 2 e |V|/10 arcos não adjacentes que serão trocados de lugares, realizando assim
		o double-bridge
		-------------------
		Pegarei a solução s e dividirei por 2, terei 2 metades onde:
		1) aleatoriamente escolho 1 conjunto de vértice na 1a metade, diferente do 1o e último. Parto de um vértice e seleciono os demais
		à direita, definindo o 1o conjunto de arestas/vértices a ser removida;
		2) aleatoriamente escolho 1 conjunto de vértice na 2a metade, diferente do último. Pego seu(s) vértice(s) à direita, definindo o
		2o conjunto de arestas/vértices a ser removida;
		3) Na sequencia, troco os conjuntos de arestas/vértices de lugares, obtendo uma nova solução.
		*/
		//declarações
	srand((unsigned)time(NULL));//srand(time(NULL)); //semente para fazer com que o rand não se inicie sempre por 1
	//considerando que terei um máximo de V=1000, meus conjuntos obedecerão à fórmula |V|/10, para se definir o máximo de vértices em troca
	int ParVerticesA[500] = { -1 };
	int ParVerticesB[500] = { -1 };
	/* Não irei guardar a solução s em vetores auxiliares, irei considerar a metade do vetor na função rand() para a 1a metade,
	e para a 2a metade, somo o valor da divisão no resultado do rand() para encontrar o índice na 2a metade.
	Ex: total vértices = 6, metade 3, a função rand() irá variar de 0 a 3. Para a 1a metade considero o resultado que der. Mas para a
	2a somo 3, e assim tenho o índice do vértice na 2a metade. */
	//clono a solução principal s1 passada como parâmetro
	//vector<int> s2;
	s2 = s1;
	int meio_vetor, v_aleatorio, vertice_origem;
	vertice_origem = s1[0];
	//tam_conjunto : varia entre 2 e |V|/10
	int tam_conjunto = round(dimension / 10);
	if (tam_conjunto < 2)
		tam_conjunto = 2;
	meio_vetor = dimension / 2;
	bool P_metade = true; //flag que indica se estou buscando pares na 1a metade do vetor
	bool fim = false;
	while (!fim)
	{
		v_aleatorio = (rand() % meio_vetor);
		if (P_metade)
		{
			//só verifico se for um valor diferente do vértice de origem e menor que o último elemento da 1a metade do vetor
			if ((v_aleatorio != vertice_origem) && (v_aleatorio < meio_vetor - 1) && (v_aleatorio + tam_conjunto < meio_vetor))
			{
				//obtenho o 1o par de índices de vértices, que me dá a 1a aresta
				/*ParVerticesA[0] = v_aleatorio;
				ParVerticesA[1] = v_aleatorio + 1;
				O for abaixo substitui as linhas acima e cobre quaisquer valores possíveis entre 2 e |V|/10*/
				if (v_aleatorio == 0)
					v_aleatorio = 1; //evitar que o vértice na posição zero/origem seja trocado
				for (int i = 0; i < tam_conjunto; i++)
				{
					ParVerticesA[i] = v_aleatorio + i;
				}
				P_metade = false;
			}
		}
		else //obter o 2o par
		{
			//só verifico se for menor que o último elemento da 2a metade do vetor
			if ((v_aleatorio < meio_vetor - 1) && (v_aleatorio + tam_conjunto < meio_vetor))
			{
				//obtenho o 2o par de índices de vértices, que me dá a 2a aresta
				/*ParVerticesB[0] = v_aleatorio + meio_vetor;
				ParVerticesB[1] = v_aleatorio + meio_vetor + 1;
				O for abaixo substitui as linhas acima e cobre quaisquer valores possíveis entre 2 e |V|/10 */
				for (int i = 0; i < tam_conjunto; i++)
				{
					ParVerticesB[i] = v_aleatorio + meio_vetor + i;
				}
				fim = true;
			}
		}
	}
	//saindo do while, posso realizar as trocas entre as arestas
	for (int i = 0; i < tam_conjunto; i++)
	{
		s2[ParVerticesA[i]] = s1[ParVerticesB[i]];
		s2[ParVerticesB[i]] = s1[ParVerticesA[i]];
	}

	//impressao de teste
	/*std::cout << endl;
	printS(s1);
	std::cout << "Permutacao | s1 -> s2" << endl;
	printS(s2);*/

	/*==>> término bloco 2a estrutura - 2-opt—N(2)—Two non-adjacent arcs are removed and
			another two are inserted in order to build a new feasible tour.*/
}

