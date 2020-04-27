#!/bin/python3

import matplotlib.pyplot as plt
import sys
import csv
import os

try:
    filename = sys.argv[1]

    with open(filename) as f:
        reader = csv.reader(f)
        next(reader)  #skip name
        data = [int(row[0]) for row in reader]
 
    base = os.path.basename(filename)
    name = os.path.splitext(base)[0]

    plt.figure()
    plt.title(name)
    plt.xlabel("Index")
    plt.ylabel("List size")

    plt.bar(range(len(data)), data)
    plt.tight_layout()
    plt.savefig(name + ".pdf")
except Exception as e:
    print(e)
