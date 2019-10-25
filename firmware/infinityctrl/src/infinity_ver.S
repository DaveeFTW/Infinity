/*
Copyright (C) 2015, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

	.set noreorder
    .set noat
.global infinityVersionAppend
.ent    infinityVersionAppend
infinityVersionAppend:
addu $a3, $ra, $0
bal loop
addu $v0, $a1, $0

.global return_address
return_address:
nop

loop:
lhu $v1, 0($v0)
bnez $v1, loop
addiu $v0, $v0, 2
addu $v1, $0, 0x221E
lhu $t0, -4($v0)
beql $v1, $t0, exit
addu $t0, $0, $t0

trim_whitespace:
lhu $t0, -4($v0)
addu $t1, $0, 0x20
beql $t1, $t0, trim_whitespace
addiu $v0, $v0, -2

write_symbol:
sh $0, 0($v0)
sh $v1, -2($v0)

exit:
addu $t0, $0, $0
addu $t1, $0, $0
lw $v0, 0($ra)
addu $ra, $a3, $0
jr $v0
addu $a3, $0, $0
.end infinityVersionAppend
.global infinityVersionAppendEnd
infinityVersionAppendEnd: