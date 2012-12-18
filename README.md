Forth
=====

A toy Forth interpreter to demonstrate how Forth works.

I wrote this a few years ago as a demonstration of a Forth interpreter
entirely in C.  It is a pseudo-ITC implementation. This is still a 
work in progress.

This will only build on 32 bit machines currently.

--------------------------

NOTES:
-----
This cannot be linked static currently, because the readline library
calls getpwnam and the current libc.a uses nss calls which require
dynamic linking to work.

If stacks were initialized before the dictionary was created - could
words be executed to build the dictionary?  Is this worthwhile?

Bugs:

o  executable-name is being set incorrectly
o  Need bye defined earlier in the system ... so it can be used in the
   cold-chain
o  #user seems to have a bogus value once the interpreter is up
   it looks like it has an acf stored there instead of a user#
o  outer interpreter doesn't compile literals while in compile mode (FIXED)
o  u. in hex is printing garbage (FIXED 7jul99)
o  find doesn't check smudge bit when searching
   - fix search-wordlist in vocabulary.c
o  Decompiler bugs:
   - need special treatment for compile ... read next word .name
     then jump past to next word
   - doesn't report immediate-ness
o  Need to track top of memory and realloc when we use it all.

ToDo:

1  Add library search path code to find .fth files to load;
   add autoload search for file in library code to find words.
2  Change $find code to be faster.
3  Add w@, c@ ... short char words

o  Fix so missing base.fth gives error - bus doesn't die!!
o  Fix "words" so it uses the top of context instead of current
  (see words.fth)
o  Add words for ELF handling
o  Signal handling for Linux

o  Maybe move unnest from primitives to kernel.c - then move cold-code
   first definition from primitive to kernel
o  Change the "not found" functionality to try and do an autoload??
o  Maybe change control flow words to be in compiler vocabulary
   and make : change search order ??
o  Need to use a search path for loading forth files
o  Change fileio words to use mmap
o  Change hide reveal immediate words to have sub words that work
   on an xt then have these words use lastacf mumble to set the bits
o  Be cool to implement the memory allocation wordset as brk/sbrk
   but - need to remove readline first, because it uses malloc and
   you can't mix malloc usage and brk/sbrk.
o  implement unloop ... alias of leave ??
o  implement case statement
o  implement forget and marker
o  Add : split  ( adr len -- adr1 len1 ... adrn lenn n ) \ char to split on
o  Add argv and envp to the environment ... pass as arguments to StartVM ??
   - maybe add to unix.c and make an array of pointers to counted strings
o  Add fload .forthrc to cold
o  Maybe provide access to dlopen ... and use that to implement the ELF
   vocabulary.
   --- generate dlopen ( adr len -- fid )
       which reads the DLL and generates a vocabulary called the
       name of the file with words for wach function in the file?
o  Add cmd line args to the stack, parse args in cold  (DONE)
o  Add defer words ... maybe call them vector ... then change included
   to use this for printing file names when loaded.  (DONE)
o  Forth debugger
   [ change NEXT implementation to save context and prompt (or to
   print current word and stack and prompt)]
