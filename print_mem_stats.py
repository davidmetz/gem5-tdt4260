import os
import re

def parse_stats():
    with open("m5out/stats.txt") as f:
        lines = f.readlines()
    stats = []
    for line in lines:
        if line == "" or line == "\n":
            continue
        elif line.startswith("---------- Begin "):
            snapshot = {}
        elif line.startswith("---------- End "):
            stats.append(snapshot)
        elif match := re.match(r"(\S+)\s+([0-9.e]+).*", line):
            assert match.group(1) not in snapshot
            snapshot[match.group(1)] = float(match.group(2))
        else:
            print("unparsed stat line:", "'"+line+"'")
            assert False
    return stats

stats = parse_stats()

def hits(stat):
    return int(stat['system.cpu.cache_hierarchy.L1_Cache.reqsServiced'])
def misses(stat):
    return int(stat['system.cpu.cache_hierarchy.L1_IC.reqsReceived'])
def loads(stat):
    return int(stat['system.cpu.commitStats0.numLoadInsts'])
def stores(stat):
    return int(stat['system.cpu.commitStats0.numStoreInsts'])

for stat in stats:
    print(f"Loads: {loads(stat)} Stores: {stores(stat)} Hits: {hits(stat)} Misses: {misses(stat)}")