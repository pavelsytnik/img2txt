/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"

#include "util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct args_parser {
    const struct args_program *program;
    struct args_state *state;
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

static const struct args_option *args_option_find_by_name(
    const struct args_option *opts,
    const char *name
) {
    for (int i = 0; opts[i].key; i++) {
        if (longopts_match(name, opts[i].name)) {
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

    const struct args_option *opt = args_option_find_by_name(
        parser->program->options,
        name
    );

    if (!opt) {
        error("Unrecognized option '%.*s'", name_len, name);
    }

    const char *optarg = NULL;

    if (opt->arg) {
        if (eq) {
            optarg = eq + 1;
        } else if (parser->argi + 1 < parser->argc) {
            parser->argi++;
            optarg = parser->argv[parser->argi];
        } else {
            error("Option '%.*s' requires an argument", name_len, name);
        }
    }

    parser->argi++;

    parser->program->parser(opt->key, optarg, parser->state);
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

    parser->program->parser(opt->key, optarg, parser->state);
}

static void args_parser_parse_arg(struct args_parser *parser) {
    parser->program->parser(
        ARGS_KEY_ARG,
        parser->argv[parser->argi++],
        parser->state
    );

    parser->state->arg_num++;
}

static bool args_parser_parse_next(struct args_parser *parser) {
    if (parser->argi >= parser->argc) {
        return false;
    }

    if (parser->stop_options) {
        args_parser_parse_arg(parser);
        return true;
    }

    const char *arg = parser->argv[parser->argi];

    if (!strcmp(arg, "--")) {
        parser->stop_options = true;
        parser->argi++;
    } else if (!strncmp(arg, "--", 2)) {
        args_parser_parse_longopt(parser);
    } else if (arg[0] == '-' && arg[1] != '\0') {
        if (parser->subopt == 0) {
            parser->subopt = 1;
        }
        args_parser_parse_shortopt(parser);
    } else {
        args_parser_parse_arg(parser);
    }

    return true;
}

void args_parse(const struct args_program *program, int argc, char const *const *argv) {
    assert(program != NULL);
    assert(program->options != NULL);
    assert(program->parser != NULL);
    assert(argc > 0);
    assert(argv != NULL);

    struct args_state state = { 0, program->data };

    program->parser(ARGS_KEY_INIT, NULL, &state);

    struct args_parser parser = { program, &state, argv, argc, 1, 0, false };

    while (args_parser_parse_next(&parser))
        ;

    program->parser(ARGS_KEY_END, NULL, &state);
}
