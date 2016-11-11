#!/usr/bin/env python

import argparse
import logging

from gorgon.toolkit import pySSEHunter


def main():
    parser = argparse.ArgumentParser(description='Gorgon SSEHunter')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    parser.add_argument('output', action="store")
    parser.add_argument('--resolution', default=8.0)
    parser.add_argument('--threshold', default=0.38)
    parser.add_argument('--correlationWeight', default=1.0)
    parser.add_argument('--skeletonWeight', default=1.0)
    parser.add_argument('--geometryWeight', default=1.0)

    parser.add_argument('--log', action="store",
                    dest='loglevel',
                    choices=['info', 'debug'],
                    default='info',
                    help="log level"
                    )

    args = parser.parse_args()
    
    '''
    Logging setup
    '''
    loglevel = getattr(logging, args.loglevel.upper())
    
    FORMAT = "%(levelname)s:%(name)s: %(funcName)s():%(lineno)s: %(message)s"
    logging.basicConfig(level=loglevel,
                        format=FORMAT
                        )
    
    logger = logging.getLogger(__name__)

    sseh = pySSEHunter(args.volume,
                       args.skeleton,
                       args.output,
                       args.resolution,
                       args.threshold,
                       args.correlationWeight,
                       args.skeletonWeight,
                       args.geometryWeight)
    sseh.run()
    sseh.savePseudoatoms()

if __name__ == "__main__":
    main()
