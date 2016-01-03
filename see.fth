\
\ Decompiler words.
\
\ TODO:
\
\ - Detect defer / vector: words and show target
\ - Detect create, does> words and dump better
\

vocabulary decompiler
also decompiler definitions

: tab  ht emit  ;
: >target    ( ip-of-branch -- adr )  ta1+ dup @ /token * +  ;
: forward?   ( ip-of-branch -- f )    ta1+ @ 0>  ;
: backward?  ( ip-of-branch -- f )    ta1+ @ 0>  ;

: .type
   dup ['] (do)     = if  drop  ." do" cr tab tab  exit  then
   dup ['] (loop)   = if  drop  ta1+ cr tab ." loop" cr tab  exit  then
   dup ['] lit      = if  drop  ta1+ ."  h# " dup @ . exit  then
   dup ['] branch   = if  .name ta1+ ."  h# " dup @ . exit then
   dup ['] ?branch  = if  .name ta1+ ."  h# " dup @ . exit then
   dup ['] (.")     = if
      [char] . emit [char] " emit space
      drop ta1+  dup count type
      [char] " emit space
      dup c@ 1+ talign + /token -
      exit
   then
   dup ['] (abort") = if
      ." abort" [char] " emit space 
      drop ta1+ dup count type [char] " emit space
      dup c@ talign + /token - exit
   then
   .name space
;
: .variable
   ." variable " dup .name
   ."   contents: " >body >user @ . cr
;
: .constant
   dup >body @ . ."  constant " .name cr
;
: .:
   ." : " dup .name cr
   dup
   tab >body
   begin
      dup @ ['] unnest <> while
      dup @ .type
      ta1+
   repeat
   drop cr ." ; "
   immediate? if ." immediate" then
   cr
;
previous definitions

also decompiler
: (see)
   dup @ do-variable  = if  .variable exit  then
   dup @ do-constant  = if  .constant exit  then
   dup @ do-primitive = if  ." primitive " .name cr exit  then
   dup @ do-colon     = if  .:  exit  then
   dup @ do-create    = if  ." create " .name cr exit  then
   ." create " .name cr  ."   does> " cr
;
: see  ( -- ) \ name
   ' (see)
;
previous
