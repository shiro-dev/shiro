#!/usr/bin/env python3
"""
mkinitrd.py — pack a directory of files into a Shiro initrd binary.

Format (little-endian, packed):
    [uint32_t magic = 0x53484952 'SHIR']
    [uint32_t num_files]
    [for each file: char name[32], uint32_t size, uint32_t offset]
    [concatenated file payload]

`offset` is relative to the start of the binary. Names are NUL-padded,
truncated to 31 chars (one byte reserved for the trailing NUL).

Usage:  mkinitrd.py <input_dir> <output_file>
"""

import os
import struct
import sys

MAGIC = 0x53484952  # 'SHIR'
NAME_LEN = 32

def pack_name(name: str) -> bytes:
    raw = name.encode("utf-8")[:NAME_LEN - 1]
    return raw + b"\x00" * (NAME_LEN - len(raw))

def main(argv):
    if len(argv) != 3:
        print(f"usage: {argv[0]} <input_dir> <output_file>", file=sys.stderr)
        return 1

    input_dir = argv[1]
    output_path = argv[2]

    files = sorted(
        e for e in os.listdir(input_dir)
        if os.path.isfile(os.path.join(input_dir, e))
    )
    if not files:
        print(f"warning: {input_dir} is empty", file=sys.stderr)

    payloads = []
    for name in files:
        with open(os.path.join(input_dir, name), "rb") as f:
            payloads.append(f.read())

    header_size = 4 + 4 + len(files) * (NAME_LEN + 4 + 4)
    offsets = []
    cursor = header_size
    for p in payloads:
        offsets.append(cursor)
        cursor += len(p)
    total_size = cursor

    out = bytearray(total_size)
    struct.pack_into("<II", out, 0, MAGIC, len(files))
    pos = 8
    for name, payload, offset in zip(files, payloads, offsets):
        out[pos:pos + NAME_LEN] = pack_name(name)
        pos += NAME_LEN
        struct.pack_into("<II", out, pos, len(payload), offset)
        pos += 8

    for payload, offset in zip(payloads, offsets):
        out[offset:offset + len(payload)] = payload

    with open(output_path, "wb") as f:
        f.write(bytes(out))

    print(f"mkinitrd: wrote {output_path} — {len(files)} files, {total_size} bytes")
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
