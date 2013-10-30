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
