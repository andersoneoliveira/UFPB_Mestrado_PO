/*********************************************
 * OPL 12.9.0.0 Model
 * Author: anderson ernani de oliveira
 * Creation Date: 22 de jun de 2020 at 16:52:477
 * Tarefa final de semestre UFPB Mestrado - Disciplina Pesquisa Operacional - Prof. Anand
 * Objetivo: demonstrar aprendizagem em ferramenta de resolu��o de problemas de programa��o inteira. D�
 * prefer�ncia uma que esteja alinhada com o projeto de defesa do mestrado
 * Em meu caso, optei por um problema de separa��o de pedidos log�sticos, muito semelhante ao problema do caixeiro viajante
 * TSP
 * Data final de entrega: 26 junho de 2020 
 *********************************************/
//Par�metros
int nVertices = ...; //V�rtices ser�o os locais de coleta de produtos dentro do armaz�m
range Vertices = 1..nVertices;
//float x[Vertices];
//float y[Vertices];
float distancia[Vertices][Vertices] = ...;

//vari�veis de decis�o
dvar boolean rota[Vertices][Vertices];
dvar float+ u[Vertices];
//express�o - torna o c�digo mais amig�vel e permite reutiliza��o de c�digo dentro do modelo, quando necess�rio
dexpr float caminho = sum(i in Vertices, j in Vertices: i!=j) distancia[i][j]*rota[i][j]; 
//fun��o objetivo
minimize
  caminho;
//restri��es
subject to
{
 	//usando r�tulo para analisar as sa�das no gerenciador de problemas, mas consome recursos, s� usar se necess�rio
 	//flowing:
 	forall (j in Vertices)
 	  sum (i in Vertices: i!=j) rota[i][j]==1;
 	
 	//flowout:
 	forall(i in Vertices)
 	  sum(j in Vertices:i!=j) rota[i][j]==1;
 	
 	//ESubrotas:
 	forall (i in Vertices, j in Vertices: i>1 && j>1 && i!=j)
 	  u[i] - u[j] + nVertices*rota[i][j] <= nVertices-1;
 	   
} 