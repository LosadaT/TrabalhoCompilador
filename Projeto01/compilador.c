//Francisco Losada Totaro - 10364673
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MAX_BUFFER 10000

char buffer_global[MAX_BUFFER];
char *buffer;

// analisador lexico
typedef enum{
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

    program();
    consome(EOS);
    printf("\nfim de programa. %d linhas analisadas, programa sintaticamente correto\n", infoAtomo.linha);

    return 0;
}

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

    //comentario
    if (*buffer == '(' && *(buffer+1) == '*') {
        int linha_inicio = nLinha;
        buffer += 2;
        while (!(*buffer == '*' && *(buffer+1) == ')') && *buffer != '\0') {
            if (*buffer == '\n') nLinha++;
            buffer++;
        }
        //comentario nao fechado
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
        reconhe_id(&infoAtomo);
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

void reconhece_numero(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int expo = 0, sinal = 1;
    int tem_expo = 0;

    // Parte inteira
    while (isdigit(*buffer)) buffer++;

    // Notação exponencial
    if (*buffer == 'd' || *buffer == 'D') {
        tem_expo = 1;
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
    if (tam > 15) {
        infoAtomo->atomo = ERRO;
        return;
    }
    strncpy(lexema, ini_lexema, tam);
    lexema[tam] = '\0';

    float valor = atof(lexema);
    if (tem_expo) {
        if (sinal == 1)
            valor *= pow(10, expo);
        else
            valor /= pow(10, expo);
    }
    infoAtomo->atributo.numero = valor;
    infoAtomo->atomo = CONSTINT;
}

void reconhe_id(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;
    int tam = 0;
    buffer++;
    tam++;

    while(isalnum(*buffer) || *buffer == '_'){
        buffer++;
        tam++;
        if (tam > 15) {
            infoAtomo->atomo = ERRO;
            return;
        }
    }
    strncpy(infoAtomo->atributo.ID, ini_lexema, buffer-ini_lexema);
    infoAtomo->atributo.ID[buffer-ini_lexema] = '\0';

    // Verifica se é palavra reservada
    for (int i = 0; reservadas[i].palavra != NULL; i++) {
        if (strcmp(infoAtomo->atributo.ID, reservadas[i].palavra) == 0) {
            infoAtomo->atomo = reservadas[i].atomo;
            return;
        }
    }
    infoAtomo->atomo = IDENTIFICADOR;
}

// ASDR
// <program> ::= program identifier ‘;‘ <block> ‘.'
    //FIRST(<program>) = { program }
void program() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTOVIRGULA);
    block();
    consome(PONTO);
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
        consome(PONTOVIRGULA);
        while( lookahead == IDENTIFICADOR ){    
            variable_declaration();
            consome(PONTOVIRGULA);
        } 
    }  
}
// <variable_declaration> ::= identifier { ‘,’ identifier } ‘:’ <type>
    //FIRST(<variable_declaration>) = { identifier }
void variable_declaration() {
    consome(IDENTIFICADOR);
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(DOISPONTOS);
    type();
}

// <type> ::= char | integer | boolean
    //FIRST(<type>) = { char, integer, boolean }
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
    consome(ABREPARENTESES);
    consome(IDENTIFICADOR);
    while(lookahead == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(FECHAPARENTESES);
}

// <write_statement> ::= write ‘(’ identifier { ‘,’ identifier } ‘)’
    //FIRST(<write_statement>) = { write }
void write_statement() {
    consome(WRITE);
    consome(ABREPARENTESES);
    consome(IDENTIFICADOR);
    while(lookahead == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(FECHAPARENTESES);
}

// <if_statement> ::= if <expression> then <statement> [ else <statement> ]
    //FIRST(<if_statement>) = { if }
void if_statement() {
    consome(IF);
    expression();
    consome(THEN);
    statement();
    if(lookahead == ELSE ) {
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
    if(lookahead == MENOR || lookahead == MENORIGUAL || lookahead == MAIOR || lookahead == MAIORIGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == OR || lookahead == AND) {
        relational_expression();
    }
}
// <relational_operator> ::= ‘<’ | ‘<=’ | ‘>’ | ‘>=’ | ‘=’ | ‘<>’ | or | and
    //FIRST(<relational_operator>) = { <, <=, >, >=, =, <>, or, and }
void relational_expression() {
    if(lookahead == MENOR) {
        consome(lookahead);
    }
    else if(lookahead == MENORIGUAL) {
        consome(lookahead);
    }
    else if(lookahead == MAIOR) {
        consome(lookahead);
    }
    else if(lookahead == MAIORIGUAL) {
        consome(lookahead);
    }
    else if(lookahead == IGUAL) {
        consome(lookahead);
    }
    else if(lookahead == DIFERENTE) {
        consome(lookahead);
    }
    else if(lookahead == OR) {
        consome(lookahead);
    }
    else if(lookahead == AND) {
        consome(lookahead);
    }
    simple_expression();
}

// <simple_expression> ::= <term> { <adding_operator> <term> }
    //FIRST(<simple_expression>) = { identifier, constint, constchar, (, not, true, false }
void simple_expression() {
    term();
    while(lookahead == OP_SOMA || lookahead == OP_SUB) {
        adding_operator();
        term();
    }
}

// <adding_operator> ::= ‘+’ | ‘-’
    //FIRST(<adding_operator>) = { +, - }
void adding_operator() {
    if(lookahead == OP_SOMA) {
        consome(OP_SOMA);
    }
    else if(lookahead == OP_SUB) {
        consome(OP_SUB);
    }
}

// <term> ::= <factor> { <multiplying_operator> <factor> }
    //FIRST(<term>) = { identifier, constint, constchar, (, not, true, false }
void term() {
    factor();
    while(lookahead == OP_MULT || lookahead == OP_DIV) {
        multiplying_operator();
        factor();
    }
}

// <multiplying_operator> ::= ‘*’ | ‘div’
    //FIRST(<multiplying_operator>) = { *, div }
void multiplying_operator() {
    if(lookahead == OP_MULT)
        consome(OP_MULT);
    else if(lookahead == OP_DIV)
        consome(OP_DIV);
}

// <factor> ::= identifier | constint | constchar | ‘(’ <expression> ‘)’ | not <factor> | true | false
    //FIRST(<factor>) = { identifier, constint, constchar, (, not, true, false }
void factor() {
    if(lookahead == IDENTIFICADOR) {
        consome(IDENTIFICADOR);
    }
    else if(lookahead == CONSTINT) {
        consome(CONSTINT);
    }
    else if(lookahead == CONSTCHAR) {
        consome(CONSTCHAR);
    }
    else if(lookahead == ABREPARENTESES) {
        consome(ABREPARENTESES);
        expression();
        consome(FECHAPARENTESES);
    }
    else if(lookahead == NOT) {
        consome(NOT);
        factor();
    }
    else if(lookahead == TRUE) {
        consome(TRUE);
    }
    else if(lookahead == FALSE) {
        consome(FALSE);
    }
}

void consome(TAtomo atomo) {
    while (lookahead == COMENTARIO) {
        printf("\n#%2d:comentario", infoAtomo.linha);
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    
    if (lookahead == atomo) {
        switch (lookahead) {
            case IDENTIFICADOR:
                printf("\n#%2d:identifier : %s", infoAtomo.linha, infoAtomo.atributo.ID);
                break;
            case CONSTINT:
                printf("\n#%2d:CONSTINT : %g", infoAtomo.linha, infoAtomo.atributo.numero);
                break;
            case CONSTCHAR:
                printf("\n#%2d:CONSTCHAR : '%c'", infoAtomo.linha, infoAtomo.atributo.ch);
                break;
            case PONTOVIRGULA:
                printf("\n#%2d:ponto_virgula", infoAtomo.linha);
                break;
            case VIRGULA:
                printf("\n#%2d:virgula", infoAtomo.linha);
                break;
            case DOISPONTOS:
                printf("\n#%2d:dois_pontos", infoAtomo.linha);
                break;
            case ABREPARENTESES:
                printf("\n#%2d:abre_par", infoAtomo.linha);
                break;
            case FECHAPARENTESES:
                printf("\n#%2d:fecha_par", infoAtomo.linha);
                break;
            case PONTO:
                printf("\n#%2d:ponto_final", infoAtomo.linha);
                break;
            case PROGRAM:
                printf("\n#%2d:program", infoAtomo.linha);
                break;
            case VAR:
                printf("\n#%2d:var", infoAtomo.linha);
                break;
            case INTEGER:
                printf("\n#%2d:integer", infoAtomo.linha);
                break;
            case CHAR:
                printf("\n#%2d:char", infoAtomo.linha);
                break;
            case BEGIN:
                printf("\n#%2d:begin", infoAtomo.linha);
                break;
            case END:
                printf("\n#%2d:end", infoAtomo.linha);
                break;
            case READ:
                printf("\n#%2d:read", infoAtomo.linha);
                break;
            case WRITE:
                printf("\n#%2d:write", infoAtomo.linha);
                break;
            case MENOR:
                printf("\n#%2d:menor", infoAtomo.linha);
                break;
            case MENORIGUAL:
                printf("\n#%2d:menor_igual", infoAtomo.linha);
                break;
            case MAIOR:
                printf("\n#%2d:maior", infoAtomo.linha);
                break;
            case MAIORIGUAL:
                printf("\n#%2d:maior_igual", infoAtomo.linha);
                break;
            case DIFERENTE:
                printf("\n#%2d:diferente", infoAtomo.linha);
                break;
            case ASSIGN:
                printf("\n#%2d:atribuição", infoAtomo.linha);
                break;
            case COMENTARIO:
                printf("\n#%2d:comentario", infoAtomo.linha);
                break;
            case THEN:
                printf("\n#%2d:then", infoAtomo.linha);
                break;
            case BOOLEAN:
                printf("\n#%2d:boolean", infoAtomo.linha);
                break;
            case IF:
                printf("\n#%2d:if", infoAtomo.linha);
                break;
            case ELSE:
                printf("\n#%2d:else", infoAtomo.linha);
                break;
            case WHILE:
                printf("\n#%2d:while", infoAtomo.linha);
                break;
            case DO:
                printf("\n#%2d:do", infoAtomo.linha);
                break;
            case OR:
                printf("\n#%2d:or", infoAtomo.linha);
                break;
            case AND:
                printf("\n#%2d:and", infoAtomo.linha);
                break;
            case NOT:
                printf("\n#%2d:not", infoAtomo.linha);
                break;
            case TRUE:
                printf("\n#%2d:true", infoAtomo.linha);
                break;
            case FALSE:
                printf("\n#%2d:false", infoAtomo.linha);
                break;
            case OP_DIV:
                printf("\n#%2d:div", infoAtomo.linha);
                break;
            case OP_MULT:
                printf("\n#%2d:mult", infoAtomo.linha);
                break;
            case OP_SOMA:
                printf("\n#%2d:soma", infoAtomo.linha);
                break;
            case OP_SUB:
                printf("\n#%2d:sub", infoAtomo.linha);
                break;
            default:
                printf("\n#%2d:%s", infoAtomo.linha, strMensagem[lookahead]);
                //printf("");
        }
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        while (lookahead == COMENTARIO) {
            printf("\n#%2d:comentario", infoAtomo.linha);
            infoAtomo = obter_atomo();
            lookahead = infoAtomo.atomo;
        }
    }
    else {
        printf("\n#%2d:erro sintatico, esperado [%s] encontrado [%s]\n", infoAtomo.linha, strMensagem[atomo], strMensagem[lookahead]);
        exit(1);
    }
}