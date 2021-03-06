/*
 * Ficheiro que possui todas as funções globais da UI (User Interface)
 */

#ifndef _GLOBAL_
#define _GLOBAL_

//Bibliotecas Padrão
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <err.h>
#include <fcntl.h>	
#include <unistd.h>	
#include <string.h>


//verificar tipo de sistema operativo para aplicar o comando correcto para limpar ecrã.
//Windows - "cls" | Unix - "clear"
#ifdef _WIN32
#define clearScreen() system("cls")
#else
#define clearScreen() system("clear")
#endif


/********************************
 *          Headers             *
 ********************************/

//cabeçalho dos menus, recebe string da designação do menu
void cabecalho(char *str);
//função verifica largura da tela
int largTela();
//função para imprimir no centro da tela
void imprimirMeio(char *str);
//Função aguarda que utilizador pressione ENTER
void pressioneENTER();

#endif //_GLOBAL_