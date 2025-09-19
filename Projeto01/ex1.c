#include <stdio.h>
#include <stdlib.h>

char *buffer = "bcabcbb";
//A::= bBb
//B::= cC | eD
//C::= aA | Vazio
//D::= da
void A();
void B();
void C();
void D();

void A() {
    switch (*buffer) {
    case 'b':
        buffer++; 
        B();
        buffer++;
        break;
    default:
        exit(1);
    }
}

void B() {
    switch (*buffer) {
    case 'c':
        buffer++;
        C();
        break;
    
    case 'e':
        buffer++;
        D();
        break;
    
    default:
        exit(1);
    }
}

void C() {
    switch (*buffer) {
    case 'a':
        buffer++;
        A();
        break;
    default:
        break;
    }
}

void D() {
    switch (*buffer) {
    case 'd':
        buffer++;
        buffer++;
        break;
    
    default:
        exit(1);
    }
}

int main(){
    printf("Analisando: %s => ",buffer);
    A(); // chama o simbolo inicial da gramatica
    if(*buffer == '\0') {
        printf("sintaxe correta.\n");
    }   
    else {
        printf("Erro sintatico, nao cheguou ao final do buffer.\n");
        printf("fim de programa.");
    }
    return 0;
}