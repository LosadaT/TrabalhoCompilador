# TrabalhoCompilador
Trabalho da disciplina de compiladores, para fazer um analisador lexico, sintatico e o gerador de código intermediario
## Parte 1
No trabalho foi feito o analisador lexico e sintatico do compilador, sendo criado uma tabela de palavras reservadas, função consome, um analisador léxico e um ASDR para uam linguagem semelhante a Pascal.

No analisador Léxico foi feito uma função para obter o átomo, que ignora os caracteres como '/n' e ' ', cria um atomo para o comentário, identificando quando o buffer e o buffer+1 são "(* " e vai descartando o texto, até chegar no "*)", criando o átomo comentario. Também é criado o átomo para o número, com func reconhece_numero(...), que, além de idenficar ele em decimal, identifica na notação exponencial. Por último, tem um reconhecedor de identificador/palavra reservada, com func reconhece_id(...). Após isso, tem os reconhecedores dos outros sinal, que não são nem identificadores nem números, como a ',' e ')'.

Para as palavras reservadas, criei uma nova struct, com a palavra reservada e o atomo que representa ele. Dessa maneira, pude comparar o atomo com a lista de palavras reservadas.

Já no ASDR, foi criado uma função para cada regra da gramática, onde analisando a gramática e seu FIRST criei as funções. Além disso, foi utilizado um enum para padronizar os átomos e evitar possíveis problemas.

Por último, na função consome(), foi feito, no inicio e no final, dois whiles iguais, que consomem o comentário e apresenta a linha no terminal. Temos dois while iguais porque, o primeiro reconhece comentarios no começo do codigo, e o outro depois de alguma linha valida. Depois entra em um switch case em que, se o lookahead for igual ao átomo, entra no caso especifico, dando o print no terminal.

Para compilar o arquivo, utilize: gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador 

Para alterar o arquivo de entrada, mude a string na linha 138, -> FILE *f = fopen("seuarquivo.txt", "r");

No arquivo de teste, quando copiado diretamente do PDF, ocorre um erro, porque as aspas simples utilizadas no pdf (‘ ’) são diferentes da do compilador (' '). É recomendado alterar ela no arquivo teste para funcionamento correto do compilador.
Outro problema é que, quando o erro ocorre por conta de um comentario não fechado, não consegui fazer o programa retornar o erro correto. Ao invez de dizer que era esperado uma fecha de comentario, retorna dizendo que espera o [END].

## Parte 2
### Funções:
Foram implementadas 5 novas funções, a inicializa_tabela_simbolos( ), que prenche ela de NULLs, para popular ela e ser possível a comparação utilizada na função insere_tabela_simbolos(char *id), que verifica se o valor passado já existe na tabela, se sim, passa um erro e da um exit(1), caso contrario insere um novo simbolo (no) na tabela. <br>
A próxima função, busca_tabela_simbolos(char *id) percorre a tabela e verifica se a variavel já foi declarada antes no programa, se sim, retornando ela, caso contrario, passando um erro e dando um exit(1). <br>
No final da execução do programa, utilizo a função imprime_tabela_simbolos( ) para mostrar ao usuario as variáveis declaradas, seus identificadores e endereços. <br>
A ultima função implementada foi a proximo_rotulo, que apenas aumenta um contador global para passar qual o próximo Ln que o programa vai passar.
Além disso foi implementada a função hashMack(char * s) passada pelo professor.
### Structs
Para essa fase do projeto, foi implementada uma tabela de símbolos simples, como instruido na proposta do projeto
### Geração de MEPA
Para a geração de mepa, foram inseridos nas funções do analisador sintático. No começo da execução, o INPP é chamado, e no final o PARA. Para a alocação de variavel (AMEM), conto a quantidade de variaveis que foram declaradas a partir do contador endereços, que é atualizado quando um valor é inserido na tabela de simbolos.<br>
Passando agora mais rapidamente pelos os outros comandos, o ARMZ é colocado no final do assingment_statement, ou seja, após o ":=" no codigo, utilizando o endereço da variavel. Para o LEIT foi somente colocar depois de identificar que é uma leitura, o IMPR a mesma coisa, porém é utilizando um CRVL antes, para mostrar qual valor está sendo impresso. <br>
Para o DSVF foi colocado antes do statemente, na funcao if_statement, e o DSVS depois, seguindo a lógica do MEPA, depois do if vem sempre um desvia se falso, e no final sempre um desvia sempre. A lógica foi semelhante para no while. A lógica para onde é desviado vem com a função proximo_rotulo. Então o DSVF vai ser para o primeiro valor e o DSVS para o segundo, e esse valor vai aumentado em uma variavel global. <br>
Para as expressões de comparação, todas foram colocadas depois do valor de comparação ser comsumido e a comparação ser realizada. Para os operadores matematicos, a logica foi semelhante, se a operação for de soma, no final dela o SOMA é apresentado, e assim para todos<br>
Por ultimo, para os outros CRVL e CRCT que não estão relacionados a funções de read e write, são apresentados na func factor( ). Se o valor for um identifier, ele busca o valor na tabela de simbolos, e após isso da um print CRVL com o endereço da variavel e, depois, consome o identifier. Algo parecido ocorre com o CRCT, porem ele não busca na tabela de simbolos, por não ser uma variável.
### Erros Esperados
Conforme a proposta do trabalho, o programa ainda não aceita char ainda, dessa maneira, a existência delas foi ignorada nessa fase do projeto. Então, não existe um erro planejado para caso o código fonte tenha um char. Outro erro que o programa não apresenta corretamente é o caso de construções que misturam os dois tipos, por exemplo: 25+(x>y). Para os dois casos citados, um erro é apresentado e o programa encerra, só não o erro correto<br>
Alguns dos erros que são apresentados de maneira correta são para casos como duas variáveis com o mesmo nome ou a utilização de uma variável que não foi declarada, o programa idenfica corretamente o erro, entre outros.
### Decisões de código
O ponto que me deu mais problemas no desenvolvimento do projeto, além de criar as funções para manipular a tabela de simbolos e o tratamento de erros, foi que, por algum motivo que não consegui idenficar, na função variable_declaration( ), para inserir na tabela de simbolos, tive que criar uma copia do infoAtomo.atributo.ID para utilizar na função insere_tabela_simbolos(). Caso contrario, ele não identificava que a variável tinha sido declarada.
