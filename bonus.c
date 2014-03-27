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
#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

#include <omp.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include "bonus.h"

void InitPilha(pilha *p){
    int i;
    p->topo = -1;
    for(i=0; i<VKB; i++){
        p->ind[i] = -1;
    }
    for(i=0; i<VKB; i++){
        p->v[i] = '\0';
    }
}

void InitPilha2(pilha2 *p){
    int i, j;
    p->topo = -1;
    #pragma omp parallel for schedule(guided)
    for(i=0; i<VKB; i++){
        p->ind[i] = -1;
    }
    #pragma omp parallel for schedule(guided)
    for(i=0; i<VKB; i++){
        #pragma omp parallel for schedule(guided)
        for(j=0; j<16; j++){
            p->vt[i][j] = '\0';
        }
    }
}

void InsrtPilha(pilha *p, char novo){
    if(p->topo==VKB-1){
        fprintf(stderr, "Erro: Pilha Cheia!\n");
        exit(EXIT_FAILURE);
    }
    p->topo++;
    p->v[p->topo] = novo;
}

void InsrtPilha2(pilha2 *p, char novo[]){
    if(p->topo==VKB-1){
        fprintf(stderr, "Erro: Pilha Cheia!\n");
        exit(EXIT_FAILURE);
    }
    p->topo++;
    strcpy(p->vt[p->topo], novo);
}

char RemovePilha(pilha *p){
    if(p->topo == -1){
        fprintf(stderr, "Erro: Pilha Vazia!\n");
        exit(EXIT_FAILURE);
    }
    p->topo--;
    return p->v[p->topo+1];
}

char *RemovePilha2(pilha2 *p){
    if(p->topo == -1){
        fprintf(stderr, "Erro: Pilha Vazia!\n");
        exit(EXIT_FAILURE);
    }
    p->topo--;
    return p->vt[p->topo+1];
}

void limpaVetor(char *vet){
    int i, tam = sizeof(vet);

    if(tam>0){
        #pragma omp parallel for schedule(guided)
        for(i=0; i<tam; i++){
            vet[i] = '\0';
        }
    }
}

static size_t strnlen(const char *s, size_t max) {
    register const char *p;
    for(p = s; *p && max--; ++p);
    return(p - s);
}
int inet_pton4(const char *src, char *dst){
    uint8_t tmp[NS_INADDRSZ], *tp;

    int saw_digit = 0;
    int octets = 0;
    *(tp = tmp) = 0;

    int ch;
    while ((ch = *src++) != '\0'){
        if (ch >= '0' && ch <= '9')
        {
            uint32_t n = *tp * 10 + (ch - '0');

            if (saw_digit && *tp == 0)
                return 0;

            if (n > 255)
                return 0;

            *tp = n;
            if (!saw_digit)
            {
                if (++octets > 4)
                    return 0;
                saw_digit = 1;
            }
        }
        else if (ch == '.' && saw_digit)
        {
            if (octets == 4)
                return 0;
            *++tp = 0;
            saw_digit = 0;
        }
        else
            return 0;
    }
    if (octets < 4)
        return 0;

    memcpy(dst, tmp, NS_INADDRSZ);

    return 1;
}
int inet_pton6(const char *src, char *dst){
    static const char xdigits[] = "0123456789abcdef";
    uint8_t tmp[NS_IN6ADDRSZ];

    uint8_t *tp = (uint8_t*) memset(tmp, '\0', NS_IN6ADDRSZ);
    uint8_t *endp = tp + NS_IN6ADDRSZ;
    uint8_t *colonp = NULL;

    /* Leading :: requires some special handling. */
    if (*src == ':'){
        if (*++src != ':')
            return 0;
    }

    const char *curtok = src;
    int saw_xdigit = 0;
    uint32_t val = 0;
    int ch, i;
    while ((ch = tolower(*src++)) != '\0'){
        const char *pch = strchr(xdigits, ch);
        if (pch != NULL){
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return 0;
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':'){
            curtok = src;
            if (!saw_xdigit){
                if (colonp)
                    return 0;
                colonp = tp;
                continue;
            }
            else if (*src == '\0'){
                return 0;
            }
            if (tp + NS_INT16SZ > endp)
                return 0;
            *tp++ = (uint8_t) (val >> 8) & 0xff;
            *tp++ = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) && inet_pton4(curtok, (char*) tp) > 0){
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break; /* '\0' was seen by inet_pton4(). */
        }
        return 0;
    }
    if (saw_xdigit){
        if (tp + NS_INT16SZ > endp)
            return 0;
        *tp++ = (uint8_t) (val >> 8) & 0xff;
        *tp++ = (uint8_t) val & 0xff;
    }
    if (colonp != NULL){
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;

        if (tp == endp)
            return 0;

        for (i = 1; i <= n; i++){
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return 0;

    memcpy(dst, tmp, NS_IN6ADDRSZ);

    return 1;
}
int inet_pton(int af, const char *src, char *dst){
    switch (af){
    case AF_INET:
        return inet_pton4(src, dst);
    case AF_INET6:
        return inet_pton6(src, dst);
    default:
        return -1;
    }
}

long TamFile(FILE *arq){
    long pos = ftell(arq);
    fseek(arq, 0, SEEK_END);
    long tam = ftell(arq);
    fseek(arq, pos, SEEK_SET);
    return tam;
}
