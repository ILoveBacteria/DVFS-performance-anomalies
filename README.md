# DVFS-performance-anomalies

This project is a simulation and study of performance measurement anomalies in software engineering, specifically focusing on the timing of a **merge sort** routine. Inspired by a study by Sivan Toledo and presented in MIT’s Performance Engineering course, the project demonstrates how modern hardware features like **Dynamic Frequency and Voltage Scaling (DVFS)** can create "roller coaster" patterns in execution time rather than a smooth algorithmic growth curve. By measuring the runtime of sorting increasingly large arrays using `CLOCK_MONOTONIC`, this simulation exposes how system noise and thermal throttling wreak havoc on deterministic performance data.

To obtain reliable and repeatable results, I have performed **"quiescing your system"** as part of this project.

### "Producing Wrong Data Without Doing Anything Obviously Wrong"
This paper, authored by **Mytkowicz et al.**, is a foundational reference in performance engineering. It demonstrates that performance measurements can be significantly biased by "seemingly irrelevant" factors that shift memory alignment. For example:
*   **Linker Order:** Changing the order of `.o` files in a linker command can have a larger impact on performance than moving from `-O2` to `-O3` optimization levels.
*   **Program Name Length:** Because an executable's name is stored in an environment variable on the call stack, simply lengthening a program's name can shift stack alignment and slow down data access by causing it to cross page boundaries.

### Quiescing a System
**Quiescing a system** refers to the process of making a computer "quiet" enough to obtain reliable and repeatable timing measurements by eliminating external "noise". Based on the sources, this involves several core concepts:

#### The Purpose: Reducing Variance
The primary goal of quiescing is to **reduce variability**. Following the quality control theories of **Genichi Taguchi**, it is essential to minimize the "spread" (variance) of data before attempting to improve a product. In software, high variance makes it impossible to determine if a code change actually improved performance or if the result was merely a byproduct of random noise. A properly quiesced system should produce essentially the same runtime for a deterministic program every time it is executed.

#### Common Sources of "Noise"
Modern systems contain numerous features that interfere with clean measurements:
*   **System Activity:** Background jobs (daemons), periodic cron jobs, and interrupts from network traffic or even mouse movements.
*   **Hardware Features:** **DVFS**, which adjusts clock speeds based on heat, and **Turbo Boost**, which increases speeds inconsistently depending on how many cores are active.
*   **Architectural Features:** **Hyperthreading**, which shares functional units between instruction streams, and the operating system’s tendency to use **Core 0** for system interrupt handlers.

#### How to Quiesce a System
To quiesce the system for these experiments, the following actions were recommended and implemented:
*   **Shut down external interference:** All other applications were closed, daemons and cron jobs were shut down, and the network was disconnected.
*   **Hands off:** No interaction with the mouse or keyboard occurred during measurements, as these generate frequent interrupts that disrupt timing.
*   **Disable dynamic hardware scaling:** Features like DVFS, Turbo Boost, and hyperthreading were disabled to ensure the processor maintained a constant frequency.
*   **Isolate the process:** To avoid Core 0's interrupt traffic, the program was "pinned" to specific cores using the `taskset` utility, preventing the operating system from moving threads between cores during execution.

#### Demonstrated Effectiveness
The sources demonstrate the effectiveness of this process through an experiment using a Cilk program. In a standard, "noisy" environment, the slowest runs were **almost 25% slower** than the fastest runs. After quiescing the system—specifically by turning off Turbo Boost, hyperthreading, and background daemons—the variance dropped significantly, with nearly all 100 runs producing the exact same value within a margin of **less than 1%**.

## Report

## System settings

| Property | Value |
|----------|-------|
| Benchmark | Merge Sort |
| CPU | Intel® Core™ i7-6700HQ |
| Architecture | x86_64 |
| Physical Cores | 4 |
| Logical CPUs | 8 |
| Threads per Core | 2 |
| Base Frequency | 2.60 GHz |
| Maximum Frequency | 3.50 GHz |
| L1d Cache | 128 KiB |
| L1i Cache | 128 KiB |
| L2 Cache | 1 MiB |
| L3 Cache | 6 MiB |
| Memory | 12 GiB RAM, 4 GiB Swap |
| Operating System | Ubuntu 25.10 |
| Kernel | Linux 6.17.0-14-generic |
| Compiler | GCC 15.2.0 |
| Compiler Flags | `-O2` |

## References

[1] MIT OpenCourseWare, 10. Measurement and Timing, (Sep. 23, 2019). Accessed: Jul. 13, 2026. [Online Video]. Available: https://www.youtube.com/watch?v=LvX3g45ynu8

[2] C. E. Leiserson, “MEASUREMENT AND TIMING”.
