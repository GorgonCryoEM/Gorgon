#!/usr/bin/env python

import argparse
import logging
from gorgon.toolkit import *

def main():
    parser = argparse.ArgumentParser(description='Gorgon Filter')

    mode_map = {"normalize":"Normalize", "lowpass":"LowPass", "gaussian":"Gaussian"}

    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    parser.add_argument('method', choices=mode_map.keys())
    parser.add_argument('--log', action="store",
                        dest='loglevel',
                        choices=['info', 'debug'],
                        default='info',
                        help="log level"
                        )
    cmd_objects = {}
    
#     Update subparsers by initializing the objects listed in mode_map
    for method in mode_map:
        mode = globals()[mode_map[method]]
        # initialization
        cmd_objects[mode_map[method]] = mode(parser)

    args = parser.parse_args()
    
    # get selected object
    cmd = cmd_objects[mode_map[args.method]]    
    cmd.run(args)
    
# 	Logging setup
    loglevel = getattr(logging, args.loglevel.upper())
    logging.basicConfig(level=loglevel)
    logger = logging.getLogger(__name__)


if __name__ == "__main__":
    main()
