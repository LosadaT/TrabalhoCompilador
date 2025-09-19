#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> //atof
#define MAX_BUFFER 10000

char buffer_global[MAX_BUFFER];
char *buffer;
// tipos do analisador lexico
// código interno para atomos
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    OP_SUB,
    OP_DIV,
    EOS,
    PROGRAM,
    VAR,
    CHAR,
    INTEGER,
    BOOLEAN,
    READ,
    WRITE,
    IF,
    THEN,
    ELSE,
    WHILE,
    DO,
    BEGIN,
    END,
    ASSIGN,
    OR,
    AND,
    NOT,
    TRUE,
    FALSE,
    MENOR,
    MENORIGUAL,
    MAIOR,
    MAIORIGUAL,
    IGUAL,
    DIFERENTE,
    CONSTCHAR
}TAtomo;

// estrutura para retorno da funcao obter_atomo()
typedef struct{
    TAtomo atomo;
    int linha;
    union{
        float numero;
        char ID[16];
        char ch;
    }atributo;
}TInfoAtomo;

// Adicione no início do arquivo
typedef struct {
    const char *palavra;
    TAtomo atomo;
} PalavraReservada;

PalavraReservada reservadas[] = {
    {"program", PROGRAM},
    {"var", VAR},
    {"char", CHAR},
    {"integer", INTEGER},
    {"boolean", BOOLEAN},
    {"read", READ},
    {"write", WRITE},
    {"if", IF},
    {"then", THEN},
    {"else", ELSE},
    {"while", WHILE},
    {"do", DO},
    {"begin", BEGIN},
    {"end", END},
    {"or", OR},
    {"and", AND},
    {"not", NOT},
    {"true", TRUE},
    {"false", FALSE},
    {"div", OP_DIV}, // ou outro átomo para 'div'
    {NULL, ERRO}
};

// varivel global do analisador lexico
char lexema[20];
int nLinha;
char *buffer = "program xxx;";
// variavel global ASDR
TAtomo lookahead;
TInfoAtomo infoAtomo;
char *strMensagem[] = {"erro lexico", "IDENTIFICADOR", "NUMERO", "+", "*", "EOS", "PROGRAM", "VAR", "CHAR", "INTEGER", "BOOLEAN", "READ", "WRITE", "IF", "THEN", "ELSE", "WHILE", "DO", "BEGIN", "END", ":=", "OR", "AND", "NOT", "TRUE", "FALSE", "<", "<=", ">", ">=", "=", "<>", "DIV", "CONSTCHAR"};
// funcoes do analisador lexico
TInfoAtomo obter_atomo();
void reconhece_numero(TInfoAtomo *infoAtomo);
void reconhe_id(TInfoAtomo *infoAtomo);

// funcoes do ASDR
void consome(TAtomo atomo);

void program();
void block();
void variable_declaration_part();
void variable_declaration();
void type();
void statement_part();
void statement();
void assignment_statement();
void read_statement();
void write_statement();
void if_statement();
void while_statement();
void expression();
void relational_expression();
void simple_expression();
void adding_operator();
void term();
void multiplying_operator();
void factor();

// int main(){
//     nLinha = 1;
//     printf("Analisando: %s",buffer);
//     infoAtomo = obter_atomo();
//     lookahead = infoAtomo.atomo; // recebe o simbolo inicial e atualiza o buffer
//     program(); // chama o simbolo inicial da gramatica
//     consome(EOS);
//     printf("\nfim de programa. %d linhas analisadas\n", infoAtomo.linha);

//     return 0;
// }

int main(){
    nLinha = 1;
    FILE *f = fopen("codigo.txt", "r");
    if (!f) {
        printf("Erro ao abrir codigo.txt\n");
        return 1;
    }
    size_t lidos = fread(buffer_global, 1, MAX_BUFFER-1, f);
    buffer_global[lidos] = '\0';
    fclose(f);
    buffer = buffer_global;

    printf("Analisando: %s",buffer);
    infoAtomo = obter_atomo();
    lookahead = infoAtomo.atomo;
    program();
    consome(EOS);
    printf("\nfim de programa. %d linhas analisadas\n", infoAtomo.linha);

    return 0;
}

TInfoAtomo obter_atomo() {
    TInfoAtomo infoAtomo;

    infoAtomo.atomo = ERRO;
    while(*buffer == '\n' || *buffer == ' ' || *buffer == '\t' || *buffer == '\r'){
        if(*buffer == '\n')
            nLinha++;
        buffer++;
    }

    infoAtomo.linha = nLinha;

    if(isdigit(*buffer)){
        reconhece_numero(&infoAtomo);
    }
    else if(islower(*buffer) || isupper(*buffer) || *buffer == '_')
        reconhe_id(&infoAtomo);

    else if (*buffer == '\'') {
        buffer++;
        if (*buffer != '\0' && *(buffer+1) == '\'') {
            infoAtomo.atributo.ch = *buffer;
            buffer += 2;
            infoAtomo.atomo = CONSTCHAR;
        } else {
            infoAtomo.atomo = ERRO;
        }
    }

    else if (*buffer == ';') {
        buffer++;
        infoAtomo.atomo = ';';
    }

    else if (*buffer == ',') {
        buffer++;
        infoAtomo.atomo = ',';
    }

    else if (*buffer == ':') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = ASSIGN;
        } else {
            buffer++;
            infoAtomo.atomo = ':';
        }
    }

    else if (*buffer == '(') {
        buffer++;
        infoAtomo.atomo = '(';
    }

    else if (*buffer == ')') {
        buffer++;
        infoAtomo.atomo = ')';
    }

    else if (*buffer == '.') {
        buffer++;
        infoAtomo.atomo = '.';
    }

    else if (*buffer == '+') {
        buffer++;
        infoAtomo.atomo = OP_SOMA;
    }

    else if (*buffer == '-') {
        buffer++;
        infoAtomo.atomo = OP_SUB;
    }

    else if (*buffer == '*') {
        buffer++;
        infoAtomo.atomo = OP_MULT;
    }

    else if (*buffer == '/') {
        buffer++;
        infoAtomo.atomo = OP_DIV;
    }

    else if (*buffer == '<') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = MENORIGUAL;
        } 
        else if (*(buffer+1) == '>') {
            buffer += 2;
            infoAtomo.atomo = DIFERENTE;
        } 
        else {
            buffer++;
            infoAtomo.atomo = MENOR;
        }
    }

    else if (*buffer == '>') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = MAIORIGUAL;
        } else {
            buffer++;
            infoAtomo.atomo = MAIOR;
        }
    }

    else if (*buffer == '=') {
        buffer++;
        infoAtomo.atomo = IGUAL;
    }

    else if (*buffer == '\0')
        infoAtomo.atomo = EOS;

    return infoAtomo;
}

/*
void reconhece_numero(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
q1:
    if( isdigit(*buffer) ){
        buffer++;
        goto q1;
    }
    if( *buffer == '.' ){
        buffer++;
        goto q2;
    }
    return;

q2:
    if( isdigit(*buffer) ){
        buffer++;
        goto q3;
    }
    return;
q3:
    if( isdigit(*buffer) ){
        buffer++;
        goto q3;
    }
    if( isalpha(*buffer)){
        return;
    }
    // recorta lexama 
    strncpy(lexema,ini_lexema,buffer-ini_lexema);
    lexema[buffer-ini_lexema] = '\0'; // aqui temos uma string
    infoAtomo->atributo.numero = atof(lexema);
    infoAtomo->atomo = NUMERO;

    return;

}
*/

void reconhece_numero(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int is_expo = 0;

    while (isdigit(*buffer)) buffer++;

    if (*buffer == 'd' || *buffer == 'D') {
        is_expo = 1;
        buffer++;
        if (*buffer == '+' || *buffer == '-') buffer++;
        if (!isdigit(*buffer)) {
            infoAtomo->atomo = ERRO;
            return;
        }
        while (isdigit(*buffer)) buffer++;
    }

    int tam = buffer - ini_lexema;
    strncpy(lexema, ini_lexema, tam);
    lexema[tam] = '\0';

    infoAtomo->atributo.numero = atof(lexema);
    infoAtomo->atomo = NUMERO;
}


void reconhe_id(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int tam = 0;

q1:
    if(islower(*buffer) || isupper(*buffer) || *buffer == '_'){
        buffer++;
        tam++;
        goto q1;
    }
    if(isdigit(*buffer)){
        return; // sai com erro
    }
    // if (tam > 15) {
    //     infoAtomo->atomo = ERRO;
    //     return; // sai com erro
    // }
    // preenche o atributo do atomo IDENTIFICADOR
    // strncpy(infoAtomo->atributo.ID,ini_lexema,buffer-ini_lexema);
    // infoAtomo->atributo.ID[buffer-ini_lexema] = '\0'; // aqui temos um IDENTIFICADOR
    // infoAtomo->atomo = IDENTIFICADOR;
    strncpy(infoAtomo->atributo.ID, ini_lexema, buffer-ini_lexema);
    infoAtomo->atributo.ID[buffer-ini_lexema] = '\0';

    // Verifica se é palavra reservada
    for (int i = 0; reservadas[i].palavra != NULL; i++) {
        if (strcmp(infoAtomo->atributo.ID, reservadas[i].palavra) == 0) {
            infoAtomo->atomo = reservadas[i].atomo;
            return;
    }
}
infoAtomo->atomo = IDENTIFICADOR;
    return;
}

// ASDR
// <program> ::= program identifier ‘;‘ <block> ‘.'
    //FIRST(<program>) = { program }
void program() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(';');
    block();
    consome('.');
}

// <block> ::= <variable_declaration_part> <statement_part>
    //FIRST(<block>) = { var, begin }
void block() {
    variable_declaration_part();
    statement_part();
}

// <variable_declaration_part> ::= [ var <variable_declaration> ‘;’ { <variable_declaration> ‘;’ } ]
    // FIRST(<variable_declaration_part>) = { var, λ }
void variable_declaration_part() {
    if( lookahead == VAR ){
        consome(VAR);
        variable_declaration();
        consome(';');
        while( lookahead == IDENTIFICADOR ){    
            variable_declaration();
            consome(';');
        } 
    }  
}
// <variable_declaration> ::= identifier { ‘,’ identifier } ‘:’ <type>
    //FIRST(<variable_declaration>) = { identifier }
void variable_declaration() {
    consome(IDENTIFICADOR);
    while(lookahead == ','){
        consome(',');
        consome(IDENTIFICADOR);
    }
    consome(':');
    type();
}
// <type> ::= char | integer | boolean
    //FIRST(<type>) = { char, integer, boolean }
void type() {
    if(lookahead == CHAR) {
        consome(lookahead);
    }
    else if( lookahead == INTEGER ) {
        consome(lookahead);
    }   
    else if( lookahead == BOOLEAN ) {
        consome(lookahead);   
    }
}
// <statment_part> ::= begin <statement> { ‘;’ <statement> } end
    //FIRST(<statement_part>) = { begin }
    void statement_part() {
        consome(BEGIN);
        statement();
        while( lookahead == ';' ){
            consome(';');
            statement();
        }
        consome(END);
    }
// <statement> ::= <assignment_statement> | <read_statement> | <write_statement> | <if_statement> | <while_statement> | <statement_part>
    //FIRST(<statement>) = { identifier, read, write, if, while, begin }
    void statement() {
        if( lookahead == IDENTIFICADOR )
            assignment_statement();
        else if( lookahead == READ )
            read_statement();
        else if( lookahead == WRITE )
            write_statement();
        else if( lookahead == IF )
            if_statement();
        else if( lookahead == WHILE )
            while_statement();
        else if( lookahead == BEGIN )
            statement_part();
    }
// <assignment_statement> ::= identifier ‘:=’ <expression>
    //FIRST(<assignment_statement>) = { identifier }
    void assignment_statement() {
        consome(IDENTIFICADOR);
        consome(ASSIGN);
        expression();
    }
// <read_statement> ::= read ‘(’ identifier { ‘,’ identifier } ‘)’
    //FIRST(<read_statement>) = { read }
    void read_statement() {
        consome(READ);
        consome('(');
        consome(IDENTIFICADOR);
        while( lookahead == ',' ){
            consome(',');
            consome(IDENTIFICADOR);
        }
        consome(')');
    }
// <write_statement> ::= write ‘(’ identifier { ‘,’ identifier } ‘)’
    //FIRST(<write_statement>) = { write }
    void write_statement() {
        consome(WRITE);
        consome('(');
        consome(IDENTIFICADOR);
        while( lookahead == ',' ){
            consome(',');
            consome(IDENTIFICADOR);
        }
        consome(')');
    }
// <if_statement> ::= if <expression> then <statement> [ else <statement> ]
    //FIRST(<if_statement>) = { if }
    void if_statement() {
        consome(IF);
        expression();
        consome(THEN);
        statement();
        if( lookahead == ELSE ){
            consome(ELSE);
            statement();
        }
    }
// <while_statement> ::= while <expression> do <statement>
    //FIRST(<while_statement>) = { while }
    void while_statement() {
        consome(WHILE);
        expression();
        consome(DO);
        statement();
    }
// <expression> ::= <simple_expression> [ <relational_operator> <simple expression> ]
    //FIRST(<expression>) = { identifier, constint, constchar, (, not, true, false }
    void expression() {
        simple_expression();
        if( lookahead == MENOR || lookahead == MENORIGUAL || lookahead == MAIOR || lookahead == MAIORIGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == OR || lookahead == AND )
            relational_expression();
    }

// <relational_operator> ::= ‘<>’ | ‘<’ | ‘<=’ | ‘>=’ | ‘>’ | ‘=’ | or | and
    //FIRST(<relational_operator>) = { <>, <, <=, >=, >, =, or, and }
    void relational_expression() {
        if( lookahead == MENOR )
            consome(lookahead);
        else if( lookahead == MENORIGUAL )
            consome(lookahead);
        else if( lookahead == MAIOR )
            consome(lookahead);
        else if( lookahead == MAIORIGUAL )
            consome(lookahead);
        else if( lookahead == IGUAL )
            consome(lookahead);
        else if( lookahead == DIFERENTE )
            consome(lookahead);
        else if( lookahead == OR )
            consome(lookahead);
        else if( lookahead == AND )
            consome(lookahead);
        simple_expression();
    }
// <simple_expression> ::= <term> { <adding_operator> <term> }
    //FIRST(<simple_expression>) = { identifier, constint, constchar, (, not, true, false }
    void simple_expression() {
        term();
        while(lookahead == OP_SOMA){
            adding_operator();
            term();
        }
    }
// <adding_operator> ::= ‘+’ | ‘-’
    //FIRST(<adding_operator>) = { +, - }
    void adding_operator() {
        if( lookahead == OP_SOMA )
            consome(OP_SOMA);
    }
// <term> ::= <factor> { <multiplying_operator> <factor> }
    //FIRST(<term>) = { identifier, constint, constchar, (, not, true, false }
    void term() {
        factor();
        while(lookahead == OP_MULT){
            multiplying_operator();
            factor();
        }
    }
// <multiplying_operator> ::= ‘*’ | ‘div’
    //FIRST(<multiplying_operator>) = { *, div }
    void multiplying_operator() {
        if( lookahead == OP_MULT )
            consome(lookahead);
    }
// <factor> ::= identifier | constint | constchar | ‘(’ <expression> ‘)’ | not <factor> | true | false
    //FIRST(<factor>) = { identifier, constint, constchar, (, not, true, false }
    void factor() {
        if( lookahead == IDENTIFICADOR )
            consome(IDENTIFICADOR);
        else if( lookahead == NUMERO )
            consome(NUMERO);
        else if( lookahead == '(' ){
            consome('(');
            expression();
            consome(')');
        }
        else if( lookahead == NOT ){
            consome(NOT);
            factor();
        }
        else if( lookahead == TRUE )
            consome(TRUE);
        else if( lookahead == FALSE )
            consome(FALSE);
    }

void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        if (lookahead == IDENTIFICADOR)
            printf("\nIDENTIFICADOR: %s", infoAtomo.atributo.ID);
        else if (lookahead == NUMERO)
            printf("\nNUMERO: %g", infoAtomo.atributo.numero);
        else if (lookahead == CONSTCHAR)
            printf("\nCONSTCHAR: '%c'", infoAtomo.atributo.ch);

        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    else{
        printf("\n#%2d:Erro sintatico: esperado [%s] encontrado [%s]\n",infoAtomo.linha,strMensagem[atomo],strMensagem[lookahead]);
        exit(1);
    }
}

/*
A sintaxe da linguagem PasKenzie é descrita por uma gramática na notação EBNF, vale ressaltar que a notação
EBNF utiliza os símbolos especiais |, {, }, [, ] (, ) na sua metalinguagem1
. Os <não-terminais> da gramática
são nomes entre parênteses angulares < e > e os símbolos terminais (átomos do analisador léxico) estão em
negrito ou entre apóstrofos (Ex: ‘;’), observe que os símbolos especiais da notação EBNF estão em vermelho
(ex: {, }) e os terminais em apóstrofo (ex: ‘{’ e ‘}’). A construção { alfa } denotará a repetição da cadeia alfa zero,
uma ou mais vezes (alfa*) e [ β ] é equivalente a β|λ, ou seja, indica que a cadeia β é opcional. Considere que o
símbolo inicial da gramática é <program>:
*/
/*
Especificações Léxicas
Caracteres Delimitadores: Os caracteres delimitadores: espaços em branco, quebra de linhas, tabulação e
retorno de carro (‘ ’,
‘\n’
,
‘\t’
,
‘\r’) deverão ser eliminados (ignorados) pelo analisador léxico, mas o
controle de linha (contagem de linha) deverá ser mantido.
*/
/*
Comentários: Na linguagem PasKenzie só existe comentários de várias linhas que começam com ‘(*’ e
termina com ‘*)’, lembrando que a contagem de linha deve ser mantida dentro do comentário.
Importante: Os comentários devem ser repassados para o analisador sintático para serem processados e
descartados.
*/
/*
identifier: Os identificadores começam com uma letra (maiúscula ou minúscula) ou underline ‘_’, em
seguida pode vir zero ou mais letras (maiúscula ou minúscula) ou underline ‘_’ ou dígitos, limitados a 15
caracteres. Caso seja encontrado um identificador com mais de 15 caracteres deve ser retornado ERRO pelo
analisador léxico. A seguir a definição regular para o átomo identifier.
letra → a|b|...|z|A|B|...|Z|_
digito → 0|1|...|9
identifier → letra(letra|digito)*
Importante: Na saída do compilador, para átomo identifier, deverá ser impresso o lexema que gerou
o átomo, ou seja, a sequência de caracteres reconhecida. Para isso o lexema deverá ser armazenado na
estrutura TInfoAatomo no campo atributo.id.
*/
/*
Palavras reservadas: As palavras reservadas da linguagem PasKenzie são:
div | or | and | not | if | then | else | while | do | begin | end | read |
write | var | program | true | false | char | integer | boolean.
Importante: As palavras reservadas são formadas por caracteres e minúsculo, além disso o compilador é
sensível ao caso, ou seja, Program e program são átomos diferentes, o primeiro é um identificador e o
segundo é uma palavra reservada.
*/
/*
constchar: para constante caractere são aceitos qualquer caractere da tabela ASCII entre apóstrofo, por
exemplo: ‘a’ou ‘0’.
Importante: O caractere que gerou o átomo constchar deve ser armazenado na estrutura TInfoAatomo
no campo atributo.ch pelo analisador léxico e depois impresso na tela na saída do compilador.
*/
/*
constint: Para constante inteira o compilador reconhece somente números inteiros na notação decimal,
com possiblidade de escrever o número na notação exponencial (potências de 10), conforme expressão
regular abaixo:
constint → digito+((d(+|ε)digito+)|ε)
Exemplos de constantes inteiras: 1, 000, 124, 12d2 (=1200), 12d+2 (=1200)
Importante: O analisador léxico deve retornar o valor número que gerou o átomo constint utilizando a
estrutura TInfoAatomo preenchendo o campo atributo.numero, que depois será impresso na saída do
compilador.
*/

/*
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// tipos do analisador lexico
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    OP_SOMA,
    OP_MULT,
    OP_SUB,
    OP_DIV,
    EOS,
    PROGRAM,
    VAR,
    CHAR,
    INTEGER,
    BOOLEAN,
    READ,
    WRITE,
    IF,
    THEN,
    ELSE,
    WHILE,
    DO,
    BEGIN,
    END,
    ASSIGN,
    OR,
    AND,
    NOT,
    TRUE,
    FALSE,
    MENOR,
    MENORIGUAL,
    MAIOR,
    MAIORIGUAL,
    IGUAL,
    DIFERENTE,
    CONSTCHAR
}TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    union{
        float numero;
        char ID[16];
        char ch;
    }atributo;
}TInfoAtomo;

typedef struct {
    const char *palavra;
    TAtomo atomo;
} PalavraReservada;

PalavraReservada reservadas[] = {
    {"program", PROGRAM},
    {"var", VAR},
    {"char", CHAR},
    {"integer", INTEGER},
    {"boolean", BOOLEAN},
    {"read", READ},
    {"write", WRITE},
    {"if", IF},
    {"then", THEN},
    {"else", ELSE},
    {"while", WHILE},
    {"do", DO},
    {"begin", BEGIN},
    {"end", END},
    {"or", OR},
    {"and", AND},
    {"not", NOT},
    {"true", TRUE},
    {"false", FALSE},
    {"div", OP_DIV},
    {NULL, ERRO}
};

char lexema[20];
int nLinha;
char *buffer = "program xxx;";
TAtomo lookahead;
TInfoAtomo infoAtomo;
char *strMensagem[] = {
    "erro lexico", "IDENTIFICADOR", "NUMERO", "+", "*", "-", "/", "EOS",
    "PROGRAM", "VAR", "CHAR", "INTEGER", "BOOLEAN", "READ", "WRITE",
    "IF", "THEN", "ELSE", "WHILE", "DO", "BEGIN", "END", ":=", "OR", "AND", "NOT",
    "TRUE", "FALSE", "<", "<=", ">", ">=", "=", "<>", "CONSTCHAR"
};

TInfoAtomo obter_atomo();
void reconhece_numero(TInfoAtomo *infoAtomo);
void reconhe_id(TInfoAtomo *infoAtomo);
void consome(TAtomo atomo);

void program();
void block();
void variable_declaration_part();
void variable_declaration();
void type();
void statement_part();
void statement();
void assignment_statement();
void read_statement();
void write_statement();
void if_statement();
void while_statement();
void expression();
void relational_expression();
void simple_expression();
void adding_operator();
void term();
void multiplying_operator();
void factor();

int main(){
    nLinha = 1;
    printf("Analisando: %s",buffer);
    infoAtomo = obter_atomo();
    lookahead = infoAtomo.atomo;
    program();
    consome(EOS);
    printf("\nfim de programa. %d linhas analisadas\n", infoAtomo.linha);
    return 0;
}

TInfoAtomo obter_atomo() {
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;

    // Ignora espaços, tabs e novas linhas (mantendo contagem de linhas)
    while (*buffer == ' ' || *buffer == '\t' || *buffer == '\n' || *buffer == '\r') {
        if (*buffer == '\n') nLinha++;
        buffer++;
    }

    // Ignora comentários
    while (*buffer == '(' && *(buffer+1) == '*') {
        buffer += 2;
        while (!(*buffer == '*' && *(buffer+1) == ')') && *buffer != '\0') {
            if (*buffer == '\n') nLinha++;
            buffer++;
        }
        if (*buffer == '*' && *(buffer+1) == ')') buffer += 2;
        while (*buffer == ' ' || *buffer == '\t' || *buffer == '\n' || *buffer == '\r') {
            if (*buffer == '\n') nLinha++;
            buffer++;
        }
    }

    infoAtomo.linha = nLinha;

    if(isdigit(*buffer)){
        reconhece_numero(&infoAtomo);
    }
    else if(islower(*buffer) || isupper(*buffer) || *buffer == '_')
        reconhe_id(&infoAtomo);

    else if (*buffer == '\'') {
        buffer++;
        if (*buffer != '\0' && *(buffer+1) == '\'') {
            infoAtomo.atributo.ch = *buffer;
            buffer += 2;
            infoAtomo.atomo = CONSTCHAR;
        } else {
            infoAtomo.atomo = ERRO;
        }
    }
    else if (*buffer == ';') {
        buffer++;
        infoAtomo.atomo = ';';
    }
    else if (*buffer == ',') {
        buffer++;
        infoAtomo.atomo = ',';
    }
    else if (*buffer == ':') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = ASSIGN;
        } else {
            buffer++;
            infoAtomo.atomo = ':';
        }
    }
    else if (*buffer == '(') {
        buffer++;
        infoAtomo.atomo = '(';
    }
    else if (*buffer == ')') {
        buffer++;
        infoAtomo.atomo = ')';
    }
    else if (*buffer == '.') {
        buffer++;
        infoAtomo.atomo = '.';
    }
    else if (*buffer == '+') {
        buffer++;
        infoAtomo.atomo = OP_SOMA;
    }
    else if (*buffer == '-') {
        buffer++;
        infoAtomo.atomo = OP_SUB;
    }
    else if (*buffer == '*') {
        buffer++;
        infoAtomo.atomo = OP_MULT;
    }
    else if (*buffer == '/') {
        buffer++;
        infoAtomo.atomo = OP_DIV;
    }
    else if (*buffer == '<') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = MENORIGUAL;
        }
        else if (*(buffer+1) == '>') {
            buffer += 2;
            infoAtomo.atomo = DIFERENTE;
        }
        else {
            buffer++;
            infoAtomo.atomo = MENOR;
        }
    }
    else if (*buffer == '>') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = MAIORIGUAL;
        } else {
            buffer++;
            infoAtomo.atomo = MAIOR;
        }
    }
    else if (*buffer == '=') {
        buffer++;
        infoAtomo.atomo = IGUAL;
    }
    else if (*buffer == '\0')
        infoAtomo.atomo = EOS;

    return infoAtomo;
}

void reconhece_numero(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int expo = 0;
    int sinal = 1;

    // Parte inteira
    while (isdigit(*buffer)) buffer++;

    // Notação exponencial (d ou D)
    if (*buffer == 'd' || *buffer == 'D') {
        buffer++;
        if (*buffer == '+') {
            buffer++;
        } else if (*buffer == '-') {
            sinal = -1;
            buffer++;
        }
        if (!isdigit(*buffer)) {
            infoAtomo->atomo = ERRO;
            return;
        }
        while (isdigit(*buffer)) {
            expo = expo * 10 + (*buffer - '0');
            buffer++;
        }
    }

    int tam = buffer - ini_lexema;
    strncpy(lexema, ini_lexema, tam);
    lexema[tam] = '\0';

    float valor = atof(lexema);
    if (expo != 0) {
        for (int i = 0; i < expo; i++)
            valor *= 10;
        if (sinal == -1)
            valor = atof(lexema) / pow(10, expo);
    }
    infoAtomo->atributo.numero = valor;
    infoAtomo->atomo = NUMERO;
}

void reconhe_id(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int tam = 0;

    while(islower(*buffer) || isupper(*buffer) || *buffer == '_' || isdigit(*buffer)){
        buffer++;
        tam++;
        if (tam > 15) {
            infoAtomo->atomo = ERRO;
            return;
        }
    }
    strncpy(infoAtomo->atributo.ID, ini_lexema, buffer-ini_lexema);
    infoAtomo->atributo.ID[buffer-ini_lexema] = '\0';

    // Verifica se é palavra reservada
    for (int i = 0; reservadas[i].palavra != NULL; i++) {
        if (strcmp(infoAtomo->atributo.ID, reservadas[i].palavra) == 0) {
            infoAtomo->atomo = reservadas[i].atomo;
            return;
        }
    }
    infoAtomo->atomo = IDENTIFICADOR;
}

// ASDR
void program() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(';');
    block();
    consome('.');
}

void block() {
    variable_declaration_part();
    statement_part();
}

void variable_declaration_part() {
    if( lookahead == VAR ){
        consome(VAR);
        variable_declaration();
        consome(';');
        while( lookahead == IDENTIFICADOR ){    
            variable_declaration();
            consome(';');
        } 
    }  
}
void variable_declaration() {
    consome(IDENTIFICADOR);
    while(lookahead == ','){
        consome(',');
        consome(IDENTIFICADOR);
    }
    consome(':');
    type();
}
void type() {
    if( lookahead == CHAR ) {
        consome(CHAR);
    }
    else if( lookahead == INTEGER ) {
        consome(INTEGER);
    }   
    else if( lookahead == BOOLEAN ) {
        consome(BOOLEAN);   
    }
}
void statement_part() {
    consome(BEGIN);
    statement();
    while( lookahead == ';' ){
        consome(';');
        statement();
    }
    consome(END);
}
void statement() {
    if( lookahead == IDENTIFICADOR )
        assignment_statement();
    else if( lookahead == READ )
        read_statement();
    else if( lookahead == WRITE )
        write_statement();
    else if( lookahead == IF )
        if_statement();
    else if( lookahead == WHILE )
        while_statement();
    else if( lookahead == BEGIN )
        statement_part();
}
void assignment_statement() {
    consome(IDENTIFICADOR);
    consome(ASSIGN);
    expression();
}
void read_statement() {
    consome(READ);
    consome('(');
    consome(IDENTIFICADOR);
    while( lookahead == ',' ){
        consome(',');
        consome(IDENTIFICADOR);
    }
    consome(')');
}
void write_statement() {
    consome(WRITE);
    consome('(');
    consome(IDENTIFICADOR);
    while( lookahead == ',' ){
        consome(',');
        consome(IDENTIFICADOR);
    }
    consome(')');
}
void if_statement() {
    consome(IF);
    expression();
    consome(THEN);
    statement();
    if( lookahead == ELSE ){
        consome(ELSE);
        statement();
    }
}
void while_statement() {
    consome(WHILE);
    expression();
    consome(DO);
    statement();
}
void expression() {
    simple_expression();
    if( lookahead == MENOR || lookahead == MENORIGUAL || lookahead == MAIOR || lookahead == MAIORIGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == OR || lookahead == AND )
        relational_expression();
}
void relational_expression() {
    if( lookahead == MENOR )
        consome(lookahead);
    else if( lookahead == MENORIGUAL )
        consome(lookahead);
    else if( lookahead == MAIOR )
        consome(lookahead);
    else if( lookahead == MAIORIGUAL )
        consome(lookahead);
    else if( lookahead == IGUAL )
        consome(lookahead);
    else if( lookahead == DIFERENTE )
        consome(lookahead);
    else if( lookahead == OR )
        consome(lookahead);
    else if( lookahead == AND )
        consome(lookahead);
    simple_expression();
}
void simple_expression() {
    term();
    while(lookahead == OP_SOMA){
        adding_operator();
        term();
    }
}
void adding_operator() {
    if( lookahead == OP_SOMA )
        consome(OP_SOMA);
    else if( lookahead == OP_SUB )
        consome(OP_SUB);
}
void term() {
    factor();
    while(lookahead == OP_MULT || lookahead == OP_DIV){
        multiplying_operator();
        factor();
    }
}
void multiplying_operator() {
    if( lookahead == OP_MULT )
        consome(OP_MULT);
    else if( lookahead == OP_DIV )
        consome(OP_DIV);
}
void factor() {
    if( lookahead == IDENTIFICADOR )
        consome(IDENTIFICADOR);
    else if( lookahead == NUMERO )
        consome(NUMERO);
    else if( lookahead == CONSTCHAR )
        consome(CONSTCHAR);
    else if( lookahead == '(' ){
        consome('(');
        expression();
        consome(')');
    }
    else if( lookahead == NOT ){
        consome(NOT);
        factor();
    }
    else if( lookahead == TRUE )
        consome(TRUE);
    else if( lookahead == FALSE )
        consome(FALSE);
}

void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        if (lookahead == IDENTIFICADOR)
            printf("\nIDENTIFICADOR: %s", infoAtomo.atributo.ID);
        else if (lookahead == NUMERO)
            printf("\nNUMERO: %g", infoAtomo.atributo.numero);
        else if (lookahead == CONSTCHAR)
            printf("\nCONSTCHAR: '%c'", infoAtomo.atributo.ch);

        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    else{
        printf("\n#%2d:Erro sintatico: esperado [%s] encontrado [%s]\n",infoAtomo.linha,strMensagem[atomo],strMensagem[lookahead]);
        exit(1);
    }
}
*/