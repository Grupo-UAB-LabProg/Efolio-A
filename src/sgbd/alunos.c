/*
 * Ficheiro que possui todas as funções que gerem a lista de alunos
 */

#include "alunos.h"

//Criar Aluno com dados
//Retorna Aluno
ALUNO *criarAluno(int numero, wchar_t *nome, wchar_t *pais){
    ALUNO *aluno = calloc(1, sizeof(ALUNO));
    if (!aluno){
        wprintf(L"Erro %d: Impossível alocar memória para Aluno", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    aluno->numero = numero;
    aluno->nome = calloc(_TAMSTRING, sizeof(wchar_t));
    if (!aluno->nome){
        wprintf(L"Erro %d: Impossível alocar memória para Aluno", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wcsncpy(aluno->nome,nome,wcslen(nome));
    aluno->pais = calloc(_TAMSTRING, sizeof(wchar_t));
    if (!aluno->pais){
        wprintf(L"Erro %d: Impossível alocar memória para Aluno", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    wcsncpy(aluno->pais,pais,wcslen(pais));
    aluno->propina = 0;
    return aluno;
}

//Criar lista vazia
LIST_ALUNO * criarListaAluno(){
    LIST_ALUNO *lista = calloc(1, sizeof(LIST_ALUNO));
    if (!lista){
        wprintf(L"Erro %d: Impossível alocar memória para Aluno", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    lista->elementos = 0;
    lista->cauda = NULL;
    return lista;
}

//Libertar Memoria Aluno
int libertarAluno(ALUNO *aluno){
    if (aluno){
        if (aluno->nome)
            free(aluno->nome);
        aluno->nome = NULL;
        if (aluno->pais)
            free(aluno->pais);
        aluno->pais = NULL;
        aluno->propina=0;
        free(aluno);
        aluno = NULL;
        return _SUCESSO;
    }
    return _ERR_MEMORYFREE;
}

//Eliminar Lista
int libertarListaAluno(LIST_ALUNO *lista){
    if (!lista)
        return _ERR_MEMORYFREE;
    while (lista->elementos != 0){
        removerAluno(0, lista);
    }
    lista->cauda = NULL;
    free(lista);
    lista = NULL;
    return _SUCESSO;
}

//Criar No Lista Aluno
NoALUNO *criarNoAluno(ALUNO *aluno){
    NoALUNO *novo = calloc(1, sizeof(NoALUNO));
    if (!novo){
        wprintf(L"Erro %d: Impossível alocar memória para Aluno", _ERR_MEMORYALLOC);
        exit(_ERR_MEMORYALLOC);
    }
    novo->elemento = aluno;
    novo->proximo = novo;
    return novo;
}

//Libertar No Aluno
int libertarNoAluno(NoALUNO *no){
    if (!no)
        return _ERR_MEMORYFREE;
    libertarAluno(no->elemento);
    no->proximo = NULL;
    free(no);
    no = NULL;
    return _SUCESSO;
}

//Adicionar elemento na lista
int adicionarAluno(ALUNO * aluno, LIST_ALUNO *lista, int pos){
    if (!lista || !aluno){
        wprintf(L"Erro %d: Impossível adicionar elemento na lista", _ERR_EMPTYLIST);
        return _ERR_EMPTYLIST;
    }
    if (pos < 0 || pos > lista->elementos){
        wprintf(L"Erro %d: Possição inválida na lista", _ERR_IMPOSSIBLE);
        return _ERR_IMPOSSIBLE;
    }
    NoALUNO *no = criarNoAluno(aluno);
    if (lista->elementos == 0){ //Lista vazia adiciona no inicio
        lista->cauda = no;
        lista->elementos++;
    } else if (pos == lista->elementos){   // Adiciona no final
        no->proximo = lista->cauda->proximo; //Aponta novo elemento para o inicio da lista
        lista->cauda->proximo = no;          //Aponta ultimo elemento atual da lista para o novo elemento
        lista->cauda = no;                   //Atualiza o ultimo elemento
        lista->elementos++;
    } else{ //Adiciona na posição;
        NoALUNO *temp = lista->cauda->proximo;
        for (int i = 0; i < pos; i++)
            temp = temp->proximo;
        no->proximo = temp->proximo; //Aponta novo elemento para o elemento seguinte da posição da lista
        temp->proximo = no;          //Aponta posicao anterior para o novo elemento
        lista->elementos++;
    }
    return _SUCESSO;
}

//Remover elemento da lista
int removerAluno(int pos, LIST_ALUNO *lista){
    if (!lista){
        wprintf(L"Erro %d: Impossível adicionar elemento na lista", _ERR_EMPTYLIST);
        return _ERR_EMPTYLIST;
    }
    if (pos < 0 || pos >= lista->elementos){
        wprintf(L"Erro %d: Possição inválida na lista", _ERR_IMPOSSIBLE);
        return _ERR_IMPOSSIBLE;
    }
    if (pos == 0){ //Remover na cabeça da lista
        NoALUNO *temp = lista->cauda->proximo;
        lista->cauda->proximo = temp->proximo; //Aponta final da lista para segundo elemento da lista
        lista->elementos--;
        libertarNoAluno(temp);
    } else { //Remove na posição
        NoALUNO *temp = lista->cauda->proximo;
        for (int i = 0; i < pos-1; i++)
            temp = temp->proximo;
        NoALUNO *aux = temp->proximo;
        temp->proximo = aux->proximo; //Aponta elemento atual para o segundo elemento seguinte da posição da lista
        if (pos == lista->elementos - 1)
            lista->cauda = temp; //Atualiza cauda caso seja o ultimo elemento a remover
        lista->elementos--;
        libertarNoAluno(aux);
    }
    return _SUCESSO;
}

//Procurar na lista de alunos 
ALUNO * obterAlunoNum(int numeroAluno, LIST_ALUNO * lista) { //recebe numeroAluno e bd->alunos que é do tipo LIST_ALUNO
    NoALUNO * tmp;  //ponteiro para percorrer lista temporariamente
    int i = 0;
    if(!lista)
        return NULL;
    tmp = lista->cauda;
    while (i < lista->elementos && tmp->elemento->numero != numeroAluno) {
        tmp = tmp->proximo;
        i++;
    }
    if (tmp->elemento->numero == numeroAluno)
        return tmp->elemento;
    else
        return NULL;
}

//Obter Aluno na posição
ALUNO * obterAlunoPos(int pos, LIST_ALUNO * lista){
    if(!lista){
        wprintf(L"Erro %d: Lista vazia", _ERR_EMPTYLIST);
        return NULL;
    }
    if(pos < 0 || pos >lista->elementos){
        wprintf(L"Erro %d: Possição inválida na lista", _ERR_IMPOSSIBLE);
        return NULL;
    }
    if(pos == lista->elementos-1){
        return lista->cauda->elemento;
    } else{
        int i =0;
        NoALUNO * temp = lista->cauda;
        while(i<=pos){
            temp = temp->proximo;
            i++;
        }
        return temp->elemento;
    }
}

//Modificar Valores Aluno
void modificarValoresAluno(int numero, wchar_t *nome, wchar_t *pais, ALUNO * elem){
    if(numero)
        elem->numero= numero;
    if(nome)
        wcsncpy(elem->nome,nome,wcslen(nome));
    if(pais)
        wcsncpy(elem->pais,pais,wcslen(pais));
}