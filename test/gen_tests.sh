#!/bin/bash
docker run -t --rm -v ./test:/act4/config/cores/iss:ro -v ./work:/act4/work -e CONFIG_FILES=config/cores/iss/test_config.yaml ghcr.io/riscv/act4:latest make -j
# Then run with:
# python test/run_tests.py build/Debug/iss-sim work/iss/elfs/rv32i/ 