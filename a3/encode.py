#!/usr/bin/env python3

import sys
import coding2

if len(sys.argv) < 2:
    print("usage: ./encode.py <filename>")
    sys.exit(1)

coding2.encode(sys.argv[1])
