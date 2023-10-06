
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

/** \file kernel.c
 * \brief forth kernel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

/* Stack Support */

#define STACK_SIZE (sizeof(Cell) * 100)

Cell tos, *sp, *rp;
APF ip;

/**
 */
inline void push( n ) Cell n; { *(--sp) = tos; tos = n; }
inline Cell pop()             { Cell scr = tos; tos = *(sp++); return scr; }
 
 /* dictionary support */

#define SMUDGE ((char)0x40)

typedef struct {
    Cell _dp, _lastacf, origin;
} wordlist;

wordlist *forth;

/* This should be lastacf(wordlist) ((ACF)((wordlist)->_lastacf)) */
#define lastacf ((ACF)(forth->_lastacf))
#define dp      (forth->_dp)

/* User Area Support */

static Cell user_area[1000];

#define numuser  (user_area[0])  /* #user */

/**
 */
Cell *Here() { return (Cell *)dp; }

/**
 */
static Cell
ualloc( int n ) {
    Cell unum = numuser;
    numuser += n;
    return unum;
}

/**
 */
static Cell
user_number( char *name ) {
    return (Cell)(*Body(name));
}

/**
 */
Cell *AddressOf(  name ) char *name; {
    return (Cell*)&(user_area[user_number(name)]);
}

/**
 */
void To( Cell value, char *name ) {
    *(AddressOf(name)) = value;
}

/* Code Fields */

static codefield do_create;
static codefield do_primitive;
static codefield do_constant;
static codefield do_colon;
static codefield do_variable;

/**
 */
static
void _do_create( APF apf, APF does ) {
    push( (Cell)apf );
}

/**
 */
static void
_do_primitive( APF apf, APF does ) {
    (*(PFV)(*apf))();
}

/**
 */
static void
_do_constant(  apf, does ) APF apf, does; {
    push( *((Cell *)apf) );
}

/**
 */
static void
_do_does( apf, does ) APF apf, does; {
    push( (Cell)apf );
    *(++rp) = (Cell)ip;
    ip = does;
}

/**
 */
static void
_do_colon( apf, does ) APF apf, does; {
    *(++rp) = (Cell)ip;
    ip = apf;
}

/**
 */
static void
_do_variable( apf, does ) APF apf, does; {
    push( (Cell)(user_area + (long)(*apf)) );
}

/**
 */
void align() {
    dp = (dp + (sizeof(Cell)-1)) & ~(sizeof(Cell) - 1);
}

/**
 */
ANF to_name(  acf ) ACF acf; { return ((char*)acf - sizeof(ACF)) - 1; }
ALF to_link(  acf ) ACF acf; { return (ALF)acf - 1; }
ACF previous( acf ) ACF acf; { return *to_link(acf); }
static void hide()   { *(to_name(lastacf)) |= SMUDGE; }
static void reveal() { *(to_name(lastacf)) &= ~SMUDGE; }

/**
 */
char *to_string( anf ) ANF anf; {
    static char buffer[32];
    strncpy( buffer, anf - (*anf), *anf );
    *(buffer + *anf) = 0;
    return buffer;
}

/**
 */
void
Execute( acf ) ACF acf; {
    codefield cf = *acf;
    APF      apf = (APF)(acf + 1);
    APF     does = (APF)(cf + 1);
    PFV     func = (*cf);

    (*func)( apf, does );
}

/**
 */
int
matches_word( char *name, ANF anf ) {
    char len = *anf & 0x1f;
    char *s  = anf - len;

    if (*anf & SMUDGE) return -1;
    if ((len <= 0) || (strlen(name) != len)) return -1;
    return strncmp( name, s, len );
}

/**
 */
void place( Cell value ) {
    *((Cell*)dp) = value;
    dp += sizeof(Cell);
}

/**
 */
/* DEBUGGING SUPPORT - this may be remvoed */
static void
print_stack() {
    Cell *p;
    Cell *sp0 = (Cell *)(*(AddressOf("sp0")));
    Cell *rp0 = (Cell *)(*(AddressOf("rp0")));

    fprintf( stderr, " (" );
    for ( p = (sp0-1) ; p >= sp ; p-- ) {
        fprintf( stderr, " %lx", *p );
    }
    if ( sp <= sp0 )  fprintf( stderr, " %lx", tos );
    fprintf( stderr, " )\n" );

    fprintf( stderr, " ( R:" );
    for ( p = rp0 ; p <= rp ; p++ ) {
        fprintf( stderr, " %lx", *p );
    }
    fprintf( stderr, " ) ip=%lx\n", ip );
}

/**
 */
static void
__dprintf( APF apf, APF does ) {
    fprintf( stderr, "%s\n", (char*)*apf );
    /* print_stack(); */
}

/**
 */
static PFV _dprintf = &__dprintf;
void
Dprintf( char *s ) {
    ACF acf = (ACF)malloc( sizeof(codefield) + sizeof(char*) );
    *acf = &_dprintf;
    *(acf+1) = (codefield)s;
    place((Cell)acf);
}
/* DEBUGGING SUPPORT - this may be removed */

/**
 */
void place_string( char *s ) {
    int length = (strlen(s) & 0xff);

    char *p = (char *)dp;
    *p = (unsigned char)length;
    dp++;
    strncpy( (char*)dp, s, length );
    dp += length;
    align();
}

/**
 */
void place_cstring( char *s ) {
    int length = strlen(s);
    strncpy( (char*)dp, s, length+1 );
    dp += length;
    align();
}

/* FIX - remove lose for initial dictionary - use lose only for
   words added by user */
/**
 */
static void
lose() {
    fprintf( stderr, "Undefined word encountered: %s\n", (char*)ip );
    ip = Body("quit");
}

/**
 */
/* ( adr len wid -- 0 | xt -1 | xt 1 ) */
static void
search_wordlist( APF apf, APF does ) {
    char name[33];
    int len;
    ACF acf;

    len = (int)(*sp++);
    strncpy( name, (char*)(*sp), len );
    *(name+len) = '\0';

    acf = (ACF)(((wordlist*)tos)->_lastacf);
    while ( acf ) {
        if ( matches_word(name, to_name(acf)) == 0 ) {
            *sp = (Cell)acf;
            tos = ( *(to_name(acf)) & 0x80 ) ? (Cell)-1 : (Cell)1;
            return;
        }
        acf = previous(acf);
    }
    tos = (Cell)0;
    sp++;
}

ACF loseacf;

/* FIX - find would fail if it were used to create the first word
   in the dictionary, since lastacf is 0 at that point 
   and find dereferences it before testing it for 0 */

/**
 */
ACF find( char *name ) {
    ACF acf = lastacf;

    while ( matches_word(name, to_name(acf)) ) {
        acf = previous(acf);
        if ( acf == 0 ) {
            fprintf( stderr, "Error: find(%s) not found.\n", name );
            place( (Cell)loseacf );
            place_cstring( name );
            return loseacf;  /* do I want this here?? */
        }
    }
    return acf;
}

/**
 */
void c( char *name ) { place( (Cell)find(name) ); }

/* change namelen byte to set high bit */
void immediate() { ANF name = to_name( lastacf ); *name |= 0x80; }

void Literal( n )       Cell n;  { c("lit");   place(n); }
void Tick( s )          char *s; { Literal((Cell)find(s)); }
void DotQuote( s )      char *s; { c("(.\")");     place_string(s); }
void StringLiteral( s ) char *s; { c("(\")");      place_string(s); }
void Abort( s )         char *s; { c("(abort\")"); place_string(s); }

void Recurse()      { place((Cell)lastacf); }
void TailRecurse()  { c("r>"); c("drop"); Recurse(); }

void End()                { c("unnest"); reveal(); }

APF Body( name ) char *name; { return (APF)(find(name)+1); }
Cell constant(    name ) char *name; { return (Cell)(*Body(name)); }

/**
 */
void
Header( char *name ) {
    int len = strlen(name);

    /* create name field with name byte */

    /*
     * Move forward enough for the length of the name
     * and the length byte.
     */
    dp += (len + 1);

    /* move the dp forward to the next aligned addess */
    align();

    /*
     * store the length byte - one byte address back from
     * the current dp.
     */
    *((char*)dp - 1) = (char)len;

    /*
     * store the name just before the length byte.
     * NOT NULL terminated.
     */
    bcopy( name, (((char*)dp) - len) - 1, len );

    /*
     * place a Cell sized dictionary pointer as the 
     * link field (ALF).
     */
    place( (Cell)lastacf );   /* link field = lastacf */

    /*
     * set the lastacf global to the current code field
     * address (ACF).
     */
    forth->_lastacf = dp;             /* save current acf */
}

/**
 */
codefield
CodeField( char *name, PFV f ) {
    codefield cf;
    Header( name );
    place( (Cell)do_create );
    cf = (codefield)dp;
    place( (Cell)f );
    return cf;
}

/**
 */
void
Primitive( char *name, PFV f ) {
    Header( name );
    place( (Cell)do_primitive );
    place( (Cell)f );
}

/**
 */
void
Colon( char *name ) {
    Header( name );
    place( (Cell)do_colon );
    hide();
}

/**
 */
void
Constant( char *name, Cell n ) {
    Header( name );
    place( (Cell)do_constant );
    place( n );
}

/**
 */
void
nVariable( char *name, int n ) {
    Header( name );
    place( (Cell)do_variable );
    place( ualloc(n) );
}

/**
 */
void
Variable( char *name ) {
    nVariable( name, 1 );
}

/**
 */
void
ColdChain( char *s ) {
    Colon( "cold-chain" );
    if (debug) Dprintf( s );
    c("cold-chain");
}



/**
 */
void
init_compiler( int dictsize ) {
    /*
     * this should be set by environment or command line ...
     */

    debug = 1;
    forth         = (wordlist *)malloc( sizeof(wordlist) );
    forth->origin = (Cell)malloc( (size_t)dictsize );
    dp            = forth->origin;
    forth->_lastacf = (Cell)0;

    Header( "do-create" );
    place( (Cell)(dp + sizeof(Cell)) );
    do_create = (codefield)dp;
    place( (Cell)(&_do_create) );

    do_primitive = CodeField( "do-primitive", &_do_primitive );
    do_colon     = CodeField( "do-colon",     &_do_colon );
    do_variable  = CodeField( "do-variable",  &_do_variable );
    do_constant  = CodeField( "do-constant",  &_do_constant );

    Constant( "do-does", (Cell)(&_do_does) );

    Header( "#user" );          place( (Cell)do_variable ); place( (Cell)0 );
    Header( "'tib" );           place( (Cell)do_variable ); place( (Cell)3 );
    Header( ">in" );            place( (Cell)do_variable ); place( (Cell)4 );
    Header( "debug" );          place( (Cell)do_variable ); place( (Cell)5 );
    numuser = 6;

    Constant( "forth-wordlist", (Cell)forth );
    Constant( "up@", (Cell)&user_area );

    Primitive( "lose", &lose ); loseacf = lastacf;

    Primitive( "search-wordlist", &search_wordlist );
}

/**
 */
void
StartVM( char *start_word, int argc, char **argv ) {
    extern int init_handlers();
    ACF acf;
    int i;

    /* These will not be modified by command line arguments, but by
     * environment variables.
     */

    *(AddressOf("sp0")) = (Cell)malloc(STACK_SIZE) + STACK_SIZE;
    sp = (Cell*)*(AddressOf("sp0"));
    sp++;

    /* *(AddressOf("rp0")) = (Cell)malloc(STACK_SIZE) + STACK_SIZE; */
    *(AddressOf("rp0")) = (Cell)malloc(STACK_SIZE);
    rp = (Cell*)*(AddressOf("rp0"));

    ip = Body(start_word);

    /* for ( i = (argc-1) ; i >= 0 ; i-- ) { */
    for ( i = argc ; i >= 0 ; i-- ) {
        push( (Cell)(argv[i]) );
    }
    push( argc );

/* FIX - if we ^C too many times in a row then find("quit") fails and
   returns loseacf - find should not return loseacf, but maybe -1 and
   we should check that failure here */

    if (init_handlers()) { ip = Body("warm"); }

    fflush(stdout);
    for (;;) {
        acf = *ip++;
        Execute( acf );
    }
}

/* vim: set autoindent expandtab : */
