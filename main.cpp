/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geral GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2013, 2014|Tipo: WebEngine
*/
#include <stdio.h>
#include <windef.h>
#include "bonus.h"
#include "revox.h"

void InitMsg(){
    printf("Inicializando Revox...\r\n");
	printf("Copyright (C) 2013, 2014 Cleber Matheus\r\n\r\n");
	printf("This program is free software: you can redistribute it and/or modify\r\nit under the terms of the GNU General Public License as published by\r\nthe Free Software Foundation, either version 3 of the License, or\r\n(at your option) any later version.\r\n\r\n");
	printf("This program is distributed in the hope that it will be useful,\r\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\r\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\r\nGNU General Public License for more details.\r\n\r\n");
	printf("You should have received a copy of the GNU General Public License\r\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\r\nContato: <clebermatheus@outlook.com> (Cleber Matheus)\r\n");
}

void InitText(char *host){
    InitMsg();
    InitTypeParser(UrlConnect(host, 1, NULL, NULL), 1);
}
void InitFileText(char *path){
    char content[BUF32KB];
    FILE *input;
    int i;
    InitMsg();
    if((input = fopen(path, "r"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\n");
        return;
    }
    for(i=0; content[i]!='\0'; i++){
        fscanf(input, "%c", &content[i]);
    }
    htmlParser(content, path);
    fclose(input);
}

extern "C" BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    switch (fdwReason){
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
