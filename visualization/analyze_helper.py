import json

input_fname_pattern = "[BENCH_BASE_DIR]/output/results_[TOOL].json"
input_fname_pattern_mini_app_performance = "[BENCH_BASE_DIR]/output/mini_app_performance_[TOOL].json"

## entry: name: [TP,TN,FP,FN,ERR,error_present,user_expectation_met,case_id,full_case_name]
# True Positive, True Negative, False Positive, False negative,
# ERR=error in parsing the output or runnung case,
# error_present: if the error actually manifested during execution,
# user_expectation_met: if the user_expactation_met Flag was set ture by the testcase,
# case_id, full_case_name for later more in depth analysis refers to the dir_name

TP = 0
TN = 1
FP = 2
FN = 3
TW = 4
FW = 5
ERR = 6
error_present = 7
user_expectation_met = 8
case_id = 9
full_case_name = 10

#categories = ['pt2pt', 'coll', 'usertypes', 'rma']
categories = ['pt2pt', 'coll', 'usertypes', 'rma', 'openmp']

# compile #'time, 'baseline_time','mem','baseline_mem' run#'time, 'baseline_time','mem','baseline_mem'
time_compile = 0
time_baseline_compile = 1
mem_compile = 2
mem_baseline_compile = 3
time_run = 4
time_baseline_run = 5
mem_run = 6
mem_baseline_run = 7

DEADLOCK_TIME=110.0


def get_category(this_case):
    name = this_case[full_case_name]

    category = None
    for canidate in categories:
        if canidate + "/" in name:
            # only one category
            assert category == None
            category = canidate
    if 'datatype/' in name:
        # only one category
        assert category == None
        category = 'usertypes'
    return category


def add_score(score, case):
    score[0] += case[0]
    score[1] += case[1]
    score[2] += case[2]
    score[3] += case[3]
    score[4] += case[4]
    score[5] += case[5]
    score[6] += case[6]
    return score

def load_mini_app_performance_data(tools, bench_base_dir):
    data = {}
    for tool in tools:
        fname = input_fname_pattern_mini_app_performance
        fname = fname.replace("[TOOL]", tool).replace("[BENCH_BASE_DIR]", bench_base_dir)
        with open(fname, 'r') as file:
            data[tool] = json.load(file)
    return data


def load_data(tools, bench_base_dir):
    data = {}
    for tool in tools:
        fname = input_fname_pattern
        fname = fname.replace("[TOOL]", tool).replace("[BENCH_BASE_DIR]", bench_base_dir)
        with open(fname, 'r') as file:
            data[tool] = json.load(file)
    return data


def reduce_data(data, tools):
    result = data

    for tool in tools:
        for key in result[tool]:
            case = result[tool][key]
            # check if error was found at least one time
            # case where the classifications differ will be explored by a different method so no need to take care about this
            # found: ignore warnings
            if (case[FP] > 0):
                assert not case[FN] > 0
                assert not case[TP] > 0
                case[FP] = 1
                case[TN] = 0
                case[TW] = 0
                case[FW] = 0
                case[ERR] = 0

            if (case[TP] > 0):
                assert not case[TN] > 0
                assert not case[FP] > 0
                case[TP] = 1
                case[FN] = 0
                case[TW] = 0
                case[FW] = 0
                case[ERR] = 0

            if (case[TW] > 0):
                assert not case[TP] > 0
                assert not case[FP] > 0
                assert not case[FW] > 0
                assert not case[TN] > 0
                case[TW] = 1
                case[FN] = 0
                case[ERR] = 0

            if (case[FW] > 0):
                assert not case[TP] > 0
                assert not case[FP] > 0
                assert not case[TW] > 0
                assert not case[FN] > 0
                case[TN] = 0
                case[FW] = 1
                case[ERR] = 0

            if (case[TN] > 0):
                assert not case[TP] > 0
                assert not case[FP] > 0
                assert not case[FN] > 0
                case[TN] = 1
                case[ERR] = 0

            if (case[FN] > 0):
                assert not case[TN] > 0
                assert not case[FP] > 0
                assert not case[TP] > 1
                case[FN] = 1
                case[ERR] = 0

            if (case[ERR] > 0):
                assert not case[TN] > 0
                assert not case[FP] > 0
                assert not case[TP] > 0
                assert not case[FN] > 0
                # else the case was catched before
                case[ERR] = 1

            result[tool][key] = case

    return result


# takes in reduced data
def score_by_tool(tools, data):
    result = {}

    for tool in tools:
        total_score = [0, 0, 0, 0, 0, 0, 0]
        for case in data[tool].values():
            total_score = add_score(total_score, case)
        result[tool] = total_score

    return result


# takes in reduced data
def score_by_case(tools, data):
    result = {}

    for case in data[tools[0]].keys():
        total_score = [0, 0, 0, 0, 0, 0, 0]
        for tool in tools:
            total_score = add_score(total_score, data[tool][case])

        id = data[tools[0]][case][case_id]
        total_score.append(id)
        result[case] = total_score

    return result

# takes in reduced data
# scores by category
def score_by_category(tools, data):
    result = {}
    for category in categories:
        result[category] = {'base': {}, 'conflo': {}}

    for tool in tools:
        for category in categories:
            result[category]['base'][tool] = [0, 0, 0, 0, 0, 0, 0]
            result[category]['conflo'][tool] = [0, 0, 0, 0, 0, 0, 0]

        for case in data[tool].values():
            name = case[full_case_name]
            complexity = 'base'
            if 'conflo/' in name:
                complexity = 'conflo'

            category = get_category(case)
            if category:
                score = add_score(result[category][complexity][tool], case)
                result[category][complexity][tool] = score

    return result