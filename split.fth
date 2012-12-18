: $parse  ( adr len c -- adr' len' )
   \ ???
;
: split  ( adr len -- adr-1 len-1 ... adr-n len-n n ) \ char to split on
   begin
   while
   repeat
;

