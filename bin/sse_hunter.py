#!/usr/bin/env python

import argparse

from Toolkit.sse import VolumeSSEBuilderForm


def main():
    sseh = VolumeSSEBuilderForm()
    parser = argparse.ArgumentParser(description='Gorgon SSEHunter')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    parser.add_argument('output', action="store")
    
    args = parser.parse_args()


if __name__ == "__main__":
    main()
