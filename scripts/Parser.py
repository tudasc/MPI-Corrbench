import os
import argparse
import importlib.util
import json

# documentation of parse_output
# input:
# input_dir: the directory where the tool was run and output was captured
# is_error_expected: bool if an error is expected

# return values:
# error_found count off errors found
# -1 means could not tell (e.g. output is missing or not helpful)
# correct_error_found wether or not exactely the expected error was found
# -1 means could not tell (e.g. output is missing or not helpful or error specification missing) 0 means false, 1 true


# entry: name: [TP,TN,FP,FN,TW,TN,ERR,case_id]
# True Positive, True Negative, False Positive, False negative, True Warning,False Warning,ERR=error in parsing the output or runnung case, case_id for later analysis refers to the dir_name
TP = 0
TN = 1
FP = 2
FN = 3
TW = 4
FW = 5
ERR = 6
case_id = 7

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];


def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that evaluates the report from a tool"
        )
    )

    parser.add_argument('IN_DIR')
    parser.add_argument('TOOL', choices=['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH'])
    parser.add_argument('--outfile', default="results.json")


    args = parser.parse_args()
    return args


# Helper function to for importing stuff
def module_from_path(filepath):
    module_name = os.path.splitext(os.path.basename(filepath))[0]
    spec = importlib.util.spec_from_file_location(module_name, filepath)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    if (not BENCH_BASE_DIR):
        print("Error: provide MPI_CORRECTNESS_BM_DIR environment variable")
    args = parse_command_line_args()
    SCRIPTS_DIR = args.BENCH_BASE_DIR + "/scripts"
    module_name = SCRIPTS_DIR + "/" + args.TOOL + "/parse-output.py"
    parser = module_from_path(module_name)

    basic_data = {}
    # basic data: evaluate based if tool found any error
    # entry: name: [TP,TN,FP,FN]
    # True Positive, True Negative, False Positive, False negative
    for test_dir in os.scandir(args.IN_DIR):
        # only read the directories
        if not test_dir.is_dir():
            continue

        case_id=test_dir.name
        # read the case name from directory
        with open(test_dir.path + "/case_name", 'r') as f:
            full_case = f.read().rstrip()
            case = os.path.basename(full_case)

        code_has_error = True
        if "correct/" in full_case:
            code_has_error = False

        error_found, correct_error_found = parser.parse_output(test_dir.path, code_has_error, "")
        data = [0, 0, 0, 0,0,0, 0, case_id,full_case]

        ## -1 = error processing case
        if error_found == -1:
            data[ERR] = 1
        else:
            if code_has_error:
                if error_found >0:
                    # TP
                    data[TP] = 1
                else:
                    if error_found ==-2:
                        data[TW]=1
                    else:
                        data[FN] = 1
            else:
                if error_found>0:
                    # FP
                    data[FP] = 1
                else:
                    if error_found == -2:
                        data[FW] = 1
                    else:
                        data[TN] = 1

        basic_data[case_id] = data

    outfile = args.IN_DIR + "/" + args.outfile
    with open(outfile, 'w') as file:
        json.dump(basic_data, file)

if __name__ == '__main__':
    main()
