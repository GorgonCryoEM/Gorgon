#!/usr/bin/env python

import argparse
import logging
from gorgon.toolkit import *

def main():
    parser = argparse.ArgumentParser(description='Gorgon Skeletonizer')

    mode_map = {"binary":"Binary", "grayscale":"GrayScale"}
    parser.add_argument('--log', action="store",
                        dest='loglevel',
                        choices=['info', 'debug'],
                        default='info',
                        help="log level"
                        )
    
    subparsers = parser.add_subparsers(dest='operation')
    
    cmd_objects = {}
    
#     Update subparsers by initializing the objects listed in mode_map
    for k in mode_map:
        subparser = subparsers.add_parser(k)
        subparser.add_argument('operation_type', choices=(mode_map[k]))
        for opt in mode_map[k]:
            mode = globals()[mode_map[k][opt]]
            # initialization
            cmd_objects[mode_map[k][opt]] = mode(subparser)

    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    
    args = parser.parse_args()
    
    # get selected object
    cmd = cmd_objects[mode_map[args.operation][args.operation_type]]    
    cmd.run(args)
    
# 	Logging setup
    loglevel = getattr(logging, args.loglevel.upper())
    logging.basicConfig(level=loglevel)
    logger = logging.getLogger(__name__)


if __name__ == "__main__":
    main()
