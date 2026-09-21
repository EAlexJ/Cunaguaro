## About The Project

Cunaguaro is a C++ 23 RISC-V Instruction Set Simulator (ISS). It is kept lightweight to test and evaluate performance and latency impacts of changes to the ISS.

### Built With

The project is built using cmake, with appropriate presets available.
If a RISC-V toolchain is available, basic firmware can built.
If docker is available, RISC-V Compliance and [riscv-tests](https://github.com/riscv-software-src/riscv-tests) benchmarks can be generated.

## Getting Started

Build the Simulator:
```
cmake --preset Release && cmake --build --preset Release --parallel
```

The same applies to the 'hello_world' firmware.

In order to generate the riscv-tests (which include a dhrystone that runs for a few seconds), follow the instructions laid out int 'riscv-tests/generate.txt'.

In order to generate the self-testing RISC-V [architectural compliance tests](https://github.com/riscv/riscv-arch-test), take a look at 'tests/generate.sh'.

### Prerequisites

- cmake
- docker (if available)
- riscv64-unknown-elf-* toolchain (if available)
  
## Usage

The ISS only implements basic argument parsing:
```
./build/Release/cunaguaro-sim <ELF-file>
```

## License

See `LICENSE` for more information.

## Contact

- [Github](https://github.com/EAlexJ)
- [E-Mail](mailto:jentzschalexander@gmail.com)
