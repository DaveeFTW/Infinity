.set noreorder
.set noat
.global interceptMemsetEntry
.ent    interceptMemsetEntry
interceptMemsetEntry:
j interceptMemset
move $a3, $ra
.end interceptMemsetEntry
