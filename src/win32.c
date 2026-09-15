/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef _WIN32

#include "win32.h"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

static wchar_t *utf8_to_wide(const char *str) {
    if (!str) return NULL;

    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, NULL, 0);
    if (len == 0) return NULL;

    wchar_t *wstr = malloc(sizeof(wchar_t) * len);
    if (!wstr) return NULL;

    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, wstr, len) == 0) {
        free(wstr);
        return NULL;
    }

    return wstr;
}

FILE *win32_fopen(const char *filename, const char *mode) {
    wchar_t *wfilename = utf8_to_wide(filename);
    wchar_t *wmode = utf8_to_wide(mode);

    FILE *file = NULL;
    if (wfilename && wmode) {
        file = _wfopen(wfilename, wmode);
    }

    free(wmode);
    free(wfilename);

    return file;
}

void win32_console_enable_utf8(void) {
    SetConsoleOutputCP(CP_UTF8);
}

char **win32_argv_fetch(void) {
    int argc;
    char **argv;

    wchar_t **wargv;

    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) return NULL;

    argv = calloc(argc + 1, sizeof(*argv));
    if (!argv) goto cleanup_wargv;

    for (int i = 0; i < argc; i++) {
        int arg_len = WideCharToMultiByte(
            CP_UTF8, WC_ERR_INVALID_CHARS, wargv[i], -1, NULL, 0, NULL, NULL
        );
        if (arg_len == 0) goto cleanup_argv;

        argv[i] = malloc(sizeof(**argv) * arg_len);
        if (!argv[i]) goto cleanup_argv;

        if (WideCharToMultiByte(
            CP_UTF8, WC_ERR_INVALID_CHARS, wargv[i], -1, argv[i], arg_len, NULL, NULL
        ) == 0) {
            goto cleanup_argv;
        }
    }

    LocalFree(wargv);

    return argv;

cleanup_argv:
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

cleanup_wargv:
    LocalFree(wargv);

    return NULL;
}

void win32_argv_free(char **argv) {
    if (!argv) return;

    for (int i = 0; argv[i]; i++) {
        free(argv[i]);
    }
    free(argv);
}

#endif // _WIN32
