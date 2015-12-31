
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

/** \file init.c
 * \brief Initialize the dictionary
 */

#include "compiler.h"
#include "kernel.h"
#include "controlflow.h"

extern void init_compiler();
extern void memory_words();

extern void compile_primitives();
extern void compile_core_constants();
extern void core_words();
extern void core_extension_words();
extern void compile_dictionary_words();
extern void controlflow_words();
extern void more_core_words();

extern void compile_double();
extern void string_words();
extern void file_words();
extern void format_words();
extern void compile_stack_words();

extern void implementation_words();
extern void vocabulary_words();
extern void interpreter_words();
extern void exception_words();

extern void unix_words();

/** \brief Load all the initial dictionary words.
 */
void
init_dictionary(int dictsize) {
    init_compiler(dictsize);
    memory_words();

    compile_primitives();
    compile_core_constants();
    core_words();
    core_extension_words();
    compile_dictionary_words();
    controlflow_words();
    more_core_words();

    compile_double();
    string_words();
    exception_words();
    file_words();
    format_words();
    compile_stack_words();

    implementation_words();
    vocabulary_words();
    interpreter_words();

    unix_words();
/* FIX - NEED TO DO THESE */
//   Primitive( "d<",     &lesser );      /* ( d1 d2 -- f ) */
//   Primitive( "d>",     &lesser );      /* ( d1 d2 -- f ) */

//   Primitive( "u*",      &um_star );   /* ( x y -- x*y ) */
//   Primitive( "du*",     &um_star );   /* ( x y -- x*y ) */
//   Primitive( "du/mod",  &um_star );   /* ( x y -- x*y ) */
//   Primitive( "du<",     &um_star );   /* ( x y -- x*y ) */

//   Primitive( "m+",      &um_star );   /* ( x y -- x*y ) */
//   Primitive( "m-",      &um_star );   /* ( x y -- x*y ) */
//   Primitive( "m*",      &um_star );   /* ( x y -- x*y ) */
//   Primitive( "m*/",     &um_star );   /* ( x y -- x*y ) */
//   Primitive( "m/",      &um_star );   /* ( x y -- x*y ) */
//   Primitive( "m/mod",   &um_star );   /* ( x y -- x*y ) */
/* FIX - NEED TO DO THESE */

   /* FIX  - these two words are no longer used - why are they here ?? */
//   Primitive( "um*",  &um_star );   /* ( x y -- x*y ) */
//   Colon( "udm*" );  /* ( d.lo d.hi n -- d.lo' d.hi' ) */
//          c("tuck"); c("um*"); c("drop"); c("-rot"); c("um*");
//          c("rot"); c("+"); End();

    Colon( ".version" );
          DotQuote("Portable ANS Forth - Karl.Redgate@gmail.com");
          c("cr"); End();

    Colon( "rstrace" );
          DotQuote("( R: ");
          c("rp@"); c("rp0"); c("@"); Do();
             c("i"); c("."); c("space");
          c("/cell"); PlusLoop();
          DotQuote(" )"); c("cr");
          End();

    Variable( "argument-hook" );
    /* To( Tick("ndrop"), "argument-hook" ); */
    To( Tick("exit"), "argument-hook" );

    Colon( "cold" );
          c(".version");
          c("cold-chain");
          /* included may be replaced with path-included which reads
             FORTH_PATH environment variable to look in selected
             directories */
          c("debug"); c("@"); If();
             DotQuote("Loading Forth files...");
          Then();
          /* StringLiteral("base.fth"); c("included"); */
          DotQuote("loading base.fth"); c("cr");
          StringLiteral("base.fth"); Tick("included"); c("catch");
          c("?aborted"); If(); DotQuote("Failed to load."); c("cr"); Then();

          c("debug"); c("@"); If();
             DotQuote("done."); c("cr");
          Then();
          c("hex");
          c("argument-hook"); c("@"); Tick("execute"); c("catch");
          c("?aborted"); If();
              DotQuote("argument-hook failed"); c("cr");
          Then();
          c("read-eval-loop"); End();

    Colon( "warm" );
          DotQuote( "Warm Started." ); c("cr");
          c("rstrace");
          c("quit"); End();

    /* header */
    Colon( "name," );
          c("dup"); c("1+"); c("talign"); c("allot");
          c("dup"); c("here"); c("1-"); c("c!");
          c("here"); c("1-"); c("over"); c("-");
          c("swap"); c("cmove"); End();

    Colon( "header" );
          c("name,"); c("link"); c("do-create"); c(","); End();
    Colon( "create" );
          c("parse-word"); c("header"); End();

    Colon( "codefield" );  /* ( codefield -- ) */
          c("lastacf"); c("!"); End();

    Colon( ":" );
          c("create"); c("hide");
          c("do-colon"); c("codefield");
          c("]"); End();
    Colon( ";" );
          c("compile"); c("unnest");
          c("["); c("reveal"); End(); immediate();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
