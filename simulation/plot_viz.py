import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

def plot_entropy_decay():
    df = pd.read_csv('data/entropy_decay.csv')
    plt.figure(figsize=(10, 6))
    plt.plot(df['ids'], df['standard_distributed'], marker='o', color='red', label='Standard Distributed (Snowflake)')
    plt.plot(df['ids'], df['chronoid'], marker='s', color='blue', label='ChronoID (Active Divergence)')

    plt.title('Empirical Graph A: Collision Resistance vs Scale')
    plt.xlabel('Concurrent Node Count (Simulated)')
    plt.ylabel('Observed Collision Probability (%)')
    plt.legend()
    plt.grid(True, which="both", ls="-", alpha=0.3)

    # Format x-axis
    plt.xticks(df['ids'], [f'{int(x/1000)}k' for x in df['ids']])

    plt.savefig('plots/entropy_decay.png')
    print("✅ Saved plots/entropy_decay.png")

def plot_throughput_cliff():
    df = pd.read_csv('data/throughput_cliff.csv')
    plt.figure(figsize=(10, 6))
    plt.plot(df['rows'], df['uuid'], marker='o', color='red', label='UUIDv7 (Random B-Tree)')
    plt.plot(df['rows'], df['chronoid'], marker='s', color='blue', label='ChronoID (Sequential B-Tree)')

    plt.title('Empirical Graph B: Ingestion Velocity & B-Tree Fragmentation')
    plt.xlabel('Database Scale (Cumulative Record Count)')
    plt.ylabel('Tail Latency (ms per 200k Insertions)')
    plt.ylim(bottom=0)
    plt.legend()
    plt.grid(True, which="both", ls="-", alpha=0.3)

    # Format x-axis
    plt.xticks(df['rows'], [f'{x/1_000_000:.1f}M' for x in df['rows']])

    plt.savefig('plots/throughput_cliff.png')
    print("✅ Saved plots/throughput_cliff.png")

def plot_storage_footprint():
    df = pd.read_csv('data/storage_footprint.csv')
    plt.figure(figsize=(10, 6))
    bars = plt.bar(df['type'], df['size_gb'], color=['red', 'orange', 'blue'])

    plt.title('Empirical Graph C: Physical Index Footprint (100M Scale)')
    plt.ylabel('Total Storage Volume (GB)')
    plt.grid(axis='y', ls="-", alpha=0.3)

    # Add data labels
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 0.1, f'{yval} GB', ha='center', va='bottom', fontweight='bold')

    plt.savefig('plots/storage_footprint.png')
    print("✅ Saved plots/storage_footprint.png")

def plot_storage_tenant():
    df = pd.read_csv('data/storage_tenant.csv')
    plt.figure(figsize=(10, 6))
    bars = plt.bar(df['type'], df['size_mb'], color=['red', 'orange', 'green'])

    plt.title('Empirical Graph D: Multi-Tenant Density (Full 16.7M IDs in Mode B)')
    plt.ylabel('Index Memory Footprint (MB)')
    plt.grid(axis='y', ls="-", alpha=0.3)

    # Add data labels
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 0.1, f'{yval} MB', ha='center', va='bottom', fontweight='bold')

    plt.savefig('plots/storage_tenant.png')
    print("✅ Saved plots/storage_tenant.png")

def plot_routing_efficiency():
    df = pd.read_csv('data/routing_efficiency.csv')
    plt.figure(figsize=(10, 6))

    scales = df['scale'].unique()
    types = df['type'].unique()

    x = np.arange(len(scales))
    width = 0.35

    map_times = df[df['type'] == 'Map Lookup']['time_ms'].values
    shift_times = df[df['type'] == 'Bit-Shift']['time_ms'].values

    plt.bar(x - width/2, map_times, width, label='Map Lookup', color='grey')
    plt.bar(x + width/2, shift_times, width, label='Bit-Shift', color='blue')

    plt.title('Empirical Graph E: Shard Routing Execution Cost')
    plt.xlabel('Routing Request Volume (Deterministic Suffix)')
    plt.ylabel('CPU Processing Time (ms) - Log Scale')
    plt.xticks(x, scales)
    plt.yscale('log')
    plt.legend()
    plt.grid(axis='y', ls="-", alpha=0.3)

    # Add data labels
    for i, scale in enumerate(scales):
        plt.text(i - width/2, map_times[i] * 1.05, f'{map_times[i]:.1f}ms', ha='center', va='bottom', fontsize=8)
        plt.text(i + width/2, shift_times[i] * 1.05, f'{shift_times[i]:.3f}ms', ha='center', va='bottom', fontsize=8, color='blue', fontweight='bold')

    plt.savefig('plots/routing_efficiency.png')
    print("✅ Saved plots/routing_efficiency.png")

if __name__ == "__main__":
    if not os.path.exists('plots'):
        os.makedirs('plots')
    plot_entropy_decay()
    plot_throughput_cliff()
    plot_storage_footprint()
    plot_storage_tenant()
    plot_routing_efficiency()
