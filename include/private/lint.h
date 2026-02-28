/* SPDX-License-Identifier: MIT */
/* Copyright (C) 1998-2024 Daniel Veillard and the libxml2 contributors. */

#ifndef XML_LINT_H_PRIVATE__
#define XML_LINT_H_PRIVATE__

#include <stdio.h>

#include <libxml/parser.h>

int
xmllintMain(int argc, const char **argv, FILE *errStream,
            xmlResourceLoader loader);

void
xmllintShell(xmlDoc *doc, const char *filename, FILE *output);

#endif /* XML_LINT_H_PRIVATE__ */
