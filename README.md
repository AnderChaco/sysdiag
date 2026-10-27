SysDiag

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Lightweight, modular, visually‑consistent system diagnostics tool for Linux.

SysDiag provides a clean, unified way to inspect system health using a modular architecture.
Each diagnostic module returns structured text, and the main runner handles all presentation using a consistent visual theme.

 - Features

    Modular diagnostic system

    Unified visual theme (colors, symbols, formatting)

    Memory diagnostics (/proc/meminfo)

    CPU diagnostics (/proc/cpuinfo, /proc/loadavg)

    Disk usage diagnostics (statvfs)

    Network diagnostics (/sys/class/net, /proc/net/dev)

    Clean and readable output

    Easy to extend with new modules

 - Project Structure

```text
├── include/
│   ├── checks.h
│   ├── colors.h
│   ├── style.h
│   └── version.h.in
├── src/
│   ├── main.c
│   ├── checks/
│   │   ├── checks.c
│   │   ├── cpu.c
│   │   ├── disk.c
│   │   ├── memory.c
│   │   └── network.c
│   └── utils/
│       └── style.c
└── docs/
    └── sysdiag_design.md
```

 - Architecture

SysDiag uses the architecture conventions:

    Each module implements:

    check_status check_<module>(char *msg, size_t msg_size);

    Modules do not print.

    They fill msg with multiple lines.

    run_all_checks():

        prints banner

        calls each module

        prints status label

        prints each line using print_key_val()

This keeps logic and presentation fully separated.

- Visual Theme

SysDiag uses ANSI colors and symbols:

    ✓ OK (green)

    ⚠ WARN (yellow)

    ✗ FAIL (red)

    Cyan titles

    Bright gray keys

    White values

Example:

```text
✓ OK CPU Status
  Model:          Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz
  Cores:          6
  Frequency:      2.60 GHz
  Load (1m):      0.42
```

-  Modules

Memory

Reads /proc/meminfo and reports total, available memory and usage status.

CPU

Reads /proc/cpuinfo and /proc/loadavg to report model, cores, frequency and load.

Disk

Uses statvfs() to report total, used, available space and usage percentage.

Network

Reads /sys/class/net, /proc/net/dev, and /proc/net/fib_trie to report interface, IP, speed, RX/TX bytes.

 - Installation

Build:

```bash
mkdir build
cd build
cmake ..
make
```

Run:

```bash
./sysdiag
```

 - Example Output

```text
SysDiag — System Diagnostic Tool
--------------------------------------

✓ OK Available Memory
  Total:          7.66 GiB | 7847.23 MiB | 8035564 KiB
  Available:      3.23 GiB | 3306.20 MiB | 3385548 KiB

✓ OK CPU Status
  Model:          Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz
  Cores:          6
  Frequency:      2.60 GHz
  Load (1m):      0.42

✓ OK Disk Usage
  Total:          512.00 GiB
  Used:           120.00 GiB
  Available:      392.00 GiB
  Usage:          23.4%

✓ OK Network Status
  Interface:      enp3s0
  IP Address:     192.168.0.15
  Speed:          1000 Mb/s
  RX Bytes:       123456789
  TX Bytes:       987654321
```

- Roadmap

    Add service status module

    Add temperature module (lm-sensors)

    Add battery module

    Add JSON output mode

    Add export-to-file mode

    Add plugin system for external modules

Explore next steps:

    service module

    temperature module

    JSON output

    plugin system

 -  License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

