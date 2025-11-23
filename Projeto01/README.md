# Trabalho Compilador Parte 2
Francisco Losada Totaro - 10364673
## O que foi implementado:
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