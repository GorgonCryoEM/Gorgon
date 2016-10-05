#!/usr/bin/env python

import argparse
import logging

from toolkit import *


def main():
    mode_map = {'filter': {"normalize":"Normalize", "lowpass":"LowPass", "gaussian":"Gaussian"},
                'skeletonize': {"binary":"Binary", "grayscale":"GrayScale"}
                }
    
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    parser.add_argument('--log', action="store",
                        dest='loglevel',
                        choices=['info', 'debug'],
                        default='info',
                        help="log level"
                        )
    
    group = parser.add_mutually_exclusive_group(required=True)
    
#     Update parser with list of available options
    for k in mode_map:
        group.add_argument('--'+k, choices=(mode_map[k]))

    args, args_extra = parser.parse_known_args()
    
#     Initialize object requested in the parser
    for k in mode_map:
        opt = getattr(args, k)
        if opt:
            mode = globals()[mode_map[k][opt]]
            mode(args.input, args.output, args_extra)
            
# 	Logging setup
    loglevel = getattr(logging, args.loglevel.upper())
    logging.basicConfig(level=loglevel)
    logger = logging.getLogger(__name__)


if __name__ == "__main__":
    main()
