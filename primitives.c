
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

/** \file primitives.c
 * \brief Forth Primitives
 *
 * These words are the equivalent of CODE words in a native
 * Forth implementation.  The last instructions of each of these
 * words implement NEXT (the inner or address interpreter).
 */

#include <stdio.h>
#include <string.h>

#include "compiler.h"

/**
 */
static void unnest()   { ip = (APF)*(rp--); }
static void to_r()     { *(++rp) = pop(); }
static void r_from()   { push( *(rp--) ); }
static void r_fetch()  { push( *rp ); }
static void store()    { *((Cell*)tos) = *sp; tos = *(++sp); sp++; }
static void fetch()    { tos = *((Cell*)tos);  }
static void c_fetch()  { tos = *((unsigned char*)tos);  }
static void and()      { tos &= *(sp++); }
static void or()       { tos |= *(sp++); }
static void xor()      { tos ^= *(sp++); }
static void plus()     { tos += *(sp++); }
static void multiply() { tos *= *(sp++); }
static void dup()      { *(--sp) = tos; }
static void swap()     { Cell scr = tos;  tos = *sp;  *sp = scr; }
static void drop()     { tos = *(sp++); }
static void lshift()   { tos = (uCell)(*(sp++)) << tos; }
static void rshift()   { tos = (uCell)(*(sp++)) >> tos; }

/* Some compilers set true to 1 instead of -1, so for all
   predicates we have to use this (test) ? -1 : 0
   The primary reason for this is "value test and" phrases */

static void greater()   { tos = (*(sp++) > tos)               ? TRUE : FALSE ; }
static void u_greater() { tos = ((uCell)*(sp++) > (uCell)tos) ? TRUE : FALSE ; }
static void lesser()    { tos = (*(sp++) < tos)               ? TRUE : FALSE ; }
static void u_lesser()  { tos = ((uCell)*(sp++) < (uCell)tos) ? TRUE : FALSE ; }
static void equals()    { tos = (*(sp++) == tos)              ? TRUE : FALSE ; }

/**
 */
static void c_store() {
    *((char*)tos) = (char)(*sp & 0xff);
    tos = *(++sp);
    sp++;
}

/**
 */
static void slash_mod() {
    Cell quotient, remainder;

    remainder = *sp % tos;
    quotient  = *sp / tos;
    *sp = remainder;
    tos = quotient;
}

/**
 */
static void star_slash_mod() {
    DoubleCell M;

    M = (*(sp+1) * *sp);
    sp++;

    *sp = M % tos;  /* remainder */
    tos = M / tos;  /* quotient */
}

/**
 */
static void rot() {
    Cell scr = *(sp + 1);

    *(sp + 1) = *sp;
    *sp = tos;
    tos = scr;
}

/*
 * This sp@ word works even though the top of stack is cached in a
 * register because we push the current cached TOS and then take
 * the address of that value and stuff it into the cached TOS.
 */
static void sp_fetch() { *(--sp) = tos; tos = (Cell)sp; }
static void sp_store() { sp = (Cell*)tos; tos = *(sp++); }
static void rp_fetch() { push((Cell)rp); }
static void rp_store() { rp = (Cell *)pop(); }
static void key()      { *(--sp) = tos; tos = (Cell)getchar(); }
static void emit()     { putchar((char)tos); tos = *(sp++); }

/**
 */
/* ( adr1 len1 adr2 len2 -- n ) */
static void compare() {
    if (tos != *(sp+1)) { tos = -1;  sp += 3; return; }
    tos = (Cell)strncmp( (char*)*(sp+2), (char*)*sp, (int)*(sp+1) );
    sp += 3;
}

/**
 */
static void execute() {
    ACF acf = (ACF)tos;
    tos = *sp++;
    Execute( acf );
}

/**
 */
void compile_primitives() {
    Primitive( "unnest", &unnest );
    Primitive( "exit",  &unnest );
    Primitive( ">r",    &to_r );           /* ( x -- ) (R: -- x ) */
    Primitive( "r>",    &r_from );         /* ( -- x ) (R: x -- ) */
    Primitive( "r@",    &r_fetch );        /* ( -- x ) (R: x -- x ) */
    Primitive( "!",     &store );          /* ( x adr -- ) */
    Primitive( "@",     &fetch );          /* ( adr -- x ) */
    Primitive( "+",     &plus );           /* ( x y -- x+y ) */
    Primitive( "*",     &multiply );       /* ( x y -- x*y ) */
    Primitive( "*/mod", &star_slash_mod ); /* ( x y z -- q r ) */
    Primitive( "/mod",  &slash_mod );      /* ( x y -- q r ) */
    Primitive( "and",   &and );            /* ( x y -- x&y ) */
    Primitive( "or",    &or );             /* ( x y -- x|y ) */
    Primitive( "xor",   &xor );            /* ( x y -- x^y ) */
    Primitive( "dup",   &dup );            /* ( x -- x x ) */
    Primitive( "swap",  &swap );           /* ( x y -- y x ) */
    Primitive( "drop",  &drop );           /* ( x y -- x ) */
    Primitive( "<<",    &lshift );         /* ( n s -- n<<s ) */
    Primitive( ">>",    &rshift );         /* ( n s -- n>>s ) */
    Primitive( "rot",   &rot );            /* ( x y z -- y z x ) */
    Primitive( "c!",    &c_store );        /* ( c adr -- ) */
    Primitive( "c@",    &c_fetch );        /* ( adr -- c ) */
    Primitive( ">",     &greater );
    Primitive( "u>",    &u_greater );
    Primitive( "<",     &lesser );
    Primitive( "u<",    &u_lesser );
    Primitive( "=",     &equals );
    Primitive( "sp@",   &sp_fetch );
    Primitive( "sp!",   &sp_store );
    Primitive( "rp@",   &rp_fetch );
    Primitive( "rp!",   &rp_store );
    Primitive( ">ip",   &to_r );
    Primitive( "ip>",   &r_from );
    Primitive( "ip@",   &r_fetch );
    Primitive( "execute", &execute );
    Primitive( "emit",  &emit );
    Primitive( "key",   &key );

    /* This should be implemented in Forth...
     * : compare
     *     rot tuck  ( adr1 adr2 len1 len2 len1 )
     * ;
     */
    Primitive( "compare", &compare );  /* ( adr1 len1 adr2 len2 -- n ) */

    /* Not really a primitive, but I want this declared before any Colon
     * words are defined. */
    Colon( "cold-chain" ); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
