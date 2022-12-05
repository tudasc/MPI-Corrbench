from analyze_helper import *
import json
import os
import subprocess

input_fname_pattern = "[BENCH_BASE_DIR]/output/results_[TOOL].json"

Tools = ['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH']

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];

def load_data_and_merge(tools, bench_base_dir):
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

        for category in categories:
            try:
                fname = input_fname_pattern
                fname = fname.replace("[TOOL]", ("%s_%s" % (category,tool))).replace("[BENCH_BASE_DIR]", bench_base_dir)
                with open(fname, 'r') as file:
                    category_data= json.load(file)
                    for k, val in category_data.items():
                        data[tool][k+category] = val
            except:
                pass

    return data


def save_data(data, tools, bench_base_dir):
    for tool in tools:
        fname = input_fname_pattern
        fname = fname.replace("[TOOL]", tool).replace("[BENCH_BASE_DIR]", bench_base_dir)
        with open(fname, 'w') as file:
            json.dump(data[tool], file)

# will ignore the directroy names before 0-level and only consider the part after bench base dir
def is_same_case(case1,case2):
    return case1.split("0-level/")[-1] == case2.split("0-level/")[-1]


def add_missing_data(data, tools, bench_base_dir):
    find_out = subprocess.run(["find", ("%s/micro-benches/0-level" % BENCH_BASE_DIR), "-type", "f", "-name", "*.c"], stdout=subprocess.PIPE)
    testcases = find_out.stdout.decode("utf-8").split()

    for tool in tools:
        # dict keys are strings (originally they are the execution directories name)
        if(len(testcases) != len(data[tool])):
            for case in testcases:
                has_case=False
                for k,v in data[tool].items():
                    # need to add missing data
                    if is_same_case(case,v[full_case_name]):
                        has_case=True
                        break
                if not has_case:
                    data[tool][case]=[0,0,0,0,0,0,1,1,1,"0",case,"",1]

        assert (len(testcases) == len(data[tool]))
    return data

def main():
    data = load_data_and_merge(Tools,BENCH_BASE_DIR)
    data = add_missing_data(data,Tools, BENCH_BASE_DIR)
    save_data(data,Tools, BENCH_BASE_DIR)




if __name__ == '__main__':
     main()