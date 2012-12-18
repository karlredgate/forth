
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

/** \file file.c
 * \brief File interface words
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

extern int errno;

#include "compiler.h"

/**
 */
static void create_file() {  /* ( adr len fam -- fid ior ) */
}

/* Defined on page 84 of ANS Spec
 * ( c-addr u fam -- fileid ior )
 */
static void
open_file() {  /* ( adr len fam -- fid ior ) */
    char *mode = (char*)tos;
    int len = (int)*sp++;
    char name[1024];
    FILE *f;

    strncpy( name, (char*)*sp, len );
    *(name+len) = '\0';
    f = fopen(name, mode);
    *sp = (Cell)f;
    tos = ( f == NULL ) ? errno : 0;
}

/**
 */
static void
close_file() {  /* ( fid -- ior ) */
    tos = (Cell)fclose( (FILE*)tos );
}

/**
 */
static void
delete_file() {  /* ( adr len -- ior ) */
}

/**
 */
static void
file_position() {  /* ( fid -- ud ior ) */
    *(--sp) = (Cell)ftell( (FILE*)tos );
    tos = (Cell)errno;
}

/**
 */
static void
reposition_file() {  /* ( ud fid -- ior ) */
    fseek( (FILE*)tos, (long)*sp++, SEEK_SET );
    tos = (Cell)errno;
}

/**
 */
static void
file_size() {  /* ( fid -- ud ior ) */
    struct stat buffer;

    fstat( fileno((FILE*)tos), &buffer );
    *(--sp) = (Cell)(buffer.st_size);
    tos = (Cell)errno;
}

/**
 */
static void
resize_file() {  /* ( ud fid -- ior ) */
}

/* Defined on page 86 of ANS Spec
 * ( c-addr u fileid -- u2 flag ior )
 */
static void
read_line() {  /* ( adr len fid -- len2 f ior ) */
    if ( feof((FILE*)tos) ) {
        tos     = (Cell)0;
        *sp     = FALSE;
        *(sp+1) = (Cell)0;
        return;
    }
    if ( fgets( (char*)*(sp+1), (int)*sp, (FILE*)tos ) ) {
        *sp     = TRUE;
        *(sp+1) = (Cell)strlen( (char*)*(sp+1) );
    } else {
        *sp     = FALSE;
        *(sp+1) = (Cell)0;
    }
    tos     = (Cell)ferror( (FILE*)tos );
}

/**
 */
static void
read_file() {  /* ( adr len fid -- len2 ior ) */
    size_t len = (size_t)*sp++;
    *sp = (Cell)fread( (void*)*sp, (size_t)1, len, (FILE*)tos );
    tos = ferror( (FILE*)tos );
}

/**
 */
static void
write_line() {  /* ( adr len fid -- ior ) */
    int len = (int)*sp++;
    char buffer[1024];

    strncpy( buffer, (char*)*sp++, len );
    *(buffer+len) = '\0';
    fputs( buffer, (FILE*)tos );
    tos = (Cell)ferror( (FILE*)tos );
}

/**
 */
static void
write_file() {  /* ( adr len fid -- ior ) */
    size_t len = (size_t)*sp++;
    fwrite( (void*)*sp++, (size_t)1, len, (FILE*)tos );
    tos = (Cell)ferror( (FILE*)tos );
}

/**
 */
static void
flush_file() {  /* ( fid -- ior ) */
    fflush( (FILE*)tos );
    tos = (Cell)errno;
}

/**
 */
void
file_words() {
    Variable( "file" );
    Constant( "/file", (Cell)1024 );
    Variable( "'file-buffer" );
    Colon( "file-buffer" );  c("'file-buffer"); c("@"); End();

    Colon( "new-file-buffer" );
          c("/file"); c("allocate"); Abort("Cannot allocate file buffer.");
          c("'file-buffer"); c("!");
          End();

    ColdChain( "Initializing file buffer and variable." );
          c("new-file-buffer"); c("0"); c("file"); c("!"); End();

    Primitive( "create-file",     &create_file );
    Primitive( "open-file",       &open_file );
    Primitive( "close-file",      &close_file );
    Primitive( "delete-file",     &delete_file );
    Primitive( "file-position",   &file_position );
    Primitive( "reposition-file", &reposition_file );
    Primitive( "file-size",       &file_size );
    Primitive( "resize-file",     &resize_file );
    Primitive( "read-line",       &read_line );
    Primitive( "read-file",       &read_file );
    Primitive( "write-line",      &write_line );
    Primitive( "write-file",      &write_file );
    Primitive( "flush-file",      &flush_file );

    Colon( "r/o" ); c("(cstr\")"); place_cstring("r"); End();
    Colon( "r/w" ); c("(cstr\")"); place_cstring("a+"); End();
    Colon( "bin" ); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
