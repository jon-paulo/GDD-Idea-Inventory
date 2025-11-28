#include <stdio.h>
#include <string.h>
#include <stdlib.h> //nao sei porque, mas botei aqui em algum momento.
#ifdef _WIN32	
	#include <windows.h>
#endif
#include "gdd_types.h" //to usando ele na mesma pasta q o GDDI.c


FILE* f_data = NULL;
FILE* f_index = NULL;

IndiceEntry* g_indice = NULL;	// Array pra poder buscar tudo na RAM.
int g_total_ideias = 0;			// Total de ideias no Indice. usei 'g_' porque vi q pessoal fica usando isso pra variaveis globais.
int g_proximo_id = 1;			// Auto-explicativo, conta quantas ideias foram criadas, pra gerar o novo ID.

const char* arquivo_dados = "GDD_data.bin"; // botei como variaveis globais pra nao ter que ficar usando FILE* f como argumento de todas as funcoes aqui
const char* arquivo_indice = "GDD_index.bin";

// Funcoes basicas
void scanf_flush();
void ler_frase(char* str, int len);
void exibirTextoFormatado(const char* texto, int largura); // pode ser considerada funcao de UI, mas essa categoria de basicas tava mt vazia
void centralizar(int largura, const char* texto); // msm coisa

// Funcoes de mexer no arquivo
int carregar_arquivos_e_indice();
int salvar_indice(); // salvar o indice no disco
int buscar_no_indice(int id, IndiceEntry** resultado);
int ler_ideia_arquivo(long posicao, struct Ideia* ideia);
int escrever_ideia(long posicao, const struct Ideia* ideia);

// Funcoes de UI, e outras logicas principais
void criar_ideia();
void buscar_ideia(); // e Editar tambem se quiser
void descartar_ideia();
void jogar_no_lixo();
void compactar();
void ler_ideia();
void exibir_ideia (struct Ideia* ideia);
void listar_ideias();
void menu_edit(struct Ideia* ideia, IndiceEntry* entrada_indice, int modo);
void limpar_tela();

//listar ideias ganhou uma categoria propria soh porque precisa de 3 funcoes extras pra funcionar
int cmp_by_id(const void* a, const void* b); // compara por ID
int cmp_by_name(const void* a, const void* b); // por nome
int cmp_by_priority(const void* a, const void* b); // por prioridade


int main () {
	if (!carregar_arquivos_e_indice()){
		printf("Erro ao carregar o banco de dados. Saindo. \n");
		exit(1);		
	}
	
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
	#endif
	
	char string_esquerda[50];
	char string_direita[50];
	
	
	int escolha = 0;
	while (escolha != 9){
		sprintf(string_esquerda, "Ideias no Indice: %d", g_total_ideias);
		sprintf(string_direita, "Proximo ID: %d", g_proximo_id);
		printf("\n╭───────────────────────────────────────┘ GDD Idea Inventory └───────────────────────────────────────╮");
		printf("\n│%-50s%50s│", string_esquerda, string_direita);
		printf("\n│%-100s│", "  ");
		printf("\n│%-100s│", "1. Criar nova ideia");
		printf("\n│%-100s│", "2. Buscar / Editar ideia");
		printf("\n│%-100s│", "3. Descartar ideia (Soft Delete)");
		printf("\n│%-100s│", "4. EXCLUIR Ideia (Permanente)");
		printf("\n│%-100s│", "5. Listar todas as ideias");
		printf("\n│%-100s│", "6. Compactar Arquivo (Manutencao)");
	
		printf("\n│%-100s│", "9. Sair");
		printf("\n╰────────────────────────────────────────────────────────────────────────────────────────────────────╯");
		printf("\nEscolha: ");
		
		if (scanf("%d", &escolha) != 1){
			escolha = -1; //isso eh pra previnir um loop infinito caso scanf leia uma letra ou sla.
		}
		scanf_flush();
		limpar_tela(); // gostei que isso funciona pra windows e LINUX
		
		switch(escolha)
		{
			case 1: criar_ideia();			break;
			case 2: buscar_ideia();			break;
			case 3: descartar_ideia();		break;
			case 4: jogar_no_lixo();		break;
			case 5: listar_ideias();		break;
			case 6: compactar();			break;
			case 9: printf("Saindo... \n"); break;
	
			default: printf("Entrada Invalida. \a \n"); //desde que descobri \a acho mt divertido
		}
	}
	salvar_indice(); // salvar antes de fechar soh por segurança
	fclose(f_data);
	fclose(f_index);
	free(g_indice);
	return 0;
}

void scanf_flush (void)
{
	int c;
	while ( (c = getchar()) != '\n' && c != EOF) {} // esse "&& c != EOF" eh pro caso extremo do stdin estar no fim do arquivo por algum motivo e nao ter um \n pra parar o loop infinito (por exemplo: ctrl + D)
}

void ler_frase (char* str, int len)
{
	if(fgets(str, len, stdin) == NULL){ // erro ou coisa do tipo
		str[0] = '\0';
		return;
	}
	if (strchr(str, '\n') == NULL){ // se strchr nao achou \n, é porque o usuário digitou demais
		scanf_flush();
	} else {
		str[strcspn(str, "\n")] = '\0';	//transforma o '\n' em '\0'
	}
}

void exibir_texto(const char* texto, int largura){
	char linha[largura+1];
	const char* ponteiro = texto; // pra andar pelo texto
	while(strlen(ponteiro) > 0){
		if(strlen(ponteiro) <= largura){ // se cabe em uma linha soh
			printf("│%-*s│\n", largura, ponteiro);
			break;
		}
		
		int ponto_corte = largura; // pra contar pra tras
		for(int i = largura; i > 0; i--){
			if(ponteiro[i] == ' ' || ponteiro[i] == '\n'){
				ponto_corte = i; // salvar o indice onde tem espaço pra 
				break;
			}
		}
		
		strncpy(linha, ponteiro, ponto_corte);
		linha[ponto_corte] = '\0';
		printf("│%-*s│\n", largura, linha);
		
		ponteiro += ponto_corte;
		
		if (ponteiro[0] == ' ' || ponteiro[0] == '\n'){
			ponteiro++;
		}
	}
}

void centralizar(int largura, const char* texto){
	int tam_titulo = strlen(texto);
	int metade1 = (largura + tam_titulo) / 2; // pra poder ficar no meio do treco
	int metade2 = largura - metade1;
	
	printf("│%*s%*s│\n", metade1, texto, metade2, "");
	
}

int carregar_arquivos_e_indice(){
	printf("Abrindo Arquivo %s...\n", arquivo_dados);
	f_data = fopen(arquivo_dados, "rb+");
	if (f_data == NULL){
		printf("Arquivo nao encontrado. Criando novo...\n");
	 	f_data = fopen(arquivo_dados, "wb+");
	}
	if (f_data == NULL){
		printf("Erro fatal: Nao foi possivel abrir nem criar %s \n", arquivo_dados);
		return 0;
	}
	
	printf("Abrindo Arquivo %s...\n", arquivo_indice);
	f_index = fopen(arquivo_indice, "rb+");
	if (f_index == NULL){
		printf("Arquivo nao encontrado. Criando novo...\n");
	 	f_index = fopen(arquivo_indice, "wb+");
		if (f_index == NULL){
			printf("Erro fatal: Nao foi possivel abrir nem criar %s \n", arquivo_dados);
			return 0;
		}
		
		// se ele chegou aqui, o arquivo esta vazio, escreve os cabecalhos padrao(vazios). 
		fwrite(&g_proximo_id, sizeof(int), 1, f_index);
		fwrite(&g_total_ideias, sizeof(int), 1, f_index);
	} else {
		// se ele chegou aqui, o arquivo existia, entao le os cabecalhos
		fread(&g_proximo_id, sizeof(int), 1, f_index);
		fread(&g_total_ideias, sizeof(int), 1, f_index);
	}
	// aloca memodia pro indice, as buscas ficam mais rapidas assim, tudo na RAM
	if (g_total_ideias > 0) {
		g_indice = (IndiceEntry*) malloc(sizeof(IndiceEntry) * g_total_ideias);
		if (g_indice == NULL){
			printf("falha no malloc pro Indice. \n");
			return 0;
		}
		fread(g_indice, sizeof(IndiceEntry), g_total_ideias, f_index);
	} else {
		// aloca espaço minimo pra evitar realloc(NULL), nao eh extremamente necessario mas eu gosto, pra certos casos ae.
		g_indice = (IndiceEntry*) malloc(sizeof(IndiceEntry));
	}
	printf("Sistema carregado. %d ideias no indice. Proximo ID: %d \n", g_total_ideias, g_proximo_id);
	return 1;
}

int salvar_indice(){
	if (f_index == NULL) return 0;
	
	fseek (f_index, 0, SEEK_SET);
	fwrite(&g_proximo_id, sizeof(int), 1, f_index);
	fwrite(&g_total_ideias, sizeof(int), 1, f_index);

	if (g_total_ideias > 0){
		fwrite(g_indice, sizeof(IndiceEntry), g_total_ideias, f_index);
	}
	
	fflush(f_index); // soh pra garantir
	return 1;
}

int buscar_no_indice(int id, IndiceEntry** resultado){
	// eh uma busca binaria, mas ela comeca procurando como se o ID fosse a posicao
	// no melhor caso, se nunca exluirmos uma ideia, ela acha de primeira, se nao, ela busca com logN
	
	int baixo = 0;
	int alto = g_total_ideias - 1;
	
	if(alto < 0){
		*resultado = NULL;
		return -1;
	}
	if (id - 1 <= alto){ // checa se o id eh uma posicao valida, por exemplo: se tinhamos 100 ideias, excluimos 10. Procurar o ID 96 na posicao 96 do vetor com 90 ideias nao faz sentido.
		int chute = id-1;
		int id_achado = g_indice[chute].id;
		if (id_achado == id){ // se achou o id de primeira.
			*resultado = &g_indice[chute];
			return chute;
		}
		if (id_achado > id){ // como a lista esta ordenada, e IDs nao repetem, soh podem ser excluidos, nao tem como o ID achado ser menor que o procurado
			alto = chute-1; // se eu sei que o ID ta abaixo, essa posicao eh o novo alto.
		}
	}
	// se o chute falhou, busca binaria normal. Com o alto sendo o ID talvez.
	while(baixo <= alto){
		int meio = baixo + (alto-baixo)/2;
		int id_meio = g_indice[meio].id;
		
		if (id_meio == id){
			*resultado = &g_indice[meio];
			return meio;
		}
		if (id_meio < id){
			baixo = meio + 1;
		} else {
			alto = meio - 1;
		}
	}
	
	*resultado = NULL; //ideia nao encontrada
	return -1;
}

int ler_ideia_arquivo(long posicao, struct Ideia* ideia){
	if(fseek(f_data, posicao, SEEK_SET) != 0) return 0;
	
	if	(	
		fread(&ideia->id, sizeof(ideia->id), 1, f_data) == 1 &&
		fread(&ideia->est, sizeof(ideia->est), 1, f_data) == 1 &&
		fread(&ideia->nome, sizeof(ideia->nome), 1, f_data) == 1 &&
		fread(&ideia->resumo, sizeof(ideia->resumo), 1, f_data) == 1 &&
		fread(&ideia->Cat, sizeof(ideia->Cat), 1, f_data) == 1 &&
		fread(&ideia->idade, sizeof(UnionSize), 1, f_data) == 1 &&
		fread(&ideia->prioridade, sizeof(ideia->prioridade), 1, f_data) == 1
		) { // esse if com && eh soh pra parar a leitura se qualquer um deles falhar
		return 1; // conseguiu ler
	}
	return 0; // falhou
}

int escrever_ideia(long posicao, const struct Ideia* ideia){
	if (fseek(f_data, posicao, SEEK_SET) != 0) return 0;
	
	fwrite(&ideia->id, sizeof(ideia->id), 1, f_data);
	fwrite(&ideia->est, sizeof(ideia->est), 1, f_data);
	fwrite(&ideia->nome, sizeof(ideia->nome), 1, f_data);
	fwrite(&ideia->resumo, sizeof(ideia->resumo), 1, f_data);
	fwrite(&ideia->Cat, sizeof(ideia->Cat), 1, f_data);
	fwrite(&ideia->idade, sizeof(UnionSize), 1, f_data); //copia qualquer Union, Idade eh o primeiro item na hora de criar a union, entao o endereço dele é o endereço de todos.
	fwrite(&ideia->prioridade, sizeof(ideia->prioridade), 1, f_data);

	return 1;	
}

void criar_ideia(){
	fseek(f_data, 0, SEEK_END); // nova posicao neh
	long nova_posicao = ftell(f_data);
	int id_atual = g_proximo_id;
	g_proximo_id++;
	
	struct Ideia nova;
	char string_temp[50];
	int largura = 22;
	sprintf(string_temp, "Criando Ideia ID %d", id_atual);
	int tam_titulo = strlen(string_temp);
	int metade1 = (largura + tam_titulo) / 2; // pra poder ficar no meio do treco
	int metade2 = largura - metade1;
	
	printf("│───────────────────────────────────────┘%*s%*s└───────────────────────────────────────│\n", metade1, string_temp, metade2, "");
	nova.id = id_atual;
	ler_ideia(&nova);
	limpar_tela();
	
	// escreve no GDD
	if (escrever_ideia(nova_posicao, &nova) == 1){
		printf("Ideia ID %d salva com sucesso. \n", id_atual);
	} else {
		printf("Falha ao salvar a ideia ID %d. \n", id_atual);
	}
	// escreve na array Indice, tem que aumentar o tamanho dela
	g_indice = (IndiceEntry*) realloc(g_indice, sizeof(IndiceEntry) * (g_total_ideias + 1));
	if (g_indice == NULL){
		printf("Erro de realloc \n");
		exit(1);
	}
	int indice_array = g_total_ideias; // se tem 3 ideias, pra botar a ideia numero 4, ponho no indice 3, mesma coisa que total de ideias.
	g_indice[indice_array].id = id_atual;
	g_indice[indice_array].posicao = nova_posicao;
	g_indice[indice_array].status = nova.est;
	g_total_ideias++;
	
	salvar_indice();
	printf("Ideia Id %d criada com sucesso.\n", id_atual);
}

void buscar_ideia(){
	printf("Digite o ID da ideia que deseja buscar: ");
	int id;
	scanf("%d", &id);
	scanf_flush();
	
	IndiceEntry* entrada = NULL;
	buscar_no_indice(id, &entrada);
	
	if(entrada == NULL){
		printf("Ideia ID %d nao encontrada no indice.\n", id);
		return;
	}
	
	if(entrada->status == E_lixo){
		printf("Ideia Id %d foi jogada no lixo e sera removida na proxima compactacao.\n", id);
		return;
	}
	
	// se chegou aqui, achou e nao eh lixo, le os dados
	struct Ideia ideia;
	if (!ler_ideia_arquivo(entrada->posicao, &ideia)){
		printf("ERRO: Nao foi possivel ler os dados da ideia %d.\n", id);
		return;
	}
	exibir_ideia(&ideia);
	
	if (entrada->status == E_descartado){
		printf("\nEsta ideia foi Descartada. Deseja recupera-la?\n 1. Sim\n 2. Nao\n");
		int escolha;
		scanf("%d", &escolha);
		scanf_flush();
		limpar_tela();
		if (escolha == 1){
			menu_edit(&ideia, entrada, 2); // Modo 2 = recuperar
		}
	} else {
		printf("\nDeseja editar essa ideia?\n 1. Sim\n 2. Nao\n");
		int escolha;
		scanf("%d", &escolha);
		scanf_flush();
		limpar_tela();
		if(escolha == 1){
			menu_edit(&ideia, entrada, 0); // Modo 0 eh edicao normal.
		}
	}
}

void descartar_ideia(){
	printf("Digite o ID da ideia que deseja descartar: ");
	int id;
	scanf("%d", &id);
	scanf_flush();
	
	IndiceEntry* entrada = NULL;
	buscar_no_indice(id, &entrada);
	
	if (entrada == NULL || entrada->status == E_descartado || entrada->status == E_lixo){
		printf("Ideia ID %d nao encontrada ou ja inativa.\n", id);
		return;
	}
	// se chegou ate aqui, id foi encontrado e nao esta descartado. Entao descarta.
	struct Ideia ideia;
	ler_ideia_arquivo(entrada->posicao, &ideia);
	
	ideia.est = E_descartado;
	entrada->status = E_descartado;
	// salvar no arquivo e indice
	escrever_ideia(entrada->posicao, &ideia);
	salvar_indice();
	printf("Ideia Id %d marcada como descartada.\n", id);
}

void jogar_no_lixo(){
	printf("Digite o ID da ideia para JOGAR NO LIXO (PERMANENTE): ");
	int id;
	scanf("%d", &id);
	scanf_flush();
	
	IndiceEntry* entrada = NULL;
	buscar_no_indice(id, &entrada);
	
	if (entrada == NULL || entrada->status == E_lixo){
		printf("Ideia ID %d nao encontrada ou ja esta no lixo.\n", id);
		return;
	}
	
	printf("Tem certeza? Isso apagara todos os dados da ideia %d.\n 1. Sim.\n 2. Nao.\n", id);
	int escolha;
	scanf("%d", &escolha);
	scanf_flush();
	if (escolha != 1) {
		printf("Operacao cancelada.\n");
		return;
	}
	
	// se chegou ate aqui, o cara realmente odeia essa ideia.
	entrada->status = E_lixo;
	
	struct Ideia lixo_struct;
	memset(&lixo_struct, 0, sizeof(struct Ideia)); // pra criar o bloxo cheio de 0, pra apagar a ideia.
	lixo_struct.id = id;
	lixo_struct.est = E_lixo;
	// salvar no arquivo e indice
	escrever_ideia(entrada->posicao, &lixo_struct);
	salvar_indice();
	printf("Ideia Id %d foi jogada no lixo e teve seus dados apagados.\n", id);
}

void compactar(){
	printf("Iniciando compactacao...\n");
	// arquivos "temporarios" pra copiar.
	FILE* f_temp_data = fopen("temp_data.bin", "wb");
	FILE* f_temp_index = fopen("temp_index.bin", "wb");
	if (f_temp_data == NULL || f_temp_index == NULL){
		printf("Erro ao criar os arquivos temporarios");
		return;
	}
	
	IndiceEntry* novo_indice = (IndiceEntry*) malloc(sizeof(IndiceEntry) * g_total_ideias);
	int novo_total_ideias = 0;
	
	for(int i = 0; i < g_total_ideias; i++){
		if(g_indice[i].status != E_lixo){ //se o registro nao for lixo, ele copia
			struct Ideia ideia;
			ler_ideia_arquivo(g_indice[i].posicao, &ideia);
			long nova_posicao = ftell(f_temp_data);
			fwrite(&ideia, tamanho_registro, 1, f_temp_data);
			
			novo_indice[novo_total_ideias].id = ideia.id;
			novo_indice[novo_total_ideias].posicao = nova_posicao;
			novo_indice[novo_total_ideias].status = ideia.est;
			novo_total_ideias++;
		}
	}
	printf("Compactacao: %d ideias mantidas.\n", novo_total_ideias);
	
	// salvando tudo isso no arquivo novo.
	fseek(f_temp_index, 0, SEEK_SET);
	fwrite(&g_proximo_id, sizeof(int), 1, f_temp_index);
	fwrite(&novo_total_ideias, sizeof(int), 1, f_temp_index);
	fwrite(novo_indice, sizeof(IndiceEntry), novo_total_ideias, f_temp_index);
	
	// matar e substituir os arquivos antigos
	fclose(f_data);
	fclose(f_index);
	fclose(f_temp_data);
	fclose(f_temp_index);
	
	remove(arquivo_dados);
	remove(arquivo_indice);
	rename("temp_data.bin", arquivo_dados);
	rename("temp_index.bin", arquivo_indice);
	
	// limpar tudo salvo na RAM e recarregar.
	free(g_indice);
	g_indice = NULL;
	g_total_ideias = 0;
	carregar_arquivos_e_indice();
	printf("compactacao concluida\n");
}

void ler_ideia (struct Ideia* ideia)
{
	printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
	centralizar(100, "Criar Nova Ideia");
	//nome
	printf("│ %-98s │\n", "Nome da ideia (até %d caracteres):", P);
	printf("│ ► ");
	ler_frase(ideia->nome, P);

	//resumo
	printf("│ %-98s │\n", "Resumo da ideia (até %d caracteres):", T);
	ler_frase(ideia->resumo, T);
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	
	//definindo a categoria da ideia
	{
		char p;
		enum Categoria c = -1;
		while (c == -1)
		{
			printf("│ %-98s │\n", "(P)ersonagem, (I)tem, ou (C)enário, ou (O)utro?\n│");
			printf("│ ► ");
			scanf("%c", &p);
			scanf_flush(); //limpando a queue do stdin para não duplicar o while
			
			switch (p)
			{
				case ('P'): case ('p'): c = C_Personagem; 	break;
				case ('I'): case ('i'): c = C_Item;			break;
				case ('C'): case ('c'): c = C_Cenario;		break;
				case ('O'):	case ('o'): c = C_Customizado;	break;
				default: 	c = -1;
			}
		}
		ideia->Cat = c;
	}
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");

	switch (ideia->Cat)
	{
		case (C_Personagem):
			
			//idade
			printf("│ %-98s │\n", "Insira a idade do personagem:");
			printf("│ ► ");
			scanf("%d", &ideia->idade);
			scanf_flush(); //limpando queue por preocaução		

			//caracteristicas
			printf("│ %-98s │\n", "Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):", C);
			for (int i = 0; i < C; i++)
			{
				printf("│%d° caracteristica: ", i+1);
				ler_frase(ideia->carac[i], P);
			}
			break;
	
		case (C_Item):

			//tipo
			printf("│ %-98s │\n", "Insira o tipo do item: ");
			printf("│ ► ");
			ler_frase(ideia->tipo, P);

			break;
	
		case (C_Cenario):

			//cor predominante
			printf("│ %-98s │\n", "Insira em hexadecimal a cor predominante do cenário: ");
			printf("│ ► ");
			scanf("%x", &ideia->cor_predom);
			
/* debug 		printf("%6x\n", ideia->cor_predom);*/

			scanf_flush(); //limpando queue por preocaução
			
			break;
		case (C_Customizado):
			printf("│ %-98s │\n", "Digite o nome da categoria customizada: ");
			printf("│ ► ");
			ler_frase(ideia->nome_categoria_custom, P);
			printf("│ %-98s │\n", "Quantos atributos deseja adicionar (max %d)?", MAX_ATRIBUTOS);
			printf("│ ► ");
			scanf("%d", &ideia->num_atributos);
			scanf_flush();
			
			// botando limites aqui.
			if(ideia->num_atributos > MAX_ATRIBUTOS) ideia->num_atributos = MAX_ATRIBUTOS;
			if(ideia->num_atributos < 0) ideia->num_atributos = 0;
			
			for(int i = 0; i < ideia->num_atributos; i++){
				printf("│Atributo %d - Chave(nome): ", i+1);
				ler_frase(ideia->atributos[i].chave, P);
				printf("│            - Valor(conteúdo): ");
				ler_frase(ideia->atributos[i].valor, P);
			}
			break;
		default: 
			printf("erro no switch Cat em ler_ideia()");
	}
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");


	//definindo o estado de desenvolvimento da ideia
	{
		char p;
		enum Estado e = -1;
		while (e == -1)
		{
			printf("│ %-98s │\n", "Estado da ideia:\n(C)oncluído, (E)m Progresso, (A) Fazer, ou (D)escartado?│");
			printf("│ ► ");
			scanf("%c", &p);
			scanf_flush(); //limpando a queue para não duplicar o while
			
			switch (p)
			{
				case ('C'): 	e = E_concluido; 	break;
				case ('E'): 	e = E_em_progresso;	break;
				case ('A'):		e = E_a_fazer;		break;
				case ('D'):		e = E_descartado;	break;
				default:		e = -1;
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
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");

	printf("│ %-98s │\n", "Qual a prioridade da ideia no projeto?");
	printf("│ ► ");
	scanf("%d", &ideia->prioridade);
	scanf_flush(); //limpando a queue por preocaução
	printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
}

void exibir_ideia (struct Ideia* ideia) {
	char string_temp[100]; // 65 soh pra garantir
	int largura = 100;
	
	printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
	sprintf(string_temp, "(ID: %d) %s", ideia->id, ideia->nome);
	centralizar(largura, string_temp);
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│Estado: ");
	switch (ideia->est)
			{
				case (E_lixo): 			printf("%-92s│\n", "Lixo");		 	break;
				case (E_concluido): 	printf("%-92s│\n", "Concluido");	break;
				case (E_em_progresso): 	printf("%-92s│\n", "Em Progresso"); break;
				case (E_a_fazer):	 	printf("%-92s│\n", "A Fazer"); 		break;
				case (E_descartado):	printf("%-92s│\n", "Descartado"); 	break; //talvez nem cheguemos a exibir esse
				default: 				printf("%-92s│\n", "Erro no estado da ideia por algum motivo '-' ");
			}
	sprintf(string_temp, "Prioridade: %d", ideia->prioridade);
	printf("│%-100s│\n", string_temp);
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│                                               Resumo                                               │\n");
	exibir_texto(ideia->resumo, largura);
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	
	switch (ideia->Cat) 
	{
		case (C_Personagem):
			centralizar(largura, "Categoria: Personagem");
			printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
			printf("│Idade: %d\n", ideia->idade);
			printf("│Caracteristicas:\n");
			for (int i = 0; i < C; i++) {
				printf("- %s \n", ideia->carac[i]);
			} 
			printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
	
		case (C_Item):
			centralizar(largura, "Categoria: Item");
			printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
			printf("│Tipo do item: %-86s│\n", ideia->tipo);
			printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
	
		case (C_Cenario):
			centralizar(largura, "Categoria: Cenário");
			printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
			printf("│Cor predominante do cenario (Hexadecimal): %6x\n", ideia->cor_predom);
			printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
			
		case (C_Customizado):
			sprintf(string_temp, "%s %s", "Categoria:", ideia->nome_categoria_custom);
			centralizar(largura, string_temp);
			printf("│%-100s│\n", "Atributos(subcategorias):");
			for(int i = 0; i < ideia->num_atributos; i++){
				printf("│ ╭ %-97s│\n", ideia->atributos[i].chave);
				printf("│ ╰─► %-95s│\n", ideia->atributos[i].valor);
			}
			printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
		default:
			printf("erro no switch Cat em exibir_ideia()"); // nao consigo imaginar isso dando erro mas enfim neh.
	}
}

void menu_edit(struct Ideia* ideia, IndiceEntry* entrada_indice, int modo){
	
	if (modo == 2){ // pro caso de soh querer recuperar uma ideia descartada.
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
		entrada_indice->status = ideia->est;
		escrever_ideia(entrada_indice->posicao, ideia);
		salvar_indice();
		return;
	}
	int escolha = 0;
	while (escolha != 9){
		printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
		centralizar(100, "Editando Ideia");
		printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
		printf("│%-100s│\n", "1. Editar Nome");
		printf("│%-100s│\n", "2. Editar Estado");
		printf("│%-100s│\n", "3. Editar prioridade");
		printf("│%-100s│\n", "4. Editar resumo");
		printf("│%-100s│\n", "5. Mudar Categoria(apaga as subcategorias atuais)");
	
		switch (ideia->Cat) {
		case (C_Personagem):
			printf("│%-100s│\n", "6. Editar idade");
			printf("│%-100s│\n", "7. Editar Categorias:", C);
			break;
		case (C_Item):
			printf("│%-100s│\n", "6. Editar tipo do item:");
			break;
	
		case (C_Cenario):
			printf("│%-100s│\n", "6. Editar a cor predominante do cenário:");
			break;
			
		case (C_Customizado):
			printf("│%-100s│\n", "6. Editar nome da Categoria customizada");
			printf("│%-100s│\n", "7. Editar Atributos");
			break;
			
		default: 
			printf("erro no switch Cat em editar_ideia()"); //duvido
		}
		printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
		printf("│%-100s│\n", "9. Sair");
		printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
		printf("Escolha: ");
		
		if (scanf("%d", &escolha) != 1){
			escolha = -1; //isso eh pra previnir um loop infinito caso scanf leia uma letra ou sla.
		}
		scanf_flush();
		limpar_tela();
		
		switch(escolha)
		{
			case 1: // NOME
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				centralizar(100, "Editar Nome");
				printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
				printf("│ %-98s │\n", "Nome atual: ");
				printf("│ %-98s │\n", ideia->nome);
				printf("│ %-98s │\n", "Novo Nome da ideia (até %d caracteres):", P);
				printf("│ ► ");
				ler_frase(ideia->nome, P);
				printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
				break;
			case 2:  // ESTADO
			{
				char p;
				enum Estado e = -1;
				while (e == -1)
				{
					printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
					centralizar(100, "Editar Estado");
					printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
					printf("│ %-98s │\n", "Novo estado da ideia:");
					printf("│ %-99s │\n", " (C)oncluído");
					printf("│ %-98s │\n", " (E)m Progresso");
					printf("│ %-98s │\n", " (A) Fazer");
					printf("│ %-98s │\n", " (D)escartado");
					printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
					printf(" Escolha: ");
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
			case 3: // PRIORIDADE
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				centralizar(100, "Editar Prioridade");
				printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
				printf("│ %-98s │\n", "Prioridade atual: %d");
				printf("│ %-98s │\n", "Qual a nova prioridade da ideia no projeto?");
				printf("│ ► ");
				scanf("%d", &ideia->prioridade);
				scanf_flush(); 
				printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
				
				break;
			case 4: // RESUMO
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				centralizar(100, "Editar Resumo");
				printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
				printf("│ %-98s │\n", "Novo resumo da ideia (até %d caracteres):", T);
				printf("│ ► ");
				ler_frase(ideia->resumo, T);
				printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
				break;
			case 5: // Mudar Categoria
			{
				char p;
			enum Categoria c = -1;
				while (c == -1)
				{
					printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
					centralizar(100, "Mudar Categoria");
					printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
					printf("│ %-98s │\n", "ATENCAO: Mudar a categoria apagará TODOS os dados das subcategorias atuais!");
					printf("│ %-98s │\n", "(P)ersonagem, (I)tem, (C)enário, ou (O)utro?");
					printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
					printf(" Escolha: ");
					scanf("%c", &p);
					scanf_flush();
					
					switch (p)
					{
						case ('P'): case ('p'): c = C_Personagem; 	break;
						case ('I'): case ('i'): c = C_Item;			break;
						case ('C'): case ('c'): c = C_Cenario;		break;
						case ('O'):	case ('o'): c = C_Customizado;	break;
						default: 	
							c = -1;	
							limpar_tela();
					}
				}
				memset(&ideia->idade, 0, sizeof(UnionSize)); //apaga tudo
				ideia->Cat = c;
				limpar_tela();
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				centralizar(100, "Configurar Nova Categoria");
				printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
				
				switch (ideia->Cat) {
					case (C_Personagem):
						printf("│ %-98s │\n", "Insira a idade do personagem:\n");
						printf("│ ► ");
						scanf("%d", &ideia->idade);
						scanf_flush(); //limpando queue por preocaução
						printf("│ %-98s │\n", "Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):\n", C);
						for (int i = 0; i < C; i++)
						{
							printf("│ %d° caracteristica: ", i+1);
							ler_frase(ideia->carac[i], P);
						}
						scanf_flush();	
						break;
					case (C_Item):
						printf("│ %-98s │\n", "Insira o tipo do item:\n");
						printf("│ ► ");
						ler_frase(ideia->tipo, P);
						scanf_flush();
						break;
					case (C_Cenario):
						printf("│ %-98s │\n", "Insira em hexadecimal a cor predominante do cenário:\n");
						printf("│ ► ");
						scanf("%x", &ideia->cor_predom);
						scanf_flush(); //limpando queue por preocaução
						break;
					case (C_Customizado):
						printf("│ %-98s │\n", "Nome da categoria customizada: ");
						printf("│ ► ");
						ler_frase(ideia->nome_categoria_custom, P);
						scanf_flush();
						printf("│ %-98s │\n", "Quantos atributos (max %d)?", MAX_ATRIBUTOS);
						printf("│ ► ");
						scanf("%d", &ideia->num_atributos);
						scanf_flush();
						if(ideia->num_atributos > MAX_ATRIBUTOS) ideia->num_atributos = MAX_ATRIBUTOS;
						if(ideia->num_atributos < 0) ideia->num_atributos = 0;
				
						for(int i = 0; i < ideia->num_atributos; i++){
							printf("│ Atributo %d - Chave: ", i+1);
							ler_frase(ideia->atributos[i].chave, P);
							printf("│ Atributo %d - Valor: ", i+1);
							ler_frase(ideia->atributos[i].valor, P);
						}
						break;
					default: 
					printf("erro no switch Cat em editar_ideia()");
					}
					printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			}
			
			break;
			case 6: // Editar sub-categoria 1
				limpar_tela();
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				switch (ideia->Cat) {
					case (C_Personagem):
						printf("│ %-98s │\n", "Insira a idade do personagem:");
						printf("│ ► ");
						scanf_flush(); //limpando queue por preocaução		
						break;
	
					case (C_Item):
						printf("│ %-98s │\n", "Insira o tipo do item:\n");
						printf("│ ► ");
						ler_frase(ideia->tipo, P);
						break;
					case (C_Cenario):
						printf("│ %-98s │\n", "Insira em hexadecimal a cor predominante do cenário:\n");
						printf("│ ► ");
						scanf("%x", &ideia->cor_predom);
						scanf_flush(); //limpando queue por preocaução
						break;
					case (C_Customizado):
						printf("│ %-98s │\n", "Insira o nome da categoria customizada: \n");
						printf("│ ► ");
						ler_frase(ideia->nome_categoria_custom, P);
						scanf_flush();
						break;
					default: 
					printf("categoria invalida\n");
				}
				printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
			case 7: // editar sub-categoria 2
				limpar_tela();
				printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
				switch (ideia->Cat){
					case (C_Personagem):
						printf("│ %-98s │\n", "Defina o personagem em %d palavras distintas(ou frases de até 64 caracteres):\n", C);
						for (int i = 0; i < C; i++)
						{
							printf("│ %d° caracteristica (atual): %s", i+1, ideia->carac[i]);
							printf("│ %d° caracteristica nova: ");
							ler_frase(ideia->carac[i], P);
						}
						scanf_flush(); //limpando queue por preocaução
						break;
					case (C_Customizado):
						printf("Editando %d atributos:\n", ideia->num_atributos);
						for(int i = 0; i < ideia->num_atributos; i++){
							printf("│ Chave(Nome) %d (Atual: '%s')", i+1, ideia->atributos[i].chave);
							printf("│ Chave %d nova: ", i+1, ideia->atributos[i].chave);
							ler_frase(ideia->atributos[i].chave, P);
							printf("│ Valor %d (Atual: '%s')", i+1, ideia->atributos[i].valor);
							printf("│ Valor %d novo: ", i+1, ideia->atributos[i].valor);
							ler_frase(ideia->atributos[i].valor, P);
						}
						break;
					default:
						printf("Opcao invalida. \a\n"); // as vezes eu tento botar o \a pra ver se funciona, eh estranho
				}
				printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
			break;
			case 9: printf("Saindo do menu de edicao... \n"); break;
			default: printf("Entrada Invalida. \a \n"); //desde que descobri \a acho mt divertido
		}
	}
	printf("Salvando alteracoes no arquivo...\n");
	entrada_indice->status = ideia->est;
	escrever_ideia(entrada_indice->posicao, ideia);
	salvar_indice();
}

void limpar_tela(){
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}

/* Struct soh para criar a lista ordenada */
typedef struct {
	int id;
	char nome[P];
	int prioridade;
	enum Estado status;
} DisplayEntry;

int cmp_by_id(const void* a, const void* b) {
	DisplayEntry* ideaA = (DisplayEntry*)a;
	DisplayEntry* ideaB = (DisplayEntry*)b;
	return (ideaA->id - ideaB->id);
}

int cmp_by_name(const void* a, const void* b) {
	DisplayEntry* ideaA = (DisplayEntry*)a;
	DisplayEntry* ideaB = (DisplayEntry*)b;
	return strcmp(ideaA->nome, ideaB->nome); // strcmp faz a comparacao alfabetica
}

int cmp_by_priority(const void* a, const void* b) {
	DisplayEntry* ideaA = (DisplayEntry*)a;
	DisplayEntry* ideaB = (DisplayEntry*)b;
	return (ideaB->prioridade - ideaA->prioridade);
}

void listar_ideias() {
	int escolha = 0;
	char string_temp[100];

	printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
	centralizar(100, "Listar Ideias");
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│ %-98s │\n", "Como deseja ordenar a lista?");
	printf("│ %-98s │\n", "");
	printf("│ %-98s │\n", " 1. Por ID (Padrao)");
	printf("│ %-98s │\n", " 2. Por Nome (Alfabetico)");
	printf("│ %-98s │\n", " 3. Por Prioridade (Mais alta primeiro)");
	printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
	printf(" Escolha: ");
	
	scanf("%d", &escolha);
	scanf_flush();
	
	int ativas_count = 0;
	int descart_count = 0; // sei que ingles eh "discard" e caberia melhor mas to nem ai.
	for (int i = 0; i < g_total_ideias; i++) {
		if (g_indice[i].status != E_lixo) {
			if (g_indice[i].status != E_descartado) {
				ativas_count++;
			} else descart_count++;
		}
	}
	
	if (ativas_count == 0) {
		printf("Nenhuma ideia ativa para listar.\n");
		if (descart_count == 0) { //se nao tem ativas, nem descartadas, pra que neh?
			return;
		}
	}
	
	int count = 0; // soh porque eu precisava de um contador pro for alem do "i"
	DisplayEntry* display_list = NULL; // soh porque nao posso declarar a variável dentro do if
	DisplayEntry* discard_list = NULL; // msm coisa
	
	if (ativas_count > 0) {
		display_list = (DisplayEntry*) malloc(sizeof(DisplayEntry) * ativas_count);
		if (display_list == NULL) {
			printf("Erro: Falha ao alocar memoria para a lista normal.\n");
			return;
		}
		for (int i = 0; i < g_total_ideias; i++) {
			if ((g_indice[i].status != E_lixo) && (g_indice[i].status != E_descartado)) {
				struct Ideia ideia_temp;
				if (ler_ideia_arquivo(g_indice[i].posicao, &ideia_temp)) {
					display_list[count].id = ideia_temp.id;
					strcpy(display_list[count].nome, ideia_temp.nome);
					display_list[count].prioridade = ideia_temp.prioridade;
					display_list[count].status = ideia_temp.est; // (para o printf)
        	    	count++;
        		}
        	}
    	}
	}
	if (descart_count > 0) {
		discard_list = (DisplayEntry*) malloc(sizeof(DisplayEntry) * descart_count);
		if (discard_list == NULL) { // display e discard eh muito proximo pra eu nao usar
			printf("Erro: Falha ao alocar memoria para a lista de descarte.\n");
			return;
		}
		descart_count = 0; // tava afim de reutilizar esse int aqui
		for (int i = 0; i < g_total_ideias; i++) {
			if ((g_indice[i].status != E_lixo) && (g_indice[i].status == E_descartado)) {
				struct Ideia ideia_temp;
				if (ler_ideia_arquivo(g_indice[i].posicao, &ideia_temp)) {
					discard_list[descart_count].id = ideia_temp.id;
					strcpy(discard_list[descart_count].nome, ideia_temp.nome);
					discard_list[descart_count].prioridade = ideia_temp.prioridade;
					discard_list[descart_count].status = ideia_temp.est;
					descart_count++;
        		}
			}
		}
	}
	limpar_tela();
    printf("Ordenando lista...\n");
	switch (escolha) {
		case 2: // Nome
			if (count > 0){
				qsort(display_list, count, sizeof(DisplayEntry), cmp_by_name);
			}
			if (descart_count > 0){
				qsort(discard_list, descart_count, sizeof(DisplayEntry), cmp_by_name);
			} break;
		case 3: // Prioridade
			if (count > 0){
				qsort(display_list, count, sizeof(DisplayEntry), cmp_by_priority);
			} 
			if (descart_count > 0){
				qsort(discard_list, descart_count, sizeof(DisplayEntry), cmp_by_priority);
			} break;
		case 1: // ID (ou qualquer outra tecla)
		default:
			if (count > 0){
				qsort(display_list, count, sizeof(DisplayEntry), cmp_by_id);
			} 
			if (descart_count > 0){
				qsort(discard_list, descart_count, sizeof(DisplayEntry), cmp_by_id);
			} break;
    }
	printf("╭────────────────────────────────────────────────────────────────────────────────────────────────────╮\n");
	sprintf(string_temp, "Lista de Ideias Ativas (%d)", count);
	centralizar(100, string_temp);
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	printf("│ %-64s │ %-5s │ %-10s │ %-10s │\n", "NOME", "ID", "ESTADO", "PRIO");
	printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
	
	if (count > 0){
		for (int i = 0; i < count; i++) {
			char estado_str[15] = "DESCONHECIDO";
			switch (display_list[i].status) {
				case E_concluido:		strcpy(estado_str, "Concluido"); break;
				case E_em_progresso:	strcpy(estado_str, "Em Prog."); break;
				case E_a_fazer:			strcpy(estado_str, "A Fazer"); break;
				case E_descartado:		strcpy(estado_str, "Descartado"); break;
				default: break;
			}
		
			printf("│ %-64s │ %-5d │ %-10s │ %-10d │\n",
					display_list[i].nome,
					display_list[i].id,
					estado_str,
					display_list[i].prioridade);
		}
	}
	if (descart_count > 0){
		printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
		sprintf(string_temp, "Lista de Ideias Descartadas (%d)", descart_count);
		centralizar(100, string_temp);
		printf("├────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");
		
		for (int i = 0; i < descart_count; i++) {
			char estado_str[15] = "DESCONHECIDO";
			switch (discard_list[i].status) { // sei que se ele ta aqui, ele eh descartado, mas ainda gosto do switch, se por algum motivo alguma ideia vem pra ca que nao deveria, da pra ver logo de cara
				case E_concluido:		strcpy(estado_str, "Concluido"); break;
				case E_em_progresso:	strcpy(estado_str, "Em Prog."); break;
				case E_a_fazer:			strcpy(estado_str, "A Fazer"); break;
				case E_descartado:		strcpy(estado_str, "Descartado"); break;
				default: break;
			}		
			printf("│ %-64s │ %-5d │ %-10s │ %-10d │\n",
					discard_list[i].nome,
					discard_list[i].id,
					estado_str,
					discard_list[i].prioridade);
		}
	}
	printf("╰────────────────────────────────────────────────────────────────────────────────────────────────────╯\n");
	free(display_list);
	free(discard_list);
}
