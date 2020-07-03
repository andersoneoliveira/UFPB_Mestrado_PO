/*********************************************
 * OPL 12.9.0.0 Model
 * Author: anderson ernani de oliveira
 * Creation Date: 22 de jun de 2020 at 16:52:477
 * Tarefa final de semestre UFPB Mestrado - Disciplina Pesquisa Operacional - Prof. Anand
 * Objetivo: demonstrar aprendizagem em ferramenta de resolução de problemas de programação inteira. Dê
 * preferência uma que esteja alinhada com o projeto de defesa do mestrado
 * Em meu caso, optei por um problema de separação de pedidos logísticos, muito semelhante ao problema do caixeiro viajante
 * TSP
 * Data final de entrega: 26 junho de 2020 
 *********************************************/
//Parâmetros
int nVertices = ...; //Vértices serão os locais de coleta de produtos dentro do armazém
range Vertices = 1..nVertices;
//float x[Vertices];
//float y[Vertices];
float distancia[Vertices][Vertices] = ...;

//variáveis de decisão
dvar boolean rota[Vertices][Vertices];
dvar float+ u[Vertices];
//expressão - torna o código mais amigável e permite reutilização de código dentro do modelo, quando necessário
dexpr float caminho = sum(i in Vertices, j in Vertices: i!=j) distancia[i][j]*rota[i][j]; 
//função objetivo
minimize
  caminho;
//restrições
subject to
{
 	//usando rótulo para analisar as saídas no gerenciador de problemas, mas consome recursos, só usar se necessário
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