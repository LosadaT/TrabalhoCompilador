#include <stdio.h>
#include <stdlib.h>
char *buffer = "+*ab+ab";
// E ::= a | b | +EE | *EE

void E() {
    switch (*buffer) {
    case '+':
        buffer++; //consome +
        E();
        E();
        break;
    
    case '*':
        buffer++; //consome *
        E();
        E();
        break;

    case 'a':
        buffer++; //consome a
        break;
    
    case 'b':
        buffer++; //consome b
        break;
    
    default:
        exit(1);
    }
}

int main(){
    printf("Analisando: %s => ",buffer);
    E(); // chama o simbolo inicial da gramatica
    if(*buffer == '\0') {
        printf("sintaxe correta.\n");
    }   
    else {
        printf("Erro sintatico, nao cheguei ao final do buffer.\n");
        printf("fim de programa.");
    }
    return 0;
}