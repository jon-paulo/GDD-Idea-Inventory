# GGD Idea Inventory: Gerenciamento de ideias de um Game Design Document

## Breve Descrição do Projeto

Este projeto é um sistema de gerenciamento de dados, focado na organização e controle de ideias para um Game Design Document, projetos relacionados a desenvolvimentos de jogos, personagems, histórias ou outros *designs*. Ele atua como uma interface para o inventário, onde o usuário pode criar, buscar, verificar e editar o estado de cada ideia.

---

## Domínio da Aplicação: Invetário de Ideias Criativas

O domínio da aplicação é o **Gerenciamento de Elementos de Design e Desenvolvimento (GDD - Game Design Document)**.

O objetivo principal é manter um registro organizado da parte de ideias criativas de um projeto de jogo. Coisas como Personagens, Cenários e Itens.

---

## TIpos de Informações Gerenciadas

A estrutura principal é a `struct Ideia`, que usa **Enums** e **Unions** para categorizar e armazenar informações de forma eficiente.

### 1. Entidade Principal: `Ideia`
Gerencia os dados comuns a todos os conceitos:
* **ID:** Identificador único da ideia.
* **Nome:** Título da ideia (até 64 caracteres).
* **Resumo:** Descrição detalhada (até 512 caracteres).
* **Estado (`enum Estado`):** Status de desenvolvimento (A Fazer, Em Progresso, Concluído, Descartado).
* **Prioridade:** Nível de importância (0 a N).

### 2. Categorias Específicas (`union` baseada em `enum Categoria`)
Cada Ideia pertence a uma de três categorias, otimizando o armazenamento com o uso de `union`:

| Categoria | Dados Específicos |
| :---: | :--- |
| **Personagem** | Idade e 3 características (frases curtas). |
| **Item** | Tipo do item (Ex: arma, poção, artefato). |
| **Cenário** | Cor predominante (armazenada em formato hexadecimal). |

---

## ⚙️ Tipos de Operações Desenvolvidas (Funcionalidades)

O programa implementa as operações básicas de gerenciamento de dados (CRUD - **C**reate, **R**ead, **U**pdate, **D**elete), todas persistidas no arquivo `GDD.bin`:

| Tipo de Operação | Função | Descrição |
| :---: | :--- | :--- |
| **C**reate | `criar_ideia()` | Adiciona uma nova ideia ao inventário, atribuindo um ID sequencial. |
| **R**ead | `buscar_ideia()` | Localiza uma ideia pelo ID e exibe seus detalhes completos na tela. |
| **U**pdate | `editar_ideia()` / `menu_edit()` | Permite alterar o Nome, Resumo, Estado, Prioridade ou os dados específicos da Categoria (Personagem, Item, Cenário). |
| **D**elete (Lógico) | `descartar_ideia()` | Marca uma ideia como `E_descartado`, tornando-a inacessível nas buscas normais (Recuperação é possível). |
| **Persistência** | `escrever_ideia()` / `ler_ideia_arquivo()` | Funções de baixo nível que gerenciam a leitura e escrita estruturada dos dados no arquivo binário, usando `fseek` para acesso direto à posição correta. |

---

## 🚀 Como Executar o Projeto

1.  **Compilação:** Use um compilador C (Ex: GCC): `gcc -o GDD main.c`
2.  **Execução:** Execute o arquivo compilado: `./GDD`
3.  O sistema criará ou abrirá o arquivo de dados binário chamado `GDD.bin`.
