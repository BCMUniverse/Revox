#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

#ifdef BUILD_DLL
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __declspec(dllimport)
#endif

#define CALDLL __cdecl

#ifdef __cplusplus
extern "C"{
#endif

#include "bonus.h"
//Biblioteca HTMLText
#include "htmlText.h"
//Biblioteca HTTP
#include "http.h"
//Biblioteca Socket
#include "socket.h"

//Inicia o Modo GUI
void InitGumbo(char *host);
//Inicia o Modo Texto
void InitText(char *host);
//Ler um arquivo HTML
void InitFileText(char *path);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
