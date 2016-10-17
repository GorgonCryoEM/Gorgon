#!/usr/bin/env python

"""Main command-line program for Gorgon Toolkit."""

import argparse
import logging
from gorgon.toolkit import *

def main():
    """Program flow.
    
    1. Initialize objects by passing subparsers to be setup to their __init__ methods
    2. Parse args and pick the object according to the subcommand chosen on command-line
    3. Call object's run method and pass it the parsed args
    """
    mode_map = {'filter': {"normalize":"Normalize", "lowpass":"LowPass", "gaussian":"Gaussian"},
                'skeletonize': {"binary":"Binary", "grayscale":"GrayScale"}
                }
    
    parser = argparse.ArgumentParser(description='Gorgon Toolkit' + '\n\n' + __doc__)
    parser.add_argument('--log', action="store",
                        dest='loglevel',
                        choices=['info', 'debug'],
                        default='info',
                        help="log level"
                        )
    
    subparsers = parser.add_subparsers(dest='cmd')
    
    cmds = {}
    
#     Update parser with list of available options
    for k in mode_map:
        subparser = subparsers.add_parser(k)
        subparser.add_argument('operation_type', choices=(mode_map[k]))
        for opt in mode_map[k]:
            mode = globals()[mode_map[k][opt]]
            cmds[mode_map[k][opt]] = mode(subparser)

    parser.add_argument('input', action="store")
    parser.add_argument('output', action="store")
    args = parser.parse_args()
    cmd = cmds[mode_map[args.cmd][args.operation_type]]
    
    cmd.run(args)
    
# 	Logging setup
    loglevel = getattr(logging, args.loglevel.upper())
    logging.basicConfig(level=loglevel)
    logger = logging.getLogger(__name__)


if __name__ == "__main__":
    main()
