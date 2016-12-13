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

    atoms = pdb.atoms
    residues = pdb.residueList

    # Group consecutive letters into words and corresponding atom ids
    words = []
    ids_atom = []
    i_old = -1

    for i in atoms:
        i_atom = i
        i_residue = i_atom + pdb.differenceIndex
        
        if i - i_old > 1:
            words.append('')
            ids_atom.append([])
        i_old = i
        
        words[-1] += str(residues[i_residue])
        ids_atom[-1].append(i_atom)

    seq.saveToPDB(args.output)

if __name__ == "__main__":
    main()
