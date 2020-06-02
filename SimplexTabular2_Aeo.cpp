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
25-05-2020 - modifiquei para aceitar tipos diferentes de problemas:
1 - Maximização com sinais únicos e mistos (=, <= e >=)
2 - Minimização com sinais únicos e mistos (=, <= e >=)
- Quando de Minimização, resolvo com a Maximização negativa da função objetivo 
- Se <= utiliza-se as variáveis de folga / slack
- Se = apenas, não precisa de variável de folga
- Se >=, aplica-se o Simplex + Big M, com variaveis de folga, excedente e artificial
-------------------------------------
30-05-2020 - abaixo exemplos que foram utilizados para os testes:
-------------------------------------
Exemplo n1:
Max 	Z = 10x1 + 20x2 + 30x3

Sujeito a:
	2x1 + 2x2 + 4x3 <= 300
			   4x2 + 3x3 <= 200
	x1                         <= 20
	4x1 + 3x2           <= 50
	x1, x2, x3 >=0
-------------------------------------
Exemplo n2:
Max z = 3x1 + 5x2
Sujeito a:	Eq.1	x1 +x3 = 4
	Eq.2	2x2 +x4 = 12
	Eq.3	3x1 +2x2 +x5 = 18
		x1, x2, x3, x4, x5 ≥ 0

-------------------------------------
Exemplo n3:	Big M
Max P = x1 - x2 + 3x3

subject to		x1 + x2 <= 20
		x1 + x3 = 5
		x2 + x3 >= 10

	x1, x2, x3 >= 0

-------------------------------------
Exemplo n4:
Min C = 10x1 + 16x2 - 5x3

subject to
	x1 + 3x2 <= 7
	4x2 + x3 <= 5

	x1, x2, x3 >= 0


-------------------------------------
*/
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string> 
#include <cstring>

using namespace std;

/*--- função abaixo checa se o valor que está sendo informado pelo usuário é compatível com o tipo de problema escolhido, sinal de restrição informado
	e as validações de negatividade dos coeficientes e valores do lado direito de cada expressão.
	-> Se maximização e qualquer sinal (=, <= ou >=), o valor do lado direito não poderá ser negativo, os coeficientes do lado esquerdo não importam;
	-> Se minimização e sinal >= valor do lado direito qualquer valor e os coeficientes do lado esquerdo devem ser positivos;
	-> Se minimização e sinal misto (=, <= ou >=) valor do lado direito positivo e os coeficientes do lado esquerdo qualquer valor;
	*/
bool valor_ok(int tipo_problema, string sinal_restricao, bool lado_direito, bool misto, float valor, string &msg_erro, bool ha_coef_negativo)
{
	msg_erro = "";
	if (!ha_coef_negativo) //este bloco não considera o coeficiente negativo
	{
		if ((tipo_problema == 1) & (lado_direito) & (valor < 0)) //maximização
		{
			msg_erro = " Maximizacao nao permite valor negativo do lado direito.";
			return false;
		}
		else
			if ((tipo_problema == 2) & (sinal_restricao == ">=") & (!lado_direito) & (valor < 0)) //minimização
			{
				msg_erro = " Minimizacao com sinal >= nao permite valor negativo nos coeficientes.";
				return false;
			}
			else
				if ((tipo_problema == 2) & (lado_direito) & (misto) & (valor < 0)) //minimização
				{
					msg_erro = " Minimizacao com sinais mistos nao permite valor negativo do lado direito.";
					return false;
				}
				else 
					return true;
	}
	else /*ao entrar aqui o usuário acaba de informar o sinal e só preciso validar se for minimização, sinal >= e não for misto, 
		 evitando coeficientes negativos*/
	{
		if ((tipo_problema == 2) & (sinal_restricao == ">=") & (!lado_direito) & (!misto)) //minimização
		{
			msg_erro = "_Minimizacao nao mista com sinal >= nao permite valor negativo nos coeficientes da FO. Mude o sinal ou ajuste a FO.";
			return false; 
		}
		else
			return true;
	}
}
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
	cout << " --- ===================   CONSIDERACOES  ==================== ---" << endl;
	cout << " Esta implementacao tambem funciona com Maximizacao na forma padrao (todas as restricoes com sinal de = )." << endl;
	cout << endl; cout << endl;
	/*--------termino bloco para informar as Etapas do Método Simplex Tabular ---*/
	/* --- inicio bloco declaração de variáveis do programa ---*/
	int tipo_Problema = 0; // 1 Maximização e 2 Minimização - será a 1a escolha do usuário
	int num_elem_FO = 0; //número de elementos da Função Objetivo
	int indice; //indice a ser utilizado nas instruções de looping
	int vet_MaxZ[6] = { 0 }; /*vetor que irá manter os valores da FO para cálculos. Uso o indice conforme posição do elemento, ou seja,
					 [1]=1o elemento, [2]=2o elemnto etc. Por ser inteiro irei tratar a letra Z fora do vetor*/
	string s_MaxZ; /*string que irá manter a função objetivo informada pelo usuário no formato original Max Z= .*/
	string s_MinZ; /*string que irá manter a função objetivo informada pelo usuário no formato original Min Z= .*/
	bool e_MinZ = false; //flag que irá informar que a FO inicial é Minimização e foi convertida em - Max
	string s_EquacaoZ; /*string que irá manter a equação após igualar termos a zero*/
	string v_resultado; //variavel de texto para concatenar informações e exibir algum resultado ao usuário
	int num_restricoes_FO; //número de restrições da Função Objetivo
	string vet_sinal_Restricao[6]; /* O usuário irá informar: 1 - =, 2 <= e 3 - >=. Mas será armazenado o símbolo. Apesar de tamanho 6, só será usado as posições de 1 a 5, para facilitar a identificação de linha 1 da restrição com a posicao 1 do vetor. 
								   limitado a 5 restrições no máximo, assim, cada posição neste vetor corresponde a uma restrição*/
	float matriz_Restricao_FO[6][8] = { 0, 0 }; /* 6 linhas x 8 colunas. Irei usar a partir do indice=1. Sendo a coluna[6]=conteúdo variavel de folga,
								   e a coluna[7]=valor desigualdade e igualdade.*/
	int linha, coluna; // indices das linhas e colunas em uma matriz
	int continuar; //irá obter do usuário ao término de cada loop realizado entre as etapas 4 e 8, se deverá continuar ou não
	int n_iteracoes = 0;
	float matriz_Simplex[6][12] = { 0, 0 };
	/*
		coluna 0 = códigos que correspondem a Z[0], variáveis comuns de 1 a 5, variáveis de folgas 6 a 10.
		colunas 1 a 5 = conteúdo das variáveis comuns
		colunas 6 a 10 = conteúdo das variáveis de folgas
		coluna 11 = valor Lado Direito, depois do sinal de igualdade.
		E as linhas irão conter os valores de cada variável conforme ID da coluna associada
	*/
	bool ha_coef_negativo = false; //flag que informará se há coeficiente negativo na Função Objetiva
	float vet_Nova_Ln_Pivo[12] = { 0 };
	char vet_cab_matriz[12][4] = { " ", "X1", "X2", "X3", "X4", "X5", "F1", "F2", "F3", "F4", "F5", "LD" };
	string vet_id_ln_matriz[6] = { "Z", "F1", "F2", "F3", "F4", "F5" }; /*modifico abaixo conforme escolha do usuário*/
	cout.precision(2); //fixo exibição de 3 casas decimais
	cout.showpoint; //exibir números inteiros e decimais, sem converter para numero exponencial
	cout.fixed;
	/* --- variáveis para atender ao Big M ---*/
	int M = 1000000; //valor da constante Big M

	/* --- termino bloco declaração de variáveis do programa ---*/
	cout << " Informe o Tipo de Problema (1 - Maximizacao ou 2 - Minimizacao)." << endl;
	cout << "     [0 encerrar o programa.]: ";
	cin >> tipo_Problema;
	/* verifico se foi digitado 0, 1 ou 2 */
	while ((tipo_Problema < 0) || (tipo_Problema > 2))
	{
		cout << "Numeros aceitos: 0, 1 ou 2." << endl;
		cin >> tipo_Problema;
	}
	if (tipo_Problema == 2)
	{
		e_MinZ = true;
		tipo_Problema = 1;
	}
	if (tipo_Problema != 0)
	{
		cout << " Informe o numero de elementos de sua Funcao Objetivo (ex.: 10x1 + 20x2 + 30x3, serao 3 elementos)." << endl;
		cout << "     [0 encerrar o programa.]: ";
		cin >> num_elem_FO;
		/* verifico se não foi digitado um valor negativo ou maior que 5 elementos */
		while ((num_elem_FO < 0) || (num_elem_FO > 5))
		{
			cout << "Limite maximo de 5 elementos." << endl;
			cin >> num_elem_FO;
		}
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
			if (vet_MaxZ[indice] < 0)
				ha_coef_negativo = true;
			indice++;
		}
		/* --- termino bloco para ler o valor de cada elemento --- */
		/* --- realizo o passo 1
		  1a Passar todos os elementos da funcao objetivo para o outro lado da equacao igualando a zero.
		  ---*/
		if (!e_MinZ)
			v_resultado = " Max Z= ";
		else
		{
			v_resultado = " Min Z= ";
			for (indice = 1; indice <= (num_elem_FO); indice++)
			{
				v_resultado += to_string(vet_MaxZ[indice]) + "x";
				if (indice == num_elem_FO)
					v_resultado += to_string(indice);
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
			s_MinZ = v_resultado; /*guardo a FO nesta string para posterior exibição: Min Z= */
			/*inverto o sinal *-1, para converter Min Z em - Max Z */
			for (indice = 1; indice <= (num_elem_FO); indice++)
			{
				vet_MaxZ[indice] = vet_MaxZ[indice] * -1;
			}
			v_resultado = " - Max Z= "; 
		}

		for (indice = 1; indice <= (num_elem_FO); indice++)
		{
			v_resultado += to_string(vet_MaxZ[indice]) + "x";
			if (indice == num_elem_FO)
				v_resultado += to_string(indice);
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
		}

		  /* Questiono ao usuário o número de Restrições para a Função Objetivo informada.
		   A restrição de N.N. não será preciso informar (ex.: x1, x2 ...xn >=0) */
		cout << "Qual o numero de Restricoes? (1a: 2x1 + 2x2 + 4x3 <= 300, 2a: 4x2 + 3x3 <= 200, 3a: x1 <= 20 e 4a: 4x1 + 3x2 <= 50, neste exemplo, seriam 4)." << endl;
		cout << "     [Limite maximo de 5]: ";
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
		string sinal_anterior, msg_erro;
		bool e_misto = false; //inicializo como falso pois na 1a linha ainda não tenho comparativo

		for (linha = 1; linha <= num_restricoes_FO; linha++)
		{
			if (linha > 1) //só começo a comparar a partir da linha 2
				sinal_anterior = vet_sinal_Restricao[linha - 1];

			cout << "Entre com os elementos da restricao No " << linha << "." << endl;
			cout << "Sequencia: x1, x2, x3, x4, x5, sinal (=, >= ou <=) e valor do lado direito. Zero anula o elemento. " << endl;
			for (coluna = 1; coluna < 7; coluna++)
			{
				//x1, x2, x3, x4, x5
				if (coluna < 6)
				{
					cout << "Entre com o coeficiente de x" << coluna << " .: ";
					cin >> matriz_Restricao_FO[linha][coluna];

					/*--- sendo a última coluna do lado esquerdo, peço que informe o sinal (1 =, 2 <= ou 3 >=), para compor a expressão ---*/
					if (coluna == 5)
					{
						int sinal_escolhido = 9;
						while ((sinal_escolhido < 1) || (sinal_escolhido > 3))
						{
							cout << "Informe o sinal (1 =, 2 <= ou 3 >=) da expressao:" << endl;
							cin >> sinal_escolhido;
						}
						if (sinal_escolhido == 1)
						{
							vet_sinal_Restricao[linha] = "=";
							if ((msg_erro.find("_") == 0) & (ha_coef_negativo))
								ha_coef_negativo = false;
						}
						else
							if (sinal_escolhido == 2)
								vet_sinal_Restricao[linha] = "<=";
							else
							{
								vet_sinal_Restricao[linha] = ">=";
								if ((msg_erro.find("_") == 0) & (ha_coef_negativo))
									ha_coef_negativo = false;
							}
						if (linha > 1) //só começo a comparar a partir da linha 2
						{
							if (vet_sinal_Restricao[linha] != vet_sinal_Restricao[linha - 1])
								e_misto = true;
							else
								e_misto = false;
						}
						if (!valor_ok(tipo_Problema, vet_sinal_Restricao[linha], false, e_misto, matriz_Restricao_FO[linha][7], msg_erro, ha_coef_negativo))
						{
							cout << msg_erro << endl;
							coluna--; //reduzo a coluna para voltar a solicitar o valor novamente
						};
					}

				}
				else
				{
					cout << "Entre com o valor do Lado Direito do sinal " << vet_sinal_Restricao[linha] << " :";
					cin >> matriz_Restricao_FO[linha][7];
					if (!valor_ok(tipo_Problema, vet_sinal_Restricao[linha], true, e_misto, matriz_Restricao_FO[linha][7], msg_erro, false))
					{
						cout << msg_erro << endl;
						coluna--; //reduzo a coluna para voltar a solicitar o valor novamente
					};
				}

			}
			cout << endl;
		}
		/* --- termino bloco para ler o valor de cada restrição --- */
		cout << endl;
		cout << "-- ====================================== --" << endl;

		/*--- Verifico se é um conjunto de restrições mistas. Caso seja, devo adicionar o coeficiente Big M para cada 
		variável artificial adicionada. Ma(i) + Ma(i+1) ...etc*/
		int num_var_artificial = 1;
		if ((tipo_Problema == 1) & (e_misto))
		{
			for (linha = 1; linha <= num_restricoes_FO; linha++)
			{
				if ((vet_sinal_Restricao[linha] == "=") || (vet_sinal_Restricao[linha] == ">="))
				{
					/*s_MaxZ é a FO inicial*/
					s_MaxZ += " + Ma" + to_string(num_var_artificial);
					/*v_resultado é a expressão já igualada a zero*/
					v_resultado += " - Ma" + to_string(num_var_artificial);
					num_var_artificial++;
				}
			}
		}
		s_EquacaoZ = v_resultado;
		cout << endl;
		cout << endl;
		cout << "1a Passar todos os elementos da funcao objetivo para o outro lado da equacao igualando a zero." << endl;
		cout << endl;
		/* --- imprimo a função objetivo para conferencia --*/
		if (e_MinZ) //informo que por se tratar de uma Min será aplicada a fórmula: Min Z = - Max Z
		{
			cout << "Por se tratar de uma Minimizacao, sera aplicada a formula: Min Z = - Max Z. " << endl;
			cout << s_MinZ << endl;
		}
		cout << "Sua Funcao Objetivo eh " << s_MaxZ << endl;
		cout << "E a equacao eh " << s_EquacaoZ << " = 0";
		cout << endl;
		cout << endl;
		cout << "-- ====================================== --" << endl;

		/*--- Modifico os IDs das linhas da Matriz 
		  --> Maximização não mista sinal <=, mantenho os IDs da inicialização do vetor, ou seja, não altero nada aqui 
		  --> Maximização não mista sinal =, eu altero os IDs 
		  --> Maximização mista, ou seja, com sinais: =, <= e >=, nas restrições, também considero os IDs da inicialização 
		---*/
		if ((tipo_Problema == 1) & (vet_sinal_Restricao[1] == "=") & (!e_misto)) /*Maximização não mista sinal = */
		{
			/*atribuo a identificação das linhas conforme escolha do usuário
			 ex1: Maximização não mista com sinal <= tenho: vet_id_ln_matriz[6] = { "Z", "F1", "F2", "F3", "F4", "F5" };
			ex2: Maximização não mista com sinal = tenho: vet_id_ln_matriz[6] = { "Z", "X1", "X2", "X3", "X4", "X5" };*/
			string vet_id_ln_matriz_aux[6] = { "Z", "X1", "X2", "X3", "X4", "X5" };
			for (coluna = 0; coluna <=5; coluna++)
				vet_id_ln_matriz[coluna] = vet_id_ln_matriz_aux[coluna];
		}
		/*--- Início Bloco - Maximização, sinal <= e não é um conjunto de restrição mista ---*/
		if ((tipo_Problema == 1) & (vet_sinal_Restricao[1] == "<=") & (!e_misto))
		{
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
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;
			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
				}
				cout << endl;
			}
			cout << endl;

		}
			else
		/*--- Término Bloco - Maximização, sinal <= e não é um conjunto de restrição mista ---*/
		/*--- Início Bloco - Maximização, sinal = e não é um conjunto de restrição mista
		  Não é preciso adicionar variáveis de folga e nenhum outro tipo---*/
		if ((tipo_Problema == 1) & (vet_sinal_Restricao[1] == "=") & (!e_misto))
		{
			/* --- imprimo a função objetivo para conferencia --*/
			cout << "2a Etapa- Exibicao da representacao do problema (=)." << endl;
			cout << endl;
			for (linha = 1; linha <= (num_restricoes_FO); linha++)
			{
				for (coluna = 1; coluna < 8; coluna++)
				{
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
									cout << "x" << coluna;
								}
								else
								{
									cout << setw(3) << matriz_Restricao_FO[linha][coluna];
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
			/*--- atribuindo as linhas que correspondem às variáveis.
			linhas:
			F1=matriz_Simplex[1][1] = matriz_Restricao_FO[1][1],  F2=matriz_Simplex[2][1] = matriz_Restricao_FO[2][1] etc
			colunas:
			x1=matriz_Simplex[1][1], x2=matriz_Simplex[1][2], x3=matriz_Simplex[1][3] ...até x5=matriz_Simplex[1][5],
			coluna 11 = matriz_Simplex[1][11] = valor Lado Direito, depois do sinal de igualdade. ---*/
			for (linha = 1; linha <= 5; linha++)
				for (coluna = 1; coluna <= 7; coluna++)
				{
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
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			}
			cout << endl;
			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
				}
				cout << endl;
			}
			cout << endl;
		}
		   else
		/*--- Término Bloco - Maximização, sinal = e não é um conjunto de restrição mista ---*/
		/*--- Início Bloco - Maximização com um conjunto de restrição mista, contendo sinais: =, >= e <= ---*/
		if ((tipo_Problema == 1) & (e_misto))
		{
			/* --- imprimo a função objetivo para conferencia --*/
			cout << "2a Etapa- Transformar as desigualdades em igualdades e acrescentando as variaveis de folga e Artificiais." << endl;
			cout << endl;
			cout << "As equacoes sao: " << endl;
			cout << endl;
			/*
			0,3X1 + 0,1X2 + X3 = 2,7					[X3 = variável de folga]
			0,5X1 + 0,5X2         + X4 = 6				[x4 = variável artificial]
			0,6X1 + 0,4X2                 -X5 + X6 = 6 	[X5 = variável de folga a ser subtraída; X6 = variável artificial]

			*/
			int id_variavel = 1; //contador que irá definir o número sequencial das variáveis
			for (linha = 1; linha <= (num_restricoes_FO); linha++)
			{
				for (coluna = 1; coluna < 8; coluna++)
				{
					if (((coluna == 6) & (vet_sinal_Restricao[linha] == "=")) || ((coluna == 6) & (vet_sinal_Restricao[linha] == "<="))) /*irei tratar a variavel de folga e artificial da mesma maneira para os sinais = e <=, atribuo valor 1 para ela ser exibida*/
						matriz_Restricao_FO[linha][coluna] = 1;
					else /* 2 irá representar uma negativa e outra positiva na mesma restrição: -X5 + X6 */
						if ((coluna == 6) & (vet_sinal_Restricao[linha] == ">="))
							matriz_Restricao_FO[linha][coluna] = 2;
					/*Somente elementos com contéudo diferente de 0 serão exibidos na equação */
					if (matriz_Restricao_FO[linha][coluna] != 0)
					{
						if (coluna < 7) /* colunas menores que 7 guardam os valores antes do sinal de igualdade (=) */
						{
							/* checo se o elemento da matriz é positivo ou negativo para tratar o sinal a ser exibido na equação*/
							if (matriz_Restricao_FO[linha][coluna] > 0)
							{
								if (coluna > 1) /* a partir da 2a coluna preciso exibir o sinal de +, se positivo */
								{   /* 2 irá representar uma negativa e outra positiva na mesma restrição: -X5 + X6 */
									if (matriz_Restricao_FO[linha][coluna] == 2)
									{
										cout << " -" << setw(3) << (matriz_Restricao_FO[linha][coluna] - 1) << "F" << id_variavel;//variavel de folga negativa
										cout << " +" << setw(3) << (matriz_Restricao_FO[linha][coluna] - 1) << "A" << ++id_variavel;//variavel artificial 
										id_variavel++; //incremento apenas após usa-la
									}
									else
									{
										cout << " +" << setw(3) << matriz_Restricao_FO[linha][coluna];
										if ((coluna == 6) & (vet_sinal_Restricao[linha] == "<=")) //variavel de folga
										{
											cout << "F" << id_variavel;
											id_variavel++; //incremento apenas após usa-la
										}
										else
											if ((coluna == 6) & (vet_sinal_Restricao[linha] == "=")) //variavel Artificial
											{
												cout << "A" << id_variavel;
												id_variavel++; //incremento apenas após usa-la
											}
											else
												cout << "X" << coluna;//este id segue a sequencia da coluna
										
									}
								}
								else
								{
									cout << setw(3) << matriz_Restricao_FO[linha][coluna];
									if ((coluna == 6) & (vet_sinal_Restricao[linha] == "<=")) //variavel de folga
									{
										cout << "F" << id_variavel;
										id_variavel++; //incremento apenas após usa-la
									}
									else
										if ((coluna == 6) & (vet_sinal_Restricao[linha] == "=")) //variavel Artificial
										{
											cout << "A" << id_variavel;
											id_variavel++; //incremento apenas após usa-la
										}
										else
											cout << "X" << coluna;
								}
							}
							else /* se elemento com sinal negativo, não preciso tratar pois já é exibido por padrão*/
								cout << setw(3) << matriz_Restricao_FO[linha][coluna] << "x" << coluna << " ";
						}
						else
							if (coluna == 7) /* coluna com o valor após sinal de igualdade (=) */
								cout << " = " << fixed << showpoint << matriz_Restricao_FO[linha][coluna];

					}
					
				}
				cout << endl;

			}
			cout << endl;
			cout << endl;

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
			int increm_coluna = 0; /*contador que irá ajustar alinhamento da coluna para se obter os valores após identificar necessidade de variavel artificial e de folga na mesma linha*/
			for (linha = 1; linha <= 5; linha++)
				for (coluna = 1; coluna <= 7; coluna++)
				{
					if ((linha >= 1) && ((coluna) == (6))) //trato o alinhamento dos valores das colunas das variáveis de folga e artificiais
					{
						/* 2 irá representar uma negativa e outra positiva na mesma restrição: -X5 + X6 */
						if (matriz_Restricao_FO[linha][coluna] == 2)
						{
							matriz_Simplex[linha][(coluna + increm_coluna) + linha - 1] = -1;
							matriz_Simplex[linha][(coluna + increm_coluna) + linha] = 1;
							/*Atribuo também o valor da constante Big M na linha Z*/
							matriz_Simplex[0][(coluna + increm_coluna) + linha] = M;
							increm_coluna += 1;
						}
						else
						{
							matriz_Simplex[linha][(coluna + increm_coluna) + linha - 1] = matriz_Restricao_FO[linha][coluna];
							if ((coluna == 6) & (vet_sinal_Restricao[linha] == "=")) //variavel Artificial
							{
								/*Atribuo também o valor da constante Big M na linha Z*/
								matriz_Simplex[0][(coluna + increm_coluna) + linha -1] = M;
							}
						}
					}
					else
						if (coluna == 7) //jogo os valores após sinal de desigualdade para o lado direito do sinal
						{
							matriz_Simplex[linha][coluna + 4] = matriz_Restricao_FO[linha][coluna];
						}
						else
							matriz_Simplex[linha][coluna] = matriz_Restricao_FO[linha][coluna];
				}
			cout << endl;
			cout << "-- ====================================== --" << endl;
			cout << "Etapa 3.1 - Preencher a Tabela Simplex com os dados das equacoes." << endl;
			cout << endl;
			/* --- imprimo cabeçalho da matriz Simplex --- */
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;
			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
				}
				cout << endl;
			}
			cout << endl;
			
			cout << "-- ====================================== --" << endl;
			cout << "Etapa 3.2 Calculo nova linha 0: Linha0 = linha0 - (M * linha2) - (M * linha3)." << endl;
			for (coluna = 1; coluna <= 11; coluna++)
			{
				matriz_Simplex[0][coluna] = matriz_Simplex[0][coluna] - (M * matriz_Simplex[2][coluna]) - (M * matriz_Simplex[3][coluna]);
					
			}
			cout << "-- Tabela Simplex com Nova Linha 0: --" << endl;
			for (coluna = 1; coluna <= 11; coluna++)
				cout << setw(10) << "| " << vet_cab_matriz[coluna];
			cout << endl;
			for (linha = 0; linha <= 5; linha++)
			{
				cout << setw(2) << vet_id_ln_matriz[linha];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
				}
				cout << endl;
			}
			cout << endl;
		}
		/*--- Término Bloco - Maximização com um conjunto de restrição mista, contendo sinais: =, >= e <= ---*/
		bool fim = false;
		while (!fim)  /*--- looping que irá garantir a repetição das etapas de 4 a 8, enquanto houver coeficiente negativo na linha Z ---*/
		{
			n_iteracoes++; //contabilizo o número de iterações entre as etapas 4 e 8
			/* ---
			4a Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo.;
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "4a Etapa - Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo." << endl;
			/*--- percorro a linha 0 = Eq.0 = Z para encontrar o maior número negativo. Verifico da coluna 1 à 5 ---*/
			int coluna_maior_negativo = 0; //coluna_maior_negativo = coluna pivô
			float maior_negativo = 0;
			for (coluna = 1; coluna <= 10; coluna++) //BIG M considero as colunas das variáveis de folga e artificiais também
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
			float menor_razao = 10000;
			float valor_pivo = 0;
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
			if ((vet_sinal_Restricao[1] == "<=") & (!e_misto))
			{
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
			}
			else
				/*--- atualizo o ID da linha de variável de folga para o X da coluna, caso seja de sinais mistos ---*/
				if (e_misto)
				{
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
									else
										if (coluna_maior_negativo == 6)
											vet_id_ln_matriz[linha_menor_razao] = "F1";
										else
											if (coluna_maior_negativo == 7)
												vet_id_ln_matriz[linha_menor_razao] = "F2";
											else
												if (coluna_maior_negativo == 8)
													vet_id_ln_matriz[linha_menor_razao] = "F3";
												else
													if (coluna_maior_negativo == 9)
														vet_id_ln_matriz[linha_menor_razao] = "F4";
													else
														if (coluna_maior_negativo == 10)
															vet_id_ln_matriz[linha_menor_razao] = "F5";
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
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
				}
				cout << endl;
			}
			cout << endl;
			cout << "Nova Linha Pivo: " << endl;
			cout << setw(2) << " ";
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << setw(2) << " " << fixed << showpoint << setw(8) << vet_Nova_Ln_Pivo[coluna] << ", ";
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
					cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][coluna] << ", ";
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
			float valor_negativo = 0;
			for (coluna = 1; coluna <= 11; coluna++)
			{
				cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[0][coluna] << ", ";

				if ((matriz_Simplex[0][coluna] < 0) & (coluna < 11))
					valor_negativo = matriz_Simplex[0][coluna];
			}
			cout << endl;
			/*--- Início bloco Ordenação resultado
			ordeno os IDs das linhas antes de exibi-las ex.: até chegar a este ponto posso ter F1, X3, F3, X1, X2, e o
					ideal, para facilitar a leitura deve ser X1, X2, X3, F1, F3, vinculadas às suas respectivas informações ---*/
			string vet_id_ln_matriz_aux1[6]; //receberá a cada loop a posição atual dos IDs das linhas da Tabela Simplex
			string vet_id_ln_matriz_aux2[6]; /*--- vet_id_ln_matriz_aux é vetor que irá conter os IDs ordenados ao final dos 2 laços for abaixo.
											E o vetor vet_id_ln_matriz será mantido para obtermos o Indice da linha correspondente na
											matriz_Simplex ---*/
			int contador = 5;
			string menor_valor, valor_em_troca;
			for (coluna = 0; coluna <= 5; coluna++)
			{
				vet_id_ln_matriz_aux1[coluna] = vet_id_ln_matriz[coluna];
			}

			for (indice = 5; indice >= 0; indice--)
			{
				menor_valor = vet_id_ln_matriz_aux1[indice];
				for (coluna = 0; coluna <= indice; coluna++)
				{
					if (menor_valor > vet_id_ln_matriz_aux1[coluna])
					{
						valor_em_troca = menor_valor;
						menor_valor = vet_id_ln_matriz_aux1[coluna];
						vet_id_ln_matriz_aux1[coluna] = valor_em_troca;
					}
				}
				vet_id_ln_matriz_aux2[contador - indice] = menor_valor;
			}
			string conteudo1, conteudo2;
			/*--- imprimo as linhas das variáveis X e todas as colunas associadas */
			for (coluna = 0; coluna <= 5; coluna++)
			{
				conteudo1 = vet_id_ln_matriz_aux2[coluna];
				if (conteudo1.find("X") == 0)
				{
					for (linha = 0; linha <= 5; linha++)
					{
						conteudo2 = vet_id_ln_matriz[linha];
						if (conteudo2.find(conteudo1) == 0)
						{
							cout << setw(2) << vet_id_ln_matriz_aux2[coluna];
							for (indice = 1; indice <= 11; indice++)
							{
								cout << setw(2) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][indice] << ", ";
							}
							cout << endl;
						}
					}
				}
			}
			/*--- Término bloco Ordenação resultado --*/
			cout << endl; cout << endl;
			/*--- havendo valor negativo, informo ao usuário e repito etapas de de 4 a 8, novamente até não existir valor negativo ---*/
			if (valor_negativo == 0) /* Fim de jogo - Sim, a solução é ótima.*/
			{
				cout << "Sim, a solucao eh otima e possui os seguintes resultados: " << endl;
				cout << endl;
				if (!e_MinZ)
					cout << "Max Z =" << setw(6) << matriz_Simplex[0][11] << endl;
				else
					cout << "- Max Z =" << setw(6) << (-1* matriz_Simplex[0][11]) << endl;

				/*--- Variáveis X e seus valores ---*/
				cout << endl; cout << endl;
				/*--- imprimo as linhas das variáveis X e apenas o valor do lado direito */
				cout << "VARIAVEL X	| VALOR" << endl;
				for (coluna = 0; coluna <= 5; coluna++)
				{
					conteudo1 = vet_id_ln_matriz_aux2[coluna];
					if (conteudo1.find("X") == 0)
					{
						for (linha = 0; linha <= 5; linha++)
						{
							conteudo2 = vet_id_ln_matriz[linha];
							if (conteudo2.find(conteudo1) == 0)
							{
								cout << setw(2) << vet_id_ln_matriz_aux2[coluna] << setw(20) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][11] << endl;
							}
						}
					}
				}
				/*--- imprimo as linhas das variáveis de FOLGA (F) e apenas o valor do lado direito */
				cout << endl; cout << endl;
				cout << "VARIAVEL DE FOLGA | VALOR" << endl;
				for (coluna = 0; coluna <= 5; coluna++)
				{
					conteudo1 = vet_id_ln_matriz_aux2[coluna];
					if (conteudo1.find("F") == 0)
					{
						for (linha = 0; linha <= 5; linha++)
						{
							conteudo2 = vet_id_ln_matriz[linha];
							if (conteudo2.find(conteudo1) == 0)
							{
								cout << setw(2) << vet_id_ln_matriz_aux2[coluna] << setw(20) << " " << fixed << showpoint << setw(8) << matriz_Simplex[linha][11] << endl;
							}
						}
					}
				}
				cout << endl;
				cout << "Numero de iteracoes= " << n_iteracoes << endl;
				cout << endl; cout << endl;
				fim = true;
			}
			else //Ainda tenho valor negativo na linha de Z. Então preciso repetir processo para está coluna até torná-al positiva
			{
				if (n_iteracoes >= num_restricoes_FO * 2)
				{
					fim = true;

				}
				else
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
		/* Não calculo o range para Maximização não mista com sinal e = */
		if ((tipo_Problema == 1) & (vet_sinal_Restricao[1] == "<=") & (!e_misto))
		{
			fim = false;
			while (!fim)
			{
				continuar = 9;
				while ((continuar != 0) && (continuar != 1))
				{
					cout << "Deseja calcular o limite de variacao [range] do lado direito [LD]? [ 0 - Nao, 1 - Sim ]" << endl;
					cin >> continuar;
				}
				if (continuar == 0)
					fim = true;
				else // continuar == 1 - início o cálculo dos limites de variação
				{
					cout << endl;
					/*meu programa prevê um máximo de 5 variáveis basicas e 5 variáveis de folga, então o looping irá considerar
					indice = 5  = b(indice) = b(i)*/
					cout << "LD Atual |" << setw(10) << " Incremento |" << setw(10) << "Decremento" << endl;
					for (indice = 1; indice <= 5; indice++)
					{
						float vet_Delta_b[6] = { 0 }; /*apesar de ser de tamanho 6, irei utilizar apenas 5 posições, sendo a 1a indice=1. facilita acompanhar o contador do For.
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

						/* --- imprimo o resultado para cada b(i)  --- */
						float incremento = 0.0001;
						float decremento = -0.0001;
						int tem_negativo = 0;
						int tem_positivo = 0;
						//Só será exibido se LD for diferente de zero
						if (matriz_Restricao_FO[indice][7] != 0)
						{

							for (coluna = 1; coluna <= 5; coluna++)
							{
								if (vet_Ranges[coluna] < -0.01)
								{
									if (tem_negativo == 0)
									{
										decremento = vet_Ranges[coluna];
										tem_negativo++;
									}
									else
										if (vet_Ranges[coluna] > decremento)
										{
											decremento = vet_Ranges[coluna];
											tem_negativo++;
										}
								}
								else
									if (vet_Ranges[coluna] > 0.01)
									{
										if (tem_positivo == 0)
										{
											incremento = vet_Ranges[coluna];
											tem_positivo++;
										}
										else
											if (vet_Ranges[coluna] < incremento)
											{
												incremento = vet_Ranges[coluna];
												tem_positivo++;
											}
									}

							}
							// aqui irei imprimir a análise
							cout << setw(5) << matriz_Restricao_FO[indice][7]; // ID da linha
							// incremento
							if (tem_positivo != 0)
								cout << setw(12) << incremento;
							else
								cout << setw(12) << "INF";
							//decremento
							if (tem_negativo != 0)
								cout << setw(12) << decremento;
							else
								cout << setw(12) << "INF";
							cout << endl; cout << endl;
						}
					}
					fim = true;
				}

			}
		}
	}
	cout << endl; cout << endl;
	cout << "Fim - Tecle 0 e <Enter> para encerrar." << endl; cout << endl;
	cin >> continuar;

	return 0;
}