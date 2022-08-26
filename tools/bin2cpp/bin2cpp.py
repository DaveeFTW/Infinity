#!/usr/bin/env python3

import argparse
import hashlib

def readfile(file):
    if file[0] == '*':
        return bytearray.fromhex(file[1:])
    else:
        with open(file, 'rb') as f:
            return f.read()

def split_values(x):
    s = x.split('=')
    return {s[0] : readfile(s[1])}

def generate_h(name, data):
    s = 'namespace {} {{\n'.format(name)
    s += 'extern std::array<unsigned char, {}> data;\n'.format(len(data))
    s += '}\n'
    return s

def generate_cpp(name, data):
    c_bytes = ", ".join(['0x{:02X}'.format(x) for x in data])
    s = '#include <array>\n'
    s += 'namespace {} {{\n'.format(name)
    s += 'alignas(64) std::array<unsigned char, {}> data = {{ {} }};\n'.format(len(data), c_bytes)
    s += '}\n'
    return s

parser = argparse.ArgumentParser(description="bin2cpp, embed files in your binaries")
parser.add_argument('files', metavar='NAME=FILE', type=split_values, nargs='+',
                    help='Key value pair of names to files to generate hashes for')
parser.add_argument('header_output', type=argparse.FileType('w'), help='Location to store the output header file')
parser.add_argument('cpp_output', type=argparse.FileType('w'), help='Location to store the output cpp file')
args = parser.parse_args()

args.header_output.write('#pragma once\n\n')
args.header_output.write('#include <array>\n\n')
args.header_output.write('namespace files {\n')

args.cpp_output.write('#include <array>\n')
args.cpp_output.write('namespace files {\n')

for hash in args.files:
    args.header_output.write('{}\n'.format(generate_h(*hash.keys(), *hash.values())))
    args.cpp_output.write('{}\n'.format(generate_cpp(*hash.keys(), *hash.values())))

args.header_output.write('}\n')
args.cpp_output.write('}\n')
