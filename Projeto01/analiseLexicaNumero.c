#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define ERRO 0
#define NUMERO 1
#define EOS 2

char *buffer = "10.2;    1.2,,,,,,4.4";
char lexema[20];

int reconhece_numero(void) {
    char *iniLexema;
q0:
    iniLexema = buffer;
    if(isdigit(*buffer)) {
        buffer++;
        goto q1;
    }
    if (*buffer == '\0') {
        return EOS;
    }
    buffer++;
    goto q0;

    //return ERRO;

q1:
    if(isdigit(*buffer)) {
        buffer++;
        goto q1;
    }
    if(*buffer == '.') {
        buffer++;
        goto q2;
    }
    return ERRO;

q2: 
    if(isdigit(*buffer)) {
        buffer++;
        goto q3;
    }
    return ERRO;

q3:
    if(isdigit(*buffer)) {
        buffer++;
        goto q3;
    }
    if(isalpha(*buffer)){
        return ERRO;
    }
    strncpy(lexema, iniLexema, buffer - iniLexema);
    lexema[buffer-iniLexema] = '\0';
    return NUMERO;
}

int main() {
    int atomo;
    printf("Analisando: %s => \n",buffer);
    do {
        atomo = reconhece_numero();
        if(atomo == NUMERO) {
            printf("Atomo NUMERO reconhecido [%s].\n",lexema);
        }
        else {
            printf("Atomo NUMERO NAO reconhecido.\n");
            break;
        }
    } while(atomo != EOS && atomo != ERRO);

    printf("fim de programa.\n");

    return 0;
}   