#ifdef _WIN32

#include "win32.h"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

static wchar_t *utf8_to_wide(const char *str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);

    wchar_t *wstr = malloc(sizeof(wchar_t) * len);

    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);

    return wstr;
}

FILE *win32_fopen(const char *filename, const char *mode) {
    wchar_t *wfilename = utf8_to_wide(filename);
    wchar_t *wmode = utf8_to_wide(mode);

    FILE *file = _wfopen(wfilename, wmode);

    free(wmode);
    free(wfilename);

    return file;
}

void win32_console_enable_utf8(void) {
    SetConsoleOutputCP(CP_UTF8);
}

void win32_args_fetch(int *out_argc, char ***out_argv) {
    int argc;
    char **argv;

    wchar_t **wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

    argv = malloc(sizeof(char *) * (argc + 1));

    argv[argc] = NULL;

    for (int i = 0; i < argc; i++) {
        int arg_size = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);

        argv[i] = malloc(arg_size);

        WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], arg_size, NULL, NULL);
    }

    LocalFree(wargv);

    *out_argc = argc;
    *out_argv = argv;
}

void win32_args_free(int *out_argc, char ***out_argv) {
    int argc = *out_argc;
    char **argv = *out_argv;

    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

    *out_argc = 0;
    *out_argv = NULL;
}

#endif // _WIN32
