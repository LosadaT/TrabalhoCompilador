/*
Implementacao do mini analisador lexico conforme slides da aula Aula3(miniLexico).pdf
 IDENTIFICADOR -> LETRA_MINUSCULA(LETRA_MINUSCULA|DIGITO)*
 NUMERO -> DIGITO+.DIGITO+

       
Para compilar no vscode use:
gcc miniLexico.c -Wall -Og -g -o miniLexico

-Wall: habilita todos avisos do compilador. Este modo indica possíveis erros cometidos no programa.
-Og: somente aplicar otimizações que não atrapalham debuging.
-g: informações para debug usando gdb
-o: output -- nome do executável gerado 

Como usar o gdb
https://diveintosystems.org/book/C3-C_debug/gdb_commands.html

Teste de memoria
https://diveintosystems.org/book/C3-C_debug/valgrind.html

Rode o Valgrind com 
valgrind --leak-check=yes ./miniLexico 

caso não esteja instalado use
sudo apt update
sudo apt install valgrind
sudo apt upgrade
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> //atof

// código interno para atomos
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    EOS
}TAtomo;

// estrutura para retorno da funcao obter_atomo()
typedef struct{
    TAtomo atomo;
    int linha;
    union{
        float numero;
        char ID[16];
    }atributo;
}TInfoAtomo;

// variavel global
char *buffer ="\n\n12.2\n\n\t 1.1 \n var1 " ;
char lexema[20];
int nLinha;

TInfoAtomo obter_atomo();
void reconhece_numero(TInfoAtomo *infoAtomo);
void reconhe_id(TInfoAtomo *infoAtomo);


int main(){
    TInfoAtomo infoAtomo;
    printf("Analisando: %s => \n", buffer);
    nLinha = 1;
    do{
        infoAtomo = obter_atomo();
        if( infoAtomo.atomo == NUMERO )
            printf("Atomo NUMERO reconhecido [%f] na linha [%d].\n",infoAtomo.atributo.numero, infoAtomo.linha);
        if( infoAtomo.atomo == IDENTIFICADOR )
            printf("Atomo IDENTIFICADOR reconhecido [%s] na linha [%d].\n",infoAtomo.atributo.ID, infoAtomo.linha);
        else if(infoAtomo.atomo == ERRO)
            printf("Erro lexico na linha [%d].\n",infoAtomo.linha);
        else if( infoAtomo.atomo == EOS)
            printf("Fim de buffer.\n");
    } while(infoAtomo.atomo != ERRO && infoAtomo.atomo != EOS);
    
    printf("fim de programa. %d linhas analisadas\n", infoAtomo.linha);
    return 0;
}

TInfoAtomo obter_atomo(){
    TInfoAtomo infoAtomo;

    infoAtomo.atomo = ERRO;
    while( *buffer == '\n' || *buffer == ' ' || *buffer == '\t'){
        if( *buffer == '\n')
            nLinha++;

        buffer++;
    }

    infoAtomo.linha = nLinha;

    if(isdigit(*buffer)){
        reconhece_numero(&infoAtomo);
    }
    else if(islower(*buffer))
        reconhe_id(&infoAtomo);
    else if (*buffer == '\0')
        infoAtomo.atomo = EOS;

    return infoAtomo;    
}

/*
funcao implementa o automato para a expressao regular
DIGITO -> 0|1|...|9 
NUMERO -> DIGITO+.DIGITO+
*/
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

void reconhe_id(TInfoAtomo *infoAtomo){
    char *ini_lexema = buffer;

q1:
    if( islower(*buffer) || isdigit(*buffer) ){
        buffer++;
        goto q1;
    }
    if( isupper(*buffer)) 
        return; // sai com erro
    
    // preenche o atributo do atomo IDENTIFICADOR
    strncpy(infoAtomo->atributo.ID,ini_lexema,buffer-ini_lexema);
    infoAtomo->atributo.ID[buffer-ini_lexema] = '\0'; // aqui temos um IDENTIFICADOR

    infoAtomo->atomo = IDENTIFICADOR;

    return ;
}