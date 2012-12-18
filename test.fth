
variable file
: ftest 
   " blurfle" r/w open-file drop file !
   " Hello this is a test" file @ write-line drop
   file @ flush-file drop
   file @ file-size drop ." File size: " .d cr

   h# 90000 d# 1024
   PROT_READ PROT_WRITE or
   MAP_SHARED file @ 0
   mmap if
      ." mmap succeeded at address " . cr
   else
      ." mmap failed at address " . cr
   then

   file @ close-file drop
;

: zero  ( file size -- )
   " /dev/zero" r/o open-file drop >r
   0    \ use default address
   over \ file size
   PROT_READ
   MAP_SHARED
   r@   \ /dev/zero
   0    \ offset
   mmap 0= abort" Cannot mmap /dev/zero"
   r> close-file drop

   dup >r  ( R: addr )
   over >r ( R: addr size )
   -rot swap write-file
   \ Now write size to file
   \ close file
   \ munmap /dev/zero 
;

: initfile 
   " elf" r/w open-file drop file !
   file @ file-size drop 0= if
      ." File size is zero ... creating" cr
   then
;
