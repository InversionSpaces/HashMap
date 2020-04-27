#!/bin/python3

import matplotlib.pyplot as plt
import sys
import csv

try:
    filename = sys.argv[1]

    with open(filename) as f:
        reader = csv.reader(f)
        next(reader)  #skip name
        data = [int(row[0]) for row in reader]
    
    plt.xlabel("Index")
    plt.ylabel("List size");

    plt.bar(range(len(data)), data)
    plt.savefig(sys.argv[1] + ".pdf")
except Exception as e:
    print(e)
