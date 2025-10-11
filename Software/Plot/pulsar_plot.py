'''
Python code to plot .txt files from RAPULSAN2.C
Author: Marko Radolovic
Date: 20241006
Version: 1.0.
'''

import numpy as np
import matplotlib.pyplot as plt

# Define file path
file_path = "output_ML_test4.txt"

# split_file_path = file_path.split('_')
#
# observation_date = split_file_path[4]
# split_number = str(split_file_path[5])
# split_number = split_number.split('.')
# observation_number = split_number[0]


# Initialize empty lists for each column
#bin number
col1 = []
#power?
col2 = []
#??
col3 = []

# Read the file line by line
with open(file_path, "r") as file:
    for line in file:
        # Split each line by tab or any whitespace
        parts = line.strip().split()
        if len(parts) == 3:
            # Convert each part to appropriate type
            col1.append(int(parts[0]))
            col2.append(float(parts[1]))
            col3.append(int(parts[2]))

#average of power
average = np.average(col2)
col2 = col2 - average

plt.figure()
#plt.plot(col1, col2)
plt.plot(col1, col2, linestyle = '', marker='.', color='blue',label='Power')
plt.legend(loc='best')
plt.xlabel("Bin number [/]")
plt.ylabel("Power [a.u.]")
#plt.title(f"Power vs. Bin number\n{observation_date} - {observation_number}")
plt.tight_layout()
#plt.savefig(f"pulsar_plot_{observation_date}_{observation_number}.png")
plt.savefig("test4_bez_dedisp_norm_0302.png")
#plt.show()


