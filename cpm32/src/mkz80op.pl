#!/usr/bin/perl
# mkz80op.pl -- make z80 op-code table


sub putopcode {
	local ($nemonic) = @_;
	$table[ $opcode] = $nemonic;
	$opcode++;
}

@r8 = ('B','C','D','E','H','L','(HL)','A');
@r16 = ('BC','DE','HL','SP');
@cc = ( 'NZ','Z','NC','C', 'PO','PE','PL','MI');

# ----------- 1st OP-CODE -----------
# 00-3F
$i = 0;
for $op ( 'NOP', "EX AF,AF'", 'DJNZ rel', 'JR rel') {
	$opcode = 000 + ($i++ << 3);
	&putopcode( $op);			# 00 0xx 000
}
for $i (0..4) {
	$opcode = 040 + ($i << 3);
	&putopcode( "JR $cc[ $i],rel");		# 00 1xx 000
}
$i = 0;
for $r (@r16) {
	$add = ($r =~ /SP|HL/) ? 'abs' : $r;
	$opl = ($r eq 'HL') ? 'HL' : 'A';

	$opcode = 001 + ($i << 4);
	&putopcode( "LD $r,imm");		# 00 xx0 001
	&putopcode( "LD ($add),$opl");		# 00 xx0 010
	&putopcode( "INC $r");			# 00 xx0 011

	$opcode = 011 + ($i++ << 4);
	&putopcode( "ADD HL,$r");		# 00 xx1 001
	&putopcode( "LD $opl,($add)");		# 00 xx1 010
	&putopcode( "DEC $r");			# 00 xx1 011
}
$i = 0;
for $r (@r8) {
	$opcode = 004 + ($i++ << 3);
	&putopcode( "INC $r");			# 00 xxx 100
	&putopcode( "DEC $r");			# 00 xxx 101
	&putopcode( "LD $r,imm");		# 00 xxx 110
}
$i = 0;
for $op ( 'RLCA', 'RRCA', 'RLA', 'RRA', 'DAA', 'CPL', 'SCF', 'CCF') {
	$opcode = 007 + ($i++ << 3);
	&putopcode( $op);			# 00 xxx 111
}

# 40-7F
$opcode = 0100;
for $d (@r8) {
    for $s (@r8) {
	if ( $d eq '(HL)' && $s eq '(HL)') {	# 01 ddd sss
	    &putopcode( "HALT");
	} else {
	    &putopcode( "LD $d,$s");
	}
    }
}

# 80-BF
$opcode = 0200;
for $op ('ADD','ADC','SUB','SBC','AND','XOR','OR','CP') {
    for $s (@r8) {
	&putopcode( "$op A,$s");		# 10 ooo sss
    }
}

# C0-FF
for $i (0..7) {
	$opcode = 0300 + ($i << 3);
	&putopcode( "RET $cc[ $i]");		# 11 ccc 000
	$opcode++;
	&putopcode( "JP $cc[ $i],add");		# 11 ccc 010
	$opcode++;
	&putopcode( "CALL $cc[ $i],add");	# 11 ccc 100
}
$i = 0;
for $r (@r16) {
	$r0 = ($r eq 'SP') ? 'AF' : $r;
	$opcode = 0301 + ($i++ << 4);
	&putopcode( "POP $r0");			# 11 rr0 001
	$opcode += 3;
	&putopcode( "PUSH $r0");		# 11 rr0 101
}
$i = 0;
for $op ( 'RET', 'EXX', 'JP (HL)', 'LD SP,HL') {
	$opcode = 0311 + ($i++ << 4);
	&putopcode( $op);			# 11 xx1 001
}
$i = 0;
for $op ( 'CALL add', 'prebyte DD', 'prebyte ED', 'prebyte FD') {
	$opcode = 0315 + ($i++ << 4);
	&putopcode( $op);			# 11 xx1 101
}
$i = 0;
for $op ( 'JP add', 'prebyte CB', 'OUT (io),A', 'IN A,(io)', 
	  'EX (SP),HL', 'EX DE,HL', 'DI', 'EI') {
	$opcode = 0303 + ($i++ << 3);
	&putopcode( $op);			# 11 xxx 011
}
$i = 0;
for $op ('ADD','ADC','SUB','SBC','AND','XOR','OR','CP') {
	$opcode = 0306 + ($i++ << 3);
	&putopcode( "$op A,imm");		# 11 xxx 110
}
for $i (0..7) {
	$opcode = 0307 + ($i << 3);
	&putopcode( sprintf( "RST %02xh", $i << 3));	# 11 xxx 111
}
@opcodeXX = @table;
$i = 0;
for (@table) {
    $cpuXX[ $i++] = /^-$/ ? '-' : 
		/prebyte|AF'|EXX|rel/ ? 'z80' : '8080';
}

# ----------- 2nd OP-CODE CB-xx -----------
undef @table;
$opcode = 0;
for $op ('RLC','RRC','RL','RR','SLA','SRA','SL1','SRL') {
    for $r (@r8) {
	&putopcode(($op eq '-') ? '-' : "$op $r");	# 00 xxx rrr
    }
}
for $op ('BIT','RES','SET') {
    for $b (0..7) {
	for $r (@r8) {
	    &putopcode( "$op $r,$b");		# 01-11 bbb rrr
	}
    }
}
@opcodeCBXX = @table;
$i = 0;
for (@table) {
    $cpuCBXX[ $i++] = /^-$/ ? '-' : /^SL1/ ? 'z80u' : 'z80';
}

# ----------- 2nd OP-CODE ED-xx -----------
undef @table;
$opcode = 0;
for $r (@r8) {
	$r0 = ($r eq '(HL)') ? 'F' : $r;
	&putopcode( "IN0 $r0,(io)");		# 00 rrr 000
	&putopcode(( $r0 eq 'F') ? '-' : "OUT0 (io),$r0"); # 00 rrr 001
	&putopcode( '-');			# 00 rrr 010
	&putopcode( '-');			# 00 rrr 011
	&putopcode( "TST A,$r");		# 00 rrr 100
	&putopcode( '-');			# 00 rrr 101
	&putopcode( '-');			# 00 rrr 110
	&putopcode( '-');			# 00 rrr 111
}
$i = 0;
for $r (@r8) {
	$r0 = ($r eq '(HL)') ? 'F' : $r;
	$r1 = ($r eq '(HL)') ? '0' : $r;
	$opcode = 0100 + ($i++ << 3);
	&putopcode( "IN $r0,(C)");		# 01 rrr 000
	&putopcode( "OUT (C),$r1");		# 01 rrr 001
}
$i = 0;
for $r (@r16) {
	$opcode = 0102 + ($i << 4);
	&putopcode( "SBC HL,$r");		# 01 rr0 010
	&putopcode( "LD (abs),$r");		# 01 rr0 011
	$opcode = 0112 + ($i++ << 4);
	&putopcode( "ADC HL,$r");		# 01 rr1 010
	&putopcode( "LD $r,(abs)");		# 01 rr1 011
	&putopcode( "MLT $r");			# 01 rr1 100
}
$i = 0;
for $op ( 'NEG A', '-', 'TST A,imm', 'TSTIO (C),imm') {
	$opcode = 0104 + ($i++ << 4);
	&putopcode( $op);			# 01 xx0 100
}
for $i (0..7) {
    @op101 = ( 'RETN', 'RETI', '-', '-',  '-', '-', '-', '-');
    @op110 = ( 'IM0', '-', 'IM1', 'IM2',  '-', '-', 'SLP', '-');
    @op111 = ( 'LD I,A', 'LD R,A', 'LD A,I', 'LD A,R', 'RRD', 'RLD', '-', '-');
	$opcode = 0105 + ($i << 3);
	&putopcode( $op101[ $i]);		# 01 xxx 101
	&putopcode( $op110[ $i]);		# 01 xxx 110
	&putopcode( $op111[ $i]);		# 01 xxx 111
}
for $i (0..7) {
    @op000 = ('-', '-', '-', '-', 'LDI', 'LDD', 'LDIR', 'LDDR');
    @op001 = ('-', '-', '-', '-', 'CPI', 'CPD', 'CPIR', 'CPDR');
    @op010 = ('-', '-', '-', '-', 'INI', 'IND', 'INIR', 'INDR');
    @op011 = ('OTIM','OTDM','OTIMR','OTDMR', 'OUTI','OUTD','OTIR','OTDR');
	$opcode = 0200 + ($i << 3);
	&putopcode( $op000[ $i]);		# 10 xxx 000
	&putopcode( $op001[ $i]);		# 10 xxx 001
	&putopcode( $op010[ $i]);		# 10 xxx 010
	&putopcode( $op011[ $i]);		# 10 xxx 011
	&putopcode( '-');			# 10 xxx 100
	&putopcode( '-');			# 10 xxx 101
	&putopcode( '-');			# 10 xxx 110
	&putopcode( '-');			# 10 xxx 111
}
for $i (0..7) {
    @op101 = ('-', '-', '-', '-', '-', 'CALLN v', '-', 'RETEM');
	$opcode = 0300 + ($i << 3);
	&putopcode( '-');			# 11 xxx 000
	&putopcode( '-');			# 11 xxx 001
	&putopcode( '-');			# 11 xxx 010
	&putopcode( '-');			# 11 xxx 011
	&putopcode( '-');			# 11 xxx 100
	&putopcode( $op101[ $i]);		# 11 xxx 101
	&putopcode( '-');			# 11 xxx 110
	&putopcode( '-');			# 11 xxx 111
}
@opcodeEDXX = @table;
$i = 0;
for (@table) {
    $cpuEDXX[ $i++] = /^-$/ ? '-' : /^CALLN|RETEM/ ? 'v30' :
		      /^SLP|TST|MLT|OT[DI]MR+|IN0|OUT0/ ? '64180' :
		      /^OUT (C),0/ ? 'z80u' : 'z80';
}

# ----------- 2nd OP-CODE DD-xx -----------
undef @table;
$opcode = 0;
for (@opcodeXX) {
    ($op, $opl) = split;
    if ( $op ne 'JP' && $opl =~ s/\(HL\)/(IX+d)/ ||
	 $opl =~ s/HL/IX/g ||
	 $opl =~ s/\b(H|L)\b/IX$&/g) {
	&putopcode( "$op $opl");
    } elsif ( $_ eq 'prebyte CB') {
	&putopcode( $_);
    } else {
	&putopcode( '-');
    }
}
@opcodeDDXX = @table;
$i = 0;
for (@table) {
    $cpuDDXX[ $i++] = /^-$/ ? '-' : 
			/\b(IXL|IXH)\b/ ? 'z80u' : 'z80';
}

# ----------- 2nd OP-CODE FD-xx -----------
undef @table;
$opcode = 0;
for (@opcodeXX) {
    ($op, $opl) = split;
    if ( $op ne 'JP' && $opl =~ s/\(HL\)/(IY+d)/ ||
	 $opl =~ s/HL/IY/g ||
	 $opl =~ s/\b(H|L)\b/IY$&/g) {
	&putopcode( "$op $opl");
    } elsif ( $_ eq 'prebyte CB') {
	&putopcode( $_);
    } else {
	&putopcode( '-');
    }
}
@opcodeFDXX = @table;
$i = 0;
for (@table) {
    $cpuFDXX[ $i++] = /^-$/ ? '-' : 
			/\b(IYL|IYH)\b/ ? 'z80u' : 'z80';
}
# ----------- 3nd OP-CODE DD/FD-CB-xx -----------
undef @table;
$opcode = 0;
for (@opcodeCBXX) {
    ($op, $opl) = split;
    if ( $opl =~ s/\(HL\)/(IX+d)/) {
	&putopcode( "$op $opl");
    } else {
	&putopcode( '-');
    }
}
@opcodeDDCBXX = @table;
$i = 0;
for (@table) {
    $cpuDDCBXX[ $i++] = /^-$/ ? '-' : 'z80';
}

# ------ OUTPUT -------
for ( $i = 0; $i < 0x100; $i++) {
	printf( "%02X     %-5s  %s\n", $i, $cpuXX[ $i], $opcodeXX[ $i]);
}
for ( $i = 0; $i < 0x100; $i++) {
	printf( "CB%02X   %-5s  %s\n", $i, $cpuCBXX[ $i], $opcodeCBXX[ $i]);
}
for ( $i = 0; $i < 0x100; $i++) {
	printf( "ED%02X   %-5s  %s\n", $i, $cpuEDXX[ $i], $opcodeEDXX[ $i]);
}
for ( $i = 0; $i < 0x100; $i++) {
	printf( "DD%02X   %-5s  %s\n", $i, $cpuDDXX[ $i], $opcodeDDXX[ $i]);
}
for ( $i = 0; $i < 0x100; $i++) {
	printf( "FD%02X   %-5s  %s\n", $i, $cpuFDXX[ $i], $opcodeFDXX[ $i]);
}
for ( $i = 0; $i < 0x100; $i++) {
	printf( "DDCB%02X %-5s  %s\n", $i, $cpuDDCBXX[$i], $opcodeDDCBXX[$i]);
}
