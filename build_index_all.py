#!/usr/bin/python3
import sys
import subprocess
import os
from configparser import ConfigParser

if len(sys.argv) < 2:
    print("A config file is required")
    exit(1)

config = ConfigParser()
config.read(sys.argv[1])


class IndexBuilder:

    def __init__(self, config: ConfigParser):
        self.config = config
        self.executables_path = self.config['Common']['ExecutablesPath']
        self.validate()

        self.prefix_sort = self.config['Indexing']['StringDictionariesOutputPrefix']
        self.iris_sorted_result_path = f'{self.prefix_sort}.iris.sorted.txt'
        self.literals_sorted_result_path = f'{self.prefix_sort}.literals.sorted.txt'
        self.blanks_sorted_result_path = f'{self.prefix_sort}.blanks.sorted.txt'
        self.iris_sd_result_path = f'{self.prefix_sort}.iris.sd'
        self.literals_sd_result_path = f'{self.prefix_sort}.literals.sd'
        self.blanks_sd_result_path = f'{self.prefix_sort}.blanks.sd'

        self.triples_ids_file = f'{self.prefix_sort}.triples.bin'
        self.triples_ids_sorted_file = f'{self.prefix_sort}.triples.sorted.bin'

    def validate(self):
        return os.path.exists(self.executables_path)

    def run_splitter(self):
        splitter_path = os.path.join(self.executables_path, 'split_nt_triples')
        nt_file = self.config['Indexing']['InputNTFile']
        output_prefix = self.config['Indexing']['StringDictionariesOutputPrefix']
        buffer = self.config['Indexing']['SplitBuffer']
        command = [str(splitter_path), '-f', nt_file, '-o', output_prefix, '-B', buffer]
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        output, error = process.communicate()
        if error is not None:
            raise Exception(f'Failed at splitting step:\n{str(error)}\n')

    @staticmethod
    def handle_sort_error(error):
        if error is None:
            return
        raise Exception(f'There was an error while sorting:\n{error}\n')

    def run_sort_parts(self):
        env = os.environ.copy()
        tmp_dir = self.config['Indexing']['SortingTmpDir']
        memory_budget = self.config['Indexing']['SortingMemoryBudget']
        workers = self.config['Indexing']['SortingWorkers']

        iris_path = f'{self.prefix_sort}.iris.txt'
        literals_path = f'{self.prefix_sort}.literals.txt'
        blanks_path = f'{self.prefix_sort}.blanks.txt'

        env['LC_ALL'] = 'C'

        def specific_sort(input_file, output_file):
            command = ['sort', '-u', '-T', tmp_dir, '-S', memory_budget, f'--parallel={workers}', input_file, '-o',
                       output_file]
            process = subprocess.Popen(command, env=env)
            _, error = process.communicate()
            return error

        self.handle_sort_error(specific_sort(blanks_path, self.blanks_sorted_result_path))
        self.handle_sort_error(specific_sort(iris_path, self.iris_sorted_result_path))
        self.handle_sort_error(specific_sort(literals_path, self.literals_sorted_result_path))

        os.remove(blanks_path)
        os.remove(iris_path)
        os.remove(literals_path)

    def build_string_dictionaries(self):

        sd_builder_path = os.path.join(self.executables_path, 'sd_from_fileset_readfull')

        hrpdac_workers = self.config['Indexing']['HRPDAC_BlocksWorkers']
        hrpdac_cut_size = self.config['Indexing']['HRPDAC_CutSize']
        pfc_bucket_size = self.config['Indexing']['PFC_BucketSize']

        def common_command_builder(input_file, output_file, string_dict_value):
            return [
                str(sd_builder_path),
                '-f', input_file,
                '-o', output_file,
                '-t', string_dict_value,
            ]

        def hrpdac_blocks_command(input_file, output_file):
            return [
                *common_command_builder(input_file, output_file, 'HRPDACBlocks'),
                '-p', hrpdac_workers,
                '-c', hrpdac_cut_size
            ]

        def pfc_command(input_file, output_file):
            return [
                *common_command_builder(input_file, output_file, 'PFC'),
                '-B', pfc_bucket_size
            ]

        commands = [
            hrpdac_blocks_command(self.literals_sorted_result_path, self.literals_sd_result_path),
            pfc_command(self.iris_sorted_result_path, self.iris_sd_result_path),
            pfc_command(self.blanks_sorted_result_path, self.blanks_sd_result_path),
        ]

        commands_names = ['literals', 'iris', 'blanks']

        for i, command in enumerate(commands):
            process = subprocess.Popen(command)
            _, error = process.communicate()
            if error is not None:
                raise Exception(f'There was an error while building string dictionary for {commands_names[i]}')

        os.remove(self.literals_sorted_result_path)
        os.remove(self.iris_sorted_result_path)
        os.remove(self.blanks_sorted_result_path)

    def convert_nt_to_ids_file(self):
        nt_file = self.config['Indexing']['InputNTFile']
        converter_path = os.path.join(self.executables_path, 'convert_nt_to_binary')

        command = [
            str(converter_path),
            '-i', self.iris_sd_result_path,
            '-b', self.blanks_sd_result_path,
            '-l', self.literals_sd_result_path,
            '-I', nt_file,
            '-O', self.triples_ids_file
        ]
        process = subprocess.Popen(command)
        _, error = process.communicate()
        if error is not None:
            raise Exception(f'There was an error while producing triples ids file:\n{str(error)}\n')

    def sort_ids_file(self):
        sorter_path = os.path.join(self.executables_path, 'external_sort')
        tmp_dir = self.config['Indexing']['SortingTmpDir']
        memory_budget = self.config['Indexing']['TriplesSortingMemoryBudget']
        workers = self.config['Indexing']['SortingWorkers']
        command = [
            str(sorter_path),
            '-i', self.triples_ids_file,
            '-o', self.triples_ids_sorted_file,
            f'--tmp-dir={tmp_dir}',
            f'--max-memory={memory_budget}',
            f'--workers={workers}'
        ]
        process = subprocess.Popen(command)
        _, error = process.communicate()
        if error is not None:
            raise Exception(f'There was an error while sorting triples ids file:\n{str(error)}\n')

        os.remove(self.triples_ids_file)

    def build_k2trees(self):
        tree_depth = self.config['Indexing']['K2Tree_TreeDepth']
        cut_depth = self.config['Indexing']['K2Tree_CutDepth']
        max_nodes_count = self.config['Indexing']['K2Tree_MaxNodesCount']
        builder_path = os.path.join(self.executables_path, 'build_k2tree_index')

        k2tree_output_file = self.config['Indexing']['K2Tree_OutputFile']

        command = [str(builder_path),
                   '-i', self.triples_ids_sorted_file,
                   '-o', k2tree_output_file,
                   '-T', tree_depth,
                   '-C', cut_depth,
                   '-N', max_nodes_count
                   ]
        process = subprocess.Popen(command)
        output, error = process.communicate()
        print(output)
        if error is not None:
            raise Exception(f'There was an error while building k2trees:\n{str(error)}\n')

        os.remove(self.triples_ids_sorted_file)

    def run(self):
        print('Splitting .nt file...')
        self.run_splitter()
        print('Sorting resources uniquely...')
        self.run_sort_parts()
        print('Building String Dictionaries...')
        self.build_string_dictionaries()
        print('Converting NT to Ids...')
        self.convert_nt_to_ids_file()
        print('Sorting triples ids file...')
        self.sort_ids_file()
        print('Building K2Trees...')
        self.build_k2trees()


IndexBuilder(config).run()
