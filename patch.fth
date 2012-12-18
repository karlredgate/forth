: (patch  ( new-acf old-acf acf-of-word-to-patch -- )
   >body
   begin
      dup @ ['] unnest = if  3drop abort  then
      over over @      = if  nip ! exit   then
      cell+
   again
;
: patch  ( "new old word-to-patch" -- )  ' ' ' (patch  ;
