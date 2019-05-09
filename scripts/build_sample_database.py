#!/usr/bin/env python

"""Creates a database of samples.

This script is intended for post-processing of PEC files [1].  The
database, which is represented by a human-readable JSON file, is
constructed from JSON files with descriptions of the samples and
information about normalization for simulated samples.  The script also
checks available PEC files and includes in the database masks that
define input files for each sample.

The format of the file with descriptions of samples is as in the
following example:

  [
    {
      "datasetId": "ttbar_pw",
      "isData": false,
      "crossSection": 831.76
    },
    ...
  ]

Field "isData" is optional and defaults to false.  The cross section,
in pb, is only used for simulation.

The file with information conserning normalization of simulated samples
is created by script compute_sample_norm.py.  Mean alternative weights
are never copied to the output as this would make the produced file hard
to read to a human.

The output database file meets the specification in [2].

[1] https://github.com/andrey-popov/PEC-tuples
[2] https://github.com/andrey-popov/mensura/blob/master/include/mensura/extensions/DatasetBuilder.hpp
"""

import argparse
from collections import OrderedDict
import json
import os
import re
import sys


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
    
    # Parse arguments
    arg_parser = argparse.ArgumentParser(
        epilog=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    arg_parser.add_argument(
        'src_dir', help='Directory containing ROOT files'
    )
    arg_parser.add_argument(
        '-d', '--defs', default='samples_descriptions.json',
        help='Input JSON file with the list of samples'
    )
    arg_parser.add_argument(
        '-n', '--norm', default='samples_norm.json',
        help='JSON file with information about normalization for MC samples'
    )
    arg_parser.add_argument(
        '-o', '--output', default='samples.json',
        help='Name for output JSON file'
    )
    args = arg_parser.parse_args()
    
    
    # Read input JSON files.  The list of samples read from the file
    # with normalizations is converted into a dictionary to facilitate
    # access by dataset ID.
    with open(args.defs) as f:
        sample_definitions = json.load(f, object_pairs_hook=OrderedDict)
    
    with open(args.norm) as f:
        norm_infos = json.load(f)
    
    for collection, file_name in [
        (sample_definitions, args.defs), (norm_infos, args.norm)
    ]:
        if not all('datasetId' in entry for entry in collection):
            critical_error(
                'One or more entries in file "{}" do not have mandatory '
                'field "datasetId" specified.', file_name
            )
    
    dataset_names = {sample['datasetId'] for sample in sample_definitions}
    norm_infos = {sample['datasetId'] : sample for sample in norm_infos}


    # For each dataset, construct a list of mask that defines names of
    # ROOT files included in it.  Make these masks as tight as possible
    # to avoid false positives.
    input_file_masks = {}
    file_name_pattern = re.compile(r'^(.+?)(_ext\d+)?(\.part\d+)?\.root$')
    
    for file_name in os.listdir(args.src_dir):
        if not os.path.isfile(os.path.join(args.src_dir, file_name)):
            continue

        match = file_name_pattern.match(file_name)

        if not match:
            continue
        
        dataset_name = match.group(1)

        if dataset_name not in dataset_names:
            continue

        if dataset_name not in input_file_masks:
            input_file_masks[dataset_name] = set()

        if match.group(2):
            dataset_ext_name = dataset_name + match.group(2)
        else:
            dataset_ext_name = dataset_name

        if match.group(3):
            mask = dataset_ext_name + '.part*.root'
        else:
            mask = dataset_ext_name + '.root'

        input_file_masks[dataset_name].add(mask)
    
    
    # Combine above details keeping a logical order of main attributes.
    # Skip "meanLHEWeights" if available since adding would make the
    # resulting JSON unreadable.
    results = []
    
    for sample_definition in sample_definitions:
        dataset_name = sample_definition['datasetId']
        
        if dataset_name not in input_file_masks:
            critical_error(
                'No ROOT files found for dataset ID "{}".', dataset_name
            )
        
        combined_entry = OrderedDict([
            ('datasetId', dataset_name),
            ('files', list(input_file_masks[dataset_name]))
        ])
        norm_info = None
        
        if 'isData' in sample_definition and sample_definition['isData']:
            combined_entry['isData'] = True
            fields_to_skip = ['datasetId', 'isData']
        else:
            if dataset_name in norm_infos:
                norm_info = norm_infos[dataset_name]
                
                combined_entry['isData'] = False
                combined_entry['crossSection'] = \
                    sample_definition['crossSection']
                combined_entry['eventsProcessed'] = \
                    norm_info['eventsProcessed']
                combined_entry['meanWeight'] = norm_info['meanWeight']
                
                fields_to_skip = [
                    'datasetId', 'isData', 'crossSection', 'eventsProcessed',
                    'meanWeight', 'meanLHEWeights'
                ]
            else:
                combined_entry['isData'] = False
                fields_to_skip = ['datasetId', 'isData']
        
        # Copy all remaining parameters from the input definitions
        for key, value in sample_definition.items():
            if key not in fields_to_skip:
                combined_entry[key] = value
        
        if norm_info:
            for key, value in norm_info.items():
                if key not in fields_to_skip:
                    combined_entry[key] = value
        
        results.append(combined_entry)
    
    
    # Write the combined JSON
    out_file = open(args.output, 'w')
    json.dump(results, out_file, indent=2)
    out_file.close()

