#ifndef BONUS_H_
#define BONUS_H_

//Buffer de saída de 512KB
#define BUF32KB 524288
//Tamanho do Vetor p.v
#define VKB 256

//Pilha
typedef struct{
    char v[VKB];
    int topo, ind[VKB];
}pilha;
void InitPilha(pilha *p);
void InsrtPilha(pilha *p, char novo);
char RemovePilha(pilha *p);

static size_t strnlen(const char *s, size_t max);
int inet_pton4(const char *src, char *dst);
int inet_pton6(const char *src, char *dst);
int inet_pton(int af, const char *src, char *dst);

#endif // BONUS_H_
