\ unix access words
: system-command  ( adr len -- errno )
   pad place
   -1 parse pad concat
   pad count (system)
;

: vi  " vi " system-command  ;
: ls  " ls " system-command  ;
: make
   " make " system-command
   restart
;

   \ 4 cells allocate dup
   \ except this needs to be a c-string and so needs to be allocated
   \ and copied to the new space then null terminated
   \ executable-name over !  cell+
   \ 0 over !
   \ drop   executable-name count (exec)

