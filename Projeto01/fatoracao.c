/*
para compilar use:
gcc ASDR.c -Wall -Og -g -o ASDR
*/

#include <stdio.h>
#include <stdlib.h>

// variavel global
char *buffer ="a+b+c+c+a++";
char lookahead;

// funcoes do analisador sintatico
void consome(char atomo);

// A::= ac | ab | a vazio
// A::= a ( b | c | vazio)
// A::= a[c|b]

// prototipacao de funcao
void A(); 

int main(){
    printf("Analisando: %s => ",buffer);
    // lookahead == obter_atomo()
    lookahead = *buffer++; // recebe o simbolo inicial e atualiza o buffer

    A(); // chama o simbolo inicial da gramatica
    consome('\0');

    printf("Reconhecido.");

    return 0;
}

void A() {
    consome('a');
    if (lookahead == 'c') {
        consome('c');
    }
    else if(lookahead == 'c') {
        consome('b');
    }
}


// recebe um atomo que deveria estar no inicio do buffer, ou seja, lookhead
void consome( char atomo ){
    if( lookahead == atomo )
        lookahead = *buffer++; // obter_atomo();
    else{
        printf("erro sintatico: esperado [%c] encontrado [%c]\n",atomo,lookahead);
        exit(1);
    }
}
