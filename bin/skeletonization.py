#!/usr/bin/env python

import argparse

from Toolkit import Binary, GrayScale


def main():
    mode_map = {'binary': "Binary", 'grayscale': "GrayScale"}
    
    parser = argparse.ArgumentParser(description='Gorgon skeletonizer')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    parser.add_argument('--mode', required=True, choices=(mode_map.keys()))
    
    args = parser.parse_args()
    
    mode = globals()[mode_map[args.mode]]
    mode(args.volume, args.skeleton)

if __name__ == "__main__":
    main()
