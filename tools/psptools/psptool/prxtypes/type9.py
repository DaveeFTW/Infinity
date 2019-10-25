from ecdsa.ellipticcurve import CurveFp, Point
import hashlib
from ecdsa.numbertheory import inverse_mod
from ecdsa import SigningKey
from ecdsa.curves import Curve
import psptool.kirk as kirk

from .common import expand_seed, prx_header, set_kirk_cmd_1
from Crypto.Util.strxor import strxor as xor
from Crypto.Hash import SHA1


class prx_header_9(object):
    def __init__(self, header):
        prx = prx_header(header)
        self.header = prx.personalisation() + prx.btcnf_id() + prx.sha1_hash() + prx.kirk_aes_key() + prx.kirk_cmac_key() + \
            prx.kirk_cmac_header_hash() + prx.kirk_cmac_data_hash() + \
            prx.kirk_metadata() + prx.elf_info()

    def tag(self):
        return self.header[:0x4]

    def btcnf_id(self):
        return self.header[0x5C:0x6C]

    def sha1_hash(self):
        return self.header[0x6C:0x80]

    def personalisation(self):
        return self.header[:0x5C]

    def prx_ecdsa(self):
        return self.header[0x34:0x5C]

    def kirk_aes_key(self):
        return self.header[0x80:0x90]

    def kirk_cmac_key(self):
        return self.header[0x90:0xA0]

    def kirk_cmac_header_hash(self):
        return self.header[0xA0:0xB0]

    def kirk_cmac_data_hash(self):
        return self.header[0xB0:0xC0]

    def kirk_metadata(self):
        return self.header[0xC0:0xD0]

    def kirk_block(self):
        return self.header[0x80:0xC0]

    def elf_info(self):
        return self.header[0xD0:]

    def decrypt_header(self, key):
        self.header = self.header[:0x5C] + kirk.kirk7(
            self.header[0x5C:0x5C+0x60], key) + self.header[0x5C+0x60:]


def decrypt(prx, meta, **kwargs):
    xorbuf = expand_seed(meta['seed'], meta['key'])

    # check if range contains nonzero
    if any(x != 0 for x in prx[0xD4:0xD4+0x30]):
        return False

    p = prx_header_9(prx)

    print(meta['pubkey'])
    print(p.prx_ecdsa().hex())

    # check ECDSA signature
    # kirk.kirk11(bytes.fromhex(meta['pubkey']), p.prx_ecdsa(
    # ), prx[4:0x104] + b'\x00'*0x28 + prx[0x12C:])

    h2 = SHA1.new()
    h2.update(prx[4:0x104] + b'\x00'*0x28 + prx[0x12C:])
    print(h2.hexdigest())

    # decrypt the header information
    p.decrypt_header(meta['key'])

    # calculate SHA1 of header
    h = SHA1.new()
    h.update(p.tag())
    h.update(xorbuf[:0x10])
    h.update(b'\x00'*0x58)
    h.update(p.btcnf_id())
    h.update(p.kirk_aes_key())
    h.update(p.kirk_cmac_key())
    h.update(p.kirk_cmac_header_hash())
    h.update(p.kirk_cmac_data_hash())
    h.update(p.kirk_metadata())
    h.update(p.elf_info())

    # sanity check that our SHA1 actually matches
    if h.digest() != p.sha1_hash():
        return False

    # decrypt the kirk block
    header = xor(p.kirk_block(), xorbuf[0x10:0x50])
    header = kirk.kirk7(header, meta['key'])
    header = xor(header, xorbuf[0x50:])

    # prepare the kirk block
    block = header + b'\x00'*0x30
    block = set_kirk_cmd_1(block)
    block = block + p.kirk_metadata() + b'\x00'*0x10 + \
        p.elf_info() + prx[0x150:]

    return kirk.kirk1(block)
