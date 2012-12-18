\
\ Create OBP style "action" objects.
\

: actions  ( n -- adr adr )
   here  swap cells allot  dup
;

: action:  ( adr -- adr' )
   here over ! cell+
   do-colon ,  ]
;

: (action)
   ip> dup cell+ >ip
   @ codefield
;

: use-actions  ( adr adr -- )
   compile (action)
   drop ,
; immediate

: perform-action  ( xt n -- )
   dup 0= if r> 2drop execute then
   over >body -rot
   swap @ swap cells -
   r> drop execute
;
: get  ( xt -- val )  0 perform-action  ;
: set  ( val xt -- )  1 perform-action  ;

3 actions
action: @ ;
action: ! ;
action: ;
: foo:  create , use-actions  ;

