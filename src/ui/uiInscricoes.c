/*
 * Ficheiro que possui todas as funções da UI (User Interface) de Inscrições
 */
#include "uiInscricoes.h"

//Adicionar uma nova Inscrição
void uiAdicionarNovaInscricao(SGBD * bd) {
    clearScreen();
    INSCRICAO * novo;
    UC * uc;
    ALUNO * aluno;
    int opcao, numeroUC, numeroAluno;
    wchar_t * anoLetivo;   
    anoLetivo = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!anoLetivo){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    do {
        opcao = -1;
        clearScreen();
        wprintf(L"\nPreencha os dados da nova Inscrição");
        wprintf(L"\nNúmero da UC: ");
        wscanf(L"%d", &numeroUC);
        uc = obterUCNum(numeroUC, bd->ucs);
        if (uc != NULL) {
            wprintf(L"\nNúmero de Aluno: ");
            wscanf(L"%d", &numeroAluno);
            aluno = obterAlunoNum(numeroAluno, bd->alunos);
            if (aluno != NULL) {
                wprintf(L"\nAno Letivo [xxxx/yyyy]: ");
                wscanf(L"%S", anoLetivo);
                novo = procuraInscricao(bd->inscricoes, anoLetivo,numeroAluno,numeroUC);
                if (novo == NULL) {
                    if(validarInscricoes(bd, aluno, anoLetivo, uc->ects) == _TRUE_){
                        novo = criarInscricao(numeroUC,0, numeroAluno, anoLetivo);
                        if(adicionarInscricao(novo, bd->inscricoes) == _SUCESSO){
                            wprintf(L"\nIncrição Efectuada\n\nDeseja continuar a inscrever?\n\t0 - Sim\n\t1 - Não\nopcao: ");
                            wscanf(L"%d",&opcao);
                        }
                        else
                            wprintf(L"\nErro na Inscrição"); 
                    }
                    else
                        wprintf(L"\nErro na Inscrição"); 
                }
                else{
                    wprintf(L"\nEsta inscrição já existe\n\nDeseja tentar de novo?\n\t0 - Sim\n\t1 - Sair\nopcão: ");
                    wscanf(L"%d", &opcao);
                }
            }
            else {
                wprintf(L"\nNúmero de Aluno inexistente\n\nDeseja tentar de novo?\n\t0 - Sim\n\t1 - Sair\nopcão: ");
                wscanf(L"%d", &opcao);
            }
        }
        else {
            wprintf(L"\nUnidade Curricular inexistente\n\nDeseja tentar de novo?\n\t0 - Sim\n\t1 - Sair\nopcão: ");
            wscanf(L"%d", &opcao);
        }
    }while (opcao == 0);
    free(anoLetivo);
    pressioneENTER();
}

//Mostrar lista de inscrições por ano letivo
void uiImprimirListaInscricoes(SGBD * bd){
    clearScreen();
    NO_PASTA * pasta;
    wchar_t * anoLetivo;
    anoLetivo = calloc(_TAMDATAS, sizeof(wchar_t));
    wprintf(L"\nIndique o Ano Letivo que pretende consultar: ");
    wscanf(L"%S", anoLetivo);
    pasta = obterPastaAno(anoLetivo, bd->inscricoes);
    if (pasta != NULL)
        uiImprimirInscricoes(pasta);
    else
        wprintf(L"\nNão existe referência ao Ano Letivo introduzido");
    free(anoLetivo);
    pressioneENTER();
}

//Remover uma inscrição
void uiRemoverInscricao(SGBD * bd){
    clearScreen();
    NO_PASTA * pasta;
    int id, opcao, numeroUC, numeroAluno;
    wchar_t * anoLetivo;   
    anoLetivo = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!anoLetivo){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wprintf(L"\nIndique o Ano Letivo que pretende consultar: ");
    wscanf(L"%S", anoLetivo);
    pasta = obterPastaAno(anoLetivo, bd->inscricoes);
    if(pasta != NULL){
        do {
            if(pasta->elementos){
                opcao = -1;
                id = -1;
                do{
                    clearScreen();
                    uiImprimirInscricoes(pasta);
                    wprintf(L"\nIndique o ID da inscrição que pretende remover: ");
                    wscanf(L"%d",&id);
                    if(id <= 0 || id > pasta->elementos){
                        wprintf(L"\nID da inscrição inválido\n");
                        pressioneENTER();
                    }
                }while(id <= 0 || id > pasta->elementos);
                wprintf(L"\nTem a certeza que deseja remover esta inscrição?\n\t0 - Sim\n\t1 - Sair\nopção: ");
                wscanf(L"%d", &opcao);
                if(opcao == 0)
                    if(removerInscricoes(id-1,pasta) == _SUCESSO) {
                        wprintf(L"\nInscrição removida\n");
                        wprintf(L"\nDeseja continuar a remover?\n\t0 - Sim\n\t1 - Sair\nopcao: ");
                        wscanf(L"%d", &opcao);
                    }
            }
            else {
                wprintf(L"\nNão existe Inscrições neste Ano Letivo");
                opcao = -1;
            }
        }while(opcao == 0);
    }
    else
        wprintf(L"\nNão existe referência ao Ano Letivo introduzido");
    free(anoLetivo);
    pressioneENTER(); 
}

//Imprimir dados da inscrições
void uiImprimirInscricoes(NO_PASTA * pasta){
    clearScreen();
    NO * tmp;
    int i, j;
    tmp = pasta->cauda;
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n|%6S|%23S|%18S|%10S|%17S|\n",L"ID",L"Número de Aluno",L"Número de UC",L"Nota",L"Ano Letivo");
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n");
    for(j=0; j < pasta->elementos; j++) {
        if(j == 0)
            tmp = tmp->proximo;
        wprintf(L"|%6d|%23d|%18d|%10d|%17S|\n", j+1, tmp->elemento->numeroAluno, tmp->elemento->numeroUC,tmp->elemento->nota, tmp->elemento->anoLetivo);
        tmp = tmp->proximo;
    }
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n|%52S%26d|\n", L"Total de Inscrições",pasta->elementos);
    for(i =0; i<80; i++)
        wprintf(L"-");
}

//Modificar valores de uma inscrição
void uiAlterarInscricao(SGBD * bd){
    clearScreen();
    NO_PASTA * pasta;
    INSCRICAO * inscricao;
    int id, n, opcao, continuar, numeroAluno, numeroUC;
    wchar_t * anoLetivo;
    anoLetivo = calloc(_TAMDATAS,sizeof(wchar_t));
    if(!anoLetivo){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wprintf(L"\nIndique o Ano Letivo que pretende consultar: ");
    wscanf(L"%S", anoLetivo);
    pasta = obterPastaAno(anoLetivo, bd->inscricoes);
    if(pasta != NULL){
        do{
            if(pasta->elementos){
                opcao = -1;
                id = -1;
                do{
                    clearScreen();
                    uiImprimirInscricoes(pasta);
                    wprintf(L"\nQual o ID da inscrição que deseja modificar: ");
                    wscanf(L"%d",&id);
                    if(id <= 0|| id > pasta->elementos){
                        wprintf(L"\nID da inscrição inválido\n");
                        pressioneENTER();
                    }
                }while(id <= 0|| id > pasta->elementos);
                inscricao = obterInscricao(id-1,pasta);
                do{
                    wprintf(L"\nQual o dado a alterar?\n\t0 - Número Aluno\n\t1 - Número Unidade Curricular\n\t2 - Nota\n\t3 - Cancelar\nOpção: ");
                    wscanf(L"%d",&opcao);
                }while(opcao<0 || opcao>3);
                n = -1;
                //Executar opção escolhida
                switch(opcao){
                    case 0:
                        wprintf(L"\nNovo Número de Aluno: ");
                        wscanf(L"%d", &n);
                        if(obterAlunoNum(n,bd->alunos))
                            modificarValorInscricao(n,inscricao->numeroUC, inscricao->nota,inscricao);
                        else
                            wprintf(L"\nNúmero de Aluno inexistente");
                        break;
                    case 1:
                        wprintf(L"\nNova Unidade Curricular: ");
                        wscanf(L"%d", &n);
                        if(obterUCNum(n,bd->ucs))
                            modificarValorInscricao(inscricao->numeroAluno, n , inscricao->nota,inscricao);
                        else
                            wprintf(L"\nNúmero de UC inexistente");
                        break;
                    case 2:
                        do{
                            wprintf(L"\nNova nota: ");
                            wscanf(L"%d", &n);
                        }while(n< 0 || n> 20);
                        modificarValorInscricao(inscricao->numeroAluno, inscricao->numeroUC,n, inscricao);
                        break;
                    case 3:
                        continuar =0; continue;
                }
                wprintf(L"\nPretende continuar a alterar?\n\t0 - Não\n\t1 - Sim\nOpção: ");
                wscanf(L"%d",&continuar);
            }
            else {
                wprintf(L"\nNão existe Inscrições neste Ano Letivo");
                break;
            }
        }while(continuar != 0);
    }
    else
        wprintf(L"\nNão existe referência ao Ano Letivo introduzido");
    free(anoLetivo);
    pressioneENTER();
}

//Imprimir dado da inscrição
void uiImprimirDadoInscricao(INSCRICAO * inscricao){
    int i;
    for(i =0; i<46; i++)
        wprintf(L"-");
    wprintf(L"\n|%23S|%23S|\n",L"Número de Aluno",L"Número de UC");
    for(i =0; i<46; i++)
        wprintf(L"-");
    wprintf(L"\n");
    wprintf(L"|%23d|%23d|\n", inscricao->numeroAluno, inscricao->numeroUC);
    for(i =0; i<46; i++)
        wprintf(L"-");
}