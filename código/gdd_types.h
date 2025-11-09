//gdd_types.h

#ifndef GDD_TYPES_H  // include guards bla bla bla
#define GDD_TYPES_H

// 1. DEFINICOES GLOBAIS

#define P 64     // numero de caracteres em poucas palavras
#define T 512    // numero de caracteres em um bloco de texto
#define C 3      // numero de caracteristicas de um personagem
#define MAX_ATRIBUTOS 10 // Pra a categoria customizada

// 2. ENUMS (ETIQUETAS)

enum Categoria {
    C_Personagem,
    C_Item,
    C_Cenario,
    C_Customizado 
};

enum Estado {
	E_lixo,			//Exclusao de verdade
    E_concluido,
    E_em_progresso,
    E_a_fazer,
    E_descartado	// soft delete
};

// 3. UNION DISCRIMINADA E STRUCTS

// A struct para um atributo customizado (Chave/Valor)
typedef struct {
    char chave[P];
    char valor[P];
} Atributo;

// A Union que usamos soh pro sizeof(), ja que a union na struct principal eh anonima
// Preferi deixar anonima pra ser mais facil de escrever e ler mas ainda precisava do sizeof.
typedef union {
    // Tipo 1: Personagem
    struct {
        int idade;
        char carac[C][P];
    } p_dados; // (Damos um nome aqui soh para o sizeof funcionar)

    // Tipo 2: Item
    char tipo[P];

    // Tipo 3: Cenario
    int cor_predom; 
    
    // Tipo 4: Customizada (Flexivel)
    struct {
        char nome_categoria_custom[P];
        int num_atributos;
        Atributo atributos[MAX_ATRIBUTOS];
    } custom;

} UnionSize;

// STRUCT PRINCIPAL
struct Ideia {
    int id;
    enum Estado est;
    
    char nome[P];
    char resumo[T];

    enum Categoria Cat;
    union { // A union anonima real
        // Personagem
        struct {
            int idade;
            char carac[C][P];
        };
        // Item
        char tipo[P];
        // Cenario
        int cor_predom;
        // Customizada
        struct {
            char nome_categoria_custom[P];
            int num_atributos;
            Atributo atributos[MAX_ATRIBUTOS];
        };
    };
    
    int prioridade;
};

// 4. Estrutura do Indice

typedef struct {
    int id;
    long posicao;
    enum Estado status; // Espelho do status (pra buscar mais rapido, se a ideia for descartada, o programa nem lê o resto)
} IndiceEntry;

// 5. TAMANHO DO REGISTRO
// soh pra evitar padding e essas coisas fazendo sizeof(struct)
#define tamanho_registro ( \
      sizeof(int)				+ /* id */ \
      sizeof(enum Estado)		+ /* est */ \
      sizeof(char[P])			+ /* nome */ \
      sizeof(char[T])			+ /* resumo */ \
      sizeof(enum Categoria)	+ /* Cat */ \
      sizeof(UnionSize)			+ /* A union */ \
      sizeof(int)				  /* prioridade */ \
)

#endif