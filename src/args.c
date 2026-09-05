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

struct args_parser {
    const struct args_program *program;
    const char *const *argv;
    int argc;
    int argi;
    int subopt;
};

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

static const struct args_option *args_option_find_by_key(
    const struct args_option *opts,
    int key
) {
    for (int i = 0; opts[i].key; i++) {
        if (opts[i].key == key) {
            return &opts[i];
        }
    }

    return NULL;
}

static void args_parser_parse_longopt(struct args_parser *parser) {
    const char *name = parser->argv[parser->argi] + 2;
    const char *eq = strchr(name, '=');

    size_t name_len = (eq) ? (size_t)(eq - name) : strlen(name);

    const struct args_option *opt = args_option_find(
        parser->program->options,
        parser->argv[parser->argi]
    );

    if (!opt) {
        error("Unrecognized option '%.*s'", name_len, name);
    }

    const char *arg = NULL;

    if (opt->arg) {
        arg = (eq) ? eq + 1 : parser->argv[++parser->argi];
        if (!arg) {
            error("Option '%.*s' requires an argument", name_len, name);
        }
    }

    parser->program->parser(opt->key, arg, parser->program->data);
}

static void args_parser_parse_shortopt(struct args_parser *parser) {
    const char *optopt = parser->argv[parser->argi] + parser->subopt;

    const struct args_option *opt = args_option_find_by_key(
        parser->program->options,
        optopt[0] & 0xFF
    );

    if (!opt) {
        error("Unrecognized option '%c'", optopt[0]);
    }

    const char *optarg = NULL;

    if (opt->arg) {
        if (optopt[1]) {
            optarg = &optopt[1];
        } else if (parser->argi + 1 < parser->argc) {
            parser->argi++;
            optarg = parser->argv[parser->argi];
        } else {
            error("Option '%c' requires an argument", optopt[0]);
        }
    }

    if (opt->arg || !optopt[1]) {
        parser->subopt = 0;
        parser->argi++;
    } else {
        parser->subopt++;
    }

    parser->program->parser(opt->key, optarg, parser->program->data);
}

static bool args_parser_parse_next(struct args_parser *parser) {
    if (parser->argi >= parser->argc) {
        return false;
    }

    if (!strncmp(parser->argv[parser->argi], "--", 2)) {
        args_parser_parse_longopt(parser);
    } else if (parser->argv[parser->argi][0] == '-' && parser->argv[parser->argi][1] != '\0') {
        parser->subopt = 1;
        while (parser->subopt != 0) {
            args_parser_parse_shortopt(parser);
        }
    } else {
        parser->program->parser(
            ARGS_KEY_ARG,
            parser->argv[parser->argi],
            parser->program->data
        );
    }

    parser->argi++;
    return true;
}

void args_parse(const struct args_program *program, int argc, char const *const *argv) {
    program->parser(ARGS_KEY_INIT, NULL, program->data);

    struct args_parser parser = { program, argv, argc, 1, 0 };

    while (args_parser_parse_next(&parser))
        ;

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
