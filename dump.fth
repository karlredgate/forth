\
\ Memory dump words
\

d# 16 constant /dump
: .addr  ( adr -- )
   s>d <# [char] : hold # # # # [char] . hold # # # # #> type
;
: .bytes  ( adr -- )
   /dump bounds do
      i c@ 2 u.r0 space
   loop
;
: .ascii  ( adr -- )
   dup ascii? if emit exit then
   drop [char] . emit
;
: .chars  ( adr -- )
   /dump bounds do  i c@ .ascii  loop
;
: .line  ( adr -- )
   dup .addr space  dup .bytes space  .chars cr
;
: dump  ( adr len -- )
   base( hex
   [ d# 11 literal ] spaces  over
   [ d# 16 literal ] mod 3 * spaces  [char] v emit cr
   swap /dump round-down
   swap /dump round-up
   bounds do
      i .line
   /dump +loop )base
;
