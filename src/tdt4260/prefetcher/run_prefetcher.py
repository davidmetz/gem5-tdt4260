#!/usr/bin/env python3
import os
import shutil
import sys
import subprocess

cwd = os.getcwd()
gem5_root = os.path.abspath("../../..")
gem5_bin = f"{gem5_root}/build/X86/gem5.opt"
config = f"{gem5_root}/configs/tdt4260/prefetcher.py"

binaries = ["gcc", "exchange2", "mcf", "deepsjeng", "x264"]
welcome_message = True

if (welcome_message):
    print('''
The prefetcher run script is now invoked and will run each benchmark from checkpoints.
Each run will load a checkpoint from mid-execution, and then run for a set amount of instructions.
This run will generate results that you can view afterwards. Your goal is to improve the IPC with
your implementation over the baseline. The baseline can be found by running with your 
prefetcher disabled or unimplemented. You can change which prefetcher is being used (or disable it)
by modifying the configuration script, found under configs/tdt4260/prefetcher.py
Modify args.{l1/l2/l3}_hwp_type to change the prefetcher.

To disable this message on start, set welcome_message=False
''')

num_benchmarks = len(binaries)
count = 0
for x in range(0, num_benchmarks):
    os.chdir("spec2017")
    output_dir = f"prefetcher_out_{x}"
    if (os.path.exists(f"prefetcher_out_{x}")):
        shutil.rmtree(f"prefetcher_out_{x}")
    proc = subprocess.run([gem5_bin, "-v", "-r", f"--outdir={output_dir}", config,
                    "--iteration", str(x)])
    print(f"{proc.returncode} was returned for binary {binaries[x]}")
    if (proc.returncode == 0):
        count = count + 1
    os.chdir(cwd)


print(f"Runs completed, {count} benchmarks completed successfully, collating results")
result_dst = "results/results_summary.txt"

ipcs = []
others = []
for x in range(num_benchmarks):
    data = []
    with open(f"spec2017/prefetcher_out_{x}/stats.txt", "r") as stats:
        lines = stats.read().split("\n")
        for line in lines:
            line = line.split()
            if (len(line) == 0):
                continue
            if "system.switch_cpus.commitStats0.ipc" in line[0]:
                ipcs.append(line[1])
            if "prefetcher" in line[0]:# or "cache" in line[0]:
                data.append((line[0], line[1]))
    others.append(data)

b_names = ["gcc_s", "exchange2_s", "mcf_s", "deepsjeng_s", "x264_s"]

with open(result_dst, "w") as out:
    out.write("Summary of benchmarking follows...\n")
    out.write("-----IPC-----\n")
    for x in range(len(ipcs)):
        out.write(f"{b_names[x]}: {ipcs[x]}\n")

    out.write("\n\n\n")

    out.write("Detailed stats follow...\n")
    for x in range(len(others)):
        out.write(f"{b_names[x]} stats:\n")
        for y in others[x]:
            out.write(f"{y[0]}: {y[1]}\n")
        out.write("\n\n\n")

print(f"Results collated to {result_dst}")
