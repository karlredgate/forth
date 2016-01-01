
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

/** \file signal.c
 * \brief Recover from errors in forth programs.
 */

/* Add to init function a way to pass in a struct like:
   struct debug {
      char *name
      caddr_t start, end;
   }
   and then have dump_registers search this list and print the
   function name for the PC and nPC values.
 */

/*
  /usr/include/ucontext.h:43:2: error: The deprecated ucontext routines require _XOPEN_SOURCE to be defined
  #error The deprecated ucontext routines require _XOPEN_SOURCE to be defined
*/

#define _XOPEN_SOURCE 700

#include <signal.h>
/* #include <siginfo.h> */
#include <ucontext.h>

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf env;

/**
 */
static void
dump_registers( globals ) greg_t *globals; {
    fprintf( stderr, "\n" );
}

/**
 */
static void
SIGINT_handler( int signal, siginfo_t *info, void *data ) {
    ucontext_t *context = (ucontext_t *)data;

    fflush( stdout );
    fprintf( stderr, "Interrupted.\n" );
    longjmp(env,signal);
}

/**
 */
static void
SIGSEGV_handler( int signal, siginfo_t *info, void *data ) {
    ucontext_t *context = (ucontext_t *)data;

    fflush( stdout );
    switch (info->si_code) {
    case SEGV_MAPERR:
        fprintf( stderr, "\nSEGV: Address not mapped to object 0x%x\n",
                info->si_addr );
        break;
    case SEGV_ACCERR:
        fprintf( stderr, "\nSEGV: Invalid permissions for mapped object 0x%x\n",
                info->si_addr );
        break;
    default:
        fprintf( stderr, "Unknown signal code.\n" );
        exit(0);
    }
    dump_registers( context->uc_mcontext.gregs );
    longjmp(env,signal);
}

/**
 */
static void
SIGBUS_handler( int signal, siginfo_t *info, void *data ) {
    ucontext_t *context = (ucontext_t *)data;

    fflush( stdout );
    switch (info->si_code) {
    case BUS_ADRALN:
        fprintf( stderr, "\nBUSERR: Invalid address alignment 0x%x\n",
                 info->si_addr );
        break;
    case BUS_ADRERR:
        fprintf( stderr, "\nBUSERR: Non-existent physical address 0x%x\n",
                 info->si_addr );
        break;
    case BUS_OBJERR:
        fprintf( stderr, "\nBUSERR: Object specific hardware error 0x%x\n",
                 info->si_addr );
        break;
    default:
        fprintf( stderr, "Unknown signal code.\n" );
        exit(0);
    }
    dump_registers( context->uc_mcontext.gregs );
    longjmp(env,signal);
}

/**
 */
int init_handlers() {
    struct sigaction action;
    int signal;
    sigset_t set;

    action.sa_flags     = SA_SIGINFO;
    action.sa_sigaction = SIGINT_handler;
    if (sigaction( SIGINT, &action, 0 )) {
        fprintf( stderr, "Cannot modify SIGINT handler.\n" );
    }
    action.sa_sigaction = SIGSEGV_handler;
    if (sigaction( SIGSEGV, &action, 0 )) {
        fprintf( stderr, "Cannot modify SIGSEGV handler.\n" );
    }
    action.sa_sigaction = SIGBUS_handler;
    if (sigaction( SIGBUS, &action, 0 )) {
        fprintf( stderr, "Cannot modify SIGBUS handler.\n" );
    }

    signal = setjmp(env);
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGSEGV);
    sigaddset(&set,SIGBUS);

    switch (signal) {
    case 0: break;
    case SIGINT:
        break;
    case SIGQUIT:
        exit(0);
    case SIGBUS:
    case SIGSEGV:
        fprintf( stderr, "Returning from TRAP.\n" );
    default:
        break;
    }
    if (sigprocmask( SIG_UNBLOCK, &set, 0 ))
        fprintf( stderr, "Failed to clear procmask.\n" );

    return signal;
}

/*
 * vim:autoindent
 * vim:expandtab
 */
