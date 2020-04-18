#include "ficheiros.h"
#include "macro.h"

#ifndef _FICHEIROS_

/* **********************
 *  Ficheiros de Texto  *
 * **********************/

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//ler lista de alunos

void lerlistalunos(LIST_ALUNO * aluno){
    FILE * fp = fopen("Listaalunos.txt","r");
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_READFILE);
        exit(_ERR_READFILE);
    }
    wchar_t * linhas = calloc(_TAMSTRING,sizeof(wchar_t));
    if(!linhas){
        wprintf(L"Erro %d: Não foi possivel alocar memoria para a linha",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    while(!feof(fp)){
        fwscanf(fp,L"%l[^;];\n",linhas);
        if(wmemcmp(linhas,L"#",1)==0 || wcsncmp(linhas,L" ",1)==0)
            continue;
        processarLinhalunos(linhas,aluno);
    }
    fclose(fp);
}

// processar linha de texto do  ficheiro da lista de alunos
void processarLinhalunos(wchar_t * linhas, LIST_ALUNO * aluno){
    wchar_t * temp, * buffer;
    int numero = wcstol(wcstok(linhas, L"-", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL, L"-", &buffer);
    wchar_t * pais = wcstok(NULL, L"-", &buffer);
   
    adicionarAluno(criarAluno(numero,nome,pais),aluno,aluno->elementos);//Adiciona sempre no final
}


//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//Ler ficheiro de UC
void lerUCTexto(LIST_UC * lista){
    FILE * fp = fopen("dadosUC.txt","r");
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_READFILE);
        exit(_ERR_READFILE);
    }
    wchar_t * linha = calloc(_TAMSTRING,sizeof(wchar_t));
    if(!linha){
        wprintf(L"Erro %d: Não foi possivel alocar memoria para a linha",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    while(!feof(fp)){
        fwscanf(fp,L"%l[^;];\n",linha);
        if(wmemcmp(linha,L"#",1)==0 || wcsncmp(linha,L" ",1)==0)
            continue;
        processarLinhaUC(linha,lista);
    }
    free(linha);
    linha = NULL;
    fclose(fp);
}

//Processar Linha de Texto do ficheiro de UC
void processarLinhaUC(wchar_t * linha, LIST_UC * lista){
    wchar_t * temp, * buffer;
    int numero = wcstol(wcstok(linha, L"-", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL, L"-", &buffer);
    int ano = wcstol(wcstok(NULL, L"-", &buffer), &temp,10);
    int semestre = wcstol(wcstok(NULL, L"-", &buffer), &temp,10);
    adicionarUC(criarUC(numero,nome,ano,semestre),lista,lista->elementos);//Adiciona sempre no final
}

#endif