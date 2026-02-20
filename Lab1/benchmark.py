#!/usr/bin/env python3
import matplotlib.pyplot as plt


n = [1000, 10000, 100000, 500000]
time_sec = [0.008, 0.014, 0.108, 0.582]  # реальное время в секундах
memory_kb = [7424, 9284, 28084, 111544]   # память в КБ

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

ax1.plot(n, time_sec, 'bo-', linewidth=2, markersize=8)
ax1.set_xlabel('Количество записей (N)')
ax1.set_ylabel('Время (сек)')
ax1.set_title('Зависимость времени от N')
ax1.grid(True, alpha=0.3)
ax1.set_xscale('log')
ax1.set_yscale('log')

linear = [time_sec[0] * (x / n[0]) for x in n]
ax1.plot(n, linear, 'r--', alpha=0.5, label='O(n)')
ax1.legend()

ax2.plot(n, memory_kb, 'go-', linewidth=2, markersize=8)
ax2.set_xlabel('Количество записей (N)')
ax2.set_ylabel('Память (КБ)')
ax2.set_title('Зависимость памяти от N')
ax2.grid(True, alpha=0.3)
ax2.set_xscale('log')
ax2.set_yscale('log')

plt.tight_layout()
plt.savefig('benchmark.png', dpi=150)
print("График сохранён в benchmark.png")

print("\n" + "="*50)
print("РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ")
print("="*50)
print(f"{'N записей':<12} {'Время (сек)':<15} {'Память (КБ)':<15}")
print("-"*50)
for i in range(len(n)):
    print(f"{n[i]:<12} {time_sec[i]:<15.3f} {memory_kb[i]:<15}")
print("="*50)

print("\nПроверка линейной сложности:")
for i in range(1, len(n)):
    ratio_n = n[i] / n[0]
    ratio_time = time_sec[i] / time_sec[0]
    print(f"N×{ratio_n:.0f} → время×{ratio_time:.2f} (ожидалось ×{ratio_n:.0f})")
