'''
Python code to plot .txt files from RAPULSAN2.C
Author: Marko Radolovic
Date: 20241006
Version: 1.0.
'''

import numpy as np
import matplotlib.pyplot as plt

# Define file path
file_path = "rag_obs_20251003_02_FFT_RAFFT22.txt"

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

# Read the file line by line
with open(file_path, "r") as file:
    for line in file:
        # Split each line by tab or any whitespace
        parts = line.strip().split()
        if len(parts) == 2:
            # Convert each part to appropriate type
            col1.append(int(parts[0]))
            col2.append(float(parts[1]))

#average of power
#average = np.average(col2)
#col2 = col2 - average

# Parameters
Fc = 422e6      # Center frequency (Hz)
Fs = 2.4e6      # Sample rate (Hz)
N  = 1024       # FFT size (number of bins)

# Compute frequency for each FFT bin (0 to N-1)
freqs = [Fc - Fs/2 + (Fs/N)*k for k in range(N)]
freqs_MHz = [f / 1e6 for f in freqs]

plt.figure()
#plt.plot(col1, col2, color='red')
plt.plot(freqs_MHz, col2, color='red')
#plt.plot(col1, col2, linestyle = '', marker='.', color='blue',label='Power')
#plt.legend(loc='best')
plt.xlabel("Bin number [/]")
plt.ylabel(r"Power or PSD [W or W/$\sqrt{Hz}$, uncalibrated]")
#plt.title(f"Power vs. Bin number\n{observation_date} - {observation_number}
#plt.title("SNR vs. Bin number - B0329+54 - Visnjan, Tican\nFirst light on 20250310\n30 min sample from 2h obs time")
plt.title("FFT - B0329+54 - Visnjan, Tican\nFirst light on 20250310 - 2h obs time")
plt.tight_layout()
#plt.savefig(f"pulsar_plot_{observation_date}_{observation_number}.png")
plt.savefig("ragobscutfold_FFT_full.png", dpi=300)
plt.show()


