/*-----------------------------------
30-abril-2020 - Implementa��o do M�todo Simplex Tabular

UFPB - Mestrado em Inform�tica - Disciplina: Pesquisa Operacional - Prof. Anand
Aluno: Anderson Ernani de Oliveira

Fontes de refer�ncias:
- AULA5.pdf slides apresentados durante v�deo aulas;
- https://www.youtube.com/watch?v=7qOdbo-xPaA - v�deo aula complementar.

Sobre a implementa��o: o programa ir� guiar seu usu�rio para inser��o dos dados iniciais, e � medida que as etapas abaixo forem sendo processadas,
elas ser�o exibidas para um melhor entendimento de como foi realizado cada regra.

Etapas:
- 1a Passar todos os elementos da fun��o objetivo para o outro lado da equa��o igualando a zero.
- 2a Transformar as desigualdades em igualdades e acrescentando as vari�veis de folga.
- 3a Preencher a tabela [matrix e/ou vetores de suporte] Simplex com os dados das equa��es.
- 4a Escolher a Coluna Piv�: olhar na linha Z qual � o n�mero mais negativo. Essa � a primeira Coluna piv�.
- 5a Escolher a Linha Piv�: menor resultado para LD / CP (com exece��o da linha Z).
- 6a Dividir Linha Pivo pelo numero Pivo. O resultado serah uma nova linha.
- 7a Novas linhas: Linha Antiga - (coeficiente Coluna Piv�) x Nova Linha Piv�.
- 8a Teste de Otimalidade - A solu��o � �tima se e somente se cada coeficiente da Z/eq.(0) for n�o-negativo.
- *** Repitir processo a partir do passo 4, ate se obter um resultado aceitavel.

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

	/*--------inicio bloco para informar as Etapas do M�todo Simplex Tabular ---*/
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
	/*--------termino bloco para informar as Etapas do M�todo Simplex Tabular ---*/
	/* --- inicio bloco declara��o de vari�veis do programa ---*/
	int num_elem_FO; //n�mero de elementos da Fun��o Objetivo
	int indice; //indice a ser utilizado nas instru��es de looping
	int vet_MaxZ[6] = { 0 }; /*vetor que ir� manter os valores da FO para c�lculos. Uso o indice conforme posi��o do elemento, ou seja,
					 [1]=1o elemento, [2]=2o elemnto etc. Por ser inteiro irei tratar a letra Z fora do vetor*/
	string s_MaxZ; /*string que ir� manter a fun��o objetivo informada pelo usu�rio no formato original Max Z= .*/
	string s_EquacaoZ; /*string que ir� manter a equa��o ap�s igualar termos a zero*/
	string v_resultado; //variavel de texto para concatenar informa��es e exibir algum resultado ao usu�rio
	int num_restricoes_FO; //n�mero de restri��es da Fun��o Objetivo
	int matriz_Restricao_FO[6][8] = { 0, 0 };; /* 6 linhas x 8 colunas. Irei usar a partir do indice=1. Sendo a coluna[6]=conte�do variavel de folga,
								   e a coluna[7]=valor desigualdade e igualdade.*/
	int linha, coluna; // indices das linhas e colunas em uma matriz
	int continuar; //ir� obter do usu�rio ao t�rmino de cada loop realizado entre as etapas 4 e 8, se dever� continuar ou n�o

	float matriz_Simplex[6][12] = { 0, 0 };
	/*
		coluna 0 = c�digos que correspondem a Z[0], vari�veis comuns de 1 a 5, vari�veis de folgas 6 a 10.
		colunas 1 a 5 = conte�do das vari�veis comuns
		colunas 6 a 10 = conte�do das vari�veis de folgas
		coluna 11 = valor Lado Direito, depois do sinal de igualdade.
		E as linhas ir�o conter os valores de cada vari�vel conforme ID da coluna associada
	*/
	float vet_Nova_Ln_Pivo[12] = {0};
	char vet_cab_matriz[12][4] = { " ", "X1", "X2", "X3", "X4", "X5", "F1", "F2", "F3", "F4", "F5", "LD" };
	string vet_id_ln_matriz[6] = { "Z", "F1", "F2", "F3", "F4", "F5" };
	cout.precision(2); //fixo exibi��o de 3 casas decimais
	cout.showpoint; //exibir n�meros inteiros e decimais, sem converter para numero exponencial
	cout.fixed;
	/* --- termino bloco declara��o de vari�veis do programa ---*/

	cout << " Informe o numero de elementos de sua Funcao Objetivo (ex.: 10x1 + 20x2 + 30x3, serao 3 elementos):"<<endl;
	cout << "     [0 encerrar o programa.]:" << endl;
	cin >> num_elem_FO;
	/* verifico se n�o foi digitado um valor negativo ou maior que 5 elementos */
	while ((num_elem_FO < 0) || (num_elem_FO > 5))
	{
		cout << "Limite maximo de 5 elementos." << endl;
		cin >> num_elem_FO;
	}
	/* se o usu�rio informou 0 at� este ponto, ir� encerrar o programa. */
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
				/* checo se o elemento � positivo ou negativo para tratar o sinal a ser concatenado na string*/
				if (vet_MaxZ[indice] > 0)
				{
					/* se o indice no pr�ximo la�o for o �ltimo n�o concateno o sinal de + */
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
		s_MaxZ = v_resultado; /*guardo a FO nesta string para posterior exibi��o - Max Z= */
		/* --- repito o looping anterior para passar igualar a equa��o a zero. Multiplicando o conte�do por -1. ---*/
		v_resultado = " Z";
		for (indice = 1; indice <= (num_elem_FO); indice++)
		{
			/* Multiplico por -1, antes de usar o conte�do do vetor*/
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
		/* --- imprimo a fun��o objetivo para conferencia --*/
		cout << "Sua Funcao Objetivo eh " << s_MaxZ << endl;
		cout << "E a equacao eh " << s_EquacaoZ;
		cout << endl;
		cout << endl;
		cout << "-- ====================================== --" << endl;
		/* --- realizo o passo 2
		  2a Transformar as desigualdades em igualdades e acrescentando as variaveis de folga.
		  ---*/

		  /* Questiono ao usu�rio o n�mero de Restri��es para a Fun��o Objetivo informada.
		   A restri��o de N.N. n�o ser� preciso informar (ex.: x1, x2 ...xn >=0) */
		cout << "Qual o numero de Restricoes? (1a: 2x1 + 2x2 + 4x3 <= 300, 2a: 4x2 + 3x3 <= 200, 3a: x1 <= 20 e 4a: 4x1 + 3x2 <= 50, neste exemplo, seriam 4):" << endl;
		cout << "     [Limite maximo de 5]:" << endl;
		cin >> num_restricoes_FO;
		/* verifico se foi digitado ao menos 1 restri��o, limitado ao m�ximo 5 */
		while ((num_restricoes_FO < 1) || (num_restricoes_FO > 5))
		{
			cout << "Limite maximo de 5 restricoes." << endl;
			cin >> num_restricoes_FO;
		}

		/* --- inicio bloco para ler o valor de cada restri��o --- */

		/* --- no pr�ximo looping ser� solicitado os 6 elementros [colunas] de cada restri��o [linhas].
				x1, x2, x3, x4, x5 e Vlr_igualdade.
				Quando o usu�rio n�o quiser o elementro, basta informar 0 que ele n�o ser� inserido na equa��o.
		---*/
		// o looping para a linha corresponde � quantidade de restri��es. E as colunas seus valores
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
		/* --- termino bloco para ler o valor de cada restri��o --- */
		cout << endl;
		cout << "-- ====================================== --" << endl;
		/* --- imprimo a fun��o objetivo para conferencia --*/
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
				/*Somente elementos com cont�udo diferente de 0 ser�o exibidos na equa��o */
				if (matriz_Restricao_FO[linha][coluna] != 0)
				{
					if (coluna < 7) /* colunas menores que 7 guardam os valores antes do sinal de igualdade (=) */
					{
						/* checo se o elemento da matriz � positivo ou negativo para tratar o sinal a ser exibido na equa��o*/
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
						else /* se elemento com sinal negativo, n�o preciso tratar pois j� � exibido por padr�o*/
							cout << setw(3) << matriz_Restricao_FO[linha][coluna] << "x" << coluna << " ";
					}
					else
						if (coluna == 7) /* coluna com o valor ap�s sinal de igualdade (=) */
							cout << " = " << matriz_Restricao_FO[linha][coluna];

				}
			}
			cout << endl;

		}
		cout << endl;
		cout << endl;
		cout << "-- ====================================== --" << endl;

		/*--- Realizo o passo 3
		3o Preencher a tabela Simplex com os dados das equa��es
		---*/
		/* Eq(0) - O vetor "vet_MaxZ" cont�m os valores Z - Ax1 - Bx2 - Cx3 - Dx4 - Ex5 = 0.
		  E seu conte�do ser� passado � linha [0] da matriz_Simplex */
		for (coluna = 1; coluna <= 5; coluna++)
			matriz_Simplex[0][coluna] = vet_MaxZ[coluna];
		/*--- atribuindo as linhas que correspondem �s vari�veis de folga.
		linhas:
		F1=matriz_Simplex[1][1] = matriz_Restricao_FO[1][1],  F2=matriz_Simplex[2][1] = matriz_Restricao_FO[2][1] etc
		colunas:
		x1=matriz_Simplex[1][1], x2=matriz_Simplex[1][2], x3=matriz_Simplex[1][3] ...at� x5=matriz_Simplex[1][5],
		F1=matriz_Simplex[1][6], F2=matriz_Simplex[1][7], F3=matriz_Simplex[1][8] ...at� F5=matriz_Simplex[1][10],
		coluna 11 = matriz_Simplex[1][11] = valor Lado Direito, depois do sinal de igualdade.
		 F1, F2, F3 ...Fn ---*/
		for (linha = 1; linha <= 5; linha++)
			for (coluna = 1; coluna <= 7; coluna++)
			{
				if ((linha >= 2) && (coluna == 6)) //trato o alinhamento dos valores das colunas das vari�veis de folga
					matriz_Simplex[linha][coluna + linha - 1] = matriz_Restricao_FO[linha][coluna];
				else
					if (coluna == 7) //jogo os valores ap�s sinal de desigualdade para o lado direito do sinal
						matriz_Simplex[linha][coluna + 4] = matriz_Restricao_FO[linha][coluna];
					else
						matriz_Simplex[linha][coluna] = matriz_Restricao_FO[linha][coluna];
			}
		cout << endl;
		cout << "-- ====================================== --" << endl;
		cout << "3o Etapa - Preencher a Tabela Simplex com os dados das equacoes." << endl;
		cout << endl;
		/* --- imprimo cabe�alho da matriz Simplex --- */
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
		while (!fim)  /*--- looping que ir� garantir a repeti��o das etapas de 4 a 8, enquanto houver coeficiente negativo na linha Z ---*/
		{
			/* ---
			4a Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo.;
			---*/
			cout << "-- ====================================== --" << endl;
			cout << "4a Etapa - Escolher a Coluna Pivo: olhar na linha Z qual eh o numero mais negativo. Essa eh a primeira Coluna pivo." << endl;
			/*--- percorro a linha 0 = Eq.0 = Z para encontrar o maior n�mero negativo. Verifico da coluna 1 � 5 ---*/
			int coluna_maior_negativo = 0; //coluna_maior_negativo = coluna piv�
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
			/*--- fixo a coluna piv� e calculo a raz�o para cada linha, exceto a linha 0 = Eq.0 = Z, pelo valor do lado direito(coluna=11), at� encontrar
			 o menor resultado. Verifico da linha 1 � 5 ---*/
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
			for (coluna = 1; coluna <= 11; coluna++) /*coluna vai at� 11 pois divido tamb�m as vari�veis de folga e o valor LD*/
			{
				if (matriz_Simplex[linha_menor_razao][coluna] != 0)
				{
					matriz_Simplex[linha_menor_razao][coluna] = matriz_Simplex[linha_menor_razao][coluna] / valor_pivo;
				}
				/*--- guardo a nova linha pivo em um novo vetor ---*/
				vet_Nova_Ln_Pivo[coluna] = matriz_Simplex[linha_menor_razao][coluna];
			}
			/*--- atualizo o ID da linha de vari�vel de folga para o X da coluna ---*/
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
			/* --- imprimo a matriz novamente - cabe�alho da matriz Simplex --- */
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
				//obtenho o coeficiente da coluna pivo - ele s� ir� mudar em n�vel de linha
				coeficiente_col_pivo = matriz_Simplex[linha][coluna_maior_negativo];
				for (coluna = 1; coluna <= 11; coluna++)
				{
					if (linha != linha_menor_razao) //realizar o c�lculo para todas as linhas, exceto a pr�pria nova linha piv�
					{
						matriz_Simplex[linha][coluna] = matriz_Simplex[linha][coluna] - (coeficiente_col_pivo * vet_Nova_Ln_Pivo[coluna]);
					}
				}
			}
			/* --- imprimo a matriz novamente - cabe�alho da matriz Simplex --- */
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
			/* --- imprimo a apenas a linha Z da matriz  + cabe�alho
			 E verifico se h� algum coeficiente negativo --- */
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
			/*--- havendo valor negativo, informo ao usu�rio e repito etapas de de 4 a 8, novamente at� n�o existir valor negativo ---*/
			if (valor_negativo == 0) /* Fim de jogo - Sim, a solu��o � �tima.*/
			{
				cout << "Sim, a solucao eh otima e possui os seguintes resultados: " << endl;
				cout << "Max Z =" << setw(6) << matriz_Simplex[0][11] << endl;
			
				for (linha = 1; linha <= 5; linha++)
				{
					cout << vet_id_ln_matriz[linha] << " = " << setw(6) << matriz_Simplex[linha][11] << endl;
				}
				fim = true;
			}
			else //Ainda tenho valor negativo na linha de Z. Ent�o preciso repetir processo para est� coluna at� torn�-al positiva
			{
				cout << "Ainda ha valor negativo na linha de Z. As etapas de 4 a 8 serao repetidas ate torna-la positiva." << endl;
				cout << endl;
				//ao t�rmino de cada loop o usu�rio ser� indagado se deseja continuar ou n�o
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
	return 0;
}