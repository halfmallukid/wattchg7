	file	 "hello.c"
data

; cc1 (2.7.2.2) arguments: -O -fdefer-pop -fomit-frame-pointer
; -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations
; -fthread-jumps -fstrength-reduce -funroll-loops -fwritable-strings
; -fpeephole -fforce-mem -ffunction-cse -finline-functions -finline
; -freg-struct-return -fdelayed-branch -frerun-cse-after-loop
; -fschedule-insns -fschedule-insns2 -fcommon -fgnu-linker -m88110 -m88100
; -m88000 -mno-ocs-debug-info -mno-ocs-frame-position -mcheck-zero-division

gcc2_compiled.:
	align	 8
@LC0:
	string	 "Hello world\n\000"
text
	align	 8
	global	 _main
_main:
	or.u	 r2,r0,hi16(@LC0)
	subu	 r31,r31,48
	st	 r1,r31,36
@Ltb0:
	bsr.n	 _printf
	or	 r2,r2,lo16(@LC0)
@Lte0:
	ld	 r1,r31,36
	jmp.n	 r1
	addu	 r31,r31,48

