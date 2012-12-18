: \       -1 parse 2drop  ; immediate
: //      -1 parse 2drop  ; immediate
: [char]  parse-word drop c@ literal  ; immediate
: [']     ' literal  ; immediate
: ?       @ .  ;
: (       [char] ) parse 2drop  ;  immediate
: .(      [char] ) parse type   ;  immediate
: [compile]  ' ,  ; immediate
: noop  ;
\ FIX ME=====================
\ These should not be interpreted because they need to be
\ forced to be treated as numbers.
: d#  base( decimal parse-word interpret-word )base  ; immediate
: h#  base( hex     parse-word interpret-word )base  ; immediate
: pad     here [ d# 128 literal ] + ;
: ualloc  ( n -- user# )  #user dup @ >r +! r>  ;
: debug?  ( -- f )  debug @  ;

\ .( Compiling control flow words ) cr
: mark-forward     ( -- F )  here 0 ,  ;
: resolve-forward  ( F -- )  here over - /token / swap !  ;
: mark-backward    ( -- B )  here  ;
: resolve-backward ( B -- )  here - /token / ,  ;

: begin  ( -- B )  mark-backward  ;  immediate
: if     ( -- F )  compile ?branch mark-forward  ;  immediate
: while  ( -- F )  [compile] if swap  ; immediate
: ahead  ( -- F )  compile branch mark-forward  ; immediate
: until  ( B -- )  compile ?branch resolve-backward  ; immediate
: again  ( B -- )  compile  branch resolve-backward  ; immediate
: then   ( F -- )  resolve-forward  ; immediate

: repeat ( F B -- )  [compile] again [compile] then  ; immediate
: else   ( F B -- )  [compile] ahead swap [compile] then  ; immediate

\ .( Compiling string words ) cr
: pack      ( adr len dst -- dst )  dup >r  2dup c! 1+ swap cmove  r>  ;
: place     ( adr len dst -- )      pack drop  ;
: ",        ( adr len -- )          here over 1+ allot place align  ;

: (s")      ( -- adr len )  ip> count 2dup + talign >ip  ;
: (c")      ( -- adr )      ip> dup  count + talign >ip  ;
: "parse",  ( "..." -- )    [char] " parse ",  ;
: s"        ( "..." -- )    compile (s")  "parse",  ; immediate
: c"        ( "..." -- )    compile (c")  "parse",  ; immediate
: ."        ( "..." -- )    compile (.")  "parse",  ; immediate
: "         ( "..." -- )    compile (")   "parse",  ; immediate
: null$     ( -- adr len )  0 0  ;
: $=        ( adr1 len1 adr2 len2 -- f )  compare 0=  ;

: skip-string ( adr -- adr )      dup c@ 1+ talign +  ;
: c+!         ( n adr -- )        tuck c@ + swap c!  ;
: concat      ( adr len dst -- )  2dup 2>r  count + swap cmove  2r> c+!  ;
: save-cstring  ( adr len -- adr )
   dup 1+ allocate >r  ( adr len R: dst )
   0 over 1+ r@ + c!  \ NULL terminate
   r> swap cmove
;

\ .( Compiling exception words ) cr
: abort"  ( "..." -- )
   state @ 0= if 
      [char] " parse rot
      0= if 2drop exit then
      type cr abort
   then
   compile (abort")  "parse",
; immediate

\ .( Compiling defining words ) cr
: postpone  ( -- ) \ name
   ' dup immediate? 0= if compile compile then  compile,
; immediate
: constant  ( n -- ) \ name
   create ,  do-constant codefield
;
: variable  ( -- ) \ name
   create  do-variable codefield
   1 ualloc ,
;

: (does>)  ( -- )  ip@ ta1+ codefield  ;
: does>    ( -- )
   compile (does>)
   compile unnest
   do-does ,  ]
; immediate

: do-vocabulary  ( apf -- )
   @ >r get-order nip r> swap set-order
;
: vocabulary:  ( wid -- ) \ name
   create ,
   does>  do-vocabulary
;
: vocabulary  ( -- ) \ name
   wordlist vocabulary:
;
forth-wordlist vocabulary: forth
: vocabulary?  ( xt -- f )
   @ ['] forth @ =
;

: buffer:  ( n -- ) \ name
   create allocate throw ,
   does> @
;
\ : :noname  ( -- acf )  null$ header do-colon codefield ] lastacf  ;
\ :noname  ." Uninitialized vector:" cr abort  ;
: novector
   ." Uninitialized vector" cr abort  ;
;
: vector:  ( -- ) \ name
   create ['] novector ,
   does> @ execute
;
: defer  ( -- ) \ name
   create ['] novector ,
   does> @ execute
;

: addr  ( "name..." -- apf )  ' >body  ;
: is    ( n "name..." -- )    addr !  ;
: alias  ( -- ) \ new-name old-name
   vector: ' lastacf >body !
;

\ .( Compiling vocabulary words ) cr
: invalid-xt?  ( xt -- f )  origin here within 0=  ;
: .name  ( acf -- )
   dup invalid-xt? if  ." xt(" . ." ) " exit  then
   >name$ type
;
: map-wordlist  ( wid predicate -- xt t | f )
   >r
   >lastacf
   begin
      @ dup while
      r@ over >r execute if  ( xt pred R: pred xt )
	 r> r> drop true exit
      then
      r> >link
   repeat r> 2drop false
;

: user-matches?  ( n xt -- n f )
   dup vocabulary? 0= if  drop false exit  then
   >body @ over =
;
: wordlist>xt  ( wid -- xt )
   >r get-order
   begin
      dup while
      1- r@ rot ['] user-matches?
      map-wordlist if
	 r> drop   \ get rid of saved wid
	 >r        \ save xt we found
	 drop      \ get rid of wid used to find
	 ndrop     \ get rid of rest of context
	 r>
	 exit
      then
      drop
   repeat
   r> 2drop 0 ( change to invalid XT once interpreter does literals )
;
: order
   ." Context: "
   get-order 0 do
      wordlist>xt .name space
   loop
   ."    Current: "  current @ wordlist>xt .name
   cr
;

\ .( Compiling file words ) cr
vector: include-hook
: report-include ( adr len -- adr len )  ." include: " 2dup type cr  ;
' report-include is include-hook
: include  ( "..." -- )  parse-word include-hook included  ;

\ .( Compiling Forth 83 compatability words ) cr
variable span
: expect  ( adr len -- )  accept span !  ;
: query   ( -- )          tib /tib expect  ;
: word    ( c -- adr )    parse here pack  ;

\ .( Compiling autoload words ) cr
vector: autoload-hook
: report-autoload  ( adr len -- adr len )
   debug? 0=  if  exit  then
   ." autoloading: " 2dup type cr
;
' report-autoload is autoload-hook
: (autoload)  ( word-adr,len file-adr,len -- )
   2over header ", ",
   does>
      also forth definitions
      dup count autoload-hook included
      previous definitions
      skip-string count (find) if  execute  then
;
: autoload  ( -- )  parse-word parse-word (autoload)  ;

autoload dump   dump.fth
autoload see    see.fth
autoload words  words.fth
autoload patch  patch.fth
autoload (patch patch.fth
autoload vi     unix.fth
autoload ls     unix.fth
autoload make   unix.fth

\ .( Argument processing ) cr
variable executable-name
: parse-argument
   1- swap dup strlen
   \ Actually this would be better served if we made a word for each arg
   \ in a parse-args vocabulary
   2dup " -s" $= if  2drop 1- swap dup strlen evaluate exit  then
   ." Invalid argument: " type cr
;
: parse-arguments  ( argN ... arg1 N -- )
   1- swap  executable-name !
   begin
      dup 0> while
      parse-argument
   repeat
;
' parse-arguments argument-hook !

: restart
   3 cells allocate dup >r
   executable-name over ! cell+
   " -restart" save-cstring dup >r over ! cell+
   0 over !  drop
   executable-name (exec)
   r> free  r> free
;

variable fence  here fence !
\ This forget is broken
: forget
   parse-word (find) 0= if
      ." Cannot find " type cr abort
   then
   dup fence @ < if
      drop ." Cannot forget to words before the fence." cr abort
   then
   >link dup @ 'lastacf !
   >name dp ! aligned
;

: headers     ." Warning: headers not implemented."    cr  ;
: headerless  ." Warning: headerless not implemented." cr  ;

: $$  ( -- pid )  getpid  ;
