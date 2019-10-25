#!/usr/bin/env python3

import argparse
import struct
import gzip
from enum import Enum, auto

PBP_HEADER_MAGIC = 0x50425000
PSP_HEADER_MAGIC = 0x5053507E
ELF_HEADER_MAGIC = 0x464C457F
ELF_TYPE_PRX     = 0xFFA0

class ExecutableType(Enum):
    USER_PRX = auto()
    KERNEL_PRX = auto()
    PBP = auto()

class PBP:
    def is_pbp(data):
        return struct.unpack('<I', data[:4])[0] == PBP_HEADER_MAGIC

    def __init__(self, data):
        (self.magic, self.version, sfo_offset, icon0_offset, 
         icon1_offset, pic0_offset, pic1_offset, 
         snd0_offset, prx_offset, psar_offset) = struct.unpack('<IIIIIIIIII', data[:0x28])

        self.sfo = data[0x28:sfo_offset]
        self.icon0 = data[icon0_offset:icon1_offset]
        self.icon1 = data[icon1_offset:pic0_offset]
        self.pic0 = data[pic0_offset:pic1_offset]
        self.pic1 = data[pic1_offset:snd0_offset]
        self.snd0 = data[snd0_offset:prx_offset]
        self.prx = data[prx_offset:psar_offset]
        self.psar = data[psar_offset:]

    def pack(self):
        return struct.pack('<IIIIIIIIII', self.magic, self.version,
            0x28, 
            0x28+len(self.sfo), 
            0x28+len(self.sfo)+len(self.icon0), 
            0x28+len(self.sfo)+len(self.icon0)+len(self.icon1),
            0x28+len(self.sfo)+len(self.icon0)+len(self.icon1)+len(self.pic0), 
            0x28+len(self.sfo)+len(self.icon0)+len(self.icon1)+len(self.pic0)+len(self.pic1),
            0x28+len(self.sfo)+len(self.icon0)+len(self.icon1)+len(self.pic0)+len(self.pic1)+len(self.snd0),
            0x28+len(self.sfo)+len(self.icon0)+len(self.icon1)+len(self.pic0)+len(self.pic1)+len(self.snd0)+len(self.prx)
        ) + self.sfo + self.icon0 + self.icon1 + self.pic0 + self.pic1 + self.snd0 + self.prx + self.psar

class Phdr:
    def __init__(self, data):
        (self.p_type, self.p_offset, self.p_vaddr,
         self.p_paddr, self.p_filesz, self.p_memsz, 
         self.p_flags, self.p_align) = struct.unpack('<IIIIIIII', data[:0x20])

class Shdr:
    def __init__(self, data):
        (self.sh_name, self.sh_type, self.sh_flags,
         self.sh_addr, self.sh_offset, self.sh_size, 
         self.sh_link, self.sh_info, self.sh_addralign,
         self.sh_entsize) = struct.unpack('<IIIIIIIIII', data[:0x28])

class ELF:
    def __init__(self, data):
        (self.e_magic, self.e_class, self.e_data, self.e_idver, 
         self.e_type, self.e_machine, self.e_version, 
         self.e_entry, self.e_phoff, self.e_shoff,
         self.e_flags, self.e_ehsize, self.e_phentsize,
         self.e_phnum, self.e_shentsize, self.e_shnum, 
         self.e_shstrndx) = struct.unpack('<IBBBxxxxxxxxxHHIIIIIHHHHHH', data[:0x34])

        self.phdrs = [Phdr(data[self.e_phoff+x*self.e_phentsize:]) for x in range(self.e_phnum)]
        self.shdrs = [Shdr(data[self.e_shoff+x*self.e_shentsize:]) for x in range(self.e_shnum)]
        self.strtab = data[self.shdrs[self.e_shstrndx].sh_offset:self.shdrs[self.e_shstrndx].sh_offset+self.shdrs[self.e_shstrndx].sh_size].decode('ascii')

class ModuleInfo:
    def __init__(self, data):
        (self.modattribute, self.modversion,
         self.modname, self.gp_value, self.ent_top, self.ent_end,
         self.stub_top, self.stub_end) = struct.unpack('<HH28sIIIII', data[:0x34])

        self.modname = self.modname.decode('ascii').rstrip('\0')
    def is_kernel(self):
        return (self.modattribute & 0x1000) != 0

class PSPHeader:
    def __init__(self, executable, elf, phdr, modinfo, is_pbp):
        self.attribute = modinfo.modattribute
        self.modinfo_offset = phdr.p_paddr
        self.version = 1

        # set compression to gzip
        self.comp_attribute = 1
        self.module_ver_lo = modinfo.modversion & 0xFF
        self.module_ver_hi = (modinfo.modversion >> 8) & 0xFF
        self.modname = modinfo.modname
        self.elf_size = len(executable)

        self.entry = elf.e_entry
        self.nsegments = elf.e_phnum if elf.e_phnum < 2 else 2
        self.seg_align = [x.p_align for x in elf.phdrs[:self.nsegments]] + [0]*(4-self.nsegments)
        self.seg_address = [x.p_vaddr for x in elf.phdrs[:self.nsegments]] + [0]*(4-self.nsegments)
        self.seg_size = [x.p_memsz for x in elf.phdrs[:self.nsegments]] + [0]*(4-self.nsegments)

        bss = next(x for x in elf.shdrs if elf.strtab[x.sh_name:].split('\0')[0] == '.bss')
        self.bss_size = bss.sh_size

        self.devkitversion = 0

        if modinfo.is_kernel():
            self.devkitversion = 0x06060110 if self.attribute & 0x2000 else 0x05070110
            self.decrypt_mode = 2

        elif is_pbp:
            # check if VSH API (updater)
            if (self.attribute & 0x800) == 0x800:
                self.decrypt_mode = 0xC

            # check if APP API (comics, etc)
            elif (self.attribute & 0x600) == 0x600:
                self.decrypt_mode = 0xE

            # check if USB WLAN API (skype, etc)
            elif (self.attribute & 0x400) == 0x400:
                self.decrypt_mode = 0x4

            # set to MS API
            else:
                # TODO: could check the SFO for POPS...
                self.attribute |= 0x200
                self.decrypt_mode = 0xD

        else:
            # standalone user prx
            # check for VSH API
            if (self.attribute & 0x800) == 0x800:
                self.decrypt_mode = 3

            # standard user prx
            else:
                self.devkitversion = 0x05070210
                self.decrypt_mode = 4
        
        # to be filled in by caller
        self.psptag = None
        self.oetag = None
        self.comp_size = None
        self.psp_size = None

        self.reserved = [0]*5
        self.key_data0 = [0xDA]*0x30
        self.reserved2 = [0]*2
        self.key_data1 = [0xDA]*0x10
        self.signcheck = [0]*0x58
        self.key_data2 = 0
        self.key_data3 = [0xDA]*0x1C

    def pack(self):
        return struct.pack('<IHHBB28sBBIIIII4H4I4I5IIBxH48BII2I16BI88BII28B', 
            PSP_HEADER_MAGIC, self.attribute, self.comp_attribute, self.module_ver_lo, self.module_ver_hi,
            self.modname.encode('ascii'), self.version, self.nsegments, self.elf_size, self.psp_size, self.entry,
            self.modinfo_offset, self.bss_size, *self.seg_align, *self.seg_address, *self.seg_size,
            *self.reserved, self.devkitversion, self.decrypt_mode, 0,
            *self.key_data0, self.comp_size, 0x80, *self.reserved2, *self.key_data1, self.psptag,
            *self.signcheck, self.key_data2, self.oetag, *self.key_data3)

def determine_exec_type(modinfo, is_pbp):
    if is_pbp:
        return ExecutableType.PBP

    elif modinfo.is_kernel():
        return ExecutableType.KERNEL_PRX

    return ExecutableType.USER_PRX

def pack_prx(executable, psptag, oetag, is_pbp):
    magic = struct.unpack('<I', executable[:4])[0]

    if magic == PSP_HEADER_MAGIC:
        return -1 # TODO: refine this
    
    if magic != ELF_HEADER_MAGIC:
        return -2 # TODO: refine this


    elf = ELF(executable)
    if elf.e_magic != ELF_HEADER_MAGIC or elf.e_type != ELF_TYPE_PRX:
        return -3 # TODO: refine this
    
    # TODO: handle where there is no modinfo
    modinfo_phdr = next(x for x in elf.phdrs if x.p_type == 1 and x.p_vaddr != x.p_paddr)

    is_kernel = (modinfo_phdr.p_paddr & 0x80000000) != 0
    modinfo = ModuleInfo(executable[modinfo_phdr.p_paddr&0x7FFFFFFF:(modinfo_phdr.p_paddr&0x7FFFFFFF)+0x34])
    
    # check for mixed privileges with kernel modules
    if is_kernel != modinfo.is_kernel():
        return -4 # TODO: refine this

    psp_header = PSPHeader(executable, elf, modinfo_phdr, modinfo, is_pbp)

    if psp_header.attribute != modinfo.modattribute:
        print("conflict module attribute {:X} vs {:X}".format( psp_header.attribute, modinfo.modattribute))

    compressed_exec = gzip.compress(executable)
    psp_header.comp_size = len(compressed_exec)
    psp_header.psp_size = len(compressed_exec) + 0x150

    exec_type = determine_exec_type(modinfo, is_pbp)
    psp_header.psptag = psptag(exec_type)
    psp_header.oetag = oetag(exec_type)
    
    return psp_header.pack() + compressed_exec

def psptag_default(type):
    if type == ExecutableType.USER_PRX:
        return 0x457B06F0
    elif type == ExecutableType.KERNEL_PRX:
        return 0xDADADAF0
    
    # must be pbp
    return 0xADF305F0

def oetag_default(type):
    if type == ExecutableType.USER_PRX:
        return 0x8555ABF2
    elif type == ExecutableType.KERNEL_PRX:
        return 0x55668D96
    
    # must be pbp
    return 0x7316308C

parser = argparse.ArgumentParser(description="Infinity psppack")
parser.add_argument('--psptag', type=lambda x: int(x, 0), help='The PSP tag to use')
parser.add_argument('--oetag', type=lambda x: int(x, 0), help='The OE tag to use')
parser.add_argument('input', type=argparse.FileType('rb'), help='The raw PRX or PBP to pack')
parser.add_argument('output', type=str, help='The output to write the packed PRX')
args = parser.parse_args()

executable = args.input.read()

psptag = psptag_default
oetag = oetag_default

if args.psptag:
    psptag = lambda x: args.psptag

if args.oetag:
    oetag = lambda x: args.oetag

if PBP.is_pbp(executable):
    pbp = PBP(executable)
    pbp.prx = pack_prx(pbp.prx, psptag, oetag, True)
    with open(args.output, 'wb') as f:
        f.write(pbp.pack())
else:
    with open(args.output, 'wb') as f:
        f.write(pack_prx(executable, psptag, oetag, False))