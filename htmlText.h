#ifndef _HTMLTEXT_H_
#define _HTMLTEXT_H_

//Em Breve
typedef struct Parsers;

//Cria Elementos para compara-los
char *CreateTag(char *inicio, char *tag, char *fim);
//Copia Substrings dentro de um string
char *SubString(char *str, int inicio, int fim);
//Copia Varios Elemento dentro de uma string retorna-as
char *SubStringP(char *str, char *elem);
//Copia várias substrings e remove-as dentro de uma string
char *SubString2(char *str, char *inicio, char *fim);
//Analisa documento HTML e converte em Texto
char *InitHTMLText(char *content);

#endif // _HTMLTEXT_H_
