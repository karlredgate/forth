
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

/** \file interpreter.c
 * \brief Definitions for words that are part of the interpreter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "linenoise.h"
#include "compiler.h"
#include "kernel.h"

   /* ICK - do this here for now, but remove when we do our own
      readline  - see below in interpreter words for other ICK*/
static char *prevline;

/**
 */
void accept() {
   char *line;

   /* Actually the prompt should be found in a variable ... maybe "prmopt"
    */
   line = linenoise( (*AddressOf("state"))? "] " : "ok> " );
   if ( line == NULL ) line = "bye";
   if ( (*line) && strcmp(line,prevline) )  linenoiseHistoryAdd(line);
   strcpy( (char*)(*sp), line );
   sp++;
   tos = strlen(line);
   free(prevline);  /* would be a memory leak if we don't do this */
   prevline = line;
}

/**
 */
void interpreter_words() {
   Variable( "state" );

   Colon( "[" ); c("state");  c("off");    End(); immediate();
   Colon( "]" ); c("state");  c("on");     End();

   Variable( "'source" );
   Variable( "#source" );
   Colon( "source" );  /* ( -- adr len ) */
          c("'source"); c("@"); c("#source"); c("@"); End();
   Colon( "set-source" );  /* ( adr len -- ) */
          c("#source"); c("!");  c("'source"); c("!");
          c("0"); c(">in"); c("!");
          End();
 
   Colon( "save-input" );
          c("r>");
          c("'source"); c("@"); c(">r");
          c("#source"); c("@"); c(">r");
          c(">in");     c("@"); c(">r");
          c("file");    c("@"); c(">r");
          c("file-buffer");     c(">r"); c("new-file-buffer");
          c(">r");
          End();
   Colon( "restore-input" );
          c("r>");
          c("file-buffer"); c("free"); Abort("Free failed for file buffer.");
          c("r>"); c("'file-buffer"); c("!");
          c("r>"); c("file");         c("!");
          c("r>"); c(">in");          c("!");
          c("r>"); c("#source");      c("!");
          c("r>"); c("'source");      c("!");
          c(">r");
          End();

   Colon( "(find)" );  /* ( adr len -- xt T | adr len F ) */
          c("context"); c("#order"); c("@"); c("cells");
          c("bounds"); Do();
             c("2dup"); c("i"); c("@"); c("search-wordlist"); If();
                c("nip"); c("nip"); c("leave"); c("true"); c("exit");
             Then();
          c("/cell"); PlusLoop(); c("false"); End();

   Colon( "digit?" );  /* ( c -- n T | F ) */
          c("dup"); Literal((Cell)'0'); Literal((Cell)'9');
          c("between"); If();
             Literal((Cell)'0'); c("-"); c("true"); c("exit");
          Then();
          c("dup"); Literal((Cell)'a'); Literal((Cell)'z');
          c("between"); If();
             Literal((Cell)'a'); c("-");
             Literal((Cell)10); c("+");
             c("true"); c("exit");
          Then();
          c("dup"); Literal((Cell)'A'); Literal((Cell)'Z');
          c("between"); If();
             Literal((Cell)'A'); c("-");
             Literal((Cell)10); c("+");
             c("true"); c("exit");
          Then();
          c("drop"); c("false"); End();

   /* FIX - not yet making double length numbers correctly */
   /* FIX - This will fail with numbers longer than 100 digits since it
      will overflow the return stack (tail recursion) */
   Colon( ">number" );
          c("dup"); c("0<="); If(); c("exit"); Then();
          c("over"); c("c@"); c("digit?"); c("0="); If(); c("exit"); Then();
          c("dup"); c("base"); c("@"); c(">"); If();
             c("drop"); c("exit");
          Then();
          c("s>d"); c("2rot"); c("base"); c("@"); c("s>d"); c("d*"); c("d+");
          c("2swap"); c("1"); c("/string"); Recurse(); End();

/* USE #PT as position of the . in the number -- refer to kelly/spies pg 361 */
/* FIX - this should also handle , and . within numbers and treat
   . at the end as forcing it to a double cell value */
   Colon( "?number" );
          c("0"); c("s>d"); c("2over");
          Begin();
             c(">number");
          c("dup"); c("0>"); While();
             c("over"); c("c@");
             c("dup");  Literal((Cell)'.'); c("=");
             c("swap"); Literal((Cell)','); c("=");
             c("or"); c("0="); If();
                c("2drop"); c("2drop"); c("false"); c("exit");
             Then();
             c("1"); c("/string");
          Repeat();
          c("2drop"); c("2swap"); c("2drop");  c("true"); End();

   Constant( "/tib", (Cell)TIB_SZ );
   Colon( "tib" ); c("'tib"); c("@"); End();
   ColdChain( "Allocating memory for TIB." );
          c("/tib"); c("allocate"); If();
             DotQuote("Couldn't allocate memory for TIB.");
             c("cr"); c("exit"); /* c("bye"); bye isn;t defined yet */
          Then(); c("'tib"); c("!"); End();

   /* ICK - do this here for now, but remove when we do our own
      readline */
   prevline = (char *)malloc( 5 );
   strcpy( prevline, "NADA" );

   /* Read a line of input into address "adr" up to a maximum length
    * of n1 and leave on the stack the actual number of characters
    * read as n2.
    */
   Primitive( "accept", &accept );   /* ( adr n1 -- n2 ) */

   /* source-id returns 0 = terminal, -1 = string/evaluate,
      otherwise input is from a file (file descriptor) */
   Colon( "source-id" );  c("file"); c("@"); End();

   Colon( "get-user-input" );  /* ( -- f ) */
          c("check-stack");
          c("showstack?"); c("@"); If(); c("(.s)"); Then();
          c("tib"); c("dup"); c("/tib"); c("accept");
          c("set-source"); c("true"); End();

   Colon( "get-file-input" );  /* ( -- f ) */
          c("file-buffer"); c("dup"); c("/file"); c("source-id");
          c("read-line"); Abort("File read failed.");
          If(); c("chop"); c("set-source"); c("true"); c("exit"); Then();
          c("2drop"); c("false");
          End();

   Colon( "input$" );  /* ( -- adr len ) */
          c("source"); c(">in"); c("@"); c("/string"); End();

   Colon( "whitespace?" );  /* ( c -- f ) */
          c("bl"); c("<="); End();

   Colon( "printable?" );  /* ( c -- f ) */
          c("bl"); c(">"); End();

   Colon( "skip" );  /* ( adr len xt -- adr' len' ) */
          c(">r");
          Begin();
             c("dup"); c("0>"); While();
             c("over"); c("c@"); c("r@"); c("execute"); c("0="); If();
                c("r>"); c("drop"); c("exit");
             Then();
             c("1"); c("/string");
          Repeat();
          c("r>"); c("drop");
          End();

   Colon( "parse" );   /* ( c -- adr len ) len is 0 at EOI */
          c("input$");
          c("over"); c(">r");   /* save the old address on the return stack */
          c("rot"); c("index");  
          c("dup"); c("1+"); c(">in"); c("+!");
          c("r>"); c("swap");
          End();

   Colon( "parse-word" );    /* ( -- adr len ) */
          c("input$"); c("over"); c(">r");
          Tick("whitespace?"); c("skip"); c("over"); c(">r");
          Tick("printable?");  c("skip"); c("drop");
          c("r>"); c("2dup"); c("-");
          c("rot"); c("r>"); c("-"); c("1+"); c(">in"); c("+!");
          End();

   Colon( "'" );
          c("parse-word"); c("(find)"); If(); c("exit"); Then();
          c("type"); DotQuote(" ?"); c("cr"); c("true"); c("abort");
          End();

   Colon( "refill" );  /* ( -- f ) */
          c("source-id"); c("0<"); If();    /* Input is string from evaluate */
             c("false"); c("exit");
          Then();
          c("source-id"); If();             /* Input is from a file */
             c("get-file-input"); c("exit");
          Then();
          c("get-user-input"); End();       /* Input is from the terminal */

   Colon( "?execute" );  /* ( xt -- ) */
          c("dup"); c("0=");         If(); c("drop");     c("exit"); Then();
          c("dup"); c("immediate?"); If(); c("execute");  c("exit"); Then();
          c("state"); c("@");        If(); c("compile,"); c("exit"); Then();
          c("execute"); End();

/* FIX - the number conversion here  has a bug where ... if the first
   characters of a word are convertible to a number but the rest are not
   the first part is converted and put on the stack, but the rest will
   be passed back and printed as an undefined word */

   Colon( "interpret-word" );  /* ( adr len -- ) */
          c("(find)"); If(); c("?execute"); c("exit"); Then();
          c("?number"); If();
             c("drop");  /* if compile mode do literal or dliteral */
             c("state"); c("@"); If();  c("literal");  Then();
             c("exit");
          Then();
          c("type"); DotQuote(" not found."); c("cr");
          End();
   Colon( "interpret" );  /* ( -- ) */
          Begin();
             c("parse-word");  c("dup");  While();  c("interpret-word");
          Repeat(); c("2drop"); End();

   Colon( "?aborted" );
          c("dup"); c("0="); If(); c("drop"); c("exit"); Then();
          c("dup"); c("0<"); If();
             c("drop"); DotQuote("Aborted!"); c("cr"); c("exit");
          Then();
          c("$."); c("cr"); End();

   Colon( "read-eval-loop" );
          Begin();
             c("refill");  While();
             Tick("interpret"); c("catch"); c("?aborted");
          Repeat(); End();

   Colon( "quit" );
          c("["); c("clear");
          c("rp0"); c("@"); c("rp!");  /* clear return stack */
          c("read-eval-loop"); End();

   Colon( "evaluate" );  /* ( adr len -- ) */
          c("save-input"); c("set-source");
          c("-1"); c("file"); c("!");
          c("interpret"); c("restore-input"); End();

   Colon( "include-file" );  /* ( file-id -- ) */
          c("save-input");
          c("file"); c("!"); c("read-eval-loop");
          c("restore-input"); End();

   Colon( "included" );  /* ( adr len -- ) */
          c("r/o"); c("open-file"); Abort("Couldn't open file.");
          c("include-file"); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
