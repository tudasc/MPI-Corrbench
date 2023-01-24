# setup
from analyze_helper import *
import os
import matplotlib.pyplot as plt
import argparse

Tools = ['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH']

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];

def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that generates the evaluation plots"
        )
    )

    parser.add_argument('--format', default="pdf")

    args = parser.parse_args()
    return args


def get_plot(output_name, fail, true_negative, true_positive, false_negative, false_positive, good_warn,output_format):
    plt.rcParams.update({'font.size': 18})
    pos = [1, 2.15, 3, 4.25]
    names_1 = ["MUST", "ITAC"]
    names_2 = ["MPI-Checker", "PARCOACH"]

    relative_x = [-0.375, -0.225, -0.075, 0.075, 0.225, 0.375]

    fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=(8, 4), sharey=True)
    ax1.set_title('Dynamic Tools')
    ax2.set_title('Static Tools')
    ax1.set_ylabel("Number of Cases")

    # plot settings
    handleheight = 2.5
    barWidth = 0.15
    # color scheme From Paul Tol: https://personal.sron.nl/~pault/
    # Tol_bright =['#EE6677', '#228833', '#4477AA', '#CCBB44', '#66CCEE', '#AA3377', '#BBBBBB']
    colors = ['#EE6677', '#228833', '#AA3377', '#CCBB44', '#66CCEE', '#BBBBBB']
    hatches = [None, None, None, None, None, None]
    edge_col = 'white'

    for ax in [ax1, ax2]:
        if ax == ax1:
            index_list = [0, 1]
        if ax == ax2:
            index_list = [2, 3]

        for idx in index_list:
            bot = 0
            tn = ax.bar(pos[idx] + relative_x[0], true_negative[idx], bottom=bot, color=colors[0], hatch=hatches[0],
                        edgecolor=edge_col, width=barWidth)
            # uncomment for stacked bar plot:
            # bot = true_negative[idx]
            tp = ax.bar(pos[idx] + relative_x[1], true_positive[idx], bottom=bot, color=colors[1], hatch=hatches[1],
                        edgecolor=edge_col, width=barWidth)
            # bot = bot+ true_positive[idx]
            w = ax.bar(pos[idx] + relative_x[2], good_warn[idx], bottom=bot, color=colors[2], hatch=hatches[2],
                       edgecolor=edge_col, width=barWidth)
            # bot = bot+ good_warn[idx]
            fp = ax.bar(pos[idx] + relative_x[3], false_positive[idx], bottom=bot, color=colors[3], hatch=hatches[3],
                        edgecolor=edge_col, width=barWidth)
            # bot= bot + false_positive[idx]
            fn = ax.bar(pos[idx] + relative_x[4], false_negative[idx], bottom=bot, color=colors[4], hatch=hatches[4],
                        edgecolor=edge_col, width=barWidth)
            # bot =bot+ false_negative[idx]
            f = ax.bar(pos[idx] + relative_x[5], fail[idx], bottom=bot, color=colors[5], hatch=hatches[5],
                       edgecolor=edge_col, width=barWidth)
            # bot = bot +fail[idx]

            ax.bar_label(f, padding=1.5)
            ax.bar_label(fn, padding=1.5)
            ax.bar_label(fp, padding=1.5)
            ax.bar_label(w, padding=1.5)
            ax.bar_label(tp, padding=1.5)
            ax.bar_label(tn, padding=1.5)
        
        #increase space for labels
        min_y,max_y=ax.get_ybound()
        ax.set_ybound([min_y,max_y*1.05])

    # reverse label ordering
    label_handler = (f, fn, fp, w, tp, tn)[::-1]
    label_text = (
                 'Tool Crashed', 'False Negative', 'False Positive', 'Warning Issued', 'True Positive', 'True Negative')[
                 ::-1]
    label_text = ('Crash', 'FN', 'FP', 'Warn', 'TP', 'TN')[::-1]


    # Custom x axis
    ax1.set_xticks(pos[0:2])
    ax1.set_xticklabels(names_1)
    ax2.set_xticks(pos[2:4])
    ax2.set_xticklabels(names_2)

    fig.tight_layout()

    # Add a legend
    plt.legend(label_handler, label_text, loc='lower center', bbox_to_anchor=(-0.25, -0.35), ncol=6, columnspacing=0.8,
               handletextpad=0.5, handlelength=1)
    # save to pdf
    plt.savefig(output_name + "."+output_format, bbox_inches='tight')

def main():

    ARGS = parse_command_line_args()

    raw_data = load_data(Tools,BENCH_BASE_DIR)
    data = reduce_data(load_data(Tools,BENCH_BASE_DIR),Tools)
    by_tools=score_by_tool(Tools,data)
    by_category=score_by_category(Tools,data)

    fail = [by_tools[tool][ERR] for tool in Tools]
    true_negative = [by_tools[tool][TN] for tool in Tools]
    true_positive = [by_tools[tool][TP] for tool in Tools]
    false_negative = [by_tools[tool][FN] for tool in Tools]
    false_positive = [by_tools[tool][FP] for tool in Tools]
    good_warn = [by_tools[tool][TW] for tool in Tools]
    bad_warn = [by_tools[tool][FW] for tool in Tools]

    true_negative = [x + y for x, y in zip(bad_warn, true_negative)]

    get_plot("basic_eval", fail, true_negative, true_positive, false_negative, false_positive, good_warn,ARGS.format)

    # plot openmp overview
    #TODO

    # plot by category
    for category in categories:
        # Data
        fail1 = [by_category[category]['base'][tool][ERR] for tool in Tools]
        fail2 = [by_category[category]['conflo'][tool][ERR] for tool in Tools]
        fail = [x + y for x, y in zip(fail1, fail2)]
        true_negative1 = [by_category[category]['base'][tool][TN] for tool in Tools]
        true_negative2 = [by_category[category]['conflo'][tool][TN] for tool in Tools]
        true_negative = [x + y for x, y in zip(true_negative1, true_negative2)]
        true_positive1 = [by_category[category]['base'][tool][TP] for tool in Tools]
        true_positive2 = [by_category[category]['conflo'][tool][TP] for tool in Tools]
        true_positive = [x + y for x, y in zip(true_positive1, true_positive2)]
        false_negative1 = [by_category[category]['base'][tool][FN] for tool in Tools]
        false_negative2 = [by_category[category]['conflo'][tool][FN] for tool in Tools]
        false_negative = [x + y for x, y in zip(false_negative1, false_negative2)]
        false_positive1 = [by_category[category]['base'][tool][FP] for tool in Tools]
        false_positive2 = [by_category[category]['conflo'][tool][FP] for tool in Tools]
        false_positive = [x + y for x, y in zip(false_positive1, false_positive2)]

        good_warn1 = [by_category[category]['base'][tool][TW] for tool in Tools]
        good_warn2 = [by_category[category]['conflo'][tool][TW] for tool in Tools]
        good_warn = [x + y for x, y in zip(good_warn1, good_warn2)]
        bad_warn1 = [by_category[category]['base'][tool][FW] for tool in Tools]
        bad_warn2 = [by_category[category]['conflo'][tool][FW] for tool in Tools]
        bad_warn = [x + y for x, y in zip(bad_warn1, bad_warn2)]

        true_negative = [x + y for x, y in zip(true_negative, bad_warn)]

        get_plot(category.replace("/", "_") + "_eval", fail, true_negative, true_positive, false_negative, false_positive, good_warn,ARGS.format)

if __name__ == '__main__':
    main()
