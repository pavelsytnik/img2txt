/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #define flockfile _lock_file
  #define funlockfile _unlock_file
#endif

struct args_parser {
    const struct args_program *program;
    struct args_state state;
    unsigned flags;
    const char *const *argv;
    int argc;
    int argi;
    int subopt;
    bool stop_options;
};

static bool longopts_match(const char *opt, const char *name) {
    while (*opt && *name && *opt != '=') {
        if (*opt++ != *name++) {
            return false;
        }
    }

    return *name == '\0' && (*opt == '\0' || *opt == '=');
}

static bool args_parser_parse_longopt(struct args_parser *parser) {
    const char *name = parser->argv[parser->argi] + 2;
    const char *eq = strchr(name, '=');

    size_t name_len = (eq) ? (size_t)(eq - name) : strlen(name);

    const struct args_option *opts = parser->program->options;
    const struct args_option *opt = NULL;
    int option_count = 0;

    for (int i = 0; opts[i].key; i++) {
        if (longopts_match(name, opts[i].name)) {
            opt = &opts[i];
            break;
        }
        option_count++;
    }

    if (!opt) {
        bool *ambig_set = NULL;
        bool ambig = false;

        for (int i = 0; opts[i].key; i++) {
            if (!strncmp(opts[i].name, name, name_len)) {
                if (!opt) {
                    opt = &opts[i];
                } else {
                    if (!ambig) {
                        ambig = true;

                        ambig_set = calloc(option_count, sizeof(*ambig_set));
                        if (!ambig_set) break;

                        ambig_set[opt - opts] = true;
                    }

                    ambig_set[i] = true;
                }
            }
        }

        if (!opt) {
            fprintf(stderr, "Unrecognized option '%.*s'\n", (int)name_len, name);
            return false;
        }

        if (ambig) {
            flockfile(stderr);
            fprintf(stderr, "Ambiguous option '%.*s'", (int)name_len, name);
            if (ambig_set) {
                fprintf(stderr, "; Possibilities:");
                for (int i = 0; opts[i].key; i++) {
                    if (ambig_set[i]) {
                        fprintf(stderr, " '%s'", opts[i].name);
                    }
                }
                free(ambig_set);
            }
            fprintf(stderr, "\n");
            funlockfile(stderr);
            return false;
        }
    }

    const char *optarg = NULL;

    if (opt->arg) {
        if (eq) {
            optarg = eq + 1;
        } else if (parser->argi + 1 < parser->argc) {
            parser->argi++;
            optarg = parser->argv[parser->argi];
        } else {
            fprintf(stderr, "Option '%s' requires an argument\n", opt->name);
            return false;
        }
    }

    if (!parser->program->parser(opt->key, optarg, &parser->state)) {
        return false;
    }

    parser->argi++;
    return true;
}

static bool args_parser_parse_shortopt(struct args_parser *parser) {
    const char *optopt = parser->argv[parser->argi] + parser->subopt;

    const struct args_option *opts = parser->program->options;
    const struct args_option *opt = NULL;

    for (int i = 0; opts[i].key; i++) {
        if (opts[i].key == (optopt[0] & 0xFF)) {
            opt = &opts[i];
            break;
        }
    }

    if (!opt) {
        fprintf(stderr, "Unrecognized option '%c'\n", optopt[0]);
        return false;
    }

    const char *optarg = NULL;

    if (opt->arg) {
        if (optopt[1]) {
            optarg = &optopt[1];
        } else if (parser->argi + 1 < parser->argc) {
            parser->argi++;
            optarg = parser->argv[parser->argi];
        } else {
            fprintf(stderr, "Option '%c' requires an argument\n", optopt[0]);
            return false;
        }
    }

    if (!parser->program->parser(opt->key, optarg, &parser->state)) {
        return false;
    }

    if (opt->arg || !optopt[1]) {
        parser->subopt = 0;
        parser->argi++;
    } else {
        parser->subopt++;
    }
    return true;
}

static bool args_parser_parse_arg(struct args_parser *parser) {
    if (!parser->program->parser(
        ARGS_KEY_ARG,
        parser->argv[parser->argi],
        &parser->state
    )) {
        return false;
    }

    parser->argi++;
    parser->state.arg_num++;
    return true;
}

static bool args_parser_parse_next(struct args_parser *parser) {
    if (parser->argi >= parser->argc) {
        return false;
    }

    if (parser->stop_options) {
        return args_parser_parse_arg(parser);
    }

    const char *arg = parser->argv[parser->argi];

    if (!strcmp(arg, "--")) {
        parser->stop_options = true;
        parser->argi++;
        return true;
    }

    if (!strncmp(arg, "--", 2)) {
        return args_parser_parse_longopt(parser);
    }

    if (arg[0] == '-' && arg[1] != '\0') {
        if (parser->subopt == 0) {
            parser->subopt = 1;
        }
        return args_parser_parse_shortopt(parser);
    }

    return args_parser_parse_arg(parser);
}

static bool args_parser_init(
    struct args_parser *parser,
    const struct args_program *program,
    int argc,
    char const *const *argv,
    unsigned flags
) {
    parser->program = program;
    parser->state.arg_num = 0;
    parser->state.input = program->input;
    parser->state.priv = parser;
    parser->flags = flags;
    parser->argv = argv;
    parser->argc = argc;
    parser->argi = 1;
    parser->subopt = 0;
    parser->stop_options = false;

    return program->parser(ARGS_KEY_INIT, NULL, &parser->state);
}

static bool args_parser_finalize(struct args_parser *parser) {
    bool ok = parser->argi == parser->argc;

    // All arguments have been consumed
    if (ok) {
        ok = parser->program->parser(ARGS_KEY_END, NULL, &parser->state);
    }

    if (!ok) {
        parser->program->parser(ARGS_KEY_ERR, NULL, &parser->state);
    }

    // It's called anyway
    parser->program->parser(ARGS_KEY_FINI, NULL, &parser->state);

    return ok;
}

bool args_parse(
    const struct args_program *program,
    int argc,
    char const *const *argv,
    unsigned flags
) {
    assert(program != NULL);
    assert(program->options != NULL);
    assert(program->parser != NULL);
    assert(argc > 0);
    assert(argv != NULL);

    struct args_parser parser;
    bool ok;

    ok = args_parser_init(&parser, program, argc, argv, flags);

    if (ok) {
        while (ok) {
            ok = args_parser_parse_next(&parser);
        }
        ok = args_parser_finalize(&parser);
    }

    if (!ok && !(flags & ARGS_NO_EXIT)) {
        exit(EXIT_FAILURE);
    }

    return ok;
}
