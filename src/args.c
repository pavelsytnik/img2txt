/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"

#include "util.h"

#include <stddef.h>
#include <string.h>

void args_parse(const struct args_program *program, int argc, char const *const *argv) {
    program->parser(ARGS_KEY_INIT, NULL, program->data);

    for (int i = 1; i < argc; i++) {

        if (!strncmp(argv[i], "--", 2)) {
            const char *name = argv[i] + 2;
            const char *eq = strchr(name, '=');

            size_t name_len = eq ? (size_t)(eq - name) : strlen(name);

            for (const struct args_option *opt = program->options; opt->key; opt++) {

                if (strlen(opt->name) == name_len && !strncmp(opt->name, name, name_len)) {
                    const char *arg = opt->arg ? (eq ? eq + 1 : argv[++i]) : NULL;

                    program->parser(opt->key, arg, program->data);

                    break;
                }
            }
        } else {
            program->parser(ARGS_KEY_ARG, argv[i], program->data);
        }
    }

    program->parser(ARGS_KEY_END, NULL, program->data);
}
