import matplotlib.pyplot as plt
import re

def parse_data(log_lines):
    """
    Parses the log lines to extract timestamp, ADC value, and buffer index.

    Args:
        log_lines: A list of strings, each representing a line from the log file.

    Returns:
        A tuple containing three lists: timestamps, adc_values, and buffer_indices.
    """
    timestamps = []
    adc_values = []
    buffer_indices = []

    for line in log_lines:
        match = re.match(r'\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)', line)
        if match:
            try:
                timestamp = int(match.group(1))
                adc_value = int(match.group(2))
                buffer_index = int(match.group(3))
                timestamps.append(timestamp)
                adc_values.append(adc_value)
                buffer_indices.append(buffer_index)
            except ValueError:
                print(f"Skipping invalid line: {line}")

    return timestamps, adc_values, buffer_indices


def plot_data(timestamps, adc_values, buffer_indices):
    """
    Plots the timestamp vs ADC value and timestamp vs buffer index.

    Args:
        timestamps: A list of timestamps.
        adc_values: A list of ADC values.
        buffer_indices: A list of buffer indices.
    """

    plt.figure(figsize=(12, 6))

    # Plot ADC value vs Timestamp
    plt.subplot(3, 1, 1)  # Create a subplot grid (1 row, 2 columns)
    plt.plot(adc_values, marker='o')
    plt.ylabel("adc_values")
    plt.grid(True)

    # Plot Buffer Index vs Timestamp
    plt.subplot(3, 1, 2)  # Second subplot
    plt.plot(buffer_indices, marker='x')
    plt.ylabel("Buffer Index")
    plt.title("Buffer Index vs. Timestamp")
    plt.grid(True)

    plt.subplot(3, 1, 3)  # Second subplot
    plt.plot(timestamps, marker='x')
    plt.ylabel("timestamps")
    plt.grid(True)

    plt.tight_layout()  # Adjust subplot parameters for a tight layout
    plt.show()


# Example usage with the provided log data:
log_data = """
Total de amostras lidas: 5
      467178 |      1866 |            0
      467178 |      1776 |            1
      467178 |      1671 |            2
Total de amostras lidas: 3
      467203 |      1653 |            3
      467203 |      1600 |            4
      467203 |      1513 |            0
      467203 |      1534 |            1
      467203 |      1399 |            2
      467203 |      1339 |            3
Total de amostras lidas: 6
AVISO: Buffer overflow detectado!
      467228 |      1307 |            4
      467228 |      1249 |            0
      467228 |      1222 |            1
      467228 |      1200 |            2
      467228 |      1259 |            3
Total de amostras lidas: 5
      467253 |      1346 |            4
      467253 |      1371 |            0
      467253 |      1389 |            1
Total de amostras lidas: 3
      467278 |      1431 |            2
      467278 |      1462 |            3
      467278 |      1555 |            4
"""

# Split the log data into lines
log_lines = log_data.strip().split('\n')

# Parse the data
timestamps, adc_values, buffer_indices = parse_data(log_lines)

# Plot the data
plot_data(timestamps, adc_values, buffer_indices)