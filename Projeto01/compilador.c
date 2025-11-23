//Francisco Losada Totaro - 10364673
//compilar: gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MAX_BUFFER 10000
#define PRIME_NUMBER 211

char buffer_global[MAX_BUFFER];
char *buffer;

// Tabela de Símbolos
typedef struct _TNo {
    char ID[16];
    int endereco;
    struct _TNo *prox;
} TNo;

typedef struct {
    TNo *entradas[PRIME_NUMBER];
} TTabelaSimbolos;

TTabelaSimbolos tabelaSimbolos;
int contadorEndereco = 0;
int contadorRotulo = 1;

// analisador lexico
typedef enum {
    ERRO,
    IDENTIFICADOR,
    CONSTINT,
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
    CONSTCHAR,
    COMENTARIO,
    PONTOVIRGULA,
    VIRGULA,
    DOISPONTOS,
    ABREPARENTESES,
    FECHAPARENTESES,
    PONTO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    union{
        float numero;
        char ID[16];
        char ch;
    }atributo;
} TInfoAtomo;

typedef struct {
    const char *palavra;
    TAtomo atomo;
} PalavraReservada;

//Lista de palavras reservadas
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
TAtomo lookahead;
TInfoAtomo infoAtomo;
char *strMensagem[] = {
    "erro lexico", "IDENTIFICADOR", "NUMERO", "+", "*", "-", "/", "EOS",
    "PROGRAM", "VAR", "CHAR", "INTEGER", "BOOLEAN", "READ", "WRITE",
    "IF", "THEN", "ELSE", "WHILE", "DO", "BEGIN", "END", ":=", "OR",
    "AND", "NOT", "TRUE", "FALSE", "<", "<=", ">", ">=", "=", "<>",
    "CONSTCHAR", "COMENTARIO", "PONTOVIRGULA", "VIRGULA", "DOISPONTOS",
    "ABREPARENTESES", "FECHAPARENTESES", "PONTO, COMENTARIO"
};

TInfoAtomo obter_atomo();
void reconhece_numero(TInfoAtomo *infoAtomo);
void reconhece_id(TInfoAtomo *infoAtomo);
void consome(TAtomo atomo);
int hashMack(char * s);

// Funções da Tabela de Simblos
void inicializa_tabela_simbolos();
void insere_tabela_simbolos(char *id);
int busca_tabela_simbolos(char *id);
void imprime_tabela_simbolos();
int proximo_rotulo();

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
    inicializa_tabela_simbolos();
    
    //ler arquivo codigo.txt
    FILE *f = fopen("codigo.txt", "r");
    if (!f) {
        printf("Erro ao abrir codigo.txt\n");
        return 1;
    }
    size_t lidos = fread(buffer_global, 1, MAX_BUFFER-1, f);
    buffer_global[lidos] = '\0';
    fclose(f);
    buffer = buffer_global;

    infoAtomo = obter_atomo();
    lookahead = infoAtomo.atomo;

    //Simbolo inicial
    program();
    consome(EOS);
    
    imprime_tabela_simbolos();
    return 0;
}

// Analisador Lexico
TInfoAtomo obter_atomo() {
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;

    //ignora
    while (*buffer == ' ' || *buffer == '\t' || *buffer == '\n' || *buffer == '\r') {
        if (*buffer == '\n') {
            nLinha++;
        }
        buffer++;
    }
    
    //reconhece_comentario
    if (*buffer == '(' && *(buffer+1) == '*') {
        int linha_inicio = nLinha;
        buffer += 2;
        while (!(*buffer == '*' && *(buffer+1) == ')') && *buffer != '\0') {
            if (*buffer == '\n') {
                nLinha++;
            }
            buffer++;
        }
        //comentário nao fechado
        if (*buffer == '\0') {
            infoAtomo.linha = linha_inicio;
            infoAtomo.atomo = ERRO;
            return infoAtomo;
        }
        buffer += 2; 
        infoAtomo.linha = linha_inicio;
        infoAtomo.atomo = COMENTARIO;
        return infoAtomo;
    }
    infoAtomo.linha = nLinha;

    //numero
    if(isdigit(*buffer)){
        reconhece_numero(&infoAtomo);
    }
    //char
    else if((isalpha(*buffer) || *buffer == '_')) {
        reconhece_id(&infoAtomo);
    }
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
        infoAtomo.atomo = PONTOVIRGULA;
    }
    else if (*buffer == ',') {
        buffer++;
        infoAtomo.atomo = VIRGULA;
    }
    else if (*buffer == ':') {
        if (*(buffer+1) == '=') {
            buffer += 2;
            infoAtomo.atomo = ASSIGN;
        } else {
            buffer++;
            infoAtomo.atomo = DOISPONTOS;
        }
    }
    else if (*buffer == '(') {
        buffer++;
        infoAtomo.atomo = ABREPARENTESES;
    }
    else if (*buffer == ')') {
        buffer++;
        infoAtomo.atomo = FECHAPARENTESES;
    }
    else if (*buffer == '.') {
        buffer++;
        infoAtomo.atomo = PONTO;
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

//reconhece numero
void reconhece_numero(TInfoAtomo *infoAtomo) {
    char *ini_lexema = buffer;
    int expo = 0;
    int sinal = 1;
    int tem_expo = 0;
    int tam;

q0:
    if (isdigit(*buffer)) {
        buffer++;
        goto q0;
    }
    if (*buffer == '.') {
        buffer++;
        goto q1;
    }
    if (*buffer == 'd' || *buffer == 'D') {
        tem_expo = 1;
        buffer++;
        goto q2;
    }
    goto q4;

q1:
    if (isdigit(*buffer)) {
        buffer++;
        goto q1;
    }
    if (*buffer == 'd' || *buffer == 'D') {
        tem_expo = 1;
        buffer++;
        goto q2;
    }
    goto q4;

q2:
    if (*buffer == '+') {
        buffer++;
        goto q3;
    }
    if (*buffer == '-') {
        sinal = -1;
        buffer++;
        goto q3;
    }
    if (isdigit(*buffer)) {
        goto q3;
    }
    infoAtomo->atomo = ERRO;
    return;

q3:
    if (isdigit(*buffer)) {
        expo = expo * 10 + (*buffer - '0');
        buffer++;
        goto q3;
    }
    goto q4;

q4:
    //limita tamanho
    tam = buffer - ini_lexema;
    if (tam > 15) { 
        infoAtomo->atomo = ERRO;
        return;
    }
    strncpy(lexema, ini_lexema, tam);
    lexema[tam] = '\0';

    // expoente
    float valor = atof(lexema);
    if (tem_expo) {
        if (sinal == 1)
            valor *= pow(10, expo);
        else
            valor /= pow(10, expo);
    }
    infoAtomo->atributo.numero = valor;
    infoAtomo->atomo = CONSTINT;
    return;
}

// void reconhece id
void reconhece_id(TInfoAtomo *infoAtomo) {
    char *ini_lexema = buffer;
    int tam = 0;

q0: 
    if (isalnum(*buffer) || *buffer == '_') {
        buffer++;
        tam++;
        if (tam > 15) {
            infoAtomo->atomo = ERRO;
            return;
        }
        goto q0;
    }
    goto q1;

q1:
    strncpy(infoAtomo->atributo.ID, ini_lexema, buffer - ini_lexema);
    infoAtomo->atributo.ID[buffer - ini_lexema] = '\0';

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

//Analisador Sintático
// ASDR
// <program> ::= program identifier ‘;‘ <block> ‘.'
    //FIRST(<program>) = { program }
void program() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTOVIRGULA);
    printf("	INPP\n");
    block();
    printf("	PARA\n");
    consome(PONTO);
}

// <block> ::= <variable_declaration_part> <statement_part>
    //FIRST(<block>) = { var, begin }
void block() {
    int numVars = contadorEndereco;
    variable_declaration_part();
    numVars = contadorEndereco - numVars;
    if (numVars > 0) {
        printf("	AMEM %d\n", numVars);
    }
    statement_part();
}

// <variable_declaration_part> ::= [ var <variable_declaration> ‘;’ { <variable_declaration> ‘;’ } ]
    // FIRST(<variable_declaration_part>) = { var, λ }
void variable_declaration_part() {
    if(lookahead == VAR){
        consome(VAR);
        variable_declaration();
        consome(PONTOVIRGULA);
        while(lookahead == IDENTIFICADOR){    
            variable_declaration();
            consome(PONTOVIRGULA);
        } 
    }  
}
// <variable_declaration> ::= identifier { ‘,’ identifier } ‘:’ <type>
    //FIRST(<variable_declaration>) = { identifier }
void variable_declaration() {
    char id[16];
    strcpy(id, infoAtomo.atributo.ID);
    consome(IDENTIFICADOR);
    insere_tabela_simbolos(id);
    
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        strcpy(id, infoAtomo.atributo.ID);
        consome(IDENTIFICADOR);
        insere_tabela_simbolos(id);
    }
    consome(DOISPONTOS);
    type();
}

// <type> ::= char | integer | boolean
    //FIRST(<type>) = { char, integer, boolean }
void type() {
    if(lookahead == CHAR) {
        consome(CHAR);
    }
    else if(lookahead == INTEGER) {
        consome(INTEGER);
    }   
    else if(lookahead == BOOLEAN) {
        consome(BOOLEAN);   
    }
}

// <statment_part> ::= begin <statement> { ‘;’ <statement> } end
    //FIRST(<statement_part>) = { begin }
void statement_part() {
    consome(BEGIN);
    statement();
    while(lookahead == PONTOVIRGULA){
        consome(PONTOVIRGULA);
        statement();
    }
    consome(END);
}

// <statement> ::= <assignment_statement> | <read_statement> | <write_statement> | <if_statement> | <while_statement> | <statement_part>
    //FIRST(<statement>) = { identifier, read, write, if, while, begin }
void statement() {
    if(lookahead == IDENTIFICADOR) {
        assignment_statement();
    }
    else if(lookahead == READ) {
        read_statement();
    }
    else if(lookahead == WRITE) {
        write_statement();
    }
    else if(lookahead == IF) {
        if_statement();
    }
    else if(lookahead == WHILE) {
        while_statement();
    }
    else if(lookahead == BEGIN) {
        statement_part();
    }
    else {
        consome(IDENTIFICADOR);
    }
}
// <assignment_statement> ::= identifier ‘:=’ <expression>
    //FIRST(<assignment_statement>) = { identifier }
void assignment_statement() {
    int endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
    consome(IDENTIFICADOR);
    consome(ASSIGN);
    expression();
    printf("\tARMZ %d\n", endereco);
}

// <read_statement> ::= read ‘(’ identifier { ‘,’ identifier } ‘)’
    //FIRST(<read_statement>) = { read }
void read_statement() {
    int endereco;
    consome(READ);
    consome(ABREPARENTESES);
    endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
    consome(IDENTIFICADOR);
    printf("\tLEIT\n");
    printf("\tARMZ %d\n", endereco);
    while(lookahead == VIRGULA) {
        consome(VIRGULA);
        endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
        consome(IDENTIFICADOR);
        printf("\tLEIT\n");
        printf("\tARMZ %d\n", endereco);
    }
    consome(FECHAPARENTESES);
}

// <write_statement> ::= write ‘(’ identifier { ‘,’ identifier } ‘)’
    //FIRST(<write_statement>) = { write }
void write_statement() {
    int endereco;
    consome(WRITE);
    consome(ABREPARENTESES);
    endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
    consome(IDENTIFICADOR);
    printf("\tCRVL %d\n", endereco);
    printf("\tIMPR\n");
    while(lookahead == VIRGULA) {
        consome(VIRGULA);
        endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
        consome(IDENTIFICADOR);
        printf("\tCRVL %d\n", endereco);
        printf("\tIMPR\n");
    }
    consome(FECHAPARENTESES);
}

// <if_statement> ::= if <expression> then <statement> [ else <statement> ]
    //FIRST(<if_statement>) = { if }
void if_statement() {
    int L1 = proximo_rotulo();
    int L2 = proximo_rotulo();
    consome(IF);
    expression();
    consome(THEN);
    printf("\tDSVF L%d\n", L1);
    statement();
    printf("\tDSVS L%d\n", L2);
    printf("L%d:\tNADA\n", L1);
    if(lookahead == ELSE ) {
        consome(ELSE);
        statement();
    }
    printf("L%d:\tNADA\n", L2);
}

// <while_statement> ::= while <expression> do <statement>
    //FIRST(<while_statement>) = {while}
void while_statement() {
    int L1 = proximo_rotulo();
    int L2 = proximo_rotulo();
    printf("L%d:\tNADA\n", L1);
    consome(WHILE);
    expression();
    consome(DO);
    printf("\tDSVF L%d\n", L2);
    statement();
    printf("\tDSVS L%d\n", L1);
    printf("L%d:\tNADA\n", L2);
}

// <expression> ::= <simple_expression> [ <relational_operator> <simple expression> ]
    //FIRST(<expression>) = {identifier, constint, constchar, (, not, true, false}
void expression() {
    simple_expression();
    if(lookahead == MENOR || lookahead == MENORIGUAL || lookahead == MAIOR || lookahead == MAIORIGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == OR || lookahead == AND) {
        relational_expression();
    }
}
// <relational_operator> ::= ‘<’ | ‘<=’ | ‘>’ | ‘>=’ | ‘=’ | ‘<>’ | or | and
    //FIRST(<relational_operator>) = {<, <=, >, >=, =, <>, or, and}
void relational_expression() {
    if(lookahead == MENOR) {
        consome(MENOR);
        simple_expression();
        printf("\tCMME\n");
    }
    else if(lookahead == MENORIGUAL) {
        consome(MENORIGUAL);
        simple_expression();
        printf("\tCMEG\n");
    }
    else if(lookahead == MAIOR) {
        consome(MAIOR);
        simple_expression();
        printf("\tCMMA\n");
    }
    else if(lookahead == MAIORIGUAL) {
        consome(MAIORIGUAL);
        simple_expression();
        printf("\tCMAG\n");
    }
    else if(lookahead == IGUAL) {
        consome(IGUAL);
        simple_expression();
        printf("\tCMIG\n");
    }
    else if(lookahead == DIFERENTE) {
        consome(DIFERENTE);
        simple_expression();
        printf("\tCMDG\n");
    }
    else if(lookahead == OR) {
        consome(OR);
        simple_expression();
        printf("\tDISJ\n");
    }
    else if(lookahead == AND) {
        consome(AND);
        simple_expression();
        printf("\tCONJ\n");
    }
}

// <simple_expression> ::= <term> { <adding_operator> <term> }
    //FIRST(<simple_expression>) = {identifier, constint, constchar, (, not, true, false}
void simple_expression() {
    term();
    while(lookahead == OP_SOMA || lookahead == OP_SUB) {
        TAtomo op = lookahead;
        adding_operator();
        term();
        if(op == OP_SOMA) {
            printf("\tSOMA\n");
        }
        else if(op == OP_SUB) {
            printf("\tSUBT\n");
        }
    }
}

// <adding_operator> ::= ‘+’ | ‘-’
    //FIRST(<adding_operator>) = {+, -}
void adding_operator() {
    //TAtomo op = lookahead;
    if(lookahead == OP_SOMA) {
        consome(OP_SOMA);
    }
    else if(lookahead == OP_SUB) {
        consome(OP_SUB);
    }
}

// <term> ::= <factor> { <multiplying_operator> <factor> }
    //FIRST(<term>) = {identifier, constint, constchar, (, not, true, false}
void term() {
    factor();
    while(lookahead == OP_MULT || lookahead == OP_DIV) {
        TAtomo op = lookahead;
        multiplying_operator();
        factor();
        if(op == OP_MULT) {
            printf("\tMULT\n");
        }
        else if(op == OP_DIV) {
            printf("\tDIVI\n");
        } 
    }
}

// <multiplying_operator> ::= ‘*’ | ‘div’
    //FIRST(<multiplying_operator>) = {*, div}
void multiplying_operator() {
    if(lookahead == OP_MULT) {
        consome(OP_MULT);
    }
    else if(lookahead == OP_DIV) {
        consome(OP_DIV);
    }
}

// <factor> ::= identifier | constint | constchar | ‘(’ <expression> ‘)’ | not <factor> | true | false
    //FIRST(<factor>) = {identifier, constint, constchar, (, not, true, false}
void factor() {
    if(lookahead == IDENTIFICADOR) {
        int endereco = busca_tabela_simbolos(infoAtomo.atributo.ID);
        printf("\tCRVL %d\n", endereco);
        consome(IDENTIFICADOR);
    }
    else if(lookahead == CONSTINT) {
        int valor = (int)infoAtomo.atributo.numero;
        printf("\tCRCT %d\n", valor);
        consome(CONSTINT);
    }
    else {
        consome(ABREPARENTESES);
        expression();
        consome(FECHAPARENTESES);
    }
    // else if(lookahead == CONSTCHAR) {
    //     consome(CONSTCHAR);
    // }
    // else if(lookahead == ABREPARENTESES) {
    //     consome(ABREPARENTESES);
    //     expression();
    //     consome(FECHAPARENTESES);
    // }
    // else if(lookahead == NOT) {
    //     consome(NOT);
    //     factor();
    // }
    // else if(lookahead == TRUE) {
    //     consome(TRUE);
    // }
    // else if(lookahead == FALSE) {
    //     consome(FALSE);
    // }
}

//consome
void consome(TAtomo atomo) {
    while (lookahead == COMENTARIO) {
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    
    if (lookahead == atomo) {
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        while (lookahead == COMENTARIO) {
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }
    }
    else {
        printf("\n#%2d:erro sintatico, esperado [%s] encontrado [%s]\n", infoAtomo.linha, strMensagem[atomo], strMensagem[lookahead]);
        exit(1);
    }
}

/*
função hashMack: implementa a dispersão padrão para um hash, 
usa um numero primo como tamanho do vetor para minimizar colisões
*/
int hashMack( char * s )
{
    char *p;
    unsigned int h = 0, g;
    for ( p = s; *p != '\0'; p = p + 1 ){
        h = ( h << 4 ) + (*p);
        g = h&0xf0000000U;
        if ( g ){
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
    }
    return h % PRIME_NUMBER;
}

// Implementação das funções da Tabela de Símbolos
void inicializa_tabela_simbolos() {
    for (int i = 0; i < PRIME_NUMBER; i++) {
        tabelaSimbolos.entradas[i] = NULL;
    }
}

//Insere variavel na tabela de simbolos
void insere_tabela_simbolos(char *id) {
    int hash = hashMack(id);
    TNo *atual = tabelaSimbolos.entradas[hash];
    
    // Verifica se já existe
    while (atual != NULL) {
        if (strcmp(atual->ID, id) == 0) {
            printf("\n#%2d:erro semantico, variavel '%s' ja declarada\n", infoAtomo.linha, id);
            exit(1);
        }
        atual = atual->prox;
    }
    
    // insere novo simbolo
    TNo *novo = (TNo *)malloc(sizeof(TNo));
    strcpy(novo->ID, id);
    novo->endereco = contadorEndereco++;
    novo->prox = tabelaSimbolos.entradas[hash];
    tabelaSimbolos.entradas[hash] = novo;
}

//busca variavel na tabela de simbolos
int busca_tabela_simbolos(char *id) {
    int hash = hashMack(id);
    TNo *atual = tabelaSimbolos.entradas[hash];
    while (atual != NULL) {
        if (strcmp(atual->ID, id) == 0) {
            return atual->endereco;
        }
        atual = atual->prox;
    }
    printf("\n#%2d:erro semantico, variavel '%s' nao declarada\n", infoAtomo.linha, id);
    exit(1);
}

//mostra a tabela de simbolos para o usuario
void imprime_tabela_simbolos() {
    printf("\nTABELA DE SIMBOLOS\n");
    for (int i = 0; i < PRIME_NUMBER; i++) {
        TNo *atual = tabelaSimbolos.entradas[i];
        while (atual != NULL) {
            printf("Entrada Tabela Simbolos: [%d] => %s | Endereco: %d\n", i, atual->ID, atual->endereco);
            atual = atual->prox;
        }
    }
}

// busca o proximo rotulo
int proximo_rotulo() {
    return contadorRotulo++;
}