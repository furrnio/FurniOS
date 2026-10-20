# FurniOS

FurniOS is a small, experimental operating system for Raspberry Pi (2b). It documents my learning process in bare-metal programming. It is primarly used in my artistic installations. 
An operating system needs some basic functionalities so that processor and board can start there work. Without a boot code, memory layout or drivers, the board stays pretty silent. But at the point, where the processor is alive and the periphery is initialized, we are hacking in and start experimenting with the untypical approach of an artistic operating system.

## Requirements
    1. Install arm tools
    2. Install qemu
    3. Set search paths for both
    4. Run `make`

### Some make options

More options are discribed in the make file.

| make options | explanation |
| --- | --- |
| make | Builds the kernel as kernel.elf and generates a dump. |
| make clean | Removes build files. |
| make qemu | Builds the kernel and runs it in QEMU |
| make qemu_debug | Builds the kernel and runs it in QEMU with debugging options |
| make gdb | Starts gdb and conntects to QEMU. |
| make install | Builds kernel.img and transfers it to the server. The board will load it on the next reset |
| make home | Copies the final image to $(TFTP_PATH) |

## Implementation details

As I'm implementing all periphery drivers from scratch (and with the help of tutorials) I have only some basic drivers implemented yet. Some are planned.

| Periphery | Status | Explanation |
| --- | --- | --- |
| UART poll | implemented | – | 
| UART interrupt-driven | planned | – |
| Timer | in progess | Timer interrupt implemented |
| SPI | planned | – |
| PCM | planned | – |


| Kernel | Status | Explanation | 
| --- | --- | --- |
| vector tables | implemented | – |
| exception handlers | implemented | – |
| context save | implemented | – |
| pre-emptive scheduler with fixed time slices | planned | – |

## License

FurniOS is licensed under the **GNU General Public License version 3 (GPLv3)** with an additional **AI training restriction clause** (Section 14). See the [LICENSE](LICENSE) file for the full legal text.
