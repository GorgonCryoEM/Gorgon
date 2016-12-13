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
    logger.debug("seq: %s" % seq)
    
    pdb = Chain.load(args.pdb)
    logger.debug("\npdb:\n%s" % str(pdb))
    logger.debug(pdb.residueRange())
    logger.debug(pdb.getSequence())

    atoms = pdb.atoms
    residues = pdb.residueList
    logger.debug(residues)
    logger.debug(atoms)
    logger.debug("%d %d" % (len(residues), len(atoms.keys())))
    logger.debug("pdb.differenceIndex: %d" % pdb.differenceIndex)

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
        
        logger.debug("%s %s %s %s" % (i_residue, i_atom, residues[i_residue], atoms[i_atom]))
    
    logger.debug(ids_atom)
    logger.debug(words)

    # Add atoms corresponding to words extracted to the  sequence
    seq_string = str(seq)
    low = 0
    
    for i in range(len(words)):
        word = words[i]
        logger.debug("---%s---" % word)
        logger.debug(ids_atom[i])
        l = len(word)
        for j in range(low, len(seq_string)-l):
            logger.debug("%s %s %s" % (i, word, seq_string[j:j+l]))
            if word == seq_string[j:j+l]:
                logger.debug("Found %s %s" % (word, seq_string[j:j+l]))
                logger.debug("   --- %s" % ids_atom[i])
                logger.debug("   --- %s" % [atoms[k] for k in ids_atom[i]])
                
                for k in range(j+1, j+l+1):
                    seq[k].addAtomObject(atoms[ids_atom[i][k-j-1]])
                low = j+l
                break
    
    seq.saveToPDB(args.output)

if __name__ == "__main__":
    main()
