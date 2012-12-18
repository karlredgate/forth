
/*
 * Copyright (c) 2010-2012 Karl N. Redgate
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/** \file memory.c
 * \brief 
 */

#include <stdlib.h>

#include "compiler.h"

/**
 */
void allocate() {
    tos = (Cell)malloc( (size_t)tos );
    push( (Cell)(tos == 0) );
}

/**
 */
void do_free() {
    free( (void *)tos );
    tos = (Cell)0;
}

/**
 */
void resize() {
    *sp = (Cell)realloc( (void *)*sp, (size_t)tos );
    tos = (Cell)(*sp == 0);
}

/**
 */
void memory_words() {
    Primitive( "allocate", &allocate );
    Primitive( "free",     &do_free );
    Primitive( "resize",   &resize );
}

/*
 * vim:autoindent
 * vim:expandtab
 */
