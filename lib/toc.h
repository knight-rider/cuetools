/*
 * toc.h -- toc function declarations
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2013 Svend Sorensen
 * For license terms, see the file COPYING in this distribution.
 */

struct Cd *toc_parse(FILE *fp);
void toc_print(FILE *fp, struct Cd *cd);
