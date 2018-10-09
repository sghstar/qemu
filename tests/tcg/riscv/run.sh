#!/bin/bash



rm -rf dsp
rm -rf sid.log
rm -rf qemu.log

riscv32-elf-g++ -mext-dsp dsp.cpp -o dsp -Wl,--defsym=_stack=0 || echo "compile error"
/local/nick/build-system-3/verification/nds32le-elf-mculib-v5/supertest/O0/SID-rv32imcxv5-fast/riscv-sim-wrapper dsp > sid.log || echo "sid error"
/home/nick/QEMU_V5_USER/build/riscv32-linux-user/qemu-riscv32 -cpu andes-n25 dsp > qemu.log || echo "qemu error"
diff sid.log qemu.log && echo "PASS" || echo "FAIL"
