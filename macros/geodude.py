#!/usr/bin/env python
import utils
from rootpy.io import root_open

'''
Turn off warnings from rootpy.tree for buffer:
    WARNING:rootpy.tree] Skipping entry in buffer with the same name as an existing branch '<>'
'''
from rootpy import log as rootpy_log
rootpy_log["/rootpy.tree"].setLevel(rootpy_log.ERROR)

import argparse
import subprocess
import os
import json
import operator

class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter):
  pass

__version__ = subprocess.check_output(["git", "describe", "--always"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
__short_hash__ = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()

parser = argparse.ArgumentParser(description='Author: G. Stark. v.{0}'.format(__version__), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30), epilog='Take in histograms, scale by the sample weight, and calculate ROCs by scanning over a variable and creating a single output ROOT file.')
parser.add_argument('files', type=str, nargs='+', help='ROOT files containing the histograms')
parser.add_argument('--weights', type=str, dest='weights', required=True, help='The weights.json file containing sample weights to apply.')
parser.add_argument('--lumi', required=False, type=int, dest='lumi', metavar='<L>', help='luminosity to use in units of ifb', default=1)
parser.add_argument('--output', required=False, type=str, dest='output', metavar='output.root', help='Output ROOT filename to use', default='golem.root')

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

    # open for updating to add a branch
    f = root_open(args.output, "CREATE")

    try:
        # get the scale factor for the given file name then
        # use reduce and multiply everything to the default (args.lumi)
        weight = weights[dsid]
        scale_factor = args.lumi
        scale_factor *= weight.get('cross section')
        scale_factor *= weight.get('filter efficiency')
        scale_factor *= weight.get('k-factor')
        scale_factor /= weight.get('num events')
    except KeyError:
        print('The weights file does not have an entry for DSID#{0:s}'.format(dsid))
        continue
