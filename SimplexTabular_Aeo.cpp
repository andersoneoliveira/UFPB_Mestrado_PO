/*-----------------------------------
30-abril-2020 - Implementação do Método Simplex Tabular

UFPB - Mestrado em Informática - Disciplina: Pesquisa Operacional - Prof. Anand
Aluno: Anderson Ernani de Oliveira

Fontes de referências:
- AULA5.pdf slides apresentados durante vídeo aulas;
- https://www.youtube.com/watch?v=7qOdbo-xPaA - vídeo aula complementar.

Sobre a implementação: o programa irá guiar seu usuário para inserção dos dados iniciais, e à medida que as etapas abaixo forem sendo processadas,
elas serão exibidas para um melhor entendimento de como foi realizado cada regra.

Etapas:
- 1a Passar todos os elementos da função objetivo para o outro lado da equação igualando a zero.
- 2a Transformar as desigualdades em igualdades e acrescentando as variáveis de folga.
- 3a Preencher a tabela [matrix e/ou vetores de suporte] Simplex com os dados das equações.
- 4a Escolher a Coluna Pivô: olhar na linha Z qual é o número mais negativo. Essa é a primeira Coluna pivô.
- 5a Escolher a Linha Pivô: menor resultado para LD / CP (com execeção da linha Z).
- 6a Dividir Linha Pivo pelo numero Pivo. O resultado serah uma nova linha.
- 7a Novas linhas: Linha Antiga - (coeficiente Coluna Pivô) x Nova Linha Pivô.
- 8a Teste de Otimalidade - A solução é ótima se e somente se cada coeficiente da Z/eq.(0) for não-negativo.
- *** Repitir processo a partir do passo 4, ate se obter um resultado aceitavel.

-------------------------------------
19-05-2020 - adicionei implementação para cálculo do limite de variação [range]
A visualização será opcional (1-Sim, 0-Não)
-------------------------------------
*/
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string> 
#include <cstring>

using namespace std;

int main()
{

	/*--------inicio bloco para informar as Etapas do Método Simplex Tabular ---*/
	cout << " Metodo Simplex Tabular - Implementado por.: Anderson Ernani de Oliveira " << endl;
	cout << " UFPB - Mestrado em Informatica - Disciplina: Pesquisa Operacional - Prof. Anand " << endl;
	cout << " Apos entrar com a Funcao Objetivo Max Z, a seguintes etapas serao executadas:" << endl;
	cout << " 1a Passar todos os elementos da funcao objetivo para o outro lado da equacao igualando a zero." << endl;
	cout << " 2a Transformar as desigualdades em igualdades e acrescentando as variaveis de folga." << endl;
	cout << " 3a Preencher a Tabela Simplex com os dados das equacoes." << endl;
	cout << " 4a Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo." << endl;
	cout << " 5a Escolher a Linha Pivo: menor resultado para [Lado Direito apos sinal =, dividido pelo Coeficiente da coluna pivo] (exceto linha Z)." << endl;
	cout << " 6a Dividir Linha Pivo pelo numero Pivo. O resultado serah uma nova linha." << endl;
	cout << " 7a Novas linhas: Linha Antiga - (coeficiente Coluna Pivo) x Nova Linha Pivo." << endl;
	cout << " 8a Teste de Otimalidade - A solucao eh otima se e somente se cada coeficiente da Z/eq.(0) for nao-negativo." << endl;
	cout << " *** Repetir processo a partir do passo 4, ate se obter um resultado aceitavel." << endl;
	cout << endl; cout << endl;
	/*--------termino bloco para informar as Etapas do Método Simplex Tabular ---*/
	/* --- inicio bloco declaração de variáveis do programa ---*/
	int num_elem_FO; //número de elementos da Função Objetivo
	int indice; //indice a ser utilizado nas instruções de looping
	int vet_MaxZ[6] = { 0 }; /*vetor que irá manter os valores da FO para cálculos. Uso o indice conforme posição do elemento, ou seja,
					 [1]=1o elemento, [2]=2o elemnto etc. Por ser inteiro irei tratar a letra Z fora do vetor*/
	string s_MaxZ; /*string que irá manter a função objetivo informada pelo usuário no formato original Max Z= .*/
	string s_EquacaoZ; /*string que irá manter a equação após igualar termos a zero*/
	string v_resultado; //variavel de texto para concatenar informações e exibir algum resultado ao usuário
	int num_restricoes_FO; //número de restrições da Função Objetivo
	int matriz_Restricao_FO[6][8] = { 0, 0 };; /* 6 linhas x 8 colunas. Irei usar a partir do indice=1. Sendo a coluna[6]=conteúdo variavel de folga,
								   e a coluna[7]=valor desigualdade e igualdade.*/
	int linha, coluna; // indices das linhas e colunas em uma matriz
	int continuar; //irá obter do usuário ao término de cada loop realizado entre as etapas 4 e 8, se deverá continuar ou não

	float matriz_Simplex[6][12] = { 0, 0 };
	/*
		coluna 0 = códigos que correspondem a Z[0], variáveis comuns de 1 a 5, variáveis de folgas 6 a 10.
		colunas 1 a 5 = conteúdo das variáveis comuns
		colunas 6 a 10 = conteúdo das variáveis de folgas
		coluna 11 = valor Lado Direito, depois do sinal de igualdade.
		E as linhas irão conter os valores de cada variável conforme ID da coluna associada
	*/
	float vet_Nova_Ln_Pivo[12] = {0};
	char vet_cab_matriz[12][4] = { " ", "X1", "X2", "X3", "X4", "X5", "F1", "F2", "F3", "F4", "F5", "LD" };
	string vet_id_ln_matriz[6] = { "Z", "F1", "F2", "F3", "F4", "F5" };
	cout.precision(2); //fixo exibição de 3 casas decimais
	cout.showpoint; //exibir números inteiros e decimais, sem converter para numero exponencial
	cout.fixed;
	/* --- termino bloco declaração de variáveis do programa ---*/

	cout << " Informe o numero de elementos de sua Funcao Objetivo (ex.: 10x1 + 20x2 + 30x3, serao 3 elementos):"<<endl;
	cout << "     [0 encerrar o programa.]:" << endl;
	cin >> num_elem_FO;
	/* verifico se não foi digitado um valor negativo ou maior que 5 elementos */
	while ((num_elem_FO < 0) || (num_elem_FO > 5))
	{
		cout << "Limite maximo de 5 elementos." << endl;
		cin >> num_elem_FO;
	}
	/* se o usuário informou 0 até este ponto, irá encerrar o programa. */
	if (num_elem_FO > 0)
	{
		/* --- inicio bloco para ler o valor de cada elemento --- */
		indice = 1;
		while (indice <= (num_elem_FO))
		{
			cout << "Entre com o elemento No " << indice << " .: ";
			cin >> vet_MaxZ[indice];
			indice++;
		}
		/* --- termino bloco para ler o valor de cada elemento --- */
		/* --- realizo o passo 1
		  1a Passar todos os elementos da funcao objetivo para o outro lado da equacao igualando a zero.
		  ---*/
		v_resultado = " Max Z= ";
		for (indice = 1; indice <= (num_elem_FO); indice++)
		{
			v_resultado += to_string(vet_MaxZ[indice]) + "x";
			if (indice == num_elem_FO)
				v_resultado += to_string(indice) + ";";
			else
				/* checo se o elemento é positivo ou negativo para tratar o sinal a ser concatenado na string*/
				if (vet_MaxZ[indice] > 0)
				{
					/* se o indice no próximo laço for o último não concateno o sinal de + */
					if (indice + 1 == num_elem_FO)
						v_resultado += to_string(indice) + "  +  ";
					else
						if (vet_MaxZ[indice + 1] < 0)
							v_resultado += to_string(indice) + "  ";
						else
							v_resultado += to_string(indice) + "  +  ";
				}
				else
				{
					if (vet_MaxZ[indice + 1] < 0)
						v_resultado += to_string(indice) + "  ";
					else
						v_resultado += to_string(indice) + "  +  ";

				}
		}
		s_MaxZ = v_resultado; /*guardo a FO nesta string para posterior exibição - Max Z= */
		/* --- repito o looping anterior para passar igualar a equação a zero. Multiplicando o conteúdo por -1. ---*/
		v_resultado = " Z";
		for (indice = 1; indice <= (num_elem_FO); indice++)
		{
			/* Multiplico por -1, antes de usar o conteúdo do vetor*/
			vet_MaxZ[indice] = vet_MaxZ[indice] * -1;
			/* checo se o elemento ficou positivo ou negativo para tratar o sinal a ser concatenado na string*/
			if (vet_MaxZ[indice] > 0)
				v_resultado += "  +  " + to_string(vet_MaxZ[indice]) + "x" + to_string(indice);
			else
				v_resultado += +"  " + to_string(vet_MaxZ[indice]) + "x" + to_string(indice);
			if (indice == num_elem_FO)
				v_resultado += " = 0;";
		}

		s_EquacaoZ = v_resultado;
		cout << endl;
		cout << endl;
		cout << "1a Passar todos os elementos da funcao objetivo para o outro lado da equacao igualando a zero." << endl;
		cout << endl;
		/* --- imprimo a função objetivo para conferencia --*/
		cout << "Sua Funcao Objetivo eh " << s_MaxZ << endl;
		cout << "E a equacao eh " << s_EquacaoZ;
		cout << endl;
		cout << endl;
		cout << "-- ====================================== --" << endl;
		/* --- realizo o passo 2
		  2a Transformar as desigualdades em igualdades e acrescentando as variaveis de folga.
		  ---*/

		  /* Questiono ao usuário o número de Restrições para a Função Objetivo informada.
		   A restrição de N.N. não será preciso informar (ex.: x1, x2 ...xn >=0) */
		cout << "Qual o numero de Restricoes? (1a: 2x1 + 2x2 + 4x3 <= 300, 2a: 4x2 + 3x3 <= 200, 3a: x1 <= 20 e 4a: 4x1 + 3x2 <= 50, neste exemplo, seriam 4):" << endl;
		cout << "     [Limite maximo de 5]:" << endl;
		cin >> num_restricoes_FO;
		/* verifico se foi digitado ao menos 1 restrição, limitado ao máximo 5 */
		while ((num_restricoes_FO < 1) || (num_restricoes_FO > 5))
		{
			cout << "Limite maximo de 5 restricoes." << endl;
			cin >> num_restricoes_FO;
		}

		/* --- inicio bloco para ler o valor de cada restrição --- */

		/* --- no próximo looping será solicitado os 6 elementros [colunas] de cada restrição [linhas].
				x1, x2, x3, x4, x5 e Vlr_igualdade.
				Quando o usuário não quiser o elementro, basta informar 0 que ele não será inserido na equação.
		---*/
		// o looping para a linha corresponde à quantidade de restrições. E as colunas seus valores
		for (linha = 1; linha <= num_restricoes_FO; linha++)
		{
			cout << "Entre com os elementos da restricao No " << linha << "." << endl;
			cout << "Sequencia: x1, x2, x3, x4, x5 <= Valor da desigualdade. Zero anula o elemento. " << endl;
			for (coluna = 1; coluna < 7; coluna++)
			{
				//x1, x2, x3, x4, x5
				if (coluna < 6)
				{
					cout << "Entre com o valor de x" << coluna << " .: ";
					cin >> matriz_Restricao_FO[linha][coluna];
				}
				else
				{
					cout << "Entre com o valor da Desigualdade (<=): ";
					cin >> matriz_Restricao_FO[linha][7];
				}

			}
			cout << endl;
		}
		/* --- termino bloco para ler o valor de cada restrição --- */
		cout << endl;
		cout << "-- ====================================== --" << endl;
		/* --- imprimo a função objetivo para conferencia --*/
		cout << "2a Etapa- Transformar as desigualdades em igualdades e acrescentando as variaveis de folga." << endl;
		cout << endl;
		cout << "As equacoes de igualdades com as variaveis de Folga sao: " << endl;
		cout << endl;
		for (linha = 1; linha <= (num_restricoes_FO); linha++)
		{
			for (coluna = 1; coluna < 8; coluna++)
			{
				if (coluna == 6) //variavel de folga, atribuo valor 1 para ela ser exibida
					matriz_Restricao_FO[linha][coluna] = 1;
				/*Somente elementos com contéudo diferente de 0 serão exibidos na equação */
				if (matriz_Restricao_FO[linha][coluna] != 0)
				{
					if (coluna < 7) /* colunas menores que 7 guardam os valores antes do sinal de igualdade (=) */
					{
						/* checo se o elemento da matriz é positivo ou negativo para tratar o sinal a ser exibido na equação*/
						if (matriz_Restricao_FO[linha][coluna] > 0)
						{
							if (coluna > 1) /* a partir da 2a coluna preciso exibir o sinal de +, se positivo */
							{
								cout << " +" << setw(3) << matriz_Restricao_FO[linha][coluna];
								if (coluna == 6) //variavel de folga
									cout << "F" << linha;
								else
									cout << "x" << coluna;
							}
							else
							{
								cout << setw(3) << matriz_Restricao_FO[linha][coluna];
								if (coluna == 6) //variavel de folga
									cout << "F" << linha;
								else
									cout << "x" << coluna;
							}
						}
						else /* se elemento com sinal negativo, não preciso tratar pois já é exibido por padrão*/
							cout << setw(3) << matriz_Restricao_FO[linha][coluna] << "x" << coluna << " ";
					}
					else
						if (coluna == 7) /* coluna com o valor após sinal de igualdade (=) */
							cout << " = " << matriz_Restricao_FO[linha][coluna];

				}
			}
			cout << endl;

		}
		cout << endl;
		cout << endl;
		cout << "-- ====================================== --" << endl;

		/*--- Realizo o passo 3
		3o Preencher a tabela Simplex com os dados das equações
		---*/
		/* Eq(0) - O vetor "vet_MaxZ" contém os valores Z - Ax1 - Bx2 - Cx3 - Dx4 - Ex5 = 0.
		  E seu conteúdo será passado à linha [0] da matriz_Simplex */
		for (coluna = 1; coluna <= 5; coluna++)
			matriz_Simplex[0][coluna] = vet_MaxZ[coluna];
		/*--- atribuindo as linhas que correspondem às variáveis de folga.
		linhas:
		F1=matriz_Simplex[1][1] = matriz_Restricao_FO[1][1],  F2=matriz_Simplex[2][1] = matriz_Restricao_FO[2][1] etc
		colunas:
		x1=matriz_Simplex[1][1], x2=matriz_Simplex[1][2], x3=matriz_Simplex[1][3] ...até x5=matriz_Simplex[1][5],
		F1=matriz_Simplex[1][6], F2=matriz_Simplex[1][7], F3=matriz_Simplex[1][8] ...até F5=matriz_Simplex[1][10],
		coluna 11 = matriz_Simplex[1][11] = valor Lado Direito, depois do sinal de igualdade.
		 F1, F2, F3 ...Fn ---*/
		for (linha = 1; linha <= 5; linha++)
			for (coluna = 1; coluna <= 7; coluna++)
			{
				if ((linha >= 2) && (coluna == 6)) //trato o alinhamento dos valores das colunas das variáveis de folga
					matriz_Simplex[linha][coluna + linha - 1] = matriz_Restricao_FO[linha][coluna];
				else
					if (coluna == 7) //jogo os valores após sinal de desigualdade para o lado direito do sinal
						matriz_Simplex[linha][coluna + 4] = matriz_Restricao_FO[linha][coluna];
					else
						matriz_Simplex[linha][coluna] = matriz_Restricao_FO[linha][coluna];
			}
		cout << endl;
		cout << "-- ====================================== --" << endl;
		cout << "3o Etapa - Preencher a Tabela Simplex com os dados das equacoes." << endl;
		cout << endl;
		/* --- imprimo cabeçalho da matriz Simplex --- */
		for (indice = 1; indice <= 11; indice++)
		{
			if (indice <= 5)
				cout << setw(10) << " |   "  << "X" << indice;
			else
				if ((indice > 5) && (indice < 11))
					cout << setw(10) << " |   " << "F" << indice - 5;
				else
					if (indice == 11)
						cout << setw(10) << " |   " << "L.Direito" << endl;
		}
		for (linha = 0; linha <= 5; linha++)
		{
			if (linha == 0)
			{
				cout << "Z ";
			}
			else
			{
				cout << "F" << linha;
			}
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << fixed << showpoint << setw(2) << linha << "." << coluna << "[" << setw(6) << matriz_Simplex[linha][coluna] << "], ";
			}
			cout << endl;
		}
		cout << endl; 
		
		bool fim = false;
		while (!fim)  /*--- looping que irá garantir a repetição das etapas de 4 a 8, enquanto houver coeficiente negativo na linha Z ---*/
		{
			/* ---
			4a Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo.;
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "4a Etapa - Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo." << endl;
			/*--- percorro a linha 0 = Eq.0 = Z para encontrar o maior número negativo. Verifico da coluna 1 à 5 ---*/
			int coluna_maior_negativo = 0; //coluna_maior_negativo = coluna pivô
			int maior_negativo = 0;
			for (coluna = 1; coluna <= 5; coluna++)
			{
				if (matriz_Simplex[0][coluna] < maior_negativo)
				{
					maior_negativo = matriz_Simplex[0][coluna];
					coluna_maior_negativo = coluna;
				}
			}

			cout << endl;
			cout << "A coluna pivo eh X" << coluna_maior_negativo << ", em funcao do valor ";
			cout << fixed << showpoint << maior_negativo << "." << endl;
			cout << endl; cout << endl;
			/* ---
			5a Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo.;
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "5a Etapa - Escolher a Linha Pivo: menor resultado para [Lado Direito dividido pelo Valor da coluna pivo] (exceto linha Z)." << endl;
			cout << endl; cout << endl;
			/*--- fixo a coluna pivô e calculo a razão para cada linha, exceto a linha 0 = Eq.0 = Z, pelo valor do lado direito(coluna=11), até encontrar
			 o menor resultado. Verifico da linha 1 à 5 ---*/
			int linha_menor_razao = 0;
			int menor_razao = 10000;
			int valor_pivo = 0;
			for (linha = 1; linha <= 5; linha++)
			{
				if ((matriz_Simplex[linha][coluna_maior_negativo] != 0) && ((matriz_Simplex[linha][11] / matriz_Simplex[linha][coluna_maior_negativo]) < menor_razao)
					)
				{
					menor_razao = matriz_Simplex[linha][11] / matriz_Simplex[linha][coluna_maior_negativo];
					linha_menor_razao = linha;
				}
			}
			cout << "A linha pivo eh F" << linha_menor_razao << ", em funcao do resultado da razao ser ";
			cout << fixed << showpoint << menor_razao << "." << endl;
			valor_pivo = matriz_Simplex[linha_menor_razao][coluna_maior_negativo];
			cout << "E o pivo eh ";
			cout << fixed << showpoint << valor_pivo << ".";
			cout << endl; cout << endl;
			/* ---
			6a Etapa - Dividir Linha Pivo pelo numero Pivo. O resultado serah uma nova linha.;
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "6a Etapa - Dividir Linha Pivo pelo numero Pivo. O resultado serah uma nova linha." << endl;
			cout << endl;
			for (coluna = 1; coluna <= 11; coluna++) /*coluna vai até 11 pois divido também as variáveis de folga e o valor LD*/
			{
				if (matriz_Simplex[linha_menor_razao][coluna] != 0)
				{
					matriz_Simplex[linha_menor_razao][coluna] = matriz_Simplex[linha_menor_razao][coluna] / valor_pivo;
				}
				/*--- guardo a nova linha pivo em um novo vetor ---*/
				vet_Nova_Ln_Pivo[coluna] = matriz_Simplex[linha_menor_razao][coluna];
			}
			/*--- atualizo o ID da linha de variável de folga para o X da coluna ---*/
			if (coluna_maior_negativo == 1)
				vet_id_ln_matriz[linha_menor_razao] = "X1";
			else
				if (coluna_maior_negativo == 2)
					vet_id_ln_matriz[linha_menor_razao] = "X2";
				else
					if (coluna_maior_negativo == 3)
						vet_id_ln_matriz[linha_menor_razao] = "X3";
					else
						if (coluna_maior_negativo == 4)
							vet_id_ln_matriz[linha_menor_razao] = "X4";
						else
							if (coluna_maior_negativo == 5)
								vet_id_ln_matriz[linha_menor_razao] = "X5";
			/* --- imprimo a matriz novamente - cabeçalho da matriz Simplex --- */
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;

			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << fixed << showpoint << setw(2) << linha << "." << coluna << "[" << setw(6) << matriz_Simplex[linha][coluna] << "], ";
				}
				cout << endl;
			}
			cout << endl;
			cout << "Nova Linha Pivo: " << endl;
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << fixed << showpoint << setw(2) << " " << "[" << setw(6) << vet_Nova_Ln_Pivo[coluna] << "], ";
			}

			cout << endl; cout << endl;
			/*---
			7a Novas linhas: Linha Antiga - (coeficiente Coluna Pivo) x Nova Linha Pivo.
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "7a Etapa - Novas linhas: Linha Antiga - (coeficiente Coluna Pivo) x Nova Linha Pivo." << endl;

			cout << endl;
			float coeficiente_col_pivo = 0;
			for (linha = 0; linha <= 5; linha++)
			{
				//obtenho o coeficiente da coluna pivo - ele só irá mudar em nível de linha
				coeficiente_col_pivo = matriz_Simplex[linha][coluna_maior_negativo];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					if (linha != linha_menor_razao) //realizar o cálculo para todas as linhas, exceto a própria nova linha pivô
					{
						matriz_Simplex[linha][coluna] = matriz_Simplex[linha][coluna] - (coeficiente_col_pivo * vet_Nova_Ln_Pivo[coluna]);
					}
				}
			}
			/* --- imprimo a matriz novamente - cabeçalho da matriz Simplex --- */
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;

			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << fixed << showpoint << setw(2) << linha << "." << coluna << "[" << setw(6) << matriz_Simplex[linha][coluna] << "], ";
				}
				cout << endl;
			}
			cout << endl; 
			/*---
			8a Teste de Otimalidade - A solucao eh otima se e somente se cada coeficiente da Z/eq.(0) for nao-negativo.
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "8a Etapa - Teste de Otimalidade - A solucao eh otima se e somente se cada coeficiente da Z/eq.(0) for NAO-NEGATIVO." << endl;
			cout << endl;
			/* --- imprimo a apenas a linha Z da matriz  + cabeçalho
			 E verifico se há algum coeficiente negativo --- */
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;

			cout << "Z ";
			int valor_negativo = 0;
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << fixed << showpoint << setw(2) << "0." << coluna << "[" << setw(6) << matriz_Simplex[0][coluna] << "], ";

				if (matriz_Simplex[0][coluna] < 0)
					valor_negativo = matriz_Simplex[0][coluna];
			}
			cout << endl; cout << endl;
			/*--- havendo valor negativo, informo ao usuário e repito etapas de de 4 a 8, novamente até não existir valor negativo ---*/
			if (valor_negativo == 0) /* Fim de jogo - Sim, a solução é ótima.*/
			{
				cout << "Sim, a solucao eh otima e possui os seguintes resultados: " << endl;
				cout << "Max Z =" << setw(6) << matriz_Simplex[0][11] << endl;
			
				for (linha = 1; linha <= 5; linha++)
				{
					cout << vet_id_ln_matriz[linha] << " = " << setw(6) << matriz_Simplex[linha][11] << endl;
				}
				fim = true;
			}
			else //Ainda tenho valor negativo na linha de Z. Então preciso repetir processo para está coluna até torná-al positiva
			{
				cout << "Ainda ha valor negativo na linha de Z. As etapas de 4 a 8 serao repetidas ate torna-la positiva." << endl;
				cout << endl;
				//ao término de cada loop o usuário será indagado se deseja continuar ou não
				continuar = 9;
				while ((continuar != 0) && (continuar != 1))
				{
					cout << "Deseja repetir? [ 0 - Nao, 1 - Sim ]" << endl;
					cin >> continuar;
				}
				if (continuar == 0)
					fim = true;
			}
		}
	}
	/* --- 19-05-2020 - adicionei implementação para cálculo do limite de variação [range]
	fórmula : b = (S*)∆b + b* ≥ 0
	*/
	
	bool fim = false;
	while (!fim)
	{
		continuar = 9;
		while ((continuar != 0) && (continuar != 1))
		{
			cout << "Deseja calcular o limite de variacao [range]? [ 0 - Nao, 1 - Sim ]" << endl;
			cin >> continuar;
		}
		if (continuar == 0)
			fim = true;
		else // continuar == 1 - início o cálculo dos limites de variação
		{
			/*meu programa prevê um máximo de 5 variáveis basicas e 5 variáveis de folga, então o looping irá considerar 
			indice = 5  = b(indice) = b(i)*/
			for (indice = 1; indice <= 5; indice++)
			{
				int vet_Delta_b[6] = { 0 }; /*apesar de ser de tamanho 6, irei utilizar apenas 5 posições, sendo a 1a indice=1. facilita acompanhar o contador do For.
											declaro dentro do for pois preciso zerá-lo a cada loop de cálculo.*/
				/*--- fórmula : b = (S*)∆b + b* ≥ 0
				1o calculo o vet_Delta_b que será utilizado para somar com o resultado da multiplicação. 
				Como irei modificar os valores do lado direito apenas e de forma que os demais zerem, só preciso considerar 1 alteração por vez.
				[l,c]	Delta B  |  B^-(modificado) | B (original Lado Direito)
				[1,11]   300	 |	600				|	300		[*2 no 1o looping]
				[2,11]	 0		 |	200				|	200
				[3,11]	 0		 |   20				|	 20
				[4,11]	 0		 |   50				|	 50
				[5,11]	 0		 |	  0				|	  0
				
				---*/
				float vet_Ranges[6] = { 0 }; /*mesma ideia do int vet_Delta_b[6], porém aqui, para cada looping será armazenado o range de valores
										   para o b(i)*/
				vet_Delta_b[indice] = matriz_Restricao_FO[indice][7]; // fazendo o delta igual ao elemento é como se fosse o dobro;
				/*for (linha = 1; linha <= 5; linha++)
				{*/
					/*	// '(1*0) + (-1,33*∆b2) + (0*0)+(-0,5*0) + 8,33 >=0
					vet_Ranges[linha] = (matriz_Simplex[linha][6] * vet_Delta_b[1]) + (matriz_Simplex[linha][7] * vet_Delta_b[2])
						+ (matriz_Simplex[linha][8] * vet_Delta_b[3]) + (matriz_Simplex[linha][9] * vet_Delta_b[4])
						+ (matriz_Simplex[linha][10] * vet_Delta_b[5]) + matriz_Simplex[linha][11];

						Para cada b(i) a ser calculado na fórmula:
						- não vou incluir sua multiplicação na soma dos elementos;
						- ao término da multiplicação e soma de todos os elementos da linha de b(i), multiplico o resultado por -1 e 
						 então divido pelo coeficiente que iria multiplicar b(i), achando o valor do range
						- impeço a divisão caso o coeficiente que iria multiplicar b(i) for igual a zero
						*/
					switch (indice) 
					{
						case 1: //b(1)
							for (linha = 1; linha <= 5; linha++)
							{
								if (matriz_Simplex[linha][6] != 0)
									vet_Ranges[linha] = (((matriz_Simplex[linha][7] * vet_Delta_b[2])
										+ (matriz_Simplex[linha][8] * vet_Delta_b[3]) + (matriz_Simplex[linha][9] * vet_Delta_b[4])
										+ (matriz_Simplex[linha][10] * vet_Delta_b[5]) + matriz_Simplex[linha][11]) * (-1)) / matriz_Simplex[linha][6];
								else
									vet_Ranges[linha] = 0;
							}
							break;

						case 2: //b(2)
							for (linha = 1; linha <= 5; linha++)
							{
								if (matriz_Simplex[linha][7] != 0)
									vet_Ranges[linha] = (((matriz_Simplex[linha][6] * vet_Delta_b[1])
										+ (matriz_Simplex[linha][8] * vet_Delta_b[3]) + (matriz_Simplex[linha][9] * vet_Delta_b[4])
										+ (matriz_Simplex[linha][10] * vet_Delta_b[5]) + matriz_Simplex[linha][11]) * (-1)) / matriz_Simplex[linha][7];
								else
									vet_Ranges[linha] = 0;
							}
							break;

						case 3: //b(3)
							for (linha = 1; linha <= 5; linha++)
							{
								if (matriz_Simplex[linha][8] != 0)
									vet_Ranges[linha] = (((matriz_Simplex[linha][6] * vet_Delta_b[1])
										+ (matriz_Simplex[linha][7] * vet_Delta_b[2]) + (matriz_Simplex[linha][9] * vet_Delta_b[4])
										+ (matriz_Simplex[linha][10] * vet_Delta_b[5]) + matriz_Simplex[linha][11]) * (-1)) / matriz_Simplex[linha][8];
								else
									vet_Ranges[linha] = 0;
							}
							break;
							
						case 4: //b(4)
							for (linha = 1; linha <= 5; linha++)
							{
								if (matriz_Simplex[linha][9] != 0)
									vet_Ranges[linha] = (((matriz_Simplex[linha][6] * vet_Delta_b[1])
										+ (matriz_Simplex[linha][7] * vet_Delta_b[2]) + (matriz_Simplex[linha][8] * vet_Delta_b[3])
										+ (matriz_Simplex[linha][10] * vet_Delta_b[5]) + matriz_Simplex[linha][11]) * (-1)) / matriz_Simplex[linha][9];
								else
									vet_Ranges[linha] = 0;
							}
							break;

						case 5: //b(5)
							for (linha = 1; linha <= 5; linha++)
							{
								if (matriz_Simplex[linha][10] != 0)
									vet_Ranges[linha] = (((matriz_Simplex[linha][6] * vet_Delta_b[1])
										+ (matriz_Simplex[linha][7] * vet_Delta_b[2]) + (matriz_Simplex[linha][8] * vet_Delta_b[3])
										+ (matriz_Simplex[linha][9] * vet_Delta_b[4]) + matriz_Simplex[linha][11]) * (-1)) / matriz_Simplex[linha][10];
								else
									vet_Ranges[linha] = 0;
							}
							break;
					}
				//}
				/* --- imprimo o resultado para cada b(i)  --- */
				cout << "Linha " << indice << " - Limite Variacao [Range]: " << endl;
				for (coluna = 1; coluna <= 5; coluna++)
					cout << setw(10) << "| " << vet_Ranges[coluna];
				cout << endl; cout << endl;
			}
			fim = true;
		}

	}

	return 0;
}