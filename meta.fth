
\ label must create a word in the target dictionary
\ and create a word in the meta dictionary so that when compiling
\ the address found (via find) will be the offset in the target
\ dictionary from the the start of the target.

\ Probably the find in the meta vocabulary should execute that word,
\ which would then push its address on the stack???

\ The target vocabulary holds the meta-compilation time words representing
\ the words that exist in the target image.  These are used to generate
\ the correct addresses in the target.
vocabulary target

\ The meta vocabulary holds the defining words which compile into the
\ target dictionary.
vocabulary meta
also meta definitions

h# 8.0000 allocate abort" Could not allocate memory for target dictionary."

: label
   \ create header in target image
   \ generate execute word in target vocabulary that "does the right thing"
;

label docreate
   \ push body of current word on stack
   \ NEXT
end-code

: create
   header
   docreate codefield,
;

: ;code
   ip@ codefield,
   state off
; immediate

: :
   create
   state on
;code
   \ push current IP on RP
   \ set IP to the body of the current word
c;
