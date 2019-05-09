#!/usr/bin/env python

"""Computes numbers of processed events and mean weights.

Information is read from the EventCounts trees stored in PEC tuples [1].
[1] https://github.com/andrey-popov/PEC-tuples
"""

import argparse
from collections import OrderedDict
import json
import os
import re
import sys

import ROOT


def critical_error(format_string, *args, **kwargs):
    """Report a critical error.
    
    Print an error message and exit the script with code 1.  The error
    message is formed from the given format string and subsequent
    arguments using method str.format.
    """
    
    print(
        'Error:', format_string.format(*args, **kwargs),
        file=sys.stderr
    )
    sys.exit(1)



if __name__ == '__main__':
    
    # Parameters
    arg_parser = argparse.ArgumentParser(epilog=__doc__)
    arg_parser.add_argument('src_dir', help='Directory with tuples')
    arg_parser.add_argument(
        '--tree-name', default='eventCounter/EventCounts',
        help='Name for the tree with event counts'
    )
    arg_parser.add_argument(
        '--drop-alt-weights', action='store_true',
        help='Do not store mean values of alternative weights'
    )
    arg_parser.add_argument(
        '-o', '--output', default='samples_norm.json',
        help='Name for output file with normalization information'
    )
    args = arg_parser.parse_args()
    
    
    # List source files and group them by the source dataset
    datasets = {}
    postfix_regex = re.compile(r'(_ext\d+)?(\.part\d+)?\.root$')
    
    for file_name in os.listdir(args.src_dir):
        
        if not file_name.endswith('.root'):
            continue
        
        if not os.path.isfile(os.path.join(args.src_dir, file_name)):
            continue
        
        
        # Identify the dataset label
        base_name = re.sub(postfix_regex, '', file_name)
        
        
        # Check if the file contains the tree with event counts
        f = ROOT.TFile(os.path.join(args.src_dir, file_name))
        tree = f.Get(args.tree_name)
        
        if tree:
            # Add the file to the dataset
            if base_name not in datasets.keys():
                datasets[base_name] = [file_name]
            else:
                datasets[base_name].append(file_name)
        
        f.Close()
    
    
    # Construct output data and store it in a proper structure
    results = []
    
    for base_name, file_names in datasets.items():
        
        # Read numbers of events processed by each job and mean weights
        num_events = []
        mean_nominal_weights = []
        mean_alt_weights = []
        
        for file_name in file_names:
            
            input_file = ROOT.TFile(os.path.join(args.src_dir, file_name))
            tree = input_file.Get(args.tree_name)
            
            for job in tree:
                num_events.append(job.NumProcessed)
                mean_nominal_weights.append(job.MeanNominalWeight)
                if hasattr(job, 'MeanAltWeights'):
                    mean_alt_weights.append(list(job.MeanAltWeights))
            
            input_file.Close()
        
        
        # Sanity checks
        if len(num_events) != len(mean_nominal_weights) or \
            mean_alt_weights and len(mean_alt_weights) != len(num_events):
            critical_error(
                'Mismatched numbers of entries in dataset "{}".', base_name
            )
        
        for i in range(1, len(mean_alt_weights)):
            if len(mean_alt_weights[i]) != len(mean_alt_weights[0]):
                critical_error(
                    'Mismatched numbers of alternative weights in '
                    'dataset "{}".', base_name
                )
        
        
        # Compute sum of all events and mean weights
        final_num_events = sum(num_events)
        factors = [float(n) / final_num_events for n in num_events]
        final_mean_nominal_weight = sum(
            f * w for f, w in zip(factors, mean_nominal_weights)
        )
        
        result = OrderedDict([
            ('datasetId', base_name), ('eventsProcessed', final_num_events),
            ('meanWeight', final_mean_nominal_weight)
        ])
        
        if mean_alt_weights and not args.drop_alt_weights:
            l = []
            for iweight in range(len(mean_alt_weights[0])):
                w = 0.
                for iJob in range(len(factors)):
                    w += factors[iJob] * mean_alt_weights[iJob][iweight]
                l.append({'index': iweight, 'value': w})
            result['meanLHEWeights'] = l
        
        results.append(result)
    
    
    # Write the output
    out_file = open(args.output, 'w')
    json.dump(results, out_file, indent=2)
    out_file.close()

