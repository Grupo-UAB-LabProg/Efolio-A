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
            //Verifica se existe o aluno
            if (aluno != NULL) {
                wprintf(L"\nAno Letivo [xxxx/yyyy]: ");
                wscanf(L"%S", anoLetivo);
                novo = procuraInscricao(bd->inscricoes, anoLetivo,numeroAluno,numeroUC);
                //Verifica se existe inscrição para esse ano
                if (novo == NULL) {
                    //Valida a inscrição do aluno
                    if(validarInscricoes(bd, aluno, anoLetivo, uc->ects) == _TRUE_){
                        novo = criarInscricao(numeroUC,-1, numeroAluno, anoLetivo);
                        //Caso seja valido adiciona a inscriçaõ a lista
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
    int id, opcao;
    wchar_t * anoLetivo;   
    anoLetivo = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!anoLetivo){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wprintf(L"\nIndique o Ano Letivo que pretende consultar: ");
    wscanf(L"%S", anoLetivo);
    //Obtem a pasta do ano letivo a consultar
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
    int id, n, opcao, continuar;
    wchar_t * anoLetivo;
    anoLetivo = calloc(_TAMDATAS,sizeof(wchar_t));
    if(!anoLetivo){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wprintf(L"\nIndique o Ano Letivo que pretende consultar: ");
    wscanf(L"%S", anoLetivo);
    //Obtem a pasta do ano letivo a pesquisar
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

//Imprimir Listagem por UC
void uiImprimirListagemPorUC(SGBD * bd){
    clearScreen();
    cabecalho("Menu Reports");
    wprintf(L"Qual o ano letivo que pretende:\nAno: ");
    wchar_t * ano = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!ano){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wscanf(L"%S", ano);
    //Obtem a pasta do ano letivo a consultar
    NO_PASTA * pasta = obterPastaAno(ano, bd->inscricoes);
    if(pasta){
        for(int i = 0; i< pasta->elementos; i++){
            INSCRICAO * aux = obterInscricao(i,pasta);
            int id = aux->numeroUC;
            int continuar=0;
            //Verifica se já imprimiu esta inscrição
            if(i>0){
                for(int e =0; e<i; e++){
                    if(obterInscricao(e,pasta)->numeroUC == id){
                        continuar =1;
                    }
                }
            }
            //Caso tenha imprimido não volta a imprimir
            if(continuar == 1){
                continue;
            }
            //Imprimir inscrição atual
            uiImprimirCabecalhoUC(obterUCNum(id,bd->ucs), ano);
            wprintf(L"|%10S|%76S|%10S|\n", L"Numero", L"Nome", L"Nota");
            wprintf(L"|%10.d|%76S|%10.d|\n", aux->numeroAluno, obterAlunoNum(aux->numeroAluno,bd->alunos)->nome, aux->nota);
            //Percorre o resto das inscrições e imprime as que forem do mesmo tipo
            for(int e = i; e<pasta->elementos-1; e++){
                INSCRICAO * ins = obterInscricao(e+1,pasta);
                if(ins->numeroUC == id){
                    wprintf(L"|%10.d|%76S|%10.d|\n", ins->numeroAluno, obterAlunoNum(ins->numeroAluno,bd->alunos)->nome, ins->nota);
                }
            }
            for(int i = 0; i<100; i++)
                wprintf(L"-");
            wprintf(L"\n");
        }
    }
    free(ano);
    pressioneENTER();
}

//Imprime Titulo da Listagem
void uiImprimirCabecalhoUC(UC * unidade, wchar_t * ano){
    for(int i =0; i<100; i++)
        wprintf(L"-");
    wprintf(L"\n");
    wprintf(L"|%10.d|%76S|%10S|\n", unidade->numero, unidade->nome, ano);
    for(int i =0; i<100; i++)
        wprintf(L"-");
    wprintf(L"\n");
}

//Imprime Titulo da Listagem
void uiImprimirCabecalhoAluno(ALUNO * aluno, wchar_t * ano){
    for(int i =0; i<100; i++)
        wprintf(L"-");
    wprintf(L"\n");
    wprintf(L"|%10.d|%76S|%10S|\n", aluno->numero, aluno->nome, ano);
    for(int i =0; i<100; i++)
        wprintf(L"-");
    wprintf(L"\n");
}

//Imprimir Listagem por Aluno
void uiImprimirListagemPorAluno(SGBD * bd){
    clearScreen();
    cabecalho("Menu Reports");
    wprintf(L"Qual o ano letivo que pretende:\nAno: ");
    wchar_t * ano = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!ano){
        wprintf(L"Erro %d: Não foi possivel alocar a memória",_ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wscanf(L"%S", ano);
    //Obtem a pasta do ano letivo a consultar
    NO_PASTA * pasta = obterPastaAno(ano, bd->inscricoes);
    if(pasta){
        for(int i = 0; i< pasta->elementos; i++){
            INSCRICAO * aux = obterInscricao(i,pasta);
            int id = aux->numeroAluno;
            int continuar=0;
            //Verifica se já imprimiu esta inscrição
            if(i>0){
                for(int e =0; e<i; e++){
                    if(obterInscricao(e,pasta)->numeroAluno == id){
                        continuar =1;
                    }
                }
            }
            //Caso tenha sido imprimida não volta a imprimir
            if(continuar == 1){
                continue;
            }
            //Imprimir inscrição atual
            uiImprimirCabecalhoAluno(obterAlunoNum(id,bd->alunos), ano);
            wprintf(L"|%10S|%76S|%10S|\n", L"Numero", L"Nome", L"Nota");
            wprintf(L"|%10.d|%76S|%10.d|\n", aux->numeroUC, obterUCNum(aux->numeroUC,bd->ucs)->nome, aux->nota);
            //Percorre o resto das inscrições e imprime as que forem do mesmo tipo
            for(int e = i; e<pasta->elementos-1; e++){
                INSCRICAO * ins = obterInscricao(e+1,pasta);
                if(ins->numeroAluno == id){
                    wprintf(L"|%10.d|%76S|%10.d|\n", ins->numeroUC, obterUCNum(ins->numeroUC,bd->ucs)->nome, ins->nota);
                }
            }
            for(int i = 0; i<100; i++)
                wprintf(L"-");
            wprintf(L"\n");
        }
    }
    free(ano);
    pressioneENTER();
}