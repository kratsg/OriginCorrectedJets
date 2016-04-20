#!/usr/bin/env python
import utils
from rootpy.io import root_open

'''
Warning: it is assumed that the ROOT files being used as inputs have been hadd'ed before passing through this script...
'''

import argparse
import subprocess
import os
import json
import operator

def ensure_dir(f):
    d = os.path.dirname(f)
    if not os.path.exists(d):
        os.makedirs(d)
    return f

class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter):
  pass

__version__ = subprocess.check_output(["git", "describe", "--always"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
__short_hash__ = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()

parser = argparse.ArgumentParser(description='Author: G. Stark. v.{0}'.format(__version__), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30), epilog='Take in histograms, scale by the sample weight, and calculate ROCs by scanning over a variable and creating a single output ROOT file.')
parser.add_argument('files', type=str, nargs='+', help='ROOT files containing the histograms')
parser.add_argument('--weights', type=str, dest='weights', required=True, help='The weights.json file containing sample weights to apply.')
parser.add_argument('--lumi', required=False, type=int, dest='lumi', metavar='<L>', help='luminosity to use in units of ifb', default=1)
parser.add_argument('--output', required=False, type=str, dest='output', metavar='output.root', help='Output directory to put the histograms', default='golem')

# parse the arguments, throw errors if missing any
args = parser.parse_args()

# load the weights file
try:
    weights = json.load(file(args.weights))
except IOError:
    print('Could not find the file')
    raise
except ValueError:
    print('Could not parse the weights file')
    raise

for fname in args.files:
    if not os.path.exists:
        print('{0:s} does not exist. Skipping.'.format(fname))
        continue

    # attempt to extract DSID from file
    try:
        dsid = utils.get_dsid(fname)
    except ValueError:
        print('Could not extract the DSID from {0:s}. Skipping.'.format(fname))
        continue

    # open to read out the histograms and write it back
    f = root_open(fname, "READ")
    # open to write out the result
    out_f = root_open(ensure_dir(os.path.join(args.output, fname)), "NEW")
    for collection in f.kinematics:
