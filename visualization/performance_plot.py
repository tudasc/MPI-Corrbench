# setup
from analyze_helper import *
import matplotlib.pyplot as plt
import numpy as np
import scipy.stats
import os

Tools = ['MUST', 'ITAC']

mini_apps = ["amg2013", "kripke", "lulesh" ]

BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];


# overhead
def get_overhead_abs(raw_data, tool, app):
    cases = [c for c in raw_data[tool][app].keys()]
    case = max(cases)

    baseline = [t[time_baseline_run] for t in raw_data[tool][app][case]]
    runtime = [t[time_run] for t in raw_data[tool][app][case]]
    return [x - y for x, y in zip(runtime, baseline)]


def get_overhead_percent(raw_data, tool, app):
    cases = [c for c in raw_data[tool][app].keys()]
    case = max(cases)

    baseline = [t[time_baseline_run] for t in raw_data[tool][app][case]]
    runtime = [t[time_run] for t in raw_data[tool][app][case]]
    return [100 * (x - y) / x for x, y in zip(runtime, baseline)]


def get_mem_overhead_abs(raw_data, tool, app):
    cases = [c for c in raw_data[tool][app].keys()]
    case = max(cases)

    baseline = [t[mem_baseline_run] for t in raw_data[tool][app][case]]
    runtime = [t[mem_run] for t in raw_data[tool][app][case]]
    return [x - y for x, y in zip(runtime, baseline)]


def get_mem_overhead_percent(raw_data, tool, app):
    cases = [c for c in raw_data[tool][app].keys()]
    case = max(cases)

    baseline = [t[mem_baseline_run] for t in raw_data[tool][app][case]]
    runtime = [t[mem_run] for t in raw_data[tool][app][case]]
    return [100 * (x - y) / x for x, y in zip(runtime, baseline)]


def get_plot(raw_data):
    output_name = "mini-app_performance"
    plt.rcParams.update({'font.size': 18})

    pos = [1, 2, 3]

    disp = [0.125, 0.125, 0.125]

    pos_must = [x - y for x, y in zip(pos, disp)]
    pos_itac = [x + y for x, y in zip(pos, disp)]

    # plot settings
    handleheight = 2.5
    barWidth = 0.25

    colors = ['#CCBB44', '#66CCEE']
    edge_col = 'white'

    must_data = [np.mean(get_overhead_percent(raw_data, "MUST", app)) for app in mini_apps]
    itac_data = [np.mean(get_overhead_percent(raw_data, "ITAC", app)) for app in mini_apps]
    # must_data = [np.mean(get_overhead_abs(raw_data,"MUST",app)) for app in mini_apps]
    # itac_data = [np.mean(get_overhead_abs(raw_data,"ITAC",app)) for app in mini_apps]

    mus = plt.bar(pos_must, must_data, color=colors[0], edgecolor=edge_col, width=barWidth)
    it = plt.bar(pos_itac, itac_data, color=colors[1], edgecolor=edge_col, width=barWidth)

    plt.bar_label(mus, padding=1.5, fmt="%1.1f")
    plt.bar_label(it, padding=1.5, fmt="%1.1f")

    #enough space for label
    plt.ylim([0,10.2])

    label_handler = (mus, it)
    label_text = ('MUST', 'ITAC')

    plt.ylabel("Overhead in percent")
    # Custom x axis
    plt.xticks(pos, mini_apps)

    # Add a legend
    plt.legend(label_handler, label_text, loc='upper left', bbox_to_anchor=(1, 1), ncol=1, handleheight=handleheight)
    # save to pdf and png
    plt.savefig(output_name + ".pdf", bbox_inches='tight')
    # plt.savefig(output_name+".png",bbox_inches='tight')

    # Show graphic
    #plt.show()

def main():
    raw_data = load_mini_app_performance_data(Tools, BENCH_BASE_DIR)
    get_plot(raw_data)

if __name__ == '__main__':
    main()
