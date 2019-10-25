#!/usr/bin/env python3

import argparse

from psptool.pack import pack_prx
from psptool.prx import encrypt

parser = argparse.ArgumentParser(description="Infinity Kernel Module Packer")
parser.add_argument('input', type=argparse.FileType('rb'),
                    help='The raw kernel PRX to pack')
parser.add_argument('--vanity', type=str,
                    help='Some vanity text in the executable header')
parser.add_argument('output', type=str,
                    help='The output to write the packed PRX')
args = parser.parse_args()

executable = args.input.read()
executable = encrypt(pack_prx(executable, is_pbp=False,
                              psptag=lambda x: 0x00000000), vanity=args.vanity)

with open(args.output, 'wb') as f:
    f.write(executable)
