
h# 1f constant register-mask

: iregister: ( n -- )  h# a0000000 + constant  ;
: fregister: ( n -- )  h# b0000000 + constant  ;
: aregister: ( n -- )  h# c0000000 + constant  ;

: iregister?  ( reg -- f )  d# 28 >> h# f and h# a =  ;
: fregister?  ( reg -- f )  d# 28 >> h# f and h# b =  ;
: aregister?  ( reg -- f )  d# 28 >> h# f and h# c =  ;


0 iregister: %r0   0 iregister: %g0
1 iregister: %r1   1 iregister: %g1
2 iregister: %r2   2 iregister: %g2
3 iregister: %r3   3 iregister: %g3
4 iregister: %r4   4 iregister: %g4
5 iregister: %r5   5 iregister: %g5
6 iregister: %r6   6 iregister: %g6
7 iregister: %r7   7 iregister: %g7
\ 0 iregister: %r0   0 iregister: %o0

: rd  ( reg opcode -- opcode' )
   swap
   dup iregister? 0= abort" Must use integer register for RD."
   register-mask and d# 25 << or
;
