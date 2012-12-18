
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

/** \file exception.c
 * \brief Exception Handling Words
 */

/**
 */
void exception_words() {
    Variable( "'exception" );
    Colon( "catch" );
            /* save the address of the top of the stack on the return stack */
           c("sp@"); c(">r");
           /* save the old exception pointer on the return stack */
           c("'exception"); c("@"); c(">r");
           /* save the current return stack pointer as the current exception pointer */
           c("rp@"); c("'exception"); c("!");
           c("execute");
           /* restore the previous exception handler */
           c("r>"); c("'exception"); c("!");
           c("r>"); c("drop");
           c("false"); End();

    Colon( "throw" ); /* ( err -- ) */
           c("dup"); c("0="); If(); c("drop"); c("exit"); Then();
           /* restore the return stack pointer to the exception handler */
           c("'exception"); c("@"); c("rp!");
           /* restore the previous exception handler */
           c("r>"); c("'exception"); c("!");
           c("r>"); c("swap"); c(">r"); c("sp!"); c("drop"); c("r>");
           End();

    Colon( "abort" ); c("-1"); c("throw"); End();

    Colon( "(abort\")" );  /* Need to implement this */
           c("ip>"); c("dup"); c("count"); c("+"); c("talign"); c(">ip");
           c("swap"); If(); c("throw"); Then(); c("drop"); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
