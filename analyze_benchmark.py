import csv
from statistics import median
import matplotlib.pyplot as plt 

with open("config.h") as f:
    definitions = [line.split() for line in f if line.startswith("#define DEFAULT_")]
config = {name: int(value) for _, name, value in definitions}
N, steps, mode = (config[k] for k in ("DEFAULT_N", "DEFAULT_STEPS", "DEFAULT_MODE"))

with open(f"reports/benchmark_N{N}_steps{steps}_mode{mode}.csv",newline = "") as f:
    rows = list(csv.DictReader(f))

times = {}
for row in rows:
    p = int(row["processes"])
    times.setdefault(p,[]).append(float(row["time_s"]))

medians = {p: median(t) for p,t in times.items()}
t1 = medians[1]
for p in sorted(medians):
    print(f"P = {p} | T = {medians[p]:.6f} | SpeedUp = {t1/medians[p]:.2f} | Efficiency = {(t1 / (medians[p] * p)):.1%}")

ps = sorted(medians)
speedups = [t1 / medians[p] for p in ps]
plt.plot(ps, speedups, 'o-', label = 'Measured')
plt.plot(ps, ps, "--", label = "Ideal")
plt.xlabel("Number of processes")
plt.ylabel("Speedup")
plt.legend()
plt.savefig(f"reports/speedup_N{N}_steps{steps}_mode{mode}.png", dpi = 150)

plt.figure()
efficiencies = [s / p for s,p in zip(speedups, ps)] 
plt.plot(ps, efficiencies, 'o-', label = "Measured")
plt.axhline(1, linestyle = '--', label = "Ideal")
plt.xlabel("Number of processes")
plt.ylabel("Efficiency")
plt.legend()
plt.savefig(f"reports/efficiency_N{N}_steps{steps}_mode{mode}.png", dpi = 150)