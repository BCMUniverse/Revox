#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bonus.h"

#define KB 1024
#pragma omp parallel

typedef struct{
    char aux[32], aux2[32], html[BUF32KB], title[1024], head[10240], *body;
}Parsers;

char *CreateTag(char *inicio, char *tag, char *fim){
    char *element = (char *)malloc(sizeof(char)*(32));

    strcpy(element, inicio);
    strcat(element, tag);
    strcat(element, fim);

    return element;
}

char *SubString(char *str, int inicio, int fim){
    char *sub;
    int i, j;

    if(inicio >= fim || fim>strlen(str)){
        fprintf(stderr, "Erro! SubString nao Encontrada!\n");
        return "\0";
    }
    sub = (char *)malloc(sizeof(char)*(fim-inicio+1));
    for(i=inicio, j=0; i<fim; i++, j++){
        sub[j] = str[i];
    }
    sub[j] = '\0';

    return sub;
}

char *SubStringP(char *str, char *elem){
    int i, j, k, n, q, r, s, t;
    char *result, l, *m, *o;
    pilha p;

    InitPilha(&p);
    result = malloc(sizeof(char)*(strlen(str)));
    m = malloc(sizeof(char)*(strlen(str)));
    o = malloc(sizeof(char)*(strlen(str)));
    for(i=0; result[i]<strlen(str)+1; i++){
        result[i] = '\0';
    }
    for(t=0; str[t]!='\0'; t++);
    for(i=0; str[i]!='\0'; i++){
        if(str[i]=='<' && str[i+1]!='/'){
            for(i=i, j=0; str[i]!='>'; i++){
                InsrtPilha(&p, str[i]);
                p.ind[j++] = i;
            }
            if(str[i]=='>'){
                InsrtPilha(&p, str[i]);
                p.ind[j++] = i;
            }
            if(strstr(p.v, elem)!=NULL || strstr(p.v, strupr(elem))!=NULL){
                l = strstr(p.v, elem);
                if(l==NULL){
                    l = strstr(p.v, strupr(elem));
                }
                m = p.ind[l];
                n = m-str;
            }
            r = p.topo;
            for(j=0; j<VKB; j++){
                RemovePilha(&p);
            }
        }
        if(str[i]=='<' && str[i+1]=='/'){
            for(i=i, j=0; str[i]!='>'; i++){
                InsrtPilha(&p, str[i]);
                p.ind[j++] = i;
            }
            InsrtPilha(&p, str[i]);
            p.ind[j++] = i;
            if(strstr(p.v, elem)!=NULL || strstr(p.v, strupr(elem))!=NULL){
                l = strstr(p.v, elem);
                if(l==NULL){
                    l = strstr(p.v, strupr(elem));
                }
                o = p.ind[l];
                q = o-str;
            }
            s = p.topo;
            for(j=0; j<VKB; j++){
                RemovePilha(&p);
            }
        }
        if(i=0){
            result = SubString(str, n+r, q-s);
        }
        else{
            strcat(result, SubString(str, n+r, q-s));
            strcat(result, "\r\n");
        }
        str = SubString(str, q+s, t);
    }
    free(m);
    free(o);

    return result;
}

char *SubString2(char *str, char *inicio, char *fim){
    int i, j, k, l = 0, m1, n1;
    char *result, *m, *n;
    result = malloc(sizeof(char)*(strlen(str)));
    m = malloc(sizeof(char)*(strlen(str)));
    n = malloc(sizeof(char)*(strlen(str)));

    for(i=0; result[i]!='\0'; i++){
        result[i] = '\0';
    }
    for(j=0; str[j]!='\0'; j++);
    for(k=0; inicio[k]!='\0'; k++);
    for(l=0; fim[l]!='\0'; l++);
    for(i=0; strlen(str)!=0; i++){
        m = strstr(str, inicio);
        n = strstr(str, fim);
        if(m==NULL){
            m = strstr(str, strupr(inicio));
        }
        if(n==NULL){
            n = strstr(str, strupr(fim));
        }
        m1 = m-str;
        n1 = n-str;
        if(m1<0){
            m1 = 0;
        }
        if(n1<0){
            n1 = 0;
        }
        if(i=0){
            result = SubString(str, m1+(k+1), n1-(l/8));
        }
        else{
            strcat(result, SubString(str, m1+(k+1), n1-(l/8)));
            strcat(result, "\r\n");
        }
        //strcpy(str, m);
        str = SubString(str, n1+l, j);
    }
    free(m);
    free(n);

    return result;
}

char *InitHTMLText(char *content){
    int i, j, k, l, inicio, fim;
    char tags[][16] = {"html\0", "head\0", "title\0", "body\0", "p\0"};
    char cesp[][4] = {"<\0", ">\0", "</\0", "/>\0"};
    char *conthtml, *aux, *aux2, *intag, *intag2, html[BUF32KB], *title, *head, *body;

    //Alocando Memória
    conthtml = (char *)malloc(sizeof(char)*(strlen(content)));
    intag = (char *)malloc(sizeof(char)*(strlen(content)));
    intag2 = (char *)malloc(sizeof(char)*(strlen(content)));
    head = (char *)malloc(sizeof(char)*(strlen(content)));
    title = (char *)malloc(sizeof(char)*2048);
    body = (char *)malloc(sizeof(char)*(strlen(content)));

    #pragma omp parallel for schedule(guided)
    #pragma omp parallel private(conthtml, intag, intag2, head, title, body)
    for(i=0; i<5; i++){
        if(i==0 || i==3){
            // Tag <html e <body
            aux = CreateTag(cesp[0], tags[i], "\0");
            // Tag </html> e </body>
            aux2 = CreateTag(cesp[2], tags[i], cesp[1]);

            for(k=0; aux[k]!='\0'; k++);
            for(l=0; aux2[l]!='\0'; l++);

            intag = strstr(content, aux);
            intag2 = strstr(content, aux2);
            if(intag==NULL){
                intag = strstr(content, strupr(aux));
            }
            if(intag2==NULL){
                intag2 = strstr(content, strupr(aux2));
            }
            inicio = intag-content;
            fim = intag2-content;
            if(inicio<0){
                inicio = 0;
            }
            if(i==0){
                conthtml = SubString(content, inicio+(k+1), fim-(l/8));
            }
            else{
                body = SubString(content, inicio, fim);
            }
        }
        if(i==1 || i==2){
            // Tag <head> e <title>
            aux = CreateTag(cesp[0], tags[i], cesp[1]);
            // Tag </head> e </title>
            aux2 = CreateTag(cesp[2], tags[i], cesp[1]);

            for(k=0; aux[k]!='\0'; k++);
            for(l=0; aux2[l]!='\0'; l++);

            if(i==1){
                intag = strstr(content, aux);
                intag2 = strstr(content, aux2);
                if(intag==NULL){
                    intag = strstr(content, strupr(aux));
                }
                if(intag2==NULL){
                    intag2 = strstr(content, strupr(aux2));
                }
                inicio = intag-content;
                fim = intag2-content;
                if(inicio<0){
                    inicio = 0;
                }
                head = SubString(content, inicio, fim);
            }
            else{
                intag = strstr(head, aux);
                intag2 = strstr(head, aux2);
                if(intag==NULL){
                    intag = strstr(head, strupr(aux));
                }
                if(intag2==NULL){
                    intag2 = strstr(head, strupr(aux2));
                }
                inicio = intag-head;
                fim = intag2-head;
                if(inicio<0){
                    inicio = 0;
                }
                title = SubString(head, inicio+k, fim-(l/8)+1);
                strcpy(html, "Titulo: ");
                strcat(html, title);
                strcat(html, "\r\n");
            }
        }
        if(i==4){
            // Tag <p...> e </p>
            conthtml = SubStringP(body, tags[i]);
            strcat(html, conthtml);
            strcat(html, "\r\n");
        }
    }

    printf("%s", html);

    //Liberando Memória Alocada
    free(conthtml);
    free(intag);
    free(intag2);
    free(head);
    free(title);
    free(body);

    //Adicionando Null aos ponteiros
    conthtml = NULL;
    intag = NULL;
    intag2 = NULL;
    head = NULL;
    title = NULL;
    body = NULL;

    return html;
}
