/* SPDX-License-Identifier: MIT */
/* Copyright (C) 1998-2024 Daniel Veillard and the libxml2 contributors. */

#ifndef XML_XPATH_H_PRIVATE__
#define XML_XPATH_H_PRIVATE__

#include <libxml/xpath.h>

XML_HIDDEN void
xmlInitXPathInternal(void);

#ifdef LIBXML_XPATH_ENABLED
XML_HIDDEN void
xmlXPathErrMemory(xmlXPathContext *ctxt);
XML_HIDDEN void
xmlXPathPErrMemory(xmlXPathParserContext *ctxt);
#endif

#endif /* XML_XPATH_H_PRIVATE__ */
