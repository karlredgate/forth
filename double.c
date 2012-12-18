
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

/** \file double.c
 * \brief Double Length Number words
 *
 * Double length numbers take two cells where the first cell is the
 * the low half and the second cell is the high half [ie - stack
 * notation is ( lo hi )].
 */

#include "compiler.h"

#define CELL_MASK ((Cell)-1)
#define HI(D)     ((Cell)(((D)>>(sizeof(Cell)*8))&CELL_MASK))
#define LO(D)     ((Cell)((D) & CELL_MASK))
#define D(hi,lo)  ((DoubleCell)(((uDoubleCell)(hi)<<(sizeof(Cell)*8)) + (uCell)(lo)))

/**
 */
static void
d_star_slash_mod() {
    DoubleCell M, divisor, quotient, remainder;

    M = D(*(sp+3), *(sp+4)) * D(*(sp+1), *(sp+2));
    divisor = D(tos, *sp);

    quotient  = M / divisor;
    remainder = M % divisor;

    sp += 2;
    *(sp+2) = LO(remainder);
    *(sp+1) = HI(remainder);
    *sp     = LO(quotient);
    tos     = HI(quotient);
}

/**
 * ( d1 d2 -- d3 d4 )
 */
static void
d_slash_mod() {
    uDoubleCell N, divisor, quotient, remainder;

    N = D(*(sp+1), *(sp+2));
    divisor = D(tos, *sp);

    quotient  = N / divisor;
    remainder = N % divisor;

    *(sp+2) = LO(remainder);
    *(sp+1) = HI(remainder);
    *sp     = LO(quotient);
    tos     = HI(quotient);
}

/**
 */
static void
d_plus() {
    DoubleCell scr;

    scr  = D(*(sp+1),*(sp+2)) + D(tos,*sp);
    sp  += 2;
    tos  = HI(scr);
    *sp  = LO(scr);
}

/**
 */
static void d_times() {
    DoubleCell scr;

    scr  = D(*(sp+1),*(sp+2)) * D(tos,*sp);
    sp  += 2;
    tos  = HI(scr);
    *sp  = LO(scr);
}

/**
 */
void compile_double() {
    Primitive( "d*/mod", &d_star_slash_mod ); /* ( d1 d2 d3 -- d4 d5 ) */
    Primitive( "d/mod",  &d_slash_mod );      /* ( d1 d2 -- d3 d4 ) */
    Primitive( "d+",     &d_plus );        /* ( d1 d2 -- d3 ) */
    Primitive( "d*",     &d_times );       /* ( d1 d2 -- d3 ) */

    Colon( "s>d" );      /* ( n -- d ) */
          c("dup"); c("0<"); End();

    Colon( "d/" );      /* ( d1 d2 -- d3 ) */
          c("d/mod"); c("2nip"); End();
    Colon( "d*/" );     /* ( d1 d2 d3 -- d4 ) */
          c("d*/mod"); c("2nip"); End();

    Colon( "d2*" );      /* ( d1 -- d2 ) */
          c("2"); c("s>d"); c("d*"); End();
    Colon( "d2/" );      /* ( d1 -- d2 ) */
          c("2"); c("s>d"); c("d/"); End();

    Colon( "2>r" );
          c("r>"); c("-rot"); c("swap"); c(">r"); c(">r"); c(">r"); End();
    Colon( "2r>" );
          c("r>"); c("r>"); c("r>"); c("swap"); c("rot"); c(">r"); End();
    Colon( "2r@" );
          c("r>");
          c("r>"); c("r@"); c("over"); c(">r");
          c("swap"); c("rot"); c(">r");
          End();

    Colon( "d1+" );
          c("1"); c("s>d"); c("d+"); End();
    Colon( "dnegate" );
          c("-1"); c("xor"); c("swap");
          c("-1"); c("xor"); c("swap");
          c("d1+"); End();
    Colon( "d-" );       /* ( d1 d2 -- d3 ) */
          c("dnegate"); c("d+"); End();

    Colon( "d=" );      /* ( d1 d2 -- f ) */
          c("rot"); c("="); c("-rot"); c("="); c("and"); End();
    Colon( "d0=" );     /* ( d -- f ) */
          c("or"); c("0="); End();
    Colon( "d0<" );     /* ( d1 d2 -- f ) */
          c("nip"); c("0<"); End();

    Colon( "dabs" );
          c("2dup"); c("d0<"); If(); c("dnegate"); Then(); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
