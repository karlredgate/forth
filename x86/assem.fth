\
\ Assembler for Intel x86
\

vocabulary assembler

also assembler definitions

vector: inst,

0 constant ax 0 constant eax
1 constant cx 1 constant ecx
2 constant dx 2 constant edx
3 constant bx 3 constant ebx
4 constant sp 4 constant esp
5 constant bp 5 constant ebp
6 constant si 6 constant esi
7 constant di 7 constant edi

: opcode0:
   create ,
   does>
      @ inst,
;

h# 27 opcode0: daa
h# 37 opcode0: aaa
h# 2f opcode0: das
h# 3f opcode0: aas
h# 90 opcode0: nop
h# c9 opcode0: leave
h# d4 opcode0: aam
h# d5 opcode0: aad
h# d7 opcode0: xlat
h# f0 opcode0: lock
h# f2 opcode0: repne
h# f3 opcode0: rep
h# f3 opcode0: repe
h# f4 opcode0: hlt
h# f5 opcode0: cmc
h# f8 opcode0: clc
h# f9 opcode0: stc
h# fa opcode0: cli
h# fb opcode0: sti
h# fc opcode0: cld
h# fd opcode0: std


previous definitions
