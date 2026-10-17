#!/usr/bin/env python3
"""Usage: python3 run_separated.py ./a.out [args...] < input.txt"""
import subprocess
import sys
import tempfile


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    # ファイルへ退避し、大量のログをメモリ上に保持しない。
    with tempfile.TemporaryFile() as out, tempfile.TemporaryFile() as err:
        result = subprocess.run(sys.argv[1:], stdout=out, stderr=err)
        for label, stream in [("cerr", err), ("cout", out)]:
            sys.stdout.buffer.write(f"[{label}]\n".encode())
            stream.seek(0)
            last = b"\n"
            while chunk := stream.read(65536):
                sys.stdout.buffer.write(chunk)
                last = chunk[-1:]
            if last != b"\n":
                sys.stdout.buffer.write(b"\n")
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
