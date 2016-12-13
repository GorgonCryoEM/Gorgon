#!/usr/bin/env python

import argparse
import logging


def main():
    parser = argparse.ArgumentParser(description='Gorgon CAlpha Prep Tool')
    
    parser.add_argument('sequence')
    parser.add_argument('pdb')
    parser.add_argument('output')

    parser.add_argument('--log', action="store",
                    dest='loglevel',
                    choices=['info', 'debug'],
                    default='info',
                    help="log level"
                    )

    args = parser.parse_args()
    
    loglevel = getattr(logging, args.loglevel.upper())
    
    FORMAT = "%(levelname)s:%(name)s: %(funcName)s():%(lineno)s: %(message)s"
    logging.basicConfig(level=loglevel,
                        format=FORMAT
                        )
    
    logger = logging.getLogger(__name__)

if __name__ == "__main__":
    main()
