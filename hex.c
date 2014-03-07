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
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/
#include <stdio.h>

char *HexCreater(char str[]){
    char hexReslt[2*strlen(str)], hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int i, j, hexp, hexp1, hexc, hexc1;

    for(i=0, j=0; str[i]!='\n' && str[i]!='\0'; i++, j+=2){
        hexc = str[i]/16;
        hexc1 = str[i]%16;
        hexp = hex[hexc];
        hexp1 = hex[hexc1];
        hexReslt[j] = hexp;
        hexReslt[j+1] = hexp1;
    }

    return hexReslt;
}

char *HexDecoder(char HexStr[]){
    char strReslt[strlen(HexStr)];
    int i, j, hexp;

    for(i=0, j=0; HexStr[i]!='\n' && HexStr[i]!='\0'; i+=2, j++){
        switch(HexStr[i]){
        case '0':
            HexStr[i] = 0;
            break;
        case '1':
            HexStr[i] = 1;
            break;
        case '2':
            HexStr[i] = 2;
            break;
        case '3':
            HexStr[i] = 3;
            break;
        case '4':
            HexStr[i] = 4;
            break;
        case '5':
            HexStr[i] = 5;
            break;
        case '6':
            HexStr[i] = 6;
            break;
        case '7':
            HexStr[i] = 7;
            break;
        case '8':
            HexStr[i] = 8;
            break;
        case '9':
            HexStr[i] = 9;
            break;
        case 'A':
            HexStr[i] = 10;
            break;
        case 'B':
            HexStr[i] = 11;
            break;
        case 'C':
            HexStr[i] = 12;
            break;
        case 'D':
            HexStr[i] = 13;
            break;
        case 'E':
            HexStr[i] = 14;
            break;
        case 'F':
            HexStr[i] = 15;
            break;
        }
        switch(HexStr[i+1]){
        case '0':
            HexStr[i+1] = 0;
            break;
        case '1':
            HexStr[i+1] = 1;
            break;
        case '2':
            HexStr[i+1] = 2;
            break;
        case '3':
            HexStr[i+1] = 3;
            break;
        case '4':
            HexStr[i+1] = 4;
            break;
        case '5':
            HexStr[i+1] = 5;
            break;
        case '6':
            HexStr[i+1] = 6;
            break;
        case '7':
            HexStr[i+1] = 7;
            break;
        case '8':
            HexStr[i+1] = 8;
            break;
        case '9':
            HexStr[i+1] = 9;
            break;
        case 'A':
            HexStr[i+1] = 10;
            break;
        case 'B':
            HexStr[i+1] = 11;
            break;
        case 'C':
            HexStr[i+1] = 12;
            break;
        case 'D':
            HexStr[i+1] = 13;
            break;
        case 'E':
            HexStr[i+1] = 14;
            break;
        case 'F':
            HexStr[i+1] = 15;
            break;
        }
        hexp = (HexStr[i]*16)+HexStr[i+1];
        strReslt[j] = hexp;
    }

    return strReslt;
}
