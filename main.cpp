#include <stdio.h>
#include <windef.h>
#include "html.h"
#include "parser.h"
#include "revox.h"
#include "bonus.h"

void InitGumbo(char *host){
    char *html = InitHTTP(host, 80, "/", NULL);
    Output* output = parse(html);
    destroy_output(&kDefaultOptions, output);
}
void InitText(char *host){
    char *content = InitHTTP(host, 80, "/", NULL);
    InitHTMLText(content);
}
void InitFileText(char *path){
    char *content = (char *)malloc(sizeof(char)*BUF32KB);
    FILE *input;
    if((input = fopen(path, "r"))==NULL){
        fprintf(stderr, "Erro: Arquivo Invalido!\n");
        return;
    }
    fscanf(input, "%s", &content);
    InitHTMLText(content);
    free(content);
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
