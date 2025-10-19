//abacate

#include <stdio.h>
#include <string.h>

#define P 64  	// numero de caracteres em poucas palavras
#define T 512 	// numero de caracteres em um bloco de texto
#define C 3 	// numero de características de um personagem

enum Categoria
{
	C_Personagem,
	C_Item,
	C_Cenario
};

enum Estado //de desenvolvimento
{
	E_concluido,
	E_em_progresso,
	E_a_fazer,
	E_descartado	
};

struct Ideia
{
	int id;
	
	char nome[P];

	char resumo[T];

	enum Categoria Cat;
	union { //Categorias
		
		struct { //Personagem
			char idade;
			char carac[C][P]; /* caracteristicas do personagem */
		};

		//Item
		char tipo[P];

		//Cenário
		int cor_predom; /* cor predominante do cenário */		
	};
	
	enum Estado est;
	int prioridade;
};

void ler_frase (char* str, int len)
{
	char buffer[len+1];		//inicia buffer que guardará a frase + o \n
	fgets(buffer, len, stdin);	//le a frase com o \n para o buffer
	strncpy(str, buffer, len-1);	//copia a frase para um tamanho de 1 caractere a menos
	str[strcspn(str, "\n")] = '\0'; 	//transforma o '\n' em '\0'
}

void scanf_flush (void)
{
	int c;
	while ( (c = getchar()) != '\n') {}
}

void ler_ideia (struct Ideia* ideia)
{
	//nome
	printf("Nome da ideia (até %d caracteres):\n", P);
	ler_frase(ideia->nome, P);

	//resumo
	printf("Resumo da ideia (até %d caracteres):\n", T);
	ler_frase(ideia->resumo, T);
	
	//definindo a categoria da ideia
	{
		char p;
	       	enum Categoria c = -1;
		while (c == -1)
		{
			printf("(P)ersonagem, (I)tem, ou (C)enário?\n");
			scanf("%c", &p);
			scanf_flush(); //limpando a queue do stdin para não duplicar o while

			switch (p)
			{
				case ('P'): 	c = C_Personagem; 	break;
				case ('I'): 	c = C_Item;		break;
				case ('C'): 	c = C_Cenario;		break;
				default: 	c = -1;
			}
		}
		ideia->Cat = c;
	}

	switch (ideia->Cat)
	{
		case (C_Personagem):
			
			//idade
			printf("Insira a idade do personagem:\n");
			scanf("%hhd", &ideia->idade);
			scanf_flush(); //limpando queue por preocaução		

			//caracteristicas
			printf("Defina o personagem em %d palavras distintas:\n", C);
			for (int i = 0; i < C; i++)
			{
				printf("%d° caracteristica: ", i+1);
				scanf("%s", &ideia->carac[i][0]);
			}
			scanf_flush(); //limpando queue por preocaução

			break;
	
		case (C_Item):

			//tipo
			printf("Insira o tipo do item:\n");
			ler_frase(ideia->tipo, P);

			break;
	
		case (C_Cenario):

			//cor predominante
			printf("Insira em hexadecimal a cor predominante do cenário:\n");
			scanf("%x", ideia->cor_predom);
			
/* debug */		printf("%6x", ideia->cor_predom);

			scanf_flush(); //limpando queue por preocaução
			
			break;
		default: 
			printf("erro no switch Cat em ler_ideia()");
	}


	//definindo o estado de desenvolvimento da ideia
	{
		char p;
	       	enum Estado e = -1;
		while (e == -1)
		{
			printf("Estado da ideia:\n(C)oncluído, (E)m Progresso, (A) Fazer, ou (D)escartado?\n");
			scanf("%c", &p);
			scanf_flush(); //limpando a queue para não duplicar o while
			
			switch (p)
			{
				case ('C'): 	e = E_concluido; 	break;
				case ('E'): 	e = E_em_progresso;	break;
				case ('A'): 	e = E_a_fazer;		break;
				case ('D'):	e = E_descartado;	break;
				default: 	e = -1;
			}

		}
		ideia->est = e;
	}
	
	if ((ideia->est == E_concluido) || (ideia->est == E_descartado)) 
	{
		ideia->est = 0;  
		
		return;
	}

	//prioridade
	printf("Qual a prioridade da ideia no projeto?\n");
	scanf("%d", &ideia->prioridade);
	scanf_flush(); //limpando a queue por preocaução
}

int main (void)
{
	struct Ideia ideia;
	ler_ideia(&ideia);	

	return 0;
}
