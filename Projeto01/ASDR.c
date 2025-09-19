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

// E::= E + T| T == E::= T {+ T}
// A::= a | b | c

// prototipacao de funcao
void E(); 
void T(); 

int main(){
    printf("Analisando: %s => ",buffer);
    // lookahead == obter_atomo()
    lookahead = *buffer++; // recebe o simbolo inicial e atualiza o buffer

    E(); // chama o simbolo inicial da gramatica
    consome('\0');

    printf("Reconhecido.");

    return 0;
}

void E() {
    T();
    while(lookahead == '+'){
        consome('+');
        T();
    }
}

void T() {
    if (lookahead == 'a' ) {
        consome('a'); // consome 'a', 'b' ou 'c'
    } 
    else if(lookahead == 'b') {
        consome('b');
    }
    else {
        consome('c');
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
