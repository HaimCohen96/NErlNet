import argparse
import os
from ErlHeadersExporterDefs import *
from JsonElementWorkerDefinitions import *
from Definitions import VERSION as NERLPLANNER_VERSION

EMPTY_LINE = '\n'
DEBUG = False

def gen_erlang_exporter_logger(message : str):
    if DEBUG:
        print(f'[NERLPLANNER][AUTO_HEADER_GENERATOR][DEBUG] {message}')

def gen_worker_fields_hrl(header_path : str, debug : bool = False):
    global DEBUG
    DEBUG = debug

    auto_generated_header = AutoGeneratedHeader()
    gen_erlang_exporter_logger(auto_generated_header.generate_code())

    nerlplanner_version = Comment(f'Generated by Nerlplanner version: {NERLPLANNER_VERSION}')
    gen_erlang_exporter_logger(nerlplanner_version.generate_code())

    fields_list_vals = [KEY_MODEL_TYPE, KEY_LAYER_SIZES_LIST,
                   KEY_LAYER_TYPES_LIST, KEY_LAYERS_FUNCTIONS,
                   KEY_LOSS_METHOD, KEY_LEARNING_RATE,
                   KEY_EPOCHS, KEY_OPTIMIZER_TYPE]
    fields_list_strs = ['KEY_MODEL_TYPE', 'KEY_LAYER_SIZES_LIST',
                   'KEY_LAYER_TYPES_LIST', 'KEY_LAYERS_FUNCTIONS',
                   'KEY_LOSS_METHOD', 'KEY_LEARNING_RATE',
                   'KEY_EPOCHS', 'KEY_OPTIMIZER_TYPE']

    fields_list_defs = [ Definition(fields_list_strs[idx], f'{Definition.assert_not_atom(fields_list_vals[idx])}') for idx in range(len(fields_list_vals))]
    [gen_erlang_exporter_logger(x.generate_code()) for x in fields_list_defs]
    

    if os.path.dirname(header_path):
        os.makedirs(os.path.dirname(header_path), exist_ok=True)

    with open(header_path, 'w') as f:
       f.write(auto_generated_header.generate_code())
       f.write(nerlplanner_version.generate_code())
       f.write(EMPTY_LINE)
       [f.write(x.generate_code()) for x in fields_list_defs]

def gen_dc_fields_hrl(header_path : str, debug : bool = False):
    global DEBUG
    DEBUG = debug

    auto_generated_header = AutoGeneratedHeader()
    gen_erlang_exporter_logger(auto_generated_header.generate_code())

    nerlplanner_version = Comment(f'Generated by Nerlplanner version: {NERLPLANNER_VERSION}')
    gen_erlang_exporter_logger(nerlplanner_version.generate_code())

    #TODO


def main():
    parser = argparse.ArgumentParser(description='Generate C++ header file for nerlPlanner')
    parser.add_argument('-o', '--output', help='output header file path', required=True)
    parser.add_argument('-d', '--debug', help='debug mode', action='store_true')
    args = parser.parse_args()
    gen_worker_fields_hrl(args.output, args.debug)
    gen_dc_fields_hrl(args.output, args.debug)

if __name__=="__main__":
    main()

