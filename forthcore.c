
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

/** \file forthcore.c
 * \brief Core Words that are not primitives.
 */

#include "compiler.h"

/**
 */
void core_words() {
    Variable( "base" );

    Colon( "-rot" );  /* ( x y z -- z x y ) */
          c("rot"); c("rot"); End();
    Colon( "over" );  /* ( x y -- x y x ) */
          c("swap"); c("dup"); c("-rot"); End();
    Colon( "nip" );   /* ( x y -- y ) */
          c("swap"); c("drop"); End();
    Colon( "tuck" );       /* ( x y -- y x y ) */
          c("dup"); c("-rot"); End();

    Colon( "/" );   /* ( x y -- x/y ) */ c("/mod"); c("nip"); End();
    Colon( "mod" ); /* ( x y -- x%y ) */ c("/mod"); c("drop"); End();
    Colon( "*/" );  /* ( x y z -- n ) */ c("*/mod"); c("nip"); End();

    Colon( "1+"  ); c("1"); c("+"); End();
    Colon( "1+!" ); c("dup"); c("@"); c("1+"); c("swap"); c("!"); End();

    /* logical inverse */
    Colon( "invert" ); c("-1"); c("xor"); End();
    /* arithmatic inverse */
    Colon( "negate" ); c("invert"); c("1+"); End();
    Colon( "-" ); /* ( x y -- x-y ) */ c("negate"); c("+"); End();
    Colon( "1-" ); c("1"); c("-"); End();

    Colon( "<>" ); c("="); c("invert"); End();
    Colon( "0>" ); c("0"); c(">");  End();
    Colon( "0<" ); c("0"); c("<");  End();
    Colon( "0=" ); c("0"); c("=");  End();
    Colon( "0<>" ); c("0"); c("<>"); End();
    Colon( ">=" ); c("<"); c("0="); End();
    Colon( "<=" ); c(">"); c("0="); End();
    Colon( "0>=" ); c("0"); c(">="); End();
    Colon( "0<=" ); c("0"); c("<="); End();

    /* Actually these are not core words */
    Colon( "off" ); c("false"); c("swap"); c("!"); End();
    Colon( "on" ); c("true");  c("swap"); c("!"); End();

    Colon( "under" ); c("over"); c("swap"); End();
    Colon( "under+" );  /* ( x y z -- x+z y ) */
          c("rot"); c("+"); c("swap"); End();
    Colon( "under1+" ); c("1"); c("under+"); End();
    Colon( "under-" );  /* ( x y z -- x-z y ) */
          c("rot"); c("swap"); c("-"); c("swap"); End();
    Colon( "under1-" ); c("1"); c("under-"); End();

    Constant( "/cell", (Cell)sizeof(Cell) ); 
    Colon( "cells" ); c("/cell"); c("*"); End();
    Colon( "cells+" ); c("cells"); c("+"); End();
    Colon( "cell+" ); c("/cell"); c("+"); End();

    Constant( "/token", (Cell)sizeof(codefield) ); 
    Colon( "ta+" ); c("/token"); c("*"); c("+"); End();
    Colon( "ta1+" ); c("1");  c("ta+"); End();
    Colon( "ta1-" ); c("-1"); c("ta+"); End();

    /* This could also be a primitive where tos = *(sp-tos); */
    Colon( "pick" );
          c("1+"); c("cells"); c("sp@"); c("+"); c("@"); End();

    Colon( "dp" ); c("forth-wordlist"); End();
    Colon( "here" ); c("dp");  c("@"); End();
    Colon( "bounds" ); c("over"); c("+"); c("swap"); End();

    Colon( "+!" );  /* ( n adr -- ) */
          c("tuck"); c("@"); c("+"); c("swap"); c("!"); End();
    Colon( "count" );     /* ( adr -- adr' len ) */
          c("dup"); c("1+"); c("swap"); c("c@"); End();

    Colon( "2drop" ); c("drop"); c("drop"); End();
    Colon( "2dup" ); c("over"); c("over"); End();
    Colon( "2nip" );
          c("rot"); c("drop"); c("rot"); c("drop"); End();
    Colon( "2swap" );
          c("rot"); c(">r"); c("rot"); c("r>"); End();
    Colon( "2over" );
          c("3"); c("pick"); c("3"); c("pick"); End();

    Colon( "3drop" ); c("2drop"); c("drop"); End();

    /* FIX - check either ANS or Kelly/Spies ... this should
       do something different - like -1 xor */
    Colon( "toggle" );  /* ( adr -- ) */
          c("dup"); c("@"); c("0="); c("swap"); c("!"); End();

    /* FIX - user area should change to dynamically allocated
     * and thus this should change also ... maybe this is the
     * "value" word that would hold that dynamically allocated
     * address   -- ALSO THESE WORDS SHOULD EXIST IN THE core_userarea()
     * function
     */
    Colon( ">user" ); c("@"); c("cells"); c("up@"); c("+"); End();
}

/** \brief Core Extension words
 */

void core_extension_words() {
    /* is n1 >= n2 and n1 < n3 */
    Colon( "within" );  /* ( n1 n2 n3 -- flag ) */
          c("over"); c("-"); c(">r"); c("-"); c("r>"); c("u<"); End();
    /* is n1 >= n2 and n1 <= n3 */
    Colon( "between" );  /* ( n1 n2 n3 -- flag ) */
          c(">r"); c("over"); c("<="); c("swap"); c("r>"); c("<="); c("and");
          End();
}

/**
 * More Core words - these depend upon the control flow words and
 * so must be defined after they have been defined.
 */
void more_core_words() {
    Colon( "type" );
          c("bounds"); Do();
             c("i"); c("c@"); c("emit");
          Loop(); End();

    Colon( "cmove" );  /* ( src-adr dst-adr len -- ) */
          c("0"); Do();
             c("over"); c("i"); c("+"); c("c@");
             c("over"); c("i"); c("+"); c("c!");
          Loop(); c("2drop"); End();
    Colon( "cmove>" ); /* ( src-adr dst-adr len -- ) */
          c("tuck"); c("+"); c("1-"); c("-rot");
          c("tuck"); c("+"); c("1-"); c("-rot");
          c("0"); Do();
             c("over"); c("i"); c("-"); c("c@");
             c("over"); c("i"); c("-"); c("c!");
          Loop(); c("2drop"); End();

    Colon( "fill" );  /* ( adr len c -- ) */
          c("-rot");
          c("bounds"); Do();
             c("dup"); c("i"); c("c!");
          Loop(); c("drop"); End();
    Colon( "blank" );  /* ( adr len -- ) */
          c("bl"); c("fill"); End();

    Colon( "roll" );
          c("dup"); c(">r"); c("pick");
          c("sp@"); c("cell+"); c("dup"); c("cell+"); c("r>"); c("cells");
          c("cmove>");
          c("nip"); End();
    Colon( "2rot" );
          c("5"); c("roll"); c("5"); c("roll"); End();

    Colon( "?dup" );  /* ( n -- n n ) if n <> 0 */
          c("dup"); c("0="); If();
             c("exit");
          Then();
          c("dup"); End();

    Colon( "cr" );
          c("(cr)"); c("emit"); c("(nl)"); c("emit"); End();
    Colon( "space" ); c("bl"); c("emit"); End();
    Colon( "spaces" );
          c("0"); Do(); c("space"); Loop(); End();

    /* FIX - need the words align and aligned ... want to change this
     * token-align and token-aligned
     */
    Colon( "round-up" );  c("1-"); c("or");     c("1+");  End();
    Colon( "round-down"); c("1-"); c("invert"); c("and"); End();

    Colon( "align" );    /* ( -- ) */
          c("here"); c("/cell"); c("tuck"); c("mod"); c("-");
          c("/cell"); c("mod"); c("allot"); End();
    Colon( "aligned" );  /* ( adr -- adr' ) */
          c("/cell"); c("round-up"); End();

    Colon( "talign" );
          Literal( sizeof(Cell)-1 ); c("+");
          Literal( sizeof(Cell)-1 ); c("invert"); c("and");
          End();

    /* FIX - are these really core words or just junk to be put at the end */
    Colon( "hex" );
          Literal((Cell)16); c("base"); c("!"); End();
    Colon( "decimal" );
          Literal((Cell)10); c("base"); c("!"); End();
    Colon( "base(" );
          c("r>"); c("base"); c("@"); c(">r"); c(">r"); End();
    Colon( ")base" );
          c("r>"); c("r>"); c("base"); c("!"); c(">r"); End();
}

/**
 * Constants
 */
void compile_core_constants() {
    Constant( "true",  TRUE );
    Constant( "false", FALSE );

    Constant( "-2", (Cell)-2 );
    Constant( "-1", (Cell)-1 );
    Constant(  "0", (Cell)0 );
    Constant(  "1", (Cell)1 );
    Constant(  "2", (Cell)2 );
    Constant(  "3", (Cell)3 );
    Constant(  "4", (Cell)4 );
    Constant(  "5", (Cell)5 );
    Constant(  "6", (Cell)6 );
    Constant(  "7", (Cell)7 );
    Constant(  "8", (Cell)8 );
    Constant(  "9", (Cell)9 );

    Constant( "(cr)", (Cell)'\r' );
    Constant( "(nl)", (Cell)'\n' );
    Constant( "bl",   (Cell)' ' );
    Constant( "ht",   (Cell)'\t' );
    Constant( "nul",  (Cell)0x00 );
    Constant( "bel",  (Cell)0x07 );
    Constant( "esc",  (Cell)0x1b );
    Constant( "del",  (Cell)0x7f );
}

/*
 * vim:autoindent
 * vim:expandtab
 */
