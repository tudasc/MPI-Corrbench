import os
import argparse
import json
import webbrowser

## entry: name: [TP,TN,FP,FN,ERR,error_present,case_id,full_case_name]
# True Positive, True Negative, False Positive, False negative,
# ERR=error in parsing the output or runnung case,
# error_present: if the error actually manifested during execution,
# case_id, full_case_name for later more in depth analysis refers to the dir_name

TP = 0
TN = 1
FP = 2
FN = 3
TW = 4
FW = 5
ERR = 6
error_present = 7
case_id = 8
full_case_name = 9


# read env vars
BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];
INPUT_DIR = os.environ["MPI_CORRECTNESS_BM_EXPERIMENT_DIR"];

TOOLS = ['MUST', 'ITAC']


def add_cases(score, case):
    score[0] += case[0]
    score[1] += case[1]
    score[2] += case[2]
    score[3] += case[3]
    score[4] += case[4]
    score[5] += case[5]
    score[6] += case[6]
    score[7] += case[7]
    assert score[case_id] == case[case_id]
    assert score[full_case_name] == case[full_case_name]

    return score


def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that evaluates the report from a tool"
        )
    )

    parser.add_argument('--outfile', default="[BENCH_BASE_DIR]/output/results_exploration.json")

    args = parser.parse_args()
    return args

def explore_case(tools_result_data,case,tool):

    print(tool)

    err_present_err_found = 0
    err_present_not_found = 0

    not_present_err_found = 0
    not_present_not_found = 0

    # try to find cases for all 4 categories:
    err_present_err_found_jid = -1
    err_present_not_found_jid = -1

    not_present_err_found_jid = -1
    not_present_not_found_jid = -1

    count = 0
    for jid in tools_result_data[tool]:
        case_data = tools_result_data[tool][jid][case]
        count += 1

        if case_data[error_present] > 0:
            if case_data[TP] > 0:
                err_present_err_found += 1
                err_present_err_found_jid = jid
            else:
                err_present_not_found += 1
                err_present_not_found_jid = jid
        if case_data[error_present] == 0:
            if case_data[TP] > 0:
                not_present_err_found += 1
                not_present_err_found_jid = jid
            else:
                not_present_not_found += 1
                not_present_not_found_jid = jid

    #sanity check:
    if err_present_err_found>0:
        assert err_present_err_found_jid!=-1
    else:
        assert err_present_err_found_jid==-1
    if err_present_not_found>0:
        assert err_present_not_found_jid!=-1
    else:
        assert err_present_not_found_jid==-1

    if not_present_err_found>0:
        assert not_present_err_found_jid!=-1
    else:
        assert not_present_err_found_jid==-1
    if not_present_not_found>0:
        assert not_present_not_found_jid!=-1
    else:
        assert not_present_not_found_jid==-1


    if case_data[ERR]>0:

       # print("percent error present:%f" % ((err_present_err_found + err_present_not_found) / count))
       # if (err_present_err_found + err_present_not_found) > 0:
       #     print("percent found if present:%f" % (err_present_err_found / (err_present_err_found + err_present_not_found)))
       # if (not_present_err_found + not_present_not_found) > 0:
       #     print(
       #         "percent found NOT present:%f" % (not_present_err_found / (not_present_err_found + not_present_not_found)))

        choice = input("Type 1 if you want to inspect it in depth")

        if choice=="1":

            case_data = tools_result_data[tool][jid][case]
            print(case_data)

            if tool=='MUST':
                jid= err_present_not_found_jid

                case_log_file= INPUT_DIR + "/" + tool+"/job"+jid+"_"+case+".out"
                case_dir= INPUT_DIR + "/" + tool+"/"+jid+"/"+case

                with open(case_log_file, 'r') as f:
                    print(f.read())
                webbrowser.open('file://'+case_dir+"/MUST_Output.html")

            if tool == 'ITAC':
                jid = err_present_not_found_jid
                case_log_file = INPUT_DIR + "/" + tool + "/job" + jid + "_" + case + ".out"
                case_dir = INPUT_DIR + "/" + tool + "/" + jid + "/" + case

                print("log:")
                with open(case_log_file, 'r') as f:
                    print(f.read())
                print("output:")
                with open(case_dir+"/output.txt", 'r') as f:
                    print(f.read())

            print("")


def read_tool_data():
    tools_result_data = {}
    jobs = 0
    # read all available input data
    for tool in TOOLS:
        tools_result_data[tool] = {}

        # advanced data evaluate if tool found correct error
        for test_dir in os.scandir(INPUT_DIR + "/" + tool):
            # only read the directories
            if not test_dir.is_dir():
                continue
                # exclude mini apps
            if test_dir.name == "kripke" or test_dir.name == "amg2013" or test_dir.name == "lulesh":
                continue

            jobs += 1

            job_id = test_dir.name
            # read the data from dir
            data = {}
            with open(test_dir.path + "/results.json", 'r') as f:
                data = json.load(f)
                tools_result_data[tool][job_id] = data
    print("Read Data from %i jobs" % (jobs))

    return tools_result_data


def main():
    if (not BENCH_BASE_DIR):
       print("Error: provide BENCH_BASE_DIR environment variable")
    args = parse_command_line_args()

    outfile = args.outfile.replace("[BENCH_BASE_DIR]", BENCH_BASE_DIR)

    tools_result_data = read_tool_data()

    explored_data = {}

    # comment if data should be overwriten
    #with open(outfile, 'r') as f:
    #    explored_data = json.load(f)

    #explore data

    for case in next(iter( tools_result_data[TOOLS[0]].values() )):
        print(next(iter( tools_result_data[TOOLS[0]].values() ))[case][full_case_name])

        # MUST
        tool = 'MUST'
        explore_case(tools_result_data, case, tool)
        tool = 'ITAC'
        explore_case(tools_result_data, case, tool)






    with open(outfile, 'w') as file:
        json.dump(explored_data, file)

if __name__ == '__main__':
    main()
