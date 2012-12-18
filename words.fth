\
\ List all words in the current vocabulary
\

\ Actually this shouldn't use lastacf, which is the current definitions
\ vocabulary; but the lastacf of the top of context
: words
   lastacf
   begin  dup  while
      dup .name space >link @
   repeat
   drop cr
;
