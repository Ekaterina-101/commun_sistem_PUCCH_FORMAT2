import matplotlib.pyplot as plt
import numpy as np
import re

def read_bler_data(filename):
    data = {}
    current_k = None
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            k_match = re.match(r'k\s*=\s*(\d+)', line)
            if k_match:
                current_k = int(k_match.group(1))
                data[current_k] = {'esn0': [], 'bler': []}
                continue
            
            if current_k is not None and 'Es/N0' in line and 'BLER' in line:
                esn0_match = re.search(r'Es/N0\s*=\s*([-+]?\d*\.?\d+(?:e[+-]?\d+)?)', line, re.IGNORECASE)
                bler_match = re.search(r'BLER\s*=\s*([-+]?\d*\.?\d+(?:e[+-]?\d+)?)', line, re.IGNORECASE)
                
                if esn0_match and bler_match:
                    esn0 = float(esn0_match.group(1))
                    bler = float(bler_match.group(1))
                    data[current_k]['esn0'].append(esn0)
                    data[current_k]['bler'].append(bler)
    
    for k in data:
        data[k]['esn0'] = np.array(data[k]['esn0'])
        bler_arr = np.array(data[k]['bler'])
        bler_arr[bler_arr < 10**(-4)] = np.nan
        data[k]['bler'] = bler_arr
    
    return data

def plot_bler_semilog(data, save_path='data/bler_plot.png'):
    plt.figure(figsize=(11, 7))
    
    colors = plt.cm.viridis(np.linspace(0.2, 0.9, len(data)))
    
    for idx, (k, values) in enumerate(sorted(data.items())):
        plt.semilogy(
            values['esn0'], 
            values['bler'], 
            'o-', 
            label=f'k = {k}', 
            linewidth=2.5, 
            markersize=6,
            color=colors[idx],
            alpha=0.85
        )
    
    plt.grid(True, which='both', ls='--', alpha=0.7, linewidth=0.8)
    plt.minorticks_on()
    plt.grid(which='minor', alpha=0.3, ls=':')
    
    plt.xlabel('E_s/N_0 (dB)', fontsize=13, fontweight='bold')
    plt.ylabel('BLER (Block Error Rate)', fontsize=13, fontweight='bold')
    plt.title('BLER(Es/N0) для разных длин информационных блоков k', 
              fontsize=15, fontweight='bold', pad=15)
    
    plt.legend(loc='upper right', fontsize=11, framealpha=0.95)
    plt.ylim(1e-4, 1.1)
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    filename = 'data/BLER_data.txt'
    
    try:
        data = read_bler_data(filename)
        plot_bler_semilog(data)
        
    except FileNotFoundError:
        print(f"Error: file '{filename}' didn't find")
    except Exception as e:
        print(f"Error: {type(e).__name__}: {e}")