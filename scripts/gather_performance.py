import os
import argparse
import importlib.util
import json
import csv

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];
INPUT_DIR = os.environ["MPI_CORRECTNESS_BM_EXPERIMENT_DIR"];


# compile #'time, 'baseline_time','mem','baseline_mem' run#'time, 'baseline_time','mem','baseline_mem'

def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that evaluates the report from a tool"
        )
    )

    parser.add_argument('TOOL', choices=['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH', 'TODO_More_Tools'])
    parser.add_argument('--outfile', default="[BENCH_BASE_DIR]/output/performance_[TOOL].json")

    args = parser.parse_args()
    return args


def main():
    # if (not BENCH_BASE_DIR):
    #    print("Error: provide BENCH_BASE_DIR environment variable")
    args = parse_command_line_args()

    # testcases = os.listdir(OUT_DIR +"/" + args.TOOL)

    data = {}

    for job_dir in os.scandir(INPUT_DIR + "/" + args.TOOL):
        # only read the directories
        if not job_dir.is_dir():
            continue
        for test_dir in os.scandir(job_dir):
            # only read the directories
            if not test_dir.is_dir():
                continue

            case_id = test_dir.name
            # read the case name from directory

            # if data is not present: fill with 0
            compile_data = [0, 0, 0, 0]
            run_data = [0, 0, 0, 0]

            try:
                with open(test_dir.path + "/compile_overhead.csv", "r") as file:
                    reader = csv.DictReader(file)
                    for row in reader:
                        compile_data = [
                            float(row['time']), float(row['baseline_time']), float(row['mem']),
                            float(row['baseline_mem'])]
            except (FileNotFoundError, ValueError):
                pass
            try:
                with open(test_dir.path + "/run_overhead.csv", "r") as file:
                    reader = csv.DictReader(file)
                    for row in reader:
                        run_data = [
                            float(row['time']), float(row['baseline_time']), float(row['mem']),
                            float(row['baseline_mem'])]
            except (FileNotFoundError, ValueError):
                pass

            perf_data = compile_data + run_data

            if sum(perf_data) > 0:
                if not case_id in data:
                    data[case_id] = [perf_data]
                data[case_id].append(perf_data)

    outfile = args.outfile.replace("[TOOL]", args.TOOL).replace("[BENCH_BASE_DIR]", BENCH_BASE_DIR)
    with open(outfile, 'w') as file:
        json.dump(data, file)


if __name__ == '__main__':
    main()
