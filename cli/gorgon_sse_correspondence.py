#!/usr/bin/env python

import argparse
import logging
import json

from gorgon.toolkit import SSEHelixCorrespondence


def main():
    parser = argparse.ArgumentParser(description='Gorgon SSE Correspondence Finder')
    
    parser.add_argument('skeleton', action="store")
    parser.add_argument('sequence', action="store")
    parser.add_argument('helix', action="store")
    parser.add_argument('output', action="store")
    parser.add_argument('--json', help='JSON file that contains settings')
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

    sse_finder = SSEHelixCorrespondence(args)
    sse_finder.accept()
    sse_finder.correspondenceEngine.saveCorrespondenceToFile(args.output)


if __name__ == "__main__":
    main()
