/*
 * Ficheiro que possui todas as funções de gestão da Base de dados
 */

#include "sgbd.h"

/************************************
 *          Motor SGBD              *
 ************************************/

//Criar Base de Dados vazia
SGBD *criarBaseDados(){
    SGBD * baseDados = calloc(1, sizeof(SGBD));
    if (!baseDados){
        wprintf(L"Erro %d: Impossível alocar memória para Base de Dados", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    baseDados->alunos = criarListaAluno();
    baseDados->inscricoes = criarListaPastas();
    baseDados->ucs = criarListaUC();
    return baseDados;
}

//Libertar Memoria Base Dados
int libertarBD(SGBD * bd){
    if(!bd){
        wprintf(L"Erro %d: Não foi possivel libertar memória!", _ERR_MEMORYFREE);
        return _ERR_MEMORYFREE;
    }
    if(bd->alunos){
        libertarListaAluno(bd->alunos);
        bd->alunos = NULL;
    }
    if(bd->ucs){
        libertarListaUC(bd->ucs);
        bd->ucs = NULL;
    }
    if(bd->inscricoes){
        libertarListaPasta(bd->inscricoes);
        bd->inscricoes = NULL;
    }
    free(bd);
    bd=NULL;
    return _SUCESSO;
}

//Gravar informação da bd nos ficheiros novamente
int gravarDados(SGBD * bd){
    if(!bd){
        wprintf(L"Erro %d: Base de Dados não existe\n");
        return _ERR_RESOURCENOTFOUND;
    }
    //Gravar ficheiro de UCS
    gravarUCTexto(bd->ucs);
    //Gravar ficheiro de Alunos
    gravarAlunosTexto(bd->alunos);
    //Gravar ficheiro de inscrições
    gravarInscricoesTexto(bd->inscricoes);
    return _SUCESSO;
}

//Ler informação dos ficheiros
int carregarFicheiros(SGBD * bd){
    if(!bd){
        wprintf(L"Erro %d: Base de Dados não existe\n");
        return _ERR_RESOURCENOTFOUND;
    }
    //Carregar ficheiro de UCS
    lerUCTexto(bd->ucs);
    //Carregar ficheiro de Alunos
    lerListaAlunos(bd->alunos);
    //Carregar ficheiro de inscrições
    lerInscricoesTexto(bd->inscricoes);
    return _SUCESSO;
}

//Validar Inscriçoes
int validarInscricoes(SGBD * bd, ALUNO * aluno, wchar_t * ano, int ects){
    return validarInscricao(aluno,bd->ucs,ects,ano,bd->inscricoes);
}

//Calcular Propinas para o ultimo Ano
void calcularPropinas(SGBD * bd, ALUNO * aluno){
    NO_PASTA * aux = obterAnoLetivoRecente(bd->inscricoes);
    if(aux){
        //Verifica se o aluno é residente ou estrangeiro e se é o 1ºanoou não
        int opcao = condicaoPropina(aluno,bd->inscricoes);
        int ects = 0;
        //Obtem o final da lista o ano letivo mais recente
        NO * temp = aux->cauda;
        //Percorre  a lista e soma o valor das ECTS a que o aluno está inscrito
        for(int i =0; i<aux->elementos; i++){
            temp = temp->proximo;
            if(temp->elemento->numeroAluno == aluno->numero){
                ects += obterECTS(obterUCNum(temp->elemento->numeroUC,bd->ucs));
            }
        }
        aluno->propina = calculoPropina(ects,opcao);
    }
}

//Cálculo da propina mediante condicão do estudante
int calculoPropina(int ects, int opcao) {
    int propina;
    if(opcao == 1)          //Residente 1º ano
        propina = 80 + 15 * ects;
    else if(opcao == 2)     //Residente não 1º ano
        propina = 15 * ects;
    else if(opcao == 3)     //Estrangeiro 1º ano
        propina = 80 + 15 * 1.2 * ects;
    else if (opcao == 4)    //Estrangeiro não 1º ano
        propina = 15 * 1.2 * ects;
    return propina;
}

//Verifica estatuto de residente do estudante e se é 1º inscrição
int condicaoPropina(ALUNO * aluno, LISTA_PASTA * inscricao) {
    int opcao;
    int res=wcscmp(L"Portugal",aluno->pais);
    if(res == 0)
        opcao = 1;
    else
        opcao = 3;
    opcao += verificaInscricoesAnterioresAluno(aluno,obterAnoLetivoRecente(inscricao)->chave, inscricao);
    return opcao;
}


/************************************
 *            Report A              *
 ************************************/
//Gerar Report A
REP_A * gerarReportA(SGBD * bd){
    int i,j, ects;
    REP_A * reportA = criarListaReportA();              //Inicializa a lista do report A
    NO_PASTA * pastaCorrente = obterAnoLetivoRecente(bd->inscricoes);   //pasta do ano corrente
    NO * no = pastaCorrente->cauda;                     //No de cada inscrição na pasta do ano corrente
    ALUNO * aluno;
    for(i=0; i < pastaCorrente->elementos; i++){        //Gerar tantos nós conforme o número de alunos inscritos
        if(verificaElementoRepA(no->elemento->numeroAluno, reportA)){   //Verifica se o aluno já têm um nó no report
            no = no->proximo;
            continue;
        }
        aluno = obterAlunoNum(no->elemento->numeroAluno, bd->alunos);      //Obter aluno em questão para extrair dados necessários
        adicionarElementoRepA(criarElementoReportA(aluno->numero,aluno->nome,0,L"N/A"),reportA);    //Caso não exista é adicionado
        no = no->proximo;
    }
    No_REP_A * noReportA = reportA->cauda;  //Aponta para a cauda da lista do Report
    for(i=0; i<reportA->elementos; i++){    //Verifica para cada aluno, quantos ECTS têm
        ects=0;
        for(j=0; j< pastaCorrente->elementos; j++){
            if(noReportA->elemento->numero == no->elemento->numeroAluno)
                ects += obterECTS(obterUCNum(no->elemento->numeroUC, bd->ucs));   
            no = no->proximo;
        }
        if(ects > 60)
            adicionarDadoElementoRepA(noReportA->elemento, ects, L"Aguarda Validação"); //Se tiver mais de 60 ECTS então lança observação para os serviços académicos
        else
            adicionarDadoElementoRepA(noReportA->elemento, ects, L"N/A");
        noReportA = noReportA->proximo;
    }
    FILE * fp = criarReportA();     //abre ficheiro
    escreverReportA(reportA,fp);    //escreve no ficheiro os dados do report A
    terminarReportA(fp);            //fecha ficheiro
    return reportA;                 
}

//Imprimir reportA total ECTS por aluno no ano corrente 
void imprimirReportA(REP_A * reportA){
    clearScreen();
    No_REP_A * norep = reportA->cauda;
    int i;
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n|%17S|%24S|%10S|%24S|\n",L"Número de Aluno",L"Nome do Aluno",L"ECTS",L"Observações");
    for(i =0; i<80; i++)
        wprintf(L"-");
    wprintf(L"\n");
    for(i=0;i<reportA->elementos;i++){
        wprintf(L"|%17d|%24S|%10d|%24S|\n", norep->elemento->numero, norep->elemento->nome,norep->elemento->ects, norep->elemento->observacao);
        norep =  norep->proximo;
    }
    for(i =0; i<80; i++)
        wprintf(L"-");
}

/************************************
 *            Report B              *
 ************************************/
//Gerar Report B
void gerarReportB(SGBD * bd){
    LIST_ALUNO * alunosAux = bd->alunos;
    LISTA_PASTA * auxInscricoes = bd->inscricoes;
    FILE * reportFile = criarReportB();
    //Percorrer a lista de todos os alunos
    for(int i =0; i< alunosAux->elementos; i++){
        REP_B * report = criarListaReportB(); //Cria estrutura de report B
        ALUNO * aluno = obterAlunoPos(i,alunosAux);
        //Percorrer todos os anos letivos
        for(int p =0; p< auxInscricoes->pastas; p++){
            NO_PASTA * pasta= obterPastaPos(p,auxInscricoes);
            NO * no = pasta->cauda;
            //Percorrer todas as inscrições do ano
            for(int n = 0; n <pasta->elementos; n++){
                no = no->proximo;
                //Verificar se inscrição pertence ao aluno
                if(no->elemento->numeroAluno == aluno->numero){
                    //Pesquisa na estrutura para verificar se encontra o id da UC na estrutura
                    REP_B_ELEM * aux = obterElementoReportBNum(no->elemento->numeroUC, report);
                    if(aux != NULL){
                        //Caso o id da uc esteja na estrutura, verifica se a mesma já está concluida, caso não esteja atualiza estado com nova nota
                        if(aux->estado != 1)
                            modificarEstado(aux,no->elemento->nota);
                    } else
                        //Caso não esteja na estrutura adiciona a uc à estrutura
                        adicionarElementoRepBFim(report, criarElementoReportB(no->elemento->numeroUC, no->elemento->nota));
                }
            }
        }
        //Verifica se estrutura possui tantos elementos como unidades na bd
        if(report->quantidade == bd->ucs->elementos){
            int contador = 0;
            REP_B_ELEM * aux = report->cauda;
            //Soma todas o valor de estados da UC na estrutura.
            for(int e =0; e < report->quantidade; e++){
                aux = aux->proximo;
                contador += aux->estado;
            }
            //Se a soma for igual ou superior ao total de ucs-3 e não for a totalidade de UC escreve no ficheiro 
            if(contador >= bd->ucs->elementos - 3 && contador < bd->ucs->elementos){
                escreverLinhaReportB(aluno,contador, reportFile);
            }
        }
        libertarListaReportB(report);
    }
    terminarReportB(reportFile);
}

//Imprimir do Report B
void imprimirReportB(){
    clearScreen();
    FILE * fp = abrirLeituraReportB();
    wchar_t * linha = calloc(_TAMSTRING,sizeof(wchar_t));
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro a terminar.", _ERR_RESOURCENOTFOUND);
        exit(_ERR_RESOURCENOTFOUND);
    }
    for(int i=0;i<80;i++)
        wprintf(L"-");
    wprintf(L"\n|%25S|%25S|%26S|\n",L"Número de Aluno", L"Nome de Aluno", L"Total de UCs Realizadas");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    while(!feof(fp)){
        fwscanf(fp,L"%l[^\n]\n",linha);
        if(wmemcmp(linha,L"#",1)==0 || wcsncmp(linha,L" ",1)==0)
            continue;
        imprimirLinhaReportB(linha);
    }
    wprintf(L"\n");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    free(linha);
    linha = NULL;
    terminarLeituraReportB(fp);
}

//Imprimir linha Report B
void imprimirLinhaReportB(wchar_t * linha){
    wchar_t * temp, * buffer;
    int numeroAluno = wcstol(wcstok(linha, L";", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL,L";",&buffer);
    int totalUC = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    wprintf(L"\n|%25d|%25S|%26d|",numeroAluno,nome,totalUC);
}

/************************************
 *            Report C              *
 ************************************/
//criar report c
void gerarReportC(SGBD * bd){
    //Percorrer cada elemento da lista de alunos
    LIST_ALUNO * alunosAux = bd->alunos;
    FILE * reportFile = criarReportC();
    //Percorrer a lista de todos os alunos
    for(int i =0; i< alunosAux->elementos; i++){
        PROB_ABANDONO * report = criarListaReportC(); //Cria estrutura de report C
        ALUNO * aluno = obterAlunoPos(i,alunosAux);
        // obter pasta do ano letivo final
        NO_PASTA * pasta = obterAnoLetivoRecente(bd->inscricoes);
        NO * no = pasta->cauda;
        //percorrer pasta 
        for(int p =0; p < pasta->elementos; p++){
            no = no->proximo;
            //comparar se inscrição pertence ao aluno
            if(no->elemento->numeroAluno == aluno->numero){
                UC * temp = obterUCNum(no->elemento->numeroUC, bd->ucs);
                //se uc estiver em 1semestre então incrementa semestre1;
                if( temp->semestre == 1){
                    report->contador_semestre_1++;
                } else //se uc->2semestre entao incrementa semestre2;*/
                    report->contador_semestre_2++;
            }
        }
        //escreve no report c
        if(report->contador_semestre_2==0){
            if(report->contador_semestre_1>=2){
                escreverLinhaReportC(aluno, reportFile);
            }
        }
         
        libertarElementoReportC(report);
    }
    terminarReportC(reportFile);
}

//Imprimir do Report C
void imprimirReportC(){
    clearScreen();
    FILE * fp = abrirLeituraReportC();
    wchar_t * linha = calloc(_TAMSTRING,sizeof(wchar_t));
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro a terminar.", _ERR_RESOURCENOTFOUND);
        exit(_ERR_RESOURCENOTFOUND);
    }
    for(int i=0;i<80;i++)
        wprintf(L"-");
    wprintf(L"\n|%39S|%38S|\n",L"Número de Aluno", L"Nome de Aluno");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    while(!feof(fp)){
        fwscanf(fp,L"%l[^\n]\n",linha);
        if(wmemcmp(linha,L"#",1)==0 || wcsncmp(linha,L" ",1)==0)
            continue;
        imprimirLinhaReportC(linha);
    }
    wprintf(L"\n");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    free(linha);
    linha = NULL;
    terminarLeituraReportC(fp);
}

//Imprimir linha Report C
void imprimirLinhaReportC(wchar_t * linha){
    wchar_t * temp, * buffer;
    int numeroAluno = wcstol(wcstok(linha, L";", &buffer), &temp,10);
    wchar_t * nome = wcstok(NULL,L";",&buffer);
    wprintf(L"\n|%39d|%38S|",numeroAluno,nome);
}

/************************************
 *            Report D              *
 ************************************/
//Gerar Report D
void gerarReportD(SGBD *bd){
    //Obter ano letivo mais antigo e mais recente
    wchar_t * ano = obterAnoLetivoMaisAntigo(bd->inscricoes)->chave;
    wchar_t * anofinal = obterAnoLetivoRecente(bd->inscricoes)->chave;
    wchar_t * temp, *buffer;
    //Transformar ano letivo antigo em inteiro para depois poder incrementar
    wchar_t * anoAux = calloc(_TAMDATAS, sizeof(wchar_t));
    if(!anoAux){
        wprintf(L"Erro %d: Não foi possivel alocar memoriapara o report.", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wcscpy(anoAux, ano);
    int ano1 = wcstol(wcstok(anoAux,L"/", &buffer), &temp, 10);
    int ano2 = wcstol(wcstok(NULL,L"", &buffer), &temp, 10);
    wcscpy(anoAux, ano);
    //Cria ficheiro report D
    FILE * reportFile = criarReportD();
    //Cria estrutura para guardar a informação
    REP_D * report = criarListaReportD();
    //Percorre as pastas de anos letivos por ordem temporal
    NO_PASTA * pasta;
    while(wcscmp(anoAux, anofinal) <= 0 ){
        //obtem pasta referente ao ano letivo que se pretende verificar
        pasta = obterPastaAno(anoAux,bd->inscricoes);
        if(pasta != NULL){
            //Cria a lista do ano letivo
            REP_D_LISTA * infoAnoletivo = criarListaAnoReportD();
            //Percorre a pasta do ano letivo
            for(int p =0; p< pasta->elementos; p++){
                INSCRICAO * no = obterInscricao(p,pasta);
                //Verifica se o idAluno já está na lista, caso não esteja adiciona
                if(!obterElementoReportDNum(no->numeroAluno, infoAnoletivo)){
                    adicionarElementoAnoRepDFim(infoAnoletivo,criarElementoAnoReportD(no->numeroAluno));
                }
            }
            //Adiciona informação a estrutura do report D
            adicionarElementoRepDFim(report,criarElementoReportD(anoAux,infoAnoletivo->elementos));
            libertarListaAnoReportD(infoAnoletivo);
            pasta = NULL;
        }
        //Atualiza valor Ano Letivo
        swprintf(anoAux, wcslen(anoAux)+1, L"%d/%d", ++ano1,++ano2);
    }
    //Escrever informação no ficheiro
    REP_D_ELEM * aux = report->cauda;
    int ultimo;
    for(int i = 0; i< report->elementos; i++){
        //verifica se é o 1º ano de dados
        if(i==0){
            ultimo =0;
        } else{
            ultimo = aux->totalAlunos;
        }
        aux = aux->proximo;
        //Calculo de Percentagem
        int percentagem =0;
        if(ultimo ==0)
            percentagem = 100;//Como não existiu alunos anteriormente o aumento foi de 100%
        else
            //Regra de 3 simples para calcular a percentagem (subtraio os 100 do valor inicial [diferença entre o ano e o ano anterior])
            percentagem =((aux->totalAlunos *100) / ultimo)-100; 
        //Escrever no Relatório
        escreverLinhaReportD(aux->ano, aux->totalAlunos, percentagem, reportFile);
    }
    free(anoAux);
    libertarListaReportD(report);
    terminarReportD(reportFile);
}

//Imprimir do Report D
void imprimirReportD(){
    clearScreen();
    FILE * fp = abrirLeituraReportD();
    wchar_t * linha = calloc(_TAMSTRING,sizeof(wchar_t));
    if(!fp){
        wprintf(L"Erro %d: Não foi possivel encontrar o ficheiro a terminar.", _ERR_RESOURCENOTFOUND);
        exit(_ERR_RESOURCENOTFOUND);
    }
    for(int i=0;i<80;i++)
        wprintf(L"-");
    wprintf(L"\n|%25S|%25S|%26S|\n",L"Ano Letivo", L"Quantidade de Alunos",L"Variação");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    while(!feof(fp)){
        fwscanf(fp,L"%l[^\n]\n",linha);
        if(wmemcmp(linha,L"#",1)==0 || wcsncmp(linha,L" ",1)==0)
            continue;
        imprimirLinhaReportD(linha);
    }
    wprintf(L"\n");
    for(int i=0;i<80;i++)
        wprintf(L"-");
    free(linha);
    linha = NULL;
    terminarLeituraReportD(fp);
}

//Imprimir linha Report D
void imprimirLinhaReportD(wchar_t * linha){
    wchar_t * temp, * buffer;
    wchar_t * anoLetivo = wcstok(linha, L";", &buffer);
    int quantidade = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    int variacao = wcstol(wcstok(NULL, L";", &buffer), &temp,10);
    wprintf(L"\n|%25S|%25d|%26d|",anoLetivo,quantidade,variacao);
}
