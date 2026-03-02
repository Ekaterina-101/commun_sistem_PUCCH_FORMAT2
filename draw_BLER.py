import matplotlib.pyplot as plt
import re
import numpy as np

def read_bler_data(filename):
    data = {}
    current_k = None
    snr_values = []
    bler_values = []
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            k_match = re.match(r'k\s*=\s*(\d+)', line)
            if k_match:
                if current_k is not None:
                    data[current_k] = (np.array(snr_values), np.array(bler_values))
                current_k = int(k_match.group(1))
                snr_values = []
                bler_values = []
                continue
            
            bler_match = re.match(r'SNR\s*=\s*([-\d.e+]+)\s+BLER\s*=\s*([-\d.e+]+)', line, re.IGNORECASE)
            if bler_match and current_k is not None:
                snr = float(bler_match.group(1))
                bler = float(bler_match.group(2))
                snr_values.append(snr)
                bler_values.append(bler)
    
    if current_k is not None:
        data[current_k] = (np.array(snr_values), np.array(bler_values))
    
    return data

def plot_bler_curves(data, save_path=None):
    plt.figure(figsize=(10, 6))
    
    colors = plt.cm.viridis(np.linspace(0, 1, len(data)))
    
    for idx, (k, (snr, bler)) in enumerate(sorted(data.items())):
        plt.plot(snr, bler, marker='o', linewidth=2, markersize=6, 
                label=f'k = {k}', color=colors[idx])
    
    plt.xlabel('SNR (dB)', fontsize=12)
    plt.ylabel('BLER (Block Error Rate)', fontsize=12)
    plt.title('Зависимость BLER от SNR для различных значений k', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(title='Длина блока', fontsize=10)
    plt.yscale('log')
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"График сохранён в файл: {save_path}")
    
    plt.show()

if __name__ == "__main__":
    filename = "BLER_data.txt"
    
    try:
        data = read_bler_data(filename)
        if not data:
            print("Ошибка: данные не были загружены. Проверьте формат файла.")
        else:
            print(f"Загружены данные для k = {sorted(data.keys())}")
            plot_bler_curves(data, save_path="bler_vs_snr.png")
    except FileNotFoundError:
        print(f"Ошибка: файл '{filename}' не найден. Убедитесь, что файл находится в текущей директории.")
    except Exception as e:
        print(f"Произошла ошибка: {e}")