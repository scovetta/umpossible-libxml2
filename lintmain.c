/* SPDX-License-Identifier: MIT */
/* Copyright (C) 1998-2024 Daniel Veillard and the libxml2 contributors. */
/*
 * lintmain.c: Main routine for xmllint
 *
 * See Copyright for the status of this software.
 */

#include <stdio.h>

#include "private/lint.h"

int
main(int argc, char **argv) {
    return(xmllintMain(argc, (const char **) argv, stderr, NULL));
}
