/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geru GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.1
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
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
