/*
para compilar use:
gcc ASDR.c -Wall -Og -g -o ASDR
*/

#include <stdio.h>
#include <stdlib.h>

// variavel global
char *buffer ="abaddc";
char lookahead;

// funcoes do analisador sintatico
void consome(char atomo);

// S::= AS | BA
// A::= aB | C 
// B ::= bA | d 
// C::= c 
// prototipacao de funcao
void S(); 
void A(); 
void B(); 
void C(); 

int main(){
    printf("Analisando: %s => ",buffer);
    // lookahead == obter_atomo()
    lookahead = *buffer++; // recebe o simbolo inicial e atualiza o buffer

    S(); // chama o simbolo inicial da gramatica
    consome('\0');

    printf("Reconhecido.");

    return 0;
}
// S::= AS | BA
void S(){
    if(lookahead == 'a' || lookahead == 'c'){
        A();
        S();
    }
    else {
        B();
        A();
    }
}
// A::= aB | C 
void A(){
    if(lookahead == 'a'){
        consome('a');
        B();
    }
    else {
        C();
    }
}

// B ::= bA | d 
void B(){
    if(lookahead == 'b'){
        consome('b');
        A();
    }
    else {
        consome('d');
    }
}

// C::= c 
void C(){
    consome('c');
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
