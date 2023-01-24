# setup
from unicodedata import category

from analyze_helper import *
import os
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import matplotlib.gridspec as gridspec
import argparse
import numpy as np

import operator

TOOLS = ['MUST', 'ITAC']

openmp_categories = ['openmp/data_race', 'openmp/ordering', 'openmp/threading', 'openmp/memory']

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];
INPUT_DIR = os.environ["MPI_CORRECTNESS_BM_EXPERIMENT_DIR"];


def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that generates the evaluation plots"
        )
    )

    parser.add_argument('--format', default="pdf")

    args = parser.parse_args()
    return args


def read_tool_data():
    tools_result_data = {}
    jobs = 0
    # read all available input data
    for tool in TOOLS:
        tools_result_data[tool] = {}

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


# from https://stackoverflow.com/questions/37765197/darken-or-lighten-a-color-in-matplotlib
def lighten_color(color, amount=0.5):
    """
    Lightens the given color by multiplying (1-luminosity) by the given amount.
    Input can be matplotlib color string, hex string, or RGB tuple.

    Examples:
    >> lighten_color('g', 0.3)
    >> lighten_color('#F034A3', 0.6)
    >> lighten_color((.3,.55,.1), 0.5)
    """
    import matplotlib.colors as mc
    import colorsys
    try:
        c = mc.cnames[color]
    except:
        c = color
    c = colorsys.rgb_to_hls(*mc.to_rgb(c))
    return colorsys.hls_to_rgb(c[0], 1 - amount * (1 - c[1]), c[2])


# [TN,TP,GW,FP,FN,ERR]

def get_plot(category, data, output_format):
    ftsize=12
    plt.rcParams.update({'font.size': ftsize})
    # plt.rcParams.update({'font.size': 18, 'hatch.linewidth': 0.0075})

    plt.clf()

    # pos = [1, 2.15, 3, 4.25]
    # names = ["with\ntool", "no\ntool"]

    case_count = len(data["MUST"][category])
    assert(case_count == len(data["ITAC"][category]))

    barWidth = 0.75
    spacing = 0.25
    # relative_x = [-0.375, -0.225, -0.075, 0.075, 0.225, 0.375]

    space_between_upper_and_lower = 0
    space_for_no_tool = 1

    stop = 1 + space_for_no_tool + (barWidth + spacing) * case_count * 2

    pos1 = np.arange(1, 1 + (barWidth + spacing) * case_count, barWidth + spacing)
    pos2 = np.arange(1 + (barWidth + spacing) * case_count + space_for_no_tool, stop, barWidth + spacing)

    #pos = np.concatenate((pos1, pos2))

    # fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=(8, 4), sharey=True)
    # fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, sharey=True)
    figsz= (8,6)
    labelpos = (-0.07, -0.055,1,1)
    if "ordering" in category:
        # more yspace
        figsz = (8, 7.5)
        labelpos = (-0.08, -0.0435,1,1)
    if "memory" in category:
        # less yspace
        figsz = (8, 4.5)
        labelpos = (-0.07, -0.0435,1,1)
    fig = plt.figure(figsize=figsz, tight_layout=True)
    #fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=figsz, sharey=True)
    spec = gridspec.GridSpec(ncols=4, nrows=2, figure=fig)

    ax1 = fig.add_subplot(spec[1, 0:2])
    ax2 = fig.add_subplot(spec[1, 2:4])
    ax3 = fig.add_subplot(spec[0, 1:3])

    # plt.title("Upper:No buffer corruption, lower buffer corruption due to datarace")
    ax1.set_title('MUST', fontsize=ftsize)
    ax2.set_title('ITAC', fontsize=ftsize)
    ax3.set_title('without tool', fontsize=ftsize)

    ax1.set_xlabel("number of runs")
    ax2.set_xlabel("number of runs")

    ax1.set_ylabel("test case number")
    ax2.set_ylabel("test case number")
    ax3.set_ylabel("test case number")
    #if "ordering" in category:
    #    # more spacing
    #    ax1.set_ylabel("     eval with tool                     eval without tool  \n test case number")


    # plot settings
    handleheight = 2.5
    # color scheme From Paul Tol: https://personal.sron.nl/~pault/
    # Tol_bright =['#EE6677', '#228833', '#4477AA', '#CCBB44', '#66CCEE', '#AA3377', '#BBBBBB']
    # colors = ['#EE6677', '#228833', '#AA3377', '#CCBB44', '#66CCEE', '#BBBBBB']
    colors = ['#88cc88', '#814db3', '#111111', '#88cc88', '#814db3', '#111111']
    hatches = [None, None, None, None, None, None]
    hatches = [None, None, None, '//', '//', '//']
    edge_col = 'white'

    ylabels = []

    index_list = range(case_count)
    for ax in [ax1, ax2,ax3]:
        if ax == ax1:
            cases=sorted(data["MUST"][category],key =operator.itemgetter(6))
        if ax == ax2:
            cases=sorted(data["ITAC"][category],key =operator.itemgetter(6))
        if ax == ax3:
            cases = sorted(data["MUST"][category], key=operator.itemgetter(6))

        # sort by case id, so that it is no longer random
        # 6 is id, 7 full case name
        #eror_present = sorted(eror_present, key=operator.itemgetter(1, 6))
        #not_present = sorted(not_present, key=operator.itemgetter(1, 6))

        for idx in index_list:
            bot = 0
            if ax != ax3:
                tp = ax.barh(pos1[idx], int(cases[idx][0]), left=bot, color=colors[0],
                             hatch=hatches[0],
                             edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][0])
                fn = ax.barh(pos1[idx], int(cases[idx][1]), left=bot, color=colors[1], hatch=hatches[1],
                             edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][1])
                f = ax.barh(pos1[idx], int(cases[idx][2]), left=bot, color=colors[2], hatch=hatches[2],
                            edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][2])

            # space between present and not present
                bot = bot + space_between_upper_and_lower

                tp_t = ax.barh(pos1[idx], int(cases[idx][3]), left=bot, color=colors[3],
                               hatch=hatches[3],
                               edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][3])
                fn_t = ax.barh(pos1[idx], int(cases[idx][4]), left=bot, color=colors[4], hatch=hatches[4],
                               edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][4])
                f_t = ax.barh(pos1[idx], int(cases[idx][5]), left=bot, color=colors[5], hatch=hatches[5],
                              edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][5])

            else:
                bot=0

                # part without tool
                ax.barh(pos1[idx], int(cases[idx][8]), left=bot, color=colors[1], hatch=hatches[1],
                             edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][8])
                ax.barh(pos1[idx], int(cases[idx][9]), left=bot, color=colors[4], hatch=hatches[4],
                               edgecolor=edge_col, height=barWidth)
                bot = bot + int(cases[idx][9])
                ax.barh(pos1[idx], int(cases[idx][10]), left=bot, color=colors[2], hatch=hatches[2],
                               edgecolor=edge_col, height=barWidth)

                ylabels.append(cases[idx][6])



    # Custom y axis
    ax1.set_yticks(pos1)
    ax1.set_yticklabels(ylabels, fontsize=ftsize-2)
    ax1.set_ymargin(0.01)

    ax2.set_yticks(pos1)
    ax2.set_yticklabels(ylabels, fontsize=ftsize - 2)
    ax2.set_ymargin(0.01)
    ax2.yaxis.tick_right()
    ax2.yaxis.set_label_position("right")

    ax3.set_yticks(pos1)
    ax3.set_yticklabels(ylabels, fontsize=ftsize-2)
    ax3.set_ymargin(0.01)
    # ax2.set_yticks([1 + case_count / 2 * barWidth, stop - case_count / 2 * barWidth])
    # ax2.set_yticklabels(names)

    ax1.set_xticks([20, 40, 60, 80, 100, 120])
    ax1.set_xticklabels([20, 40, 60, 80, 100, 120], fontsize=ftsize-2)
    ax2.set_xticks([20, 40, 60, 80, 100, 120])
    ax2.set_xticklabels([20, 40, 60, 80, 100, 120], fontsize=ftsize-2)
    ax3.set_xticks([20, 40, 60, 80, 100, 120])
    ax3.set_xticklabels([20, 40, 60, 80, 100, 120], fontsize=ftsize-2)

    ax1.set_axisbelow(True)
    ax2.set_axisbelow(True)
    ax1.grid(axis='x')
    ax2.grid(axis='x')

    #dummy_label = ax1.plot([0], marker='None',
    #                       linestyle='None', label='dummy-tophead')
    dummy_white_line = mlines.Line2D([], [], color='white')
    label_handler = (dummy_white_line,tp, fn, f, dummy_white_line,tp_t, fn_t, f_t)
    label_text = (
        'True Positive', 'False Negative', 'Tool Crashed')
    label_text = ("Error Observable",'Found', 'Missed', 'Crash', "Not Observable",'Found', 'Missed', 'Crash')

    # Add a legend
    #plt.legend(label_handler, label_text, loc='lower center', bbox_to_anchor=labelpos, ncol=6, columnspacing=0.8,
    #           handletextpad=0.5, handlelength=1, title="Error Observable                             Not Observable")
    fig.tight_layout()


    fig.legend(label_handler, label_text, loc='upper right',bbox_to_anchor=labelpos, ncol=1, columnspacing=0.8,
               handletextpad=0.5, handlelength=1)
    # plt.legend(label_handler, label_text, loc='lower center', bbox_to_anchor=(0, -0.35),ncol=6, columnspacing=0.8,
    #           handletextpad=0.5, handlelength=1)
    # save to pdf


    output_name = "nondeterminism_eval" + category.replace("openmp/", "_")
    plt.savefig(output_name + "." + output_format, bbox_inches='tight')


def add_score_nondeterminism(score, case):
    # score=
    #[PRESENT_detected,PRESENT_ missed, PRESENT_crashed, detected, missed, crashed, ID, name,PRESENT_no_tool,no_tool,crash_no_tool]
    if case[error_present] > 0:
        score[0] += int(case[TP])
        score[0] += int(case[TW])
        score[1] += int(case[FN])
        score[2] += int(case[ERR])
    else:
        score[3] += int(case[TP])
        score[3] += int(case[TW])
        score[4] += int(case[FN])
        score[5] += int(case[ERR])

    if score[6] != "_":
        assert score[6] == case[9]
    score[6] = case[9]

    if score[7] != "_":
        assert score[7] == case[10]
    score[7] = case[10]

    if int(case[exit_code_without_tool]) != 0:
        score[10]+=1
    elif int(case[error_present_without_tool]) >0:
        score[8] += 1
    else:
        score[9]+=1


    return score


def main():
    ARGS = parse_command_line_args()

    tools_result_data = read_tool_data()

    data = {}
    data_correct = {}

    for tool in TOOLS:
        data[tool] = {}
        data_correct[tool] = {}

        for category in openmp_categories:
            data[tool][category] = []

        for case in next(iter(tools_result_data[TOOLS[0]].values())):
            # print(next(iter( tools_result_data[TOOLS[0]].values() ))[case][full_case_name])
            ##[TN,TP,GW,FP,FN,ERR]

            case_score = [0, 0, 0, 0, 0, 0, '_', '_',0, 0, 0]

            category = ""
            correct = False
            count = 0
            for jid in tools_result_data[tool]:
                if case in tools_result_data[tool][jid]:
                    case_data = tools_result_data[tool][jid][case]
                    count += 1
                    category = get_category(case_data)
                    correct = is_correct_case(case_data)
                    case_score = add_score_nondeterminism(case_score, case_data)


            if not correct:
                naming = load_case_names(BENCH_BASE_DIR)
                naming["_"] = "-"
                case_score[6]= naming[case_score[7]]


                data[tool][category].append(case_score)


    print("generate plots")

    for category in openmp_categories:
        #case_count = len(data['MUST'][category]['tool_present'])
        # plot only for data_race, other skript for other category
        get_plot(category, data, ARGS.format)
        print(category)

    # get_plot("nondeterminism_eval_" + tool, data, count, ARGS.format)
   # print(data_correct)


if __name__ == '__main__':
    main()
