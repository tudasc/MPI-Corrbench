import os
import argparse
import importlib.util
import json
import shutil
import subprocess
import glob
import pandas as pd

from analyze_helper import *

# helper script for manually checking if the error message is actually helpful


# input dir to use, we need to specify the path to the data one wants to inspect
out_dir_itac = ""
out_dir_must = ""

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];
INPUT_DIR = os.environ["MPI_CORRECTNESS_BM_EXPERIMENT_DIR"];

TOOLS = [
    'MUST',
    'ITAC'
]

# option if wan wants to recheck previous results
confirm_previous_data = False
# script must execute from working dir
output_name = "correct_messages.json"


def main():
    if (not BENCH_BASE_DIR):
        print("Error: provide BENCH_BASE_DIR environment variable")
    if (out_dir_itac=="" or out_dir_must==""):
        print("Error: provide the specific Paths which data you want to inspect")

    casefile = BENCH_BASE_DIR + "/output/" + output_name

    case_data = {}
    result_data = reduce_data(load_data(TOOLS, BENCH_BASE_DIR), TOOLS)
    with open(casefile, 'r') as file:
        case_data = json.load(file)

    case_data = ask_cases(case_data, result_data,casefile)

    with open(casefile, 'w') as file:
        json.dump(case_data, file, sort_keys=True, indent=4)


def print_tool_out(tool, case):
    if tool == "MUST":
        fname = out_dir_must + "/" + case + "/MUST_Output.html"

        # read must html report
        table = pd.read_html(fname)[0]
        is_error = table[1] == "Error"
        errors_only = table[is_error]
        # if no exeption was thrown, the report was read (it my be empty though)
        # index 2 is the short error message
        for row in errors_only[2]:
            print(row)

    if tool == "ITAC":
        fname = out_dir_itac + "/" + case + "/output.txt"
        with open(fname, mode='r') as file:
            data = content = file.readlines()
            for row in data:
                if ("ERROR:" in row):
                    print(row)

    return None


def ask_cases(case_data, result_data,casefile):
    SCRIPTS_DIR = BENCH_BASE_DIR + "/scripts"
    SRC_DIR = BENCH_BASE_DIR + "/micro-benches"

    for test_dir in os.scandir(out_dir_must):
        # only read the directories
        if not test_dir.is_dir():
            continue

        case_id = test_dir.name

        if not case_id in case_data:
            case_data[case_id] = {}
        for tool in TOOLS:
            if not confirm_previous_data and tool in case_data[case_id]:
                # next case
                continue
            if not result_data[tool][case_id][TP] > 0:
                # next case
                continue

            os.system('clear')#output is better readable
            case_fname = result_data[tool][case_id][full_case_name]
            print("case: %s: %s" % (case_id, case_fname))
            # print example until main starts
            with open(case_fname) as src_file:
                finished = False
                while not finished:
                    line = next(src_file).strip()
                    print(line)
                    finished = line.startswith("int main(")

            print(
            "\nTool: %s\n"%(tool))
            print_tool_out(tool, case_id)
            print(
                "\nIs this Error message False (type 1 if so)?\n")
            input_str = input()
            if input_str == "END":
                return case_data;
            if input_str != '1':
                input_str = 0
            case_data[case_id][tool] = input_str

            # dump datas in case of script crasing
            with open(casefile, 'w') as file:
                json.dump(case_data, file)

    print("has data for all cases")
    return case_data


if __name__ == '__main__':
    main()
