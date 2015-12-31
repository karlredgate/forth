
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

/** \file unix.c
 * \brief Words for OS access.
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>

extern int errno;

#include "compiler.h"
#include "kernel.h"

/**
 */
static void _getwd() {  /* ( -- adr len ) */
    char path[1024];

    if ( getcwd(path, 1024) == 0 ) {
        fprintf( stderr, "Cannot find CWD.\n" );
        /* This is a major error - probably want to jump to warm start?? */
        ip = Body("warm");
        return;
    }
    push( (Cell)path );
    push( (Cell)strlen(path) );
}

/**
 */
static void _getenv() {  /* ( adr len -- adr len true | false ) */
    char name[1024];
    char *val;

    strncpy( name, (char*)*sp, (int)tos );
    val = getenv( name );
    if ( val != NULL ) {
        *sp = (Cell)val;
        tos = strlen(val);
        push( TRUE );
        return;
    }
    pop(); tos = FALSE;
}

/**
 */
static void _setenv() {
}

/**
 */
static void _system() {  /* ( adr len -- errcode ) */
    char command[1024];
    char *val;

    strncpy( command, (char*)*sp, (int)tos );
    *(command + (int)tos) = '\0';
    system(command);
    pop(); tos = errno;
}

/**
 */
/* This needs to also have an arg vector */
static void _exec() {  /* ( adr len -- errcode ) */
    char path[1024];
    char *val;

    strncpy( path, (char*)*sp, (int)tos );
    *(path + (int)tos) = '\0';
    execl( path, NULL );
    pop(); tos = errno;
}

/**
 */
static void do_mmap() { /* ( start length prot flags fd offset -- addr f ) */
    void  *address;
    off_t  offset = (off_t)pop();
    int    fd     = fileno( (FILE *)pop() );
    int    flags  = (int)pop();
    int    prot   = (int)pop();
    size_t length = (size_t)pop();
    void  *start  = (void *)pop();

    address = mmap( start, length, prot, flags, fd, offset );

    if ( address == MAP_FAILED ) {
        push( errno );
        push( FALSE );
        return;
    }
    push( (Cell)address );
    push( TRUE );
}

/**
 */
static void _getpid() {  /* ( -- n ) */
    push( (Cell)getpid() );
}

/**
 */
static void _getppid() {  /* ( -- n ) */
    push( (Cell)getppid() );
}

/**
 */
static void _kill() {  /* ( pid signal -- errno ) */
    int signal = (int)pop();
    pid_t pid = (pid_t)pop();

    if ( kill(pid, signal) == 0 ) {
        push( (Cell)0 );
        return;
    }

    push( (Cell)errno );
}

/**
 */
void unix_words() {
    Primitive( "getwd", &_getwd );
    Colon( "pwd" ); c("getwd"); c("type"); c("cr"); End();

    Primitive( "(getenv)", &_getenv );
    Primitive( "(setenv)", &_setenv );
    Colon( "getenv" ); c("parse-word"); c("(getenv)"); End();
    Colon( "setenv" ); c("parse-word"); c("parse-word"); c("(setenv)"); End();
    Colon( "printenv" );
          c("parse-word"); c("2dup"); c("type");
          Literal((Cell)'='); c("emit");
          c("(getenv)"); If(); c("type"); Then(); c("cr"); End();

    Primitive( "(system)", &_system );
    Primitive( "(exec)",   &_exec );

    /* mmap() protection */
    Constant( "PROT_EXEC",  (Cell)PROT_EXEC );
    Constant( "PROT_READ",  (Cell)PROT_READ );
    Constant( "PROT_WRITE", (Cell)PROT_WRITE );
    Constant( "PROT_NONE",  (Cell)PROT_NONE );

    /* mmap() Flags */
    Constant( "MAP_FIXED",   (Cell)MAP_FIXED );
    Constant( "MAP_SHARED",  (Cell)MAP_SHARED );
    Constant( "MAP_PRIVATE", (Cell)MAP_PRIVATE );

    Primitive( "mmap", &do_mmap );

    Primitive( "getpid", &_getpid );
    Primitive( "getppid", &_getppid );

    /* Signals for kill */
    Constant( "SIGHUP",    (Cell)SIGHUP );
    Constant( "SIGINT",    (Cell)SIGINT );
    Constant( "SIGQUIT",   (Cell)SIGQUIT );
    Constant( "SIGILL",    (Cell)SIGILL );
    Constant( "SIGTRAP",   (Cell)SIGTRAP );
    Constant( "SIGABRT",   (Cell)SIGABRT );
    Constant( "SIGIOT",    (Cell)SIGIOT );
    Constant( "SIGBUS",    (Cell)SIGBUS );
    Constant( "SIGFPE",    (Cell)SIGFPE );
    Constant( "SIGKILL",   (Cell)SIGKILL );
    Constant( "SIGUSR1",   (Cell)SIGUSR1 );
    Constant( "SIGSEGV",   (Cell)SIGSEGV );
    Constant( "SIGUSR2",   (Cell)SIGUSR2 );
    Constant( "SIGPIPE",   (Cell)SIGPIPE );
    Constant( "SIGALRM",   (Cell)SIGALRM );
    Constant( "SIGTERM",   (Cell)SIGTERM );
    // Constant( "SIGSTKFLT", (Cell)SIGSTKFLT );
    // Constant( "SIGCLD",    (Cell)SIGCLD );
    Constant( "SIGCHLD",   (Cell)SIGCHLD );
    Constant( "SIGCONT",   (Cell)SIGCONT );
    Constant( "SIGSTOP",   (Cell)SIGSTOP );
    Constant( "SIGTSTP",   (Cell)SIGTSTP );

    Primitive( "kill", &_kill );
}

/*
 * vim:autoindent
 * vim:expandtab
 */
