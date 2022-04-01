#!/usr/bin/env python3

import sys
import coding2

if len(sys.argv) < 2:
    print("usage: ./decode.py <filename>")
    sys.exit(1)

coding2.decode(sys.argv[1])
