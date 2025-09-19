#include <stdio.h>
#include <stdlib.h>

char *buffer = "acadbeadb";
//S ::= aAB | aBA
//A ::= b | cS
//B ::= d | eS
void S();
void A();
void B();

void S() {
    switch (*buffer) {
    case 'a':
        buffer++; 
        char lookahead = *buffer;
        if (lookahead == 'b' || lookahead == 'c') {
            A(); // S ::= aAB
            B();
        } else if (lookahead == 'd' || lookahead == 'e') {
            B(); // S ::= aBA
            A();
        } else {
            printf("Erro: símbolo inesperado após 'a': '%c'\n", lookahead);
            exit(1);
        }
        break;
    default:
        exit(1);
    }
}

void A() {
    switch (*buffer) {
    case 'b':
        buffer++;
        break;
    
    case 'c':
        buffer++;
        S();
        break;
    
    default:
        exit(1);
    }
}

void B() {
    switch (*buffer) {
    case 'd':
        buffer++;
        break;
    case 'e':
        buffer++;
        S();
        break;
    default:
        exit(1);
    }
}

int main(){
    printf("Analisando: %s => ", buffer);
    S(); // chama o simbolo inicial da gramatica
    if(*buffer == '\0') {
        printf("sintaxe correta.\n");
    }   
    else {
        printf("Erro sintatico, nao cheguou ao final do buffer.\n");
        printf("fim de programa.\n");
    }
    return 0;
}