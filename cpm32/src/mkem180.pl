#!/usr/bin/perl
# mkem180.pl -- make 64180(Z80) emulator

$z80HFflag = 0;

# ---------- print header -----------
print <<EOD;
;**************************************************************
;*         HD64180(Z80) emulator for WIN32 v0.2               *
;*                      em180.c                               *
;*         Copyleft (C) 2004-2012 by K.Murakami               *
;**************************************************************
	.386
	.model flat

	public _reg
	public _mem
	public _guard
	public _em180

	extern	_io_input:near
	extern	_io_output:near

SF	equ	10000000b
ZF	equ	01000000b
HF	equ	00010000b
PF	equ	00000100b
VF	equ	PF
NF	equ	00000010b
CF	equ	00000001b

_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends

_BSS	segment dword public use32 'BSS'
	align	4

_reg	label	byte

_BC	label	word
_C	db	?
_B	db	?
_DE	label	word
_E	db	?
_D	db	?
_HL	label	word
_L	db	?
_H	db	?
_AF	label	word
_F	db	?
_A	db	?
_IX	label	word
_IXL	db      ?
_IXH	db	?
_IY	label	word
_IYL	db	?
_IYH	db	?
_BC2	dw	?
_DE2	dw	?
_HL2	dw	?
_AF2	dw	?
_SP	dw	?
_PC	dw	?
_I	db	?
_R	db	?
_IE	db	?
	db	?

_mem	label	byte
_memw	label	word
	db	10000h dup(?)
_guard	label	byte
	db	10h dup(?)

_BSS	ends

_TEXT	segment dword public use32 'CODE'

EOD

$go_next = <<EOD;
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]
EOD

print <<EOD;

_em180	proc	near
	push	ebp
	push	ebx
	push	esi
	push	edi
	xor	edi, edi
	movzx	esi, _PC
	movzx	ebp, _SP
	movzx	ebx, _HL
	movzx	ecx, _AF
noop:
$go_next

illegalCB:
	or	ax,0cb00h
	jmp	short illegal
illegalED:
	or	ax,0ed00h
	jmp	short illegal
illegalDD:
;	or	ax,0dd00h
;	jmp	short illegal
	jmp	func[eax*4]
illegalFD:
;	or	ax,0fd00h
;	jmp	short illegal
	jmp	func[eax*4]
illegalDDCB:
	or	eax,0ddcb00h
illegal:
exit:
	mov	_PC, si
	mov	_SP, bp
	mov	_HL, bx
	mov	_AF, cx
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret

rst_xx:	and	al,38h
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	esi,eax
$go_next
EOD

# ---------- macro definition -----------

sub mklabel {
    local ($l) = @_;
    $l =~ tr/(,)+d'_ /p_/s;
    $l;
}

@reg8 = ( 'B','C','D','E','H','L','A','F', 'I','R',
	  'IXH', 'IXL', 'IYH', 'IYL');
@reg16 = ( 'BC','DE','HL','AF', 'IX','IY', 'PC','SP');
for (@reg8) { $bit{ $_} = 8;}
for (@reg16) { $bit{ $_} = 16;}
($SF, $ZF, $HF, $PF, $CF) = (0x80, 0x40, 0x10, 0x04, 0x01);


($A, $F, $H, $L) = ('ch', 'cl', 'bh', 'bl');
($AF, $HL, $SP, $PC) = ('cx', 'bx', 'bp', 'si');
%reg = ( 'A','ch', 'F','cl', 'H','bh', 'L','bl', 
	 'AF','cx', 'HL','bx', 'PC','si', 'SP','bp');
%asign = ( 'imm','_mem[esi]', '(abs)','_mem[edi]', 'add','di',
	   '(HL)',"_mem[e$HL]", '(BC)','_mem[edi]', '(DE)','_mem[edi]',
	   '(IX+d)','_mem[edi]', '(IY+d)','_mem[edi]',
	   "AF'",'_AF2', '(SP)','_memw[ebp]');
for (@reg8,@reg16) {
    $asign{ $_} = $reg{ $_} || "_$_";
}

%flag = ( 'NZ','ZF',  'Z','ZF', 'NC','CF', 'C','CF', 
	  'PO','PF', 'PE','PF', 'PL','SF', 'MI','SF');
%cc =   ( 'NZ','nz',  'Z','z', 'NC','nz', 'C','z', 
	  'PO','nz', 'PE','z', 'PL','nz', 'MI','z');

%op86 = ( 'AND','and', 'OR','or', 'XOR','xor', 'TST', 'test',
	  'ADD','add', 'ADC','adc', 'SUB','sub', 'SBC','sbb',
	  'CP', 'cmp', 'INC', 'inc', 'DEC','dec',
	  'RLCA','rol', 'RRCA','ror', 'RLA','rcl', 'RRA','rcr',
	  'RLC','rol', 'RRC','ror', 'RL','rcl', 'RR','rcr',
	  'SLA','sal', 'SRA','sar', 'SRL','shr', 'SL1','rcl');

# ---------- macro expand -----------

while (<>) {
    chop;
    ($code, $cpu, $op, $args) = split;
    ($arg1, $arg2) = split( /,/, $args);
    $code =~ /^([0-9A-F]*)([0-9A-F][0-9A-F])$/ || next;
    $precode = $1; $code = ord pack( 'H2', $2);
    $funclabel{ $#func+1} = $precode if $code == 0;

    next if $precode eq 'FDCB';

    $label = "$op_$args"; $label =~ tr/(,)/p_/;

#    if ( $op eq '-' || $cpu eq 'z80u') {
    if ( $op eq '-') {
	push( @func, "illegal$precode");

    } elsif ( $op eq 'HALT') {
	push( @func, &mklabel( "$op "));

print "$func[ $#func]:					; $op\n";
		print "	mov	al,1\n";
		print "	jmp	exit\n";

    } elsif ( $op eq 'NOP' || $op =~ /IM[012]/) {
	push( @func, 'noop');

    } elsif ( $op eq 'EI' || $op eq 'DI') {
	push( @func, &mklabel( "$op "));

print "$func[ $#func]:					; $op\n";
	if ( $op eq 'EI') {
		print "	mov	_IE,0ffh\n"; # 2012.03 set IEF1,2
	} else {
		print "	mov	_IE,0\n";    # 2012.03 reset IEF1,2
	}
print "$go_next\n";

    } elsif ( $op eq 'prebyte') {
	push( @func, &mklabel( "$op $precode$args"));

print "$func[ $#func]:					; $op $args\n";
	if ( $precode eq 'DD' || $precode eq 'FD') {
	    $src = ( $precode eq 'DD') ? '_IX' : '_IY';
	    print "	movsx	di,_mem[ esi]\n";
	    print "	inc	esi\n";
	    print "	add	di,$src\n\n";
	    $arg1 = "DD$arg1";
	}
	print "	movzx	eax,_mem[ esi]\n";
	print "	inc	esi\n";
	print "	jmp	func${arg1}[eax*4]\n\n";

    } elsif ( $op eq 'CALLN') {
	push( @func, &mklabel( "$op"));

print "$func[ $#func]:					; $op $args\n";
	print "	mov	al,_mem[ esi]\n";
	print "	inc	si\n";
	print "	mov	ah,1\n";
	print "	jmp	exit\n\n";

    } elsif ( $op eq 'RETEM') {
	push( @func, &mklabel( "$op"));

print "$func[ $#func]:					; $op $args\n";
	print "	xor	eax,eax\n";
	print "	jmp	exit\n\n";

    } elsif ( $op eq 'LD') {			#  --------- LD xx,xx
	push( @func, &mklabel( "$op $args"));
	next if ++$labels{ $func[ $#func]} > 1;
	$dst = $asign{ $arg1}; $src = $asign{ $arg2};
	$w = 'al';
	if ( $bit{ $arg1} == 16 || $bit{ $arg2} == 16) {
	    $src =~ s/_mem\[/_memw[/;
	    $dst =~ s/_mem\[/_memw[/;
	    $w = 'ax';
	}
	$w = '' if ( $reg{ $arg1} || $reg{ $arg2});

print "$func[ $#func]:					; $op $args\n";
	if ( $arg1 eq '(abs)' || $arg2 eq '(abs)') {
		print "	movzx	edi,_memw[e$PC]\n";
		print "	add	e$PC,2\n";
	} elsif ( $arg1 =~ /(I[XY])\+d/ || $arg2 =~ /(I[XY])\+d/) {
		print "	movsx	di,_mem[ esi]\n";
		print "	inc	esi\n";
		print "	add	di,_$1\n";
	} elsif ( $arg1 =~ /^\((.+)\)/ || $arg2 =~ /^\((.+)\)/) {
		print "	movzx	edi,$asign{ $1}\n" if !$reg{ $1};
	}
	if ( $w) {
		print "	mov	$w,$src\n";
		print "	mov	$dst,$w\n";
	} elsif ( $arg2 ne 'R') {
		print "	mov	$dst,$src\n";
	} else {
		print "	mov	al,19\n";
		print "	imul	$src\n";
		print "	inc	al\n";
		print "	mov	$dst,al\n";
		print "	mov	$src,al\n";
	}
	if ( $arg2 eq 'imm') {
	    if ( $src =~ /^_memw/) {
		print "	add	e$PC,2\n";
	    } else {
		print "	inc	e$PC\n";
	    }
	}
	#  2012.03 fix "LD A,R","LD A,I" flags
	if (( $arg2 eq 'I') || ( $arg2 eq 'R')) {
		print "	and	$F, not(HF or PF or NF)\n";
		print "	mov	al, _IE\n";
		print "	and	al, PF\n";
		print "	or	$F, al\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'EX') {			#  --------- EX xx,xx
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1}; $src = $asign{ $arg2};

print "$func[ $#func]:					; $op $args\n";
	if ( $reg{ $arg1} && $reg{ $arg2}) {
		print "	xchg	$dst,$src\n";
	} elsif ( $reg{ $arg1}) {
		print "	mov	ax,$src\n";
		print "	mov	$src,$dst\n";
		print "	mov	$dst,ax\n";
	} elsif ( $reg{ $arg2}) {
		print "	mov	ax,$dst\n";
		print "	mov	$dst,$src\n";
		print "	mov	$src,ax\n";
	} else { 
		print "	mov	ax,$dst\n";
		print "	mov	di,$src\n";
		print "	mov	$src,ax\n";
		print "	mov	$dst,di\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'EXX') {			#  --------- EX xx,xx
	push( @func, &mklabel( "$op"));

print "$func[ $#func]:					; $op $args\n";
	print "	mov	ax,_BC\n";
	print "	mov	di,_BC2\n";
	print "	mov	_BC2,ax\n";
	print "	mov	_BC,di\n";
	print "	mov	ax,_DE\n";
	print "	mov	di,_DE2\n";
	print "	mov	_DE2,ax\n";
	print "	mov	_DE,di\n";
	print "	mov	ax,_HL2\n";
	print "	mov	_HL2,$HL\n";
	print "	mov	$HL,ax\n";
print "$go_next\n";

    } elsif ( $op eq 'PUSH') {			#  --------- PUSH xx
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:					; $op $args\n";
	if ( $reg{ $arg1}) {
		print "	sub	bp,2\n";
		print "	mov	_memw[ebp], $asign{$arg1}\n";
	} else {
		print "	mov	di, $asign{$arg1}\n";
		print "	sub	bp,2\n";
		print "	mov	_memw[ebp], di\n";
	}
print "$go_next\n";


    } elsif ( $op eq 'POP') {			#  --------- POP xx
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:					; $op $args\n";
	if ( $reg{ $arg1}) {
		print "	mov	$asign{$arg1},_memw[ebp]\n";
		print "	add	bp,2\n";
	} else {
		print "	mov	di,_memw[ebp]\n";
		print "	add	bp,2\n";
		print "	mov	$asign{$arg1},di\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'DJNZ') {			#  --------- DJNZ xx
	push( @func, &mklabel( "$op $arg1"));

print "$func[ $#func]:					; $op $args\n";
	$ll++;
	print "	movsx	ax,_mem[ esi]\n";
	print "	inc	esi\n";
	print "	dec	_B\n";
	print "	jz	short l$ll\n";
	print "	add	si,ax\n";
print "l$ll:$go_next\n";


    } elsif ( $op eq 'JR') {			#  --------- JR [cc,]xx
	push( @func, &mklabel( "$op $arg1"));

print "$func[ $#func]:					; $op $args\n";
	print "	movsx	ax,_mem[ esi]\n";
	print "	inc	esi\n";
	$ll++;
	if ( $cc{ $arg1}) {
		print "	test	$F, $flag{$arg1}\n";
		print "	j$cc{ $arg1}	short l$ll\n";
	}
	print "	add	si,ax\n";
print "l$ll:$go_next\n";

    } elsif ( $op eq 'JP') {			#  --------- JP [cc,]xxx
	$cc = $cc{ $arg1};
	$dst = ( $cc) ? $arg2 : $arg1;
	$dst =~ s/^\((.+)\)$/$1/;
	$dst = $asign{ $dst};
	push( @func, &mklabel( "$op $arg1"));

print "$func[ $#func]:					; $op $args\n";
	if ( $dst eq 'di') {
print <<EOD
	movzx	edi, _memw[ esi]
	add	esi, 2
EOD
	}
	$ll++;
	if ( $cc) {
print <<EOD;
	test	$F, $flag{$arg1}
	j$cc	short l$ll
EOD
	}
print <<EOD;
	mov	si, $dst
l$ll:$go_next
EOD

    } elsif ( $op eq 'CALL') {			#  --------- CALL [cc,]xxx
	$cc = $cc{ $arg1};
	$dst = ( $cc) ? $arg2 : $arg1;
	$dst =~ s/^\((.+)\)$/$1/;
	$dst = $asign{ $dst};
	push( @func, &mklabel( "$op $arg1"));

print "$func[ $#func]:					; $op $args\n";
	if ( $dst eq 'di') {
print <<EOD
	movzx	edi, _memw[ esi]
	add	esi, 2
EOD
	}
	$ll++;
	if ( $cc) {
print <<EOD;
	test	$F, $flag{$arg1}
	j$cc	short l$ll
EOD
	}
print <<EOD;
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, $dst
l$ll:$go_next

EOD

    } elsif ( $op eq 'RET' || $op eq 'RETI' || $op eq 'RETN') {# --- RET [cc]
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	$ll++;
	if ( $arg1) {
print <<EOD;
	test	$F, $flag{$arg1}
	j$cc{$arg1}	short l$ll
EOD
	}
print <<EOD;
	movzx	esi, _memw[ ebp]
	add	bp, 2
l$ll:$go_next

EOD

    } elsif ( $op eq 'RST') {			#  --------- RST xx
	push( @func, 'rst_xx');

    } elsif ( $op eq 'OR' || $op eq 'AND' || $op eq 'XOR' || $op eq 'TST') {
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	if ( $arg2 =~ /(I[XY])\+d/) {
		print "	movsx	di,_mem[ esi]\n";
		print "	inc	esi\n";
		print "	add	di,_$1\n";
	}
	print "	$op86{ $op}	$asign{ $arg1},$asign{ $arg2}\n";
	print "	lahf\n";
	# 2012.03 ------ fix flags
	if ( $op eq 'AND' || $op eq 'TST') {
		print "	xor  ah,HF or NF\n";
	} else {
		print "	and  ah,not(HF or NF)\n";
	}
	# 2012.03 -----------------
	print "	mov	$F,ah\n";
	if ( $arg2 eq 'imm') {
		print "	inc	e$PC\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'ADD' || $op eq 'ADC' || 
		$op eq 'SUB' || $op eq 'SBC' || $op eq 'CP') {
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1}; $src = $asign{ $arg2};

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	if ( $arg2 =~ /(I[XY])\+d/) {
		print "	movsx	di,_mem[ esi]\n";
		print "	inc	esi\n";
		print "	add	di,_$1\n";
	}
	if ( $op eq 'SBC' || $op eq 'ADC') {
	    print "	shr	$F,1\n";
    	} elsif ( $op eq 'ADD' && $bit{ $arg1} == 16) {
	    print "	and	$F,not(NF or CF)\n";
	}
	if ( $reg{ $arg1} || $reg{ $arg2}) {
	    print "	$op86{ $op}	$dst,$src\n";
	} else {
	    print "	mov	ax,$src\n";
	    print "	$op86{ $op}	$dst,ax\n";
	}
    	if ( $op eq 'ADD' && $bit{ $arg1} == 16) {
	    print "	adc	$F,0\n";
	} else {
	    print "	seto	$F\n";
	    print "	lahf\n";
	    print "	shl	$F,2\n";
	    if ( $op eq 'ADD' || $op eq 'ADC') {
		print "	and	ah,not(PF or NF)\n";
	    } else {
		print "	and	ah,not PF\n";
	    }
	    print "	or	$F,ah\n";
	}
	if ( $arg2 eq 'imm') {
	    print "	inc	e$PC\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'INC' || $op eq 'DEC') {
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1};

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	if ( $arg1 =~ /(I[XY])\+d/) {
		print "	movsx	di,_mem[ esi]\n";
		print "	inc	esi\n";
		print "	add	di,_$1\n";
	}
	if ( $bit{ $arg1} != 16) {
	    print "	shr	$F,1\n";
    	}
	    print "	$op86{ $op}	$dst\n";
	if ( $bit{ $arg1} != 16) {
	    print "	seto	$F\n";
	    print "	lahf\n";
	    print "	shl	$F,2\n";
	    if ( $op eq 'INC') {
		print "	and	ah,not(PF or NF)\n";
	    } else {
		print "	and	ah,not PF\n";
	    }
	    print "	or	$F,ah\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'NEG') {
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	    print "	neg	$A\n";
	    print "	seto	$F\n";
	    print "	lahf\n";
	    print "	shl	$F,2\n";
	    print "	and	ah,not PF\n";
	    print "	or	$F,ah\n";
print "$go_next\n";

    } elsif ( $op eq 'CPL') {
	push( @func, &mklabel( "$op $args"));

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	    print "	not	$A\n";
	    print "	or	$F,HF or NF\n";
print "$go_next\n";

    } elsif ( $op eq 'RLCA' || $op eq 'RRCA' || $op eq 'RLA' || $op eq 'RRA') {
	push( @func, &mklabel( "$op "));

print "$func[ $#func]:\t\t\t\t; $op\n";
	if ( $op eq 'RLA' || $op eq 'RRA') {
	    print "	mov	ah,$F\n";
	    print "	and	$F,not( HF or NF or CF)\n";
	    print "	sahf\n";
	} else {
	    print "	and	$F,not( HF or NF or CF)\n";
	}
	    print "	$op86{ $op}	$A,1\n";
	    print "	adc	$F,0\n";
print "$go_next\n";

    } elsif ( $op eq 'RLC' || $op eq 'RRC' || $op eq 'RL' || $op eq 'RR' ||
		$op eq 'SLA' || $op eq 'SRA' || $op eq 'SRL' || $op eq 'SL1') {
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1};

print "$func[ $#func]:\t\t\t\t; $op\n";
	if ( $op eq 'RL' || $op eq 'RR') {
	    print "	shr	$F,1\n";
	} elsif ( $op eq 'SL1') {
	    print "	stc\n";
	}
	    print "	$op86{ $op}	$dst,1\n";
	# 2012.03 ---- change S Z P flags
	if (  $op eq 'RLC' || $op eq 'RRC' || $op eq 'RL' || $op eq 'RR' || $op eq 'SL1') {
	  if ( $reg{ $arg1}) {
	    print "	inc	$dst\n";
	    print "	dec	$dst\n";
	  } else {
	    print "	mov	al,$dst\n";
	    print "	inc	al\n";
	    print "	dec	al\n";
	  }
	}
	# 2012.03 -----------------------
	    print "	lahf\n";
	    print "	and	ah, not(HF or NF)\n";
	    print "	mov	$F,ah\n";
print "$go_next\n";

    } elsif ( $op eq 'RRD' || $op eq 'RLD') {
	push( @func, &mklabel( "$op "));

print "$func[ $#func]:\t\t\t\t; $op\n";
	    print "	mov	al,_mem[e$HL]\n";
	    print "	mov	ah,$A\n";
	    print "	ror	ax,4\n" if ($op eq 'RRD');
	    print "	rol	ah,4\n";
	    print "	rol	ax,4\n" if ($op eq 'RLD');
	    print "	mov	_mem[e$HL],al\n";
	    print "	mov	$A,ah\n";
	    print "	or	ah,ah\n";  # 2012.03 fix A to flags
	    print "	lahf\n";
	    print "	and	ah, not(HF or NF or CF)\n";
	    print "	and	$F, CF\n";
	    print "	or	$F, ah\n";
print "$go_next\n";

    } elsif ( $op eq 'CCF' || $op eq 'SCF') {		# ---- CCF/SCF
	push( @func, &mklabel( "$op"));

print "$func[ $#func]:\t\t\t\t; $op\n";
	  print "	and	$F,not(HF or NF)\n"; # 2012.03 clear H N
	if ( $op eq 'CCF') {
if ($z80HFflag) {
	  print  "	mov	al,$F\n";
	  print  "	shl	al,4\n";
	  print  "	and	al,HF\n";
	  print  "	or	$F,al\n";
}
	    print "	xor	$F,CF\n";
	} else {
	    print "	or	$F,CF\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'BIT') {				# ---- BIT xx,b
	push( @func, &mklabel( "$op $args"));
	$src = $asign{ $arg1};

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	    print "	test	$src,1 SHL $arg2\n";
	    print "	lahf\n";
	    print "	and	$F,SF or PF or CF\n";
	    print "	and	ah,ZF\n";
	    print "	or	$F,ah\n";
	    print "	or	$F,HF\n";
print "$go_next\n";

    } elsif ( $op eq 'SET' || $op eq 'RES') {		# ---- SET/RES xx,b
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1};

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	if ( $op eq 'SET') {
	    print "	or	$dst,1 SHL $arg2\n";
	} else {
	    print "	and	$dst,not( 1 SHL $arg2)\n";
	}
print "$go_next\n";

    } elsif ( $op eq 'DAA') {				# ---- DAA
	push( @func, &mklabel( "$op "));

print "$func[ $#func]:\t\t\t\t; $op\n";
	print "	mov	al,$A\n";
	print "	mov	ah,$F\n";
	print "	sahf\n";
	print "	test	ah,NF\n";
	print "	jz	short da1\n";
	print "	sahf\n";
	print "	das\n";
	print "	lahf\n"; 	# 2012.03 adjust CF
	print "	cmp	al,0fah\n";
	print "	jb	short da2\n";
	print "	or	$F,not CF\n";
	print "	and	ah,$F\n";
	print "	jmp	short da2\n";
	print "da1:	sahf\n";
	print "	daa\n";
	print "	lahf\n";	# 2012.03 clear NF
	print "	and	ah,not NF\n";
	print "da2:\n";
if ($z80HFflag) {
  print "	xor $A,al\n";
  print "	and $A,HF\n";
  print "	and ah,not HF\n";
  print "	or  ah,$A\n";
}
	print "	mov	$F,ah\n";
	print "	mov	$A,al\n";
print "$go_next\n";

    } elsif ( $op eq 'MLT') {				# ---- MLT
	push( @func, &mklabel( "$op $args"));
	$dst = $asign{ $arg1};

print "$func[ $#func]:\t\t\t\t; $op $args\n";
	    print "	mov	ax,$dst\n";
	    print "	mul	ah\n";
	    print "	mov	$dst,ax\n";
print "$go_next\n";

    } elsif ( $op =~ /LD([ID])(R)?/) {	# ---- LDI,LDD,LDIR,LDDR
	push( @func, &mklabel( "$op"));
	$updown = ($1 eq 'I') ? 'inc' : 'dec';

print "$func[ $#func]:\t\t\t\t; $op\n";
	    print "	movzx	edx,_DE\n";
	    print "	and	$F,not (HF or PF or NF)\n";
	if ( $2 eq 'R') {
	    $ll++;
	    print "	mov	di,cx\n";
	    print "	movzx	ecx,_BC\n";
	    print "l$ll:\n";
	}
	    print "	mov	al,_mem[e$HL]\n";
	    print "	$updown	$HL\n";
	    print "	mov	_mem[edx],al\n";
	    print "	$updown	dx\n";
	if ( $2 eq 'R') {
	    print "	loop	l$ll\n";
	    print "	mov	_BC,cx\n";
	    print "	mov	cx,di\n";
	} else {
	    print "	dec	_BC\n";
	    print "	setnz	al\n";
	    print "	shl	al,2\n";
	    print "	or	$F,al\n";
	}
	    print "	mov	_DE,dx\n";
print "$go_next\n";

    } elsif ( $op =~ /CP([ID])(R)?/) {	# ---- CPI,CPD,CPIR,CPIR
	push( @func, &mklabel( "$op"));

print "$func[ $#func]:\t\t\t\t; $op\n";
	    print "	and	$F, CF\n";
	if ( $2 eq 'R') {
	    $updown = ($1 eq 'I') ? '+1' : '-1';
	    $ll++;
	    print "	mov	al,$A\n";
	    print "	mov	di,cx\n";
	    print "	movzx	ecx,_BC\n";
	    print "l$ll:\n";
	    print "	cmp	al,_mem[e$HL]\n";
	    print "	lea	e$HL,[e$HL$updown]\n";
	    print "	loopnz	l$ll\n";
	    print "	lahf\n";
	    print "	and	ah,not (PF or CF)\n"; # 2012.03 HF
	    print "	mov	_BC,cx\n";
	    print "	add	cx,-1\n";
	    print "	mov	cx,di\n";
	    print "	sbb	al,al\n";
	    print "	and	al,PF\n";
	} else {
	    $updown = ($1 eq 'I') ? 'inc' : 'dec';
	    print "	cmp	$A,_mem[e$HL]\n";
	    print "	lahf\n";
	    print "	$updown	$HL\n";
	    print "	and	ah, not(PF or CF)\n"; # 2012.03 HF
	    print "	dec	_BC\n";
	    print "	setnz	al\n";
	    print "	shl	al,2\n";
	}
	    print "	or	$F,ah\n";
	    print "	or	$F,al\n";
print "$go_next\n";

    } else {
	push( @func, "illegal$precode");
	print STDERR "unkown op-code\n$_\n";
    }

}

# ---------- print footer -----------

print "\nfunc		label	dword";
$c = 0;
for (@func) {
	print(( $c++ % 4 == 0) ? "\n	dd	" : ", ");
	printf $_;

	print "\n\nfunc$l	label	dword" if ( $l = $funclabel{ $c});
}

print <<EOD;


_em180	endp

_TEXT	ends

	end
EOD
