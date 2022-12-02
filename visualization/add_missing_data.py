from analyze_helper import *
import json
import os
import subprocess

input_fname_pattern = "[BENCH_BASE_DIR]/output/results_[TOOL].json"

Tools = ['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH']

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];

def load_data(tools, bench_base_dir):
    data = {}
    for tool in tools:
        try:
            fname = input_fname_pattern
            fname = fname.replace("[TOOL]", tool).replace("[BENCH_BASE_DIR]", bench_base_dir)
            with open(fname, 'r') as file:
                data[tool] = json.load(file)
        except:
            # empty if data could not be loaded
            data[tool] = {}
    return data


def save_data(data, tools, bench_base_dir):
    for tool in tools:
        fname = input_fname_pattern
        fname = fname.replace("[TOOL]", tool).replace("[BENCH_BASE_DIR]", bench_base_dir)
        with open(fname, 'w') as file:
            json.dump(data[tool], file)

def add_missing_data(data, tools, bench_base_dir):
    find_out = subprocess.run(["find", ("%s/micro-benches/0-level" % BENCH_BASE_DIR), "-type", "f", "-name", "*.c"], stdout=subprocess.PIPE)
    testcases = find_out.stdout.decode("utf-8").split()

    for i in range(0, len(testcases)):
        for tool in tools:
        # dict keys are strings (originally they are the execution directories name)
            if not str(i) in data[tool]:
                # need to add missing data
                data[tool][str(i)]=[0,0,0,0,0,0,1,1,1,str(i),testcases[i],"",1]
            else:
                assert(len(data[tool][str(i)])>=12)
    return data

def main():
    data = load_data(Tools,BENCH_BASE_DIR)
    data = add_missing_data(data,Tools, BENCH_BASE_DIR)
    save_data(data,Tools, BENCH_BASE_DIR)




if __name__ == '__main__':
     main()