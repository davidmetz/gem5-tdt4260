import argparse
import sys
import os

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.objects.TDT4260Cache import BaseCacheHierarchy
from m5.params import NULL
from m5.util import addToPath, fatal, warn

addToPath('../')

from common import Options, Simulation, CacheConfig,\
                   CpuConfig, ObjectList, MemConfig
from common.FileSystemConfig import config_filesystem
from common.Caches import *

parser = argparse.ArgumentParser()
Options.addCommonOptions(parser)
Options.addSEOptions(parser)

parser.add_argument("--l1CacheSize", type=int, default=1024)
parser.add_argument("--l1CacheAssociativity", type=int, default=1)

args = parser.parse_args()
args.cpu_type = "AtomicSimpleCPU"
args.mem_type = "SimpleMemory"

num_cpus = 1

(cpu, mem, futureclass) = Simulation.setCPUClass(args)
cpu.numThreads = 1

system = System(cpu = [cpu(cpu_id=0)],
                mem_mode = mem,
                mem_ranges = [AddrRange("8GB")],
                cache_line_size = "64")

system.voltage_domain = VoltageDomain(voltage = "1V")

system.clk_domain = SrcClockDomain(clock = "3GHz",
                                   voltage_domain = system.voltage_domain)

system.cpu_voltage_domain = VoltageDomain()

system.cpu_clk_domain = SrcClockDomain(clock = "3GHz",
                                voltage_domain = system.cpu_voltage_domain)


system.cpu[0].clk_domain = system.cpu_clk_domain

process = Process(pid = 100)
wrkld = args.cmd
process.executable = wrkld
process.cwd = os.getcwd()
process.gid = os.getgid()
process.cmd = [wrkld, *args.options.split()]
print(process.cmd)

mp0_path = process.executable

MemClass = Simulation.setMemClass(args)
system.membus = SystemXBar()
system.system_port = system.membus.cpu_side_ports
CacheConfig.config_cache(args, system)
MemConfig.config_mem(args, system)
config_filesystem(system, args)

system.cpu[0].workload = process
system.cpu[0].createThreads()

system.cpu[0].cache_hierarchy = BaseCacheHierarchy(
    blockSize = 64,
    l1CacheSize = args.l1CacheSize,
    l1CacheAssociativity = args.l1CacheAssociativity
)

system.workload = SEWorkload.init_compatible(mp0_path)

root = Root(full_system = False, system = system)
Simulation.run(args, root, system, futureclass)