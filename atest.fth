
\ Test the assembler

: .errno  ( ior -- )
   dup 0= if  drop exit  then
   \ print errno string ...??
   ." Errno: " .d cr
   abort
;

\ Map a whole file into the address space
: map-file-size  ( " filename" size -- addr )
   >r
   r/w open-file  .errno
   r> swap >r >r ( R: fid size )
   0  r>  PROT_READ PROT_WRITE or  MAP_SHARED
   r> 0
   mmap if  exit  then
;
: map-file  ( " filename" -- addr )
   dup file-size  .errno  \ should force a close of the file here ?
   ( fid size )
;

\ mmap /dev/zero for a specific size
: map-zero  ( size -- addr )
   >r " /dev/zero" r> map-file-size
;

\ Create file of N size and map it.
: create-mapped-file  ( " filename" size -- addr )
;

\ mmap a file

variable dict-file 
" dictionary.bin" r/w open-file dup if
   ." Errno: " .d cr abort
then
drop dict-file !

\

\ accessors
