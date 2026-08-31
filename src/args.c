/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"

#include "util.h"

#ifdef ARGS_PLATFORM_WINDOWS
#  include <windows.h>
#  include <wchar.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void args_parse(const struct args_program *program, int argc, char const *const *argv) {
    program->parser(ARGS_KEY_INIT, NULL, program->data);

    for (int i = 1; i < argc; i++) {

        if (!strncmp(argv[i], "--", 2)) {
            bool found = false;

            const char *name = argv[i] + 2;
            const char *eq = strchr(name, '=');

            size_t name_len = eq ? (size_t)(eq - name) : strlen(name);

            for (const struct args_option *opt = program->options; opt->key; opt++) {

                if (strlen(opt->name) == name_len && !strncmp(opt->name, name, name_len)) {
                    const char *arg = opt->arg ? (eq ? eq + 1 : argv[++i]) : NULL;

                    if (opt->arg && !arg) {
                        error("Option '%.*s' requires an argument", name_len, name);
                    }

                    program->parser(opt->key, arg, program->data);

                    found = true;
                    break;
                }
            }

            if (!found) {
                error("Unrecognized option '%.*s'", name_len, name);
            }
        } else {
            program->parser(ARGS_KEY_ARG, argv[i], program->data);
        }
    }

    program->parser(ARGS_KEY_END, NULL, program->data);
}

#ifdef ARGS_PLATFORM_WINDOWS

void args_windows_args_fetch(int *out_argc, char ***out_argv) {
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

void args_windows_args_free(int *out_argc, char ***out_argv) {
    int argc = *out_argc;
    char **argv = *out_argv;

    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

    *out_argc = 0;
    *out_argv = NULL;
}

#endif // ARGS_PLATFORM_WINDOWS
