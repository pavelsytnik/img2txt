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

static bool option_matches(const char *arg, const char *name) {
    arg += 2;

    while (*arg && *name && *arg != '=') {
        if (*arg++ != *name++) {
            return false;
        }
    }

    return *name == '\0' && (*arg == '\0' || *arg == '=');
}

static const struct args_option *args_option_find(
    const struct args_option *opts,
    const char *arg
) {
    for (int i = 0; opts[i].key; i++) {
        if (option_matches(arg, opts[i].name)) {
            return &opts[i];
        }
    }

    return NULL;
}

void args_parse(const struct args_program *program, int argc, char const *const *argv) {
    program->parser(ARGS_KEY_INIT, NULL, program->data);

    for (int i = 1; i < argc; i++) {

        if (!strncmp(argv[i], "--", 2)) {
            const char *name = argv[i] + 2;
            const char *eq = strchr(name, '=');

            size_t name_len = eq ? (size_t)(eq - name) : strlen(name);

            const struct args_option *opt = args_option_find(program->options, argv[i]);
            if (!opt) {
                error("Unrecognized option '%.*s'", name_len, name);
            }

            const char *arg = NULL;

            if (opt->arg) {
                arg = (eq) ? eq + 1 : argv[++i];
                if (!arg) {
                    error("Option '%.*s' requires an argument", name_len, name);
                }
            }

            program->parser(opt->key, arg, program->data);
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
