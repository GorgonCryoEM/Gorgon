#!/usr/bin/env python

import argparse

from Toolkit import Binary, GrayScale


def main():
    parser = argparse.ArgumentParser(description='Gorgon skeletonizer')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    args = parser.parse_args()
    
if __name__ == "__main__":
    main()
