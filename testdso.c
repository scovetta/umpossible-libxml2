/* SPDX-License-Identifier: MIT */
/* Copyright (C) 1998-2024 Daniel Veillard and the libxml2 contributors. */

#include <stdio.h>

#define IN_LIBXML
#include "libxml/xmlexports.h"

XMLPUBFUN int hello_world(void);

int hello_world(void)
{
  printf("Success!\n");
  return 0;
}
