#!/bin/bash
docker run -t --rm -v ./test:/act4/config/cores/cuanguaro:ro -v ./work:/act4/work -e CONFIG_FILES=config/cores/cuanguaro/test_config.yaml ghcr.io/riscv/act4:latest make -j
# Then run with:
# python test/run_tests.py <executable> work/cuanguaro/elfs/rv32i/ 