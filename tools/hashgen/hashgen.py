#!/usr/bin/env python3

import argparse
import hashlib

def hash(file):
    if file[0] == '*':
        return bytearray.fromhex(file[1:])
    else:
        with open(file, 'rb') as f:
            return hashlib.sha1(f.read()).digest()

def split_values(x):
    s = x.split('=')
    return {s[0] : hash(s[1])}

def generate_h(name, hash):
    return 'extern const std::vector<unsigned char>& {}();'.format(name)

def generate_c(name, hash):
    c_bytes = ", ".join(['0x{:02X}'.format(x) for x in hash])
    return 'const std::vector<unsigned char>& {}() {{ static std::vector<unsigned char> hash{{ {} }}; return hash; }}'.format(name, c_bytes)

parser = argparse.ArgumentParser(description="Infinity hash generator")
parser.add_argument('files', metavar='NAME=FILE', type=split_values, nargs='+',
                    help='Key value pair of names to files to generate hashes for')
parser.add_argument('header_output', type=argparse.FileType('w'), help='Location to store the output header file')
parser.add_argument('source_output', type=argparse.FileType('w'), help='Location to store the output source file')
args = parser.parse_args()

args.header_output.write('#pragma once\n')
args.header_output.write('#include <vector>\n')

for hash in args.files:
    args.header_output.write('{}\n'.format(generate_h(*hash.keys(), *hash.values())))

args.source_output.write('#include <vector>\n')

for hash in args.files:
    args.source_output.write('{}\n'.format(generate_c(*hash.keys(), *hash.values())))