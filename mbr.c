#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct partition
{
	unsigned char boot_flag;	/* 0 = Not active, 0x80 = Active */
	unsigned char chs_begin[3];
	unsigned char sys_type;	 /* For example : 82 --> Linux swap, 83 --> Linux native partition, ... */
	unsigned char chs_end[3];
	unsigned char start_sector[4];
	unsigned char nr_sector[4];
};

void string_in_hex(void *in_string, int in_string_size)
{
	int i;
	int k = 0;

	for (i = 0; i < in_string_size; i++)
	{
		printf("%02x ", ((char *)in_string)[i]& 0xFF);
		k = k + 1;
		if (k == 16)
		{
			printf("\n");
			k = 0;
		}
	}
	printf("\n");
}

void dump_partition(struct partition *part, int partition_number)
{
	printf("Partition /dev/sda%d\n", partition_number + 1);
	printf("boot_flag = %02X\n", part->boot_flag);
	printf("chs_begin = ");
	string_in_hex(part->chs_begin, 3);
	printf("sys_type = %02X\n", part->sys_type);
	printf("chs_end = ");
	string_in_hex(part->chs_end, 3);
	printf("start_sector = ");
	string_in_hex(part->start_sector, 4);
	printf("nr_sector = ");
	string_in_hex(part->nr_sector, 4);
}

int main(int argc, char **argv)
{
	int /*gc = 0,*/ i = 1, nr = 0, pos = -1/*, nw = 0*/;
	int fd = 0;
	char buf[512] ;
	struct partition *sp;
	int ret = 0;

	printf("Ok ");

	if ((fd = open("/dev/sda", O_RDONLY | O_SYNC)) == -1)
	{
		perror("Opening");
		exit(1);
	}
	printf("fd is %d\n", fd);

	pos = lseek (fd, 0, SEEK_CUR);

	printf("Position of pointer is :%d\n", pos);
	if ((nr = read(fd, buf, sizeof(buf))) == -1)
	{
		perror("Read");
		exit(1);
	}

	ret = close(fd);
	if (ret == -1)
	{
		perror("close");
		exit(1);
	}

	/* Dump the MBR buffer, you can compare it on your system with the output of the command:
	 * hexdump -n 512 -C /dev/sda
	 */
	string_in_hex(buf, 512);

	printf("Size of buf = %d - and number of bytes read are %d\n", sizeof(buf), nr);
	/*if ((nw = write(1, buf, 64)) == -1)
	{
		printf("Write: Error");
		exit(1);
	}

	printf("\n\n%d bytes are just been written on stdout\nthis can also be printed\n", nw);
	*/
	//printf("\n\t\t*************Partition Table****************\n\n");
	printf("\n\t\t*************THE 4 MAIN PARTITIONS****************\n\n");

	/* Dump main partitions (4 partitions) */
	/* Note : the 4 partitions you are trying to dump are not necessarily existing! */
	for (i = 0 ; i < 4 ; i++)
	{
		sp = (struct partition *)(buf + 446 + (16 * i));
		//putchar(sp->boot_flag);
		dump_partition(sp, i);
	}

	return 0;
}
