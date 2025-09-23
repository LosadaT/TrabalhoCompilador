# TrabalhoCompilador
Trabalho da disciplina de compiladores

No trabalho foi feito o analisador lexico e sintatico do compilador, sendo criado uma tabela de palavras reservadas, função consome, um analisador léxico e um ASDR para a linguagem PasKenzie.

No analisador Léxico foi feito uma função para obter o átomo, que ignora os caracteres como '/n' e ' ', cria um atomo para o comentário, identificando quando o buffer e o buffer+1 são "(* " e vai descartando o texto, até chegar no "*)", criando o átomo comentario. Também é criado o átomo para o número, com func reconhece_numero(...), que, além de idenficar ele em decimal, identifica na notação exponencial. Por último, tem um reconhecedor de identificador/palavra reservada, com func reconhece_id(...). Após isso, tem os reconhecedores dos outros sinal, que não são nem identificadores nem números, como a ',' e ')'.

Para as palavras reservadas, criei uma nova struct, com a palavra reservada e o atomo que representa ele. Dessa maneira, pude comparar o atomo com a lista de palavras reservadas.

Já no ASDR, foi criado uma função para cada regra da gramática, onde analisando a gramática e seu FIRST criei as funções. Além disso, foi utilizado um enum para padronizar os átomos e evitar possíveis problemas.

Por último, na função consome(), foi feito, no inicio e no final, dois whiles iguais, que consomem o comentário e apresenta a linha no terminal. Temos dois while iguais porque, o primeiro reconhece comentarios no começo do codigo, e o outro depois de alguma linha valida. Depois entra em um switch case em que, se o lookahead for igual ao átomo, entra no caso especifico, dando o print no terminal.

Para compilar o arquivo, utilize: gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador 
Para alterar o arquivo de entrada, mude a string na linha 138, -> FILE *f = fopen("seuarquivo.txt", "r");

No arquivo de teste, quando copiado diretamente do PDF, ocorre um erro, porque as aspas simples utilizadas no pdf (‘ ’) são diferentes da do compilador (' '). É recomendado alterar ela no arquivo teste para funcionamento correto do compilador.
Outro problema é que, quando o erro ocorre por conta de um comentario não fechado, não consegui fazer o programa retornar o erro correto. Ao invez de dizer que era esperado uma fecha de comentario, retorna dizendo que espera o [END].
