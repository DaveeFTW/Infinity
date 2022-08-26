#!/usr/bin/env python3

import argparse

from psptool.pack import pack_prx
from psptool.prx import encrypt

parser = argparse.ArgumentParser(
    description="Infinity User Firmware Module Packer")
parser.add_argument('input', type=argparse.FileType('rb'),
                    help='The raw user PRX to pack')
parser.add_argument('--id', type=str, help='The btcnf id to set')
parser.add_argument('output', type=str,
                    help='The output to write the packed PRX')
args = parser.parse_args()

executable = args.input.read()
executable = encrypt(pack_prx(executable, is_pbp=False,
                              psptag=lambda x: 0x457B8AF0), id=bytes.fromhex(args.id))

with open(args.output, 'wb') as f:
    f.write(executable)
