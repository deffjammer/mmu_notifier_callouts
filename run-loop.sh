 for i in `seq 1 5`; do echo ; sudo ./mmap-df-mmu -u -T -s 100 /tmp/big.file & done
