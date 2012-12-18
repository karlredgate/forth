
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

/** \file vocabulary.c
 * \brief Vocabulary Words
 */

#include "compiler.h"

/**
 */
void
vocabulary_words() {
    Colon( ">dp");      End();
    Colon( ">lastacf"); c("cell+"); End();
    Colon( ">origin");  c(">lastacf"); c("cell+"); End();
    Colon( "/wordlist" ); c("3"); c("cells"); End();
    Colon( "/wordlist-data" ); Literal((Cell)10000); c("cells"); End();
    Colon( "wordlist" );  /* ( -- wid ) */
          c("/wordlist");      c("allocate"); c("throw");
          c("/wordlist-data"); c("allocate"); c("throw");
          c("over"); c("under"); c("!");
          c("over"); c("cell+"); c("cell+"); c("!");
          c("0"); c("over"); c("cell+"); c("!");
          End();

    Colon( "origin" );
          c("forth-wordlist"); c(">origin"); c("@"); End();

    Constant( "#vocs", (Cell)16 );
    Variable( "#order" );
    Variable( "current" );
    nVariable( "context", constant("#vocs") );

/* Also change to use get-current and set-current */

    Colon( "get-order" );   /* ( -- wid1 ... widn n ) */
          c("#order"); c("@"); c("0"); Do();
             c("#order"); c("@"); c("i"); c("-"); c("1-"); c("cells");
             c("context"); c("+"); c("@");
          Loop(); c("#order"); c("@"); End();
    Colon( "set-order" );   /* ( wid1 ... widn n -- ) */
          c("dup"); c("#order"); c("!");
          c("context"); c("swap"); c("cells");
          c("bounds"); Do();  /* FIX - should be ?do */
             c("i"); c("!");
          c("/cell"); PlusLoop();
          End();

    Colon( "only" );  c("forth-wordlist"); c("1"); c("set-order"); End();
    Colon( "also" ); c("get-order"); c("under"); c("1+"); c("set-order"); End();
    Colon( "previous" );
          c("get-order"); c("nip"); c("1-"); c("set-order"); End();
    Colon( "definitions" ); c("context"); c("@"); c("current"); c("!"); End();

    ColdChain( "Setting default search order." );
          c("only"); c("also"); c("definitions"); End();

    Colon( "'lastacf" ); c("current"); c("@"); c("cell+"); End();
    Colon( "lastacf" ); c("'lastacf"); c("@"); End();
    Colon( "link" );
          c("lastacf"); c(","); c("here"); c("'lastacf"); c("!"); End();

    Constant( "#smudge",    (Cell)0x40 );
    Constant( "#immediate", (Cell)0x80 );
    Colon( "name(" );  /* ( bit -- anf bit c ) */
          c("lastacf"); c(">name"); c("tuck"); c("c@"); End();
    Colon( ")name" );  /* ( adr c -- ) */
          c("swap"); c("c!"); End();
    Colon( "set-name" );  /* ( acf bit -- ) */
          c("name("); c("or"); c(")name"); End();

    Colon( "clear-name" );
          c("name("); c("swap"); c("invert"); c("and"); c(")name");
          End();

    Colon( "reveal" );  /* ( acf -- ) */
          c("#smudge"); c("clear-name"); End();
    Colon( "hide" );
          c("#smudge"); c("set-name"); End();
    Colon( "immediate" );
          c("#immediate"); c("set-name"); End();
    Colon( "immediate?" );  /* ( xt -- f ) */
          c(">name"); c("c@"); c("#immediate"); c("and"); c("0<>"); End();

#ifdef notdef
    /* This word is now defined as a primitive in kernel.c since
     * the compiling of files was so slow.
     */
    Colon( "search-wordlist" ); /* ( adr len wid -- 0|xt -1|xt 1 ) */
           c(">lastacf");
           Begin();
           c("@"); c("dup"); While(); /* while not NULL */
              c(">r");
              c("2dup"); c("r@"); c(">name"); c("name>$");
              c("compare"); c("0="); If();
              /* this "1" should be a check for immediate!!! */
                 c("2drop"); c("r>"); c("1"); c("exit");
              Then();
              c("r>"); c(">link");
           Repeat();
           c("drop"); c("2drop");
           c("0");
           End();
#endif
}

/*
 * vim:autoindent
 * vim:expandtab
 */
