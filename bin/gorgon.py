#!/usr/bin/env python

import argparse

from Toolkit import *


def main():
    mode_map = {'binary': "Binary", 'grayscale': "GrayScale"}
    
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    
    parser.add_argument('--mode', required=True, choices=(mode_map.keys()))
    
    args = parser.parse_args()
    
    mode = globals()[mode_map[args.mode]]
    mode(args.volume, args.skeleton)

if __name__ == "__main__":
    main()
