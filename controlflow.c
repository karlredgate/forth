
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


/** \file controlflow.c
 * \brief Control transfer words for use before the dictionary
 * is ready to be used.
 *
 * Need to handle 0 branches so they don't generate divide by 0 errors
 */

#include "compiler.h"

static Cell *forwardref[10], *backref[10];
static int F = -1, B = -1;

void mark_forward() { forwardref[++F] = Here(); place((Cell)0); }
void resolve_forward() {
    *(forwardref[F]) = (Cell)((Here() - forwardref[F]));
    F--;
}
void mark_back()    { backref[++B] = Here(); }
void resolve_back() { place( (Cell)(backref[B--] - Here()) ); }
void swap_forward() {
    Cell *scr = forwardref[F];
    forwardref[F]   = forwardref[F-1];
    forwardref[F-1] = scr;
}

void Begin()  { mark_back(); }
void If()     { c("?branch"); mark_forward(); }
void While()  { If(); }
void Ahead()  { c("branch");  mark_forward(); }
void Until()  { c("?branch"); resolve_back(); }
void Again()  { c("branch");  resolve_back(); }
void Then()   { resolve_forward(); }
void Repeat() { Again(); Then(); }
void Else()   { Ahead(); swap_forward(); Then(); }

void Do()       { c("(do)");    mark_back(); }
void QuestDo()  { c("(?do)");   mark_back(); }
/* ?do not implemented yet, it needs to have a forward referenece
   also, so as to have a place to jump to if the index is already
   beyond the limit */

void Loop()     { c("(loop)");  resolve_back(); }
void PlusLoop() { c("(+loop)"); resolve_back(); }

/**
 * Control Flow words
 *
 * Almost all of the control flow in Forth is based on the branch and
 * ?branch words.
 */
void controlflow_words() {
    Colon( "branch" );
          c("ip>"); c("dup"); c("@"); c("cells+"); c(">ip"); End();

    Colon( "?branch" );
          c("ip>"); c("dup"); c("cell+"); c("swap"); c("@");
          c("1-"); c("rot"); c("0="); c("and"); c("cells+");
          c(">ip"); End();

    /* ANS Spec pg.152 - 2>r is historically used to implement do */
    Colon( "(do)" );  /* ( limit index -- ) */
          c("r>"); c("-rot");
          /* this should be 2>r, but it is not defined yet */
          c("swap"); c(">r"); c(">r");
          c(">r"); End();
    Colon( "do" ); c("compile"); c("(do)"); c("here"); End();
          immediate();

    Colon( "(loop)" );  /* (R: limit index (loop)-ip ) */
          c("r>"); c("r>"); c("1+"); c("r>");  /* ( ip index limit ) */
          c("2dup"); c("<"); If();
             c(">r"); c(">r");
             c("dup"); c("@"); c("cells+"); c(">r");
             c("exit");
          Then();
          c("2drop"); c("cell+"); c(">r"); End();
    Colon( "loop" ); c("compile"); c("(loop)");
          c("here"); c("-"); c("/cell"); c("/"); c(","); End();
          immediate();

    /* FIX - +loop only works in the forward direction - look at
       +loop in the glossary of kelly/spies to use the F83 semantics
       for +loop in negative direction */
    Colon( "(+loop)" );  /* (R: limit index (+loop)-ip ) */
                               /* ( incr ) */
          c("r>"); c("swap");  /* ( incr ip incr ) */
          c("r>"); c("+"); c("r>"); c("2dup"); c("<"); If();
             c(">r"); c(">r"); c("dup"); c("@"); c("cells+");
             c(">r"); c("exit");
          Then();
          c("2drop"); c("cell+"); c(">r"); End();

    Colon( "+loop" );
          c("compile"); c("(+loop)");
          c("here"); c("-"); c("/cell"); c("/"); c(",");
          End(); immediate();

    Colon( "leave" );
          c("r>"); c("r>"); c("r>"); c("2drop"); c(">r"); End();

    Colon( "i" ); c("r>"); c("r@"); c("swap"); c(">r"); End();
    /* FIX - IS THIS RIGHT???? */
    Colon( "i'" );
          c("r>"); c("r>"); c("r@"); c("swap"); c(">r"); c("swap"); c(">r");
          End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
