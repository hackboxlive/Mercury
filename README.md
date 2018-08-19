# Mercury

Async block replication

PHASE #1:	Trying to run MBR partition table and find the block offset and save those details

	1. Script 'mbr.c' reads the primary partition table and then the logical partion table(or I hope so)

	2. To run mbr.c, compile it with gcc/g++, and then do `sudo ./a.out`. It should print your primary MBR table

	3. I am not responsible for any breakage to your system.

