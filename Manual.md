# Manual GDD Idea Inventory

<img width="500" height="162" alt="Image" src="https://github.com/user-attachments/assets/2028bdfa-27e4-486b-9406-16b6cf29b8fb" />

## Execução do projeto
Com os arquivos `GDDI.c` e `gdd_types.h` baixados (na mesma pasta!!!). Siga os seguintes passos:

1.  **Compilação:** Use um compilador C (Ex: GCC): `gcc GDDI.c -o GDDI.out` (linux) `gcc GDDI.c -o GDDI.exe` (Windows)
2.  **Execução:** Execute o arquivo compilado: `./GDDI.out` no linux ou apenas `GDDI.exe` no Windows.

O arquivo será gerado inicialmente sem idéias, porém caso já exista um arquivo gerado, ele será lido normalmente com seus devidos dados.

<img width="818" height="332" alt="Image" src="https://github.com/user-attachments/assets/e428589f-e520-4a05-aab0-04308cd41a00" />



---

## Elementos de idéias

Antes criar uma idéia é importante saber os elementos que cada idéia pode vir a conter, aqui vão definições sobre campos mais específicos.

#### Categoria:

Define o tipo de idéia, possuindo características específicas a serem adicionadas a idéia a depender dessa escolha.
#### Prioridade:

Define o grau de importância de uma idéia em relação a outras idéias, sendo um número de 0 até o limite do tipo int (2 bilhões). Números maiores tem maior prioridade, sendo também mostrados primeiro na função `exibir idéia`.<p>
<u>_Obs: É aconselhado usar um intervalo próprio._</u>

#### Estado:

Indica a grau de desenvolvimento da idéia, sendo também possível alterar ao longo do tempo editando esta parte da idéia.

## Criação de uma idéia
Cada idéia adicionada possuirá características diferentes a depender de sua categoria, siga as instruções do programa baseado na idéia selecionada.

**Exemplo de respostas para a criação de uma idéia:**

* Nome: Jorge
* Resumo: Guerreiro 
* Categoria: (P) Personagem 
* Idade: 22* 
* Característica 1: Forte *
* Característica 2: Armadura *
* Característica 3: Espada * 
* Estado: (E) Em progresso 
* Prioridade: 2

<u>_Termos definidos pela categoria marcados com (*)_</u>

<img width="838" height="404" alt="Image" src="https://github.com/user-attachments/assets/d1590c63-bf6c-41e3-848e-77de66bbdca3" />


Assim que todos os campos forem preenchidos, a idéia será criada e o próximo ID será uma unidade maior que o atual.



## Funções de idéia

#### Descartar idéia:

Solicita uma idéia através de seu ID e modifica o estado da idéia para Descartado e sua prioridade para 0, sendo uma forma recuperável de "deletar" uma idéia.

#### Buscar/editar idéia:

Ao selecionar essa opção será requisitado o ID da idéia, sendo possível observar a idéia por completo e editar quaisquer partes desejadas pelo usuário. <u>_(Incluindo idéias descartadas)_</u>


#### Exibir todas as idéias:

O programa irá identificar todas as idéias e exibir nome, ID, estado e prioridade no terminal, separando entre idéias ativas e descartadas. Sendo possível ordenar por ID, ordem alfabética ou ordem de prioridade <u>_(Maior primeiro)_</u>.

<img width="821" height="445" alt="Image" src="https://github.com/user-attachments/assets/3de8a959-cf59-4f04-9653-e4c2af065172" />

#### Excluir idéia 

Após pedir o ID respectivo, exclui uma idéia de forma definitiva substituindo seus dados por zeros, sendo irrecuperável após feito. Após isso, a idéia é torna-se do estado lixo.

#### Compactar idéia 

Libera o espaço ocupado por idéias excluídas permanentemente. <p>
**Funcionamento:** Cria um novo arquivo onde todos os dados que não são lixo serão copiados, após isso o arquivo anterior é desalocado e subsituido pela cópia.

**implementação:**

<img width="843" height="694" alt="Image" src="https://github.com/user-attachments/assets/76b3b57c-72b0-456f-be17-f2305546946d" />


