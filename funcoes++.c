/*Código feito pelo João Paulo mesmo*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> //nao sei porque, mas botei aqui.
#include <stdint.h>

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

typedef union{
	struct{
		int idade;
		char carac[C][P];
	} p_dados;
	char tipo[P];
	int cor_predom;
} UnionSize;

struct Ideia
{
	int id;
	enum Estado est;
	
	char nome[P];

	char resumo[T];

	enum Categoria Cat;
	union { //Categorias
		
		struct { //Personagem
			int idade;
			char carac[C][P]; /* caracteristicas do personagem */
		};

		//Item
		char tipo[P];

		//Cenário
		int cor_predom; /* cor predominante do cenário */		
	};
	
	int prioridade;
};

const long tamanho_registro = //soh pra poder usar serializacao e fseek.
	sizeof(int) + sizeof(char[P]) +
	sizeof(int) + sizeof(char[T]) +
	sizeof(enum Categoria) + sizeof(UnionSize) +
	sizeof(enum Estado);

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
			scanf("%d", &ideia->idade);
			scanf_flush(); //limpando queue por preocaução		

			//caracteristicas
			printf("Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):\n", C);
			for (int i = 0; i < C; i++)
			{
				printf("%d° caracteristica: ", i+1);
				ler_frase(ideia->carac[i], P);
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
			scanf("%x", &ideia->cor_predom);
			
/* debug */		printf("%6x\n", ideia->cor_predom);

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
		ideia->prioridade = 0;  
		
		return;
	}

	//prioridade
	printf("Qual a prioridade da ideia no projeto?\n");
	scanf("%d", &ideia->prioridade);
	scanf_flush(); //limpando a queue por preocaução
}

void exibir_ideia (struct Ideia* ideia) {
	printf("Nome: %s \n", ideia->nome);
	printf("Estado: ");
	switch (ideia->est)
			{
				case (E_concluido): 	printf("Concluido\n");	 	break;
				case (E_em_progresso): 	printf("Em Progresso\n"); 	break;
				case (E_a_fazer):	 	printf("A Fazer\n"); 		break;
				case (E_descartado):	printf("Descartado\n"); 	break; //talvez nem cheguemos a exibir esse
				default: 				printf("Erro no estado da ideia por algum motivo '-' ");
			}
	printf("Prioridade: %d\n", ideia->prioridade);
	printf("Resumo: %s\n", ideia->resumo);
	switch (ideia->Cat) 
	{
		case (C_Personagem):
			printf("Categoria: Personagem \n");
			printf("Idade: %d\n", ideia->idade);
			printf("Características:\n", C);
			for (int i = 0; i < C; i++) {
				printf("%s \n", ideia->carac[i]);
			} break;
	
		case (C_Item):
			printf("Categoria: Item \n");
			printf("Tipo do item: %s\n", ideia->tipo);
			break;
	
		case (C_Cenario):
			printf("Cor predominante do cenário (Hexadecimal): %6x\n", ideia->cor_predom);
			break;
			
		default: 
			printf("erro no switch Cat em exibir_ideia()");
	}
}

int escrever_ideia(FILE* f, const struct Ideia* ideia){
	long posicao = (ideia->id - 1) * tamanho_registro;
	if (fseek(f, posicao, SEEK_SET) != 0) return 0;
	
	fwrite(&ideia->id, sizeof(ideia->id), 1, f);
	fwrite(&ideia->est, sizeof(ideia->est), 1, f);
	fwrite(&ideia->nome, sizeof(ideia->nome), 1, f);
	fwrite(&ideia->resumo, sizeof(ideia->resumo), 1, f);
	fwrite(&ideia->Cat, sizeof(ideia->Cat), 1, f);
	
	fwrite(&ideia->idade, sizeof(UnionSize), 1, f); //copia qualquer Union, Idade eh o primeiro item na hora de criar a union, entao o endereço dele é o endereço de todos.
	fwrite(&ideia->prioridade, sizeof(ideia->prioridade), 1, f);

	return 1;	
}

int ler_ideia_arquivo(FILE* f, struct Ideia* ideia, int id){
	long posicao = (id - 1) * tamanho_registro;
	if(fseek(f, posicao, SEEK_SET) != 0) return 0;
	if(fread(&ideia->id, sizeof(ideia->id), 1, f) == 0) return 0; //falhou na hora de ler
	if(ideia->id != id) return 0; //testa se o id que ta sendo lido eh o mesmo que pedimos.
	if(fread(&ideia->est, sizeof(ideia->est), 1, f) == 0) return 0; //falhou na hora de ler
	if(ideia->est == E_descartado) return 0;
	
	//se ele chegou ate aqui, ele conseguiu ler o ID, e o estado, e o ID esta correto e nao foi excluido
	// entao ele le todo o resto
	fread(&ideia->nome, sizeof(ideia->nome), 1, f);
	fread(&ideia->resumo, sizeof(ideia->resumo), 1, f);
	fread(&ideia->Cat, sizeof(ideia->Cat), 1, f);
	fread(&ideia->idade, sizeof(UnionSize), 1, f);
	fread(&ideia->prioridade, sizeof(ideia->prioridade), 1, f);

	return 1;
}

void criar_ideia(FILE* f){
	fseek(f, 0, SEEK_END);
	long tamanho_total = ftell(f);
	int prox_id = (tamanho_total / tamanho_registro) + 1;
	
	struct Ideia nova;
	nova.id = prox_id;
	printf("||===============| Criando Ideia ID %d |===============||\n", prox_id);
	ler_ideia(&nova);
	
	if (escrever_ideia(f, &nova) == 1){
		printf("Ideia ID %d salva com sucesso. \n", prox_id);
	} else {
		printf("Falha ao salvar a ideia ID %d. \n", prox_id);
	}
}

void buscar_ideia(FILE* f){
	printf("Digite o ID da ideia que deseja buscar: ");
	int id;
	int edit = 0;
	scanf("%d", &id);
	scanf_flush();
	
	struct Ideia ideia_encontrada;
	
	if (ler_ideia_arquivo(f, &ideia_encontrada, id)){
		printf("||==============| Ideia Encontrada (ID %d) |==============||\n", id);
		exibir_ideia(&ideia_encontrada);
		printf("\n Deseja editar essa ideia? \n 1. Sim \n 2. Nao\n");
		scanf("%d", &edit);
	} else {
		if (ideia_encontrada.id != id){
			printf("A ideia de ID %d nao foi encontrada", id);
		} else if (ideia_encontrada.est == E_descartado) {
			printf("A ideia de ID %d foi descartada. \a\n", id);
		}
	}
	if (edit == 1){
		editar_ideia(f, id, &ideia_encontrada);
	}
}

void menu_edit(FILE* f, int id, struct Ideia* ideia, int escolha){
	if (escolha == 2){ // pro caso de soh querer recuperar uma ideia descartada.
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
				case ('D'):		e = E_descartado;	break;
				default: 		e = -1;
			}
		}
		ideia->est = e;
		long posicao = (ideia->id - 1) * tamanho_registro;
		if (fseek(f, posicao, SEEK_SET) != 0) return;
	
		fwrite(&ideia->id, sizeof(ideia->id), 1, f);
		fwrite(&ideia->est, sizeof(ideia->est), 1, f);
		return;
	}
	exibir_ideia(ideia);
	while (escolha != 9){
		printf("\n||===============| Editando Ideia |===============||\n");
		printf("1. Editar Nome\n");
		printf("2. Editar Estado\n");
		printf("3. Editar Prioridade\n");
		printf("4. Editar Resumo\n");
		printf("5. Editar Categoria(vai apagar as subcategorias abaixo tambem)\n");
		switch (ideia->Cat) {
		case (C_Personagem):
			printf("6. Editar idade:\n");
			printf("7. Editar Categorias:\n", C);
			break;
		case (C_Item):
			printf("6. Editar tipo do item:\n");
			break;
	
		case (C_Cenario):
			printf("6. Editar a cor predominante do cenário:\n");
			break;
		default: 
			printf("erro no switch Cat em editar_ideia()");
		}
		printf("9. Sair\n");
		printf("Escolha: ");
		
		if (scanf("%d", &escolha) != 1){
			escolha = -1; //isso eh pra previnir um loop infinito caso scanf leia uma letra ou sla.
		}
		scanf_flush();
		
		switch(escolha)
		{
			case 1: 
				printf("Nome da ideia (até %d caracteres):\n", P);
				ler_frase(ideia->nome, P);
				break;
			case 2: 
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
						case ('D'):		e = E_descartado;	break;
						default: 		e = -1;
					}
				}
				ideia->est = e;
				if ((ideia->est == E_concluido) || (ideia->est == E_descartado)) 
				{			
					ideia->prioridade = 0;  
				}
				break;
			}
			case 3: 
				printf("Qual a prioridade da ideia no projeto?\n");
				scanf("%d", &ideia->prioridade);
				scanf_flush(); //limpando a queue por preocaução
				break;
			case 4:
				printf("Resumo da ideia (até %d caracteres):\n", T);
				ler_frase(ideia->resumo, T);
				break;
			case 5:
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
						case ('I'): 	c = C_Item;			break;
						case ('C'): 	c = C_Cenario;		break;
						default: 	c = -1;
					}
				}
				ideia->Cat = c;
				
				memset(&ideia->idade, 0, sizeof(UnionSize)); //apaga tudo
				
				switch (ideia->Cat) {
					case (C_Personagem):
						printf("Insira a idade do personagem:\n");
						scanf("%d", &ideia->idade);
						scanf_flush(); //limpando queue por preocaução
						printf("Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):\n", C);
						for (int i = 0; i < C; i++)
						{
							printf("%d° caracteristica: ", i+1);
							ler_frase(ideia->carac[i], P);
						}
						scanf_flush();	
						break;
					case (C_Item):
						printf("Insira o tipo do item:\n");
						ler_frase(ideia->tipo, P);
						break;
					case (C_Cenario):
						printf("Insira em hexadecimal a cor predominante do cenário:\n");
						scanf("%x", &ideia->cor_predom);
						scanf_flush(); //limpando queue por preocaução
						break;
					default: 
					printf("erro no switch Cat em ler_ideia()");}
			}
			break;
			case 6:
				switch (ideia->Cat) {
					case (C_Personagem):
						printf("Insira a idade do personagem:\n");
						scanf("%d", &ideia->idade);
						scanf_flush(); //limpando queue por preocaução		
						break;
	
					case (C_Item):
						printf("Insira o tipo do item:\n");
						ler_frase(ideia->tipo, P);
						break;
					case (C_Cenario):
						printf("Insira em hexadecimal a cor predominante do cenário:\n");
						scanf("%x", &ideia->cor_predom);
						scanf_flush(); //limpando queue por preocaução
						break;
					default: 
					printf("erro no switch Cat em ler_ideia()");}
			break;
			case 7:
				printf("Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):\n", C);
				for (int i = 0; i < C; i++)
				{
					printf("%d° caracteristica: ", i+1);
					ler_frase(ideia->carac[i], P);
				}
				scanf_flush(); //limpando queue por preocaução
			break;
			
			case 9: printf("Saindo... \n"); break;
			default: printf("Entrada Invalida. \a \n"); //desde que descobri \a acho mt divertido
		}
	}
	escrever_ideia(f, ideia);
}

void editar_ideia(FILE* f, int id, struct Ideia* ideia){
	struct Ideia i;
	if (!id) {
		printf("Digite o ID da ideia que deseja editar: ");
		scanf("%d", &id);
		scanf_flush();
		ideia = &i; //pega o endereço de i e usa como se o argumento tivesse sido dado antes.
	}
	if (ler_ideia_arquivo(f, ideia, id)){
		printf("||==============| Ideia Encontrada (ID %d) |==============||\n", id);
		exibir_ideia(ideia);
		printf("\n O que deseja editar nessa ideia? \n ");
		menu_edit(f, id, ideia, 0);
	} else {
		if (ideia->id != id){
			printf("A ideia de ID %d nao foi encontrada", id);
		} else if (ideia->est == E_descartado) {
			printf("A ideia de ID %d foi descartada. Deseja recupera-la?\n 1. Sim \n 2. Nao \a\n", id);
			int escolha = 0;
			scanf("%d", &escolha);
			scanf_flush();
			if (escolha == 1) menu_edit(f, id, ideia, 2);
		}
	}
}

void descartar_ideia(FILE* f){
	printf("Digite o ID da ideia que deseja descartar: ");
	int id;
	scanf("%d", &id);
	scanf_flush();
	
	struct Ideia ideia_descartar;
	
	if (ler_ideia_arquivo(f, &ideia_descartar, id)){
		printf("Ideia '%s' será marcada como Descartada. \n", ideia_descartar.nome);
		ideia_descartar.est = E_descartado;
		ideia_descartar.prioridade = 0;
		escrever_ideia(f, &ideia_descartar);
		printf("Ideia ID %d foi descartada com sucesso. \n", id);
	} else {
		if (ideia_descartar.id != id){
			printf("A ideia de ID %d nao foi encontrada", id);
			return;
		} else if (ideia_descartar.est == E_descartado) {
			printf("A ideia de ID %d já foi descartada. \a\n", id);
			return;
		}
	}
}

int main () {
	const char* arquivo = "GDD.bin";
	
	FILE *f;
	
	printf("Abrindo Arquivo %s...\n", arquivo);
	if( !(f = fopen(arquivo, "rb+"))){ //caso fopen falhou em encontrar um arquivo existente
		printf("Arquivo nao encontrado. Criando novo...\n");
	 	if( !(f = fopen(arquivo, "wb+"))){ // caso fopen falhou em criar um arquivo tambem
			printf("ERRO: Nao foi possivel encontrar ou criar o arquivo.\n");
			exit(1);
		}
	}
	
	int escolha = 0;
	while (escolha != 9){
		printf("\n||===============| GDD Idea Inventory |===============||\n");
		printf("1. Criar nova ideia\n");
		printf("2. Buscar ideia\n");
		printf("3. Editar ideia\n");
		printf("4. Descartar ideia\n");  // ainda to vendo isso
		printf("5. Listar todas as ideias\n"); // e isso tambem
		printf("6. EXCLUIR Ideia (Permanente)\n"); // mais um
		printf("9. Sair\n");
		printf("Escolha: ");
		
		if (scanf("%d", &escolha) != 1){
			escolha = -1; //isso eh pra previnir um loop infinito caso scanf leia uma letra ou sla.
		}
		scanf_flush();
		
		switch(escolha)
		{
			case 1: criar_ideia(f); break;
			case 2: buscar_ideia(f); break;
			case 3: editar_ideia(f, 0, NULL); break;
			case 4: descartar_ideia(f); break;	
			
			case 9: printf("Saindo... \n"); break;
	
			default: printf("Entrada Invalida. \a \n"); //desde que descobri \a acho mt divertido
		}
	}
	fclose(f);
	return 0;
}

