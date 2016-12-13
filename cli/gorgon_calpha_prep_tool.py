#!/usr/bin/env python

import argparse
import logging

from gorgon.toolkit.sse.seq_model.Chain import Chain


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
    
    seq = Chain.load(args.sequence)
    pdb = Chain.load(args.pdb)

    seq.saveToPDB(args.output)

if __name__ == "__main__":
    main()
