# CH32V203C8_LibXR_Template

CH32V203C8 的 LibXR 模板工程 / LibXR template project for the CH32V203C8

## 1. 板子与平台 / Board and Platform

模板使用 WCH CH32V203C8（RISC-V，64 KB Flash，20 KB RAM），系统为 FreeRTOS，外设由 LibXR 的 `ch` 驱动提供。`User/main.c` 设置中断分组和 USB 时钟，创建运行 `app_main()` 的 FreeRTOS 任务并启动调度器，LibXR 应用代码位于 `User/app_main.cpp`。LibXR 是 `libxr/` 下的 Git 子模块，地址为 `https://github.com/xrobot-org/libxr.git`，所用版本由提交记录固定。

```text
User/app_main.cpp         LibXR 应用代码 app_main()
User/main.c               启动 FreeRTOS，创建运行 app_main() 的任务
Core/                     WCH 内核与系统文件、FreeRTOSConfig.h
Startup/ Peripheral/      启动文件、WCH 外设库
FreeRTOS/                 FreeRTOS 内核与 RISC-V 移植
Link.ld                   链接脚本（64 KB Flash，20 KB RAM）
wch-riscv.cfg             OpenOCD 配置（WCH-LinkE，SDI）
libxr/                    LibXR 子模块
```

The template uses the WCH CH32V203C8 (RISC-V, 64 KB Flash, 20 KB RAM) and runs FreeRTOS; the peripherals are provided by the LibXR `ch` driver. `User/main.c` sets the interrupt priority grouping and the USB clock, creates the FreeRTOS task that runs `app_main()` and starts the scheduler. The LibXR application code is in `User/app_main.cpp`. LibXR is the Git submodule `libxr/` at `https://github.com/xrobot-org/libxr.git`, and the commit record pins the version in use.

## 2. 配置一览 / Configurations

| 配置 | 用途 |
| --- | --- |
| `User/app_main.cpp` | 双 USB CDC 与 LED 闪烁：创建 `LibXR::CH32Timebase`，调用 `LibXR::PlatformInit(3, 1024)`，初始化 PB2 上的 LED 和 USART1（PA9 / PA10），同时启动 FSDEV 与 OTG FS 两个 USB 设备，各提供一个 CDC 串口（产品名 `CDC DEVFS` 和 `CDC OTGFS`）。`LibXR::STDIO` 绑定到 FSDEV 的 CDC，主循环中 LED 每 200 ms 翻转一次 |

| Configuration | Purpose |
| --- | --- |
| `User/app_main.cpp` | Dual USB CDC and LED blink: creates `LibXR::CH32Timebase`, calls `LibXR::PlatformInit(3, 1024)`, initializes the LED on PB2 and USART1 (PA9 / PA10), and starts the FSDEV and OTG FS USB devices, each providing one CDC serial port (product names `CDC DEVFS` and `CDC OTGFS`). `LibXR::STDIO` is bound to the FSDEV CDC, and the main loop toggles the LED every 200 ms |

## 3. 构建 / Build

构建使用 CMake 和 WCH RISC-V GCC 15.2 或更高版本，CMake 在版本低于 15.2 时报错。工具链为 `riscv32-wch-elf-gcc`，位于 `PATH` 中；位于其他位置时用 `-DCOMPILER_PREFIX=<前缀>` 指定。镜像 `ghcr.io/xrobot-org/docker-image-ch32-riscv:main` 提供该工具链、CMake 和 OpenOCD。

```bash
git clone --recursive https://github.com/xrobot-org/CH32V203C8_LibXR_Template.git
cd CH32V203C8_LibXR_Template
docker run --rm -v "$PWD:/work" -w /work ghcr.io/xrobot-org/docker-image-ch32-riscv:main \
  bash -c 'cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j"$(nproc)"'
```

产物为 `build/CH32V203C8.elf`、`build/CH32V203C8.hex` 和 `build/CH32V203C8.bin`。已克隆但未带子模块时，运行 `git submodule update --init` 获取 LibXR。

`.github/workflows/build.yml` 在上述镜像中递归检出子模块并构建。其中的 `libxr-master` 作业每天运行一次，把 `libxr/` 更新到 LibXR 的 `master` 后构建，用于提前发现兼容性变化，作业失败不影响工作流结果。

Building uses CMake and the WCH RISC-V GCC 15.2 or newer; CMake stops with an error for an older version. The toolchain is `riscv32-wch-elf-gcc` on `PATH`, or located with `-DCOMPILER_PREFIX=<prefix>`. The image `ghcr.io/xrobot-org/docker-image-ch32-riscv:main` provides the toolchain, CMake and OpenOCD.

The output is `build/CH32V203C8.elf`, `build/CH32V203C8.hex` and `build/CH32V203C8.bin`. For a clone made without submodules, `git submodule update --init` fetches LibXR.

`.github/workflows/build.yml` checks out the submodules recursively and builds in the image above. Its `libxr-master` job runs daily, updates `libxr/` to the LibXR `master` branch and builds, which reveals compatibility changes early; a failure of that job does not fail the workflow.

## 4. 烧录与运行 / Flash and Run

`wch-riscv.cfg` 是 OpenOCD 配置，使用 WCH-LinkE（`wlinke` 适配器，SDI 接口，6 MHz），需要 WCH 版 OpenOCD（镜像中已包含）。`openocd -f wch-riscv.cfg` 启动调试服务。VS Code 中 `.vscode/launch.json` 的 `Launch CH32V203` 通过 Cortex-Debug 使用该配置，下载并调试 `build/CH32V203C8.elf`，`gdbPath` 为 `riscv32-unknown-elf-gdb`。

运行后 PB2 上的 LED 每 200 ms 翻转一次，两个 USB 口各枚举出一个 CDC 串口，标准输入输出使用 FSDEV 的串口。

`wch-riscv.cfg` is an OpenOCD configuration for the WCH-LinkE (`wlinke` adapter, SDI interface, 6 MHz) and needs the WCH build of OpenOCD, which the image includes. `openocd -f wch-riscv.cfg` starts the debug server. In VS Code, `Launch CH32V203` in `.vscode/launch.json` uses this configuration through Cortex-Debug to download and debug `build/CH32V203C8.elf`, with `riscv32-unknown-elf-gdb` as `gdbPath`.

At run time the LED on PB2 toggles every 200 ms, each of the two USB ports enumerates one CDC serial port, and standard input and output use the FSDEV serial port.

本仓库以 Apache-2.0 发布，见 [LICENSE](LICENSE)；`User/main.c`、`Core/`、`Startup/`、`Peripheral/` 中的 WCH 代码和 `FreeRTOS/` 保留各自文件头中的版权与许可声明。

This repository is released under Apache-2.0, see [LICENSE](LICENSE); the WCH code in `User/main.c`, `Core/`, `Startup/` and `Peripheral/` and the code in `FreeRTOS/` keep the copyright and license notices in their file headers.
