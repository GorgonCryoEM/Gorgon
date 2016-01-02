#!/usr/bin/env python

import argparse

from Toolkit import *


def main():
    mode_map = {'filter': {"normalize":"Normalize", "lowpass":"LowPass", "gaussian":"Gaussian"}, 'skeletonize': {"binary":"Binary", "grayscale":"GrayScale"}}
    
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    
    group = parser.add_mutually_exclusive_group(required=True)
    
    for k in mode_map:
        group.add_argument('--'+k, choices=(mode_map[k]))

    args = parser.parse_args()
    
    for k in mode_map:
        opt = getattr(args, k)
        if opt:
            mode = globals()[mode_map[k][opt]]
            mode(args.input, args.output)

if __name__ == "__main__":
    main()
