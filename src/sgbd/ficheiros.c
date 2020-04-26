/*
 * Ficheiro que possui todas as funções de ficheiros
 */
#include "ficheiros.h"

/* **********************
 *   Ficheiros de UC    *
 * **********************/

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
        fwscanf(fp,L"%l[^\n]\n",linha);
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
    int numero = wcstol(wcstok(linha, L";", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL, L";", &buffer);
    int ano = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    int semestre = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    int ects = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    adicionarUC(criarUC(numero,nome,ano,semestre,ects),lista,lista->elementos);//Adiciona sempre no final
}

//Gravar Dados da UC no ficheiro
void gravarUCTexto(LIST_UC * lista){
    FILE * fp = fopen("dadosUC2.txt","w");
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_READFILE);
        exit(_ERR_READFILE);
    }
    fwprintf(fp, L"#Unidades Curriculares;\n\n#ID-Nome-Ano-Semestre;\n\n");
    for(int i= 0; i < lista->elementos; i++){
        UC * aux = obterUCPos(i, lista);
        fwprintf(fp, L"%d;%S;%d;%d;%d\n", aux->numero, aux->nome, aux->ano, aux->semestre, aux->ects);
    }
    fclose(fp);
}

/* **********************
 *  Ficheiros de Alunos *
 * **********************/

//ler lista de alunos
void lerListaAlunos(LIST_ALUNO * aluno){
    FILE * fp = fopen("dadosAlunos.txt","r");
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
        fwscanf(fp,L"%l[^\n]\n",linhas);
        if(wmemcmp(linhas,L"#",1)==0 || wcsncmp(linhas,L" ",1)==0)
            continue;
        processarLinhaAlunos(linhas,aluno);
    }
    free(linhas);
    linhas = NULL;
    fclose(fp);
}

//processar linha de texto do  ficheiro da lista de alunos
void processarLinhaAlunos(wchar_t * linhas, LIST_ALUNO * aluno){
    wchar_t * temp, * buffer;
    int numero = wcstol(wcstok(linhas, L";", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL, L";", &buffer);
    wchar_t * pais = wcstok(NULL, L";", &buffer);
    adicionarAluno(criarAluno(numero,nome,pais),aluno,aluno->elementos);//Adiciona sempre no final
}

//Gravar Dados dos Alunos no ficheiro
void gravarAlunosTexto(LIST_ALUNO * lista){
    FILE * fp = fopen("dadosAlunos1.txt","w");
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_READFILE);
        exit(_ERR_READFILE);
    }
    fwprintf(fp, L"#Alunos\n\n#Nº;Nome;País;\n\n");
    for(int i= 0; i < lista->elementos; i++){
        ALUNO * aux = obterAlunoPos(i, lista);
        fwprintf(fp, L"%d;%S;%S\n", aux->numero, aux->nome, aux->pais);
    }
    fclose(fp);
}

/* *****************************
 *   Ficheiros de Inscriçoes   *
 * *****************************/

//Ler ficheiro de Inscricoes
void lerInscricoesTexto(LISTA_PASTA * lista){
    FILE * fp = fopen("dadosInscricoes.txt","r");
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
        fwscanf(fp,L"%l[^\n]\n",linha);
        if(wmemcmp(linha,L"#",1)==0 || wcsncmp(linha,L" ",1)==0)
            continue;
        processarLinhaInscricao(linha,lista);
    }
    free(linha);
    linha = NULL;
    fclose(fp);
}

//Processar Linha de Texto do ficheiro de UC
void processarLinhaInscricao(wchar_t * linha, LISTA_PASTA * lista){
    wchar_t * temp, * buffer;
    int numeroAluno = wcstol(wcstok(linha, L";", &buffer), &temp,10);
    int numeroUC = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    wchar_t * ano = wcstok(NULL, L";", &buffer);
    int nota = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    adicionarInscricao(criarInscricao(numeroUC,nota, numeroAluno,ano),lista);
}

//Gravar Dados dos Alunos no ficheiro
void gravarInscricoesTexto(LISTA_PASTA * lista){
    FILE * fp = fopen("dadosInscricoes1.txt","w");
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_READFILE);
        exit(_ERR_READFILE);
    }
    fwprintf(fp, L"#Inscrições\n\n#NºAluno;NºUC;Ano Letivo;Nota\n\n");
    for(int i= 0; i < lista->pastas; i++){
        NO_PASTA * aux = obterPastaPos(i, lista);
        for(int e= 0; e < aux->elementos; e++){
            INSCRICAO *aux2 = obterInscricao(e,aux);
            fwprintf(fp, L"%d;%d;%S;%d\n", aux2->numeroAluno, aux2->numeroUC, aux2->anoLetivo, aux2->nota);
        }
    }
    fclose(fp);
}

/************************************
 *            Report A              *
 ************************************/

//Cria ficheiro e cabeçalho do Report A
FILE * criarReportA(){
    FILE * novo = fopen("Report A", "w");
    if(!novo){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_WRITEFILE);
        exit(_ERR_WRITEFILE);
    }
    fwprintf(novo, L"# Report A - Lista de Total de ECTS por aluno no ano letivo corrente\n\n");
    return novo;
}
//Escrever no Report A
int escreverReportA(REP_A * report, FILE * fp){
    int i;
    No_REP_A * norep = report->cauda;
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro.", _ERR_RESOURCENOTFOUND);
        return _ERR_RESOURCENOTFOUND;
    }
    for(i=0;i<report->elementos; i++){
        fwprintf(fp, L"%d;%S;%d;%S\n", norep->elemento->numero,norep->elemento->nome,norep->elemento->ects,norep->elemento->observacao);
        norep = norep->proximo;
    }
    return _SUCESSO;
}
//Termina a escrita no report A
int terminarReportA(FILE * fp){
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro a terminar.", _ERR_RESOURCENOTFOUND);
        return _ERR_RESOURCENOTFOUND;
    }
    fwprintf(fp,L"\n\n#Fim Report A");
    fclose(fp);
    return _SUCESSO;
}

/************************************
 *            Report B              *
 ************************************/

//Escrever linha no Report
int escreverLinhaReportB(ALUNO * aluno, int ucsRealizadas, FILE * fp){
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro.", _ERR_RESOURCENOTFOUND);
        return _ERR_RESOURCENOTFOUND;
    }
    fwprintf(fp, L"%d;%S;%d\n", aluno->numero, aluno->nome, ucsRealizadas);
    return _SUCESSO;
}

//Cria ficheiro e cabeçalho do Report B
FILE * criarReportB(){
    FILE * novo = fopen("Report B.txt", "w");
    if(!novo){
        wprintf(L"Erro %d: Não foi possivel abrir o ficheiro",_ERR_WRITEFILE);
        exit(_ERR_WRITEFILE);
    }
    fwprintf(novo, L"# Report B - Lista de Alunos que podem fazer exames de época especial\n\n");
    fwprintf(novo, L"# NumeroAluno;Nome;Total UC Realizadas\n\n");
    return novo;
}

//Termina a escrita no report B
int terminarReportB(FILE * fp){
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro a terminar.", _ERR_RESOURCENOTFOUND);
        return _ERR_RESOURCENOTFOUND;
    }
    fwprintf(fp,L"\n\n#Fim Report B");
    fclose(fp);
    return _SUCESSO;
}
/*
void imprimirReportB(REP_B * reportB){
    REP_B_ELEM* norep = reportB->cauda;
    int i;
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n|%20S|%30S\n",L"Número de Aluno",L"Nome do Aluno");
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n");
    for(i=0;i<reportB->quantidade;i++){
        wprintf(L"|%20S|%30S|\n",norep->chave);
        norep =  norep->proximo;
    }
    for(i =0; i<80; i++)
        wprintf(L"-");
}*/

/************************************
 *            Report C              *
 ************************************/

/************************************
 *            Report D              *
 ************************************/