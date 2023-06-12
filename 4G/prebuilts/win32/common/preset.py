#!/usr/bin/env python3
# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

import argparse
import sys
import struct
import shutil
import os
import glob
import subprocess
import json

DESCRIPTION = """
Create cio
"""

CPIO_FILE_FORMAT = '=2s12H{}s{}s'
CPIO_OLDLE_MAGIC = b'\xc7\x71'


# Create directory if not exists
def ensure_dir(dname):
    if dname and not os.path.exists(dname):
        os.makedirs(dname)


# Copy files, and return files in new directory
def copy_files(dst, fnames):
    nnames = []
    for f in fnames:
        nname = os.path.join(dst, os.path.basename(f))
        nnames.append(nname)
        shutil.copy(f, nname)
    return nnames


# Add plain_file to partition
def pgen_add_file(pgen, local_name, fs_name):
    if 'plain_file' not in pgen:
        pgen['plain_file'] = []
    pgen['plain_file'].append({'file': fs_name, 'local_file': local_name})


# Write file if changes, data is bytes
def write_file_if_change(fname, data):
    if os.path.exists(fname):
        with open(fname, 'rb') as fh:
            old_data = fh.read()
        if old_data == data:
            return

    ensure_dir(os.path.dirname(fname))
    with open(fname, 'wb') as fh:
        fh.write(data)


# Load prepack configuration
def prepack_cfg_from_json(cfg, srctop, bintop):
    with open(cfg, 'r') as fh:
        cfgtxt = fh.read()

    cfgdir = os.path.dirname(cfg)
    cfgtxt = cfgtxt.replace('@SOURCE_TOP_DIR@', srctop).replace(
        '@BINARY_TOP_DIR@', bintop)

    pc = json.loads(cfgtxt)
    prepackcfg = []
    for fm in pc.get('files', []):
        rname = fm['file'].replace('\\', '/').replace('//', '/').lstrip('/')
        fname = fm['local_file']
        if not os.path.isabs(fname):
            fname = os.path.join(cfgdir, fname)
        prepackcfg.append({'file': rname, 'local_file': fname})
    return prepackcfg


# Create prepack cpio
def create_prepack_cpio(pc, outfname):
    with open(outfname, 'wb') as fh:
        for fm in pc:
            name = fm['file'].encode('utf-8')
            name_size = len(name) + 1  # include NUL
            name_size_aligned = (name_size + 1) & ~1  # pad to even

            file_data = bytes()
            with open(fm['local_file'], 'rb') as ffh:
                file_data = ffh.read()
            file_size = len(file_data)
            file_size_aligned = (file_size + 1) & ~1  # pad to even

            fstat = os.stat(fm['local_file'])
            fh.write(struct.pack(
                CPIO_FILE_FORMAT.format(name_size_aligned, file_size_aligned),
                CPIO_OLDLE_MAGIC,
                fstat.st_dev & 0xffff,
                fstat.st_ino & 0xffff,
                fstat.st_mode & 0xffff,
                fstat.st_uid & 0xffff,
                fstat.st_gid & 0xffff,
                fstat.st_nlink & 0xffff,
                0,  # rdevice_num
                int(fstat.st_mtime) >> 16,
                int(fstat.st_mtime) & 0xffff,
                name_size & 0xffff,
                file_size >> 16,
                file_size & 0xffff,
                name,
                file_data))

        # When there are no files, write an empty file, and pcgen will
        # ignore PREPACK
        if len(pc) > 0:
            name = b'TRAILER!!!'
            name_size = len(name) + 1
            name_size_aligned = (name_size + 1) & ~1

            fh.write(struct.pack(
                CPIO_FILE_FORMAT.format(name_size_aligned, 0),
                CPIO_OLDLE_MAGIC,
                0, 0, 0, 0, 0,
                1, 0, 0, 0,
                name_size & 0xffff,
                0, 0,
                name,
                bytes()))


def mimggen8910_args(sub_parser):
    parser = sub_parser.add_parser(
        'imggen8910', help='generate dependency rule')
    parser.set_defaults(func=mimggen8910)
    parser.add_argument('--source-top', dest='srctop', required=True,
                        help='SOURCE_TOP_DIR, may be used')
    parser.add_argument('--binary-top', dest='bintop', required=True,
                        help='BINARY_TOP_DIR, may be used')
    parser.add_argument('--dep', dest='dep', default=None,
                        help='output dependency files')
    parser.add_argument('--deprel', dest='deprel', default=None,
                        help='dependency target relative directory')
    parser.add_argument('--prepackfile', dest='prepackfile', required=True,
                        help='the json file for prepack')
    parser.add_argument('variant', help='variant name')
    parser.add_argument('prepack', help='output prepack cpio file')


def mimggen8910(args):
    # Collect prepack files
    prepack_json = args.prepackfile
    prepackcfg = prepack_cfg_from_json(prepack_json, args.srctop, args.bintop)
    create_prepack_cpio(prepackcfg, args.prepack)

    return 0




def main(argv):
    parser = argparse.ArgumentParser(description=DESCRIPTION,
                                     formatter_class=argparse.RawTextHelpFormatter)
    #sub_parser = parser.add_subparsers(help='sub-commnads')

    #mimggen8910_args(sub_parser)
    parser.set_defaults(func=mimggen8910)
    parser.add_argument('--source-top', dest='srctop', required=True,
                        help='SOURCE_TOP_DIR, may be used')
    parser.add_argument('--binary-top', dest='bintop', required=True,
                        help='BINARY_TOP_DIR, may be used')
    parser.add_argument('--prepackfile', dest='prepackfile', required=True,
                        help='the json file for prepack')
    parser.add_argument('prepack', help='output prepack cpio file')

    args = parser.parse_args(argv)
    if args.__contains__('func'):
        return args.func(args)

    parser.parse_args(['-h'])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
