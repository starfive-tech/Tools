#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>
#include <errno.h>
#include <limits.h>

#define NOSIZE ((size_t)-1)

extern uint32_t crc32(uint32_t iv, uint32_t sv, const void *data, size_t n);
extern uint32_t crc32_final(uint32_t iv);

struct __attribute__((__packed__)) ubootsplhdr {
	uint8_t  res1[  2];
	uint8_t  zro1[  4];
	uint8_t  res2[  4];
	uint8_t  zro2[634];
	uint32_t vers;
	uint32_t fsiz;
	uint8_t  res3[  4];
	uint32_t crcs;
	uint8_t  zro3[364];
};

static struct ubootsplhdr ubsplhdr = {
	.res1[0] = 0x40, .res1[1] = 0x02,
	/* res2 seems to be "# Offset of backup SBL from Flash info start"
	 * from SBL_BAK_OFFSET @ input_files/input_sbl_normal.cfg */
	.res2[0] = 0x20, .res2[1] = 0x00, .res2[2] = 0x00, .res2[3] = 0x00,
	.res3[0] = 0x00, .res3[1] = 0x04, .res3[2] = 0x00, .res3[3] = 0x00,
};

static char ubootspl[131072-0x400];
static char outpath[PATH_MAX];

static void xerror(int errnoval, const char *s)
{
	if (errnoval) perror(s);
	else fprintf(stderr, "%s\n", s);
	exit(2);
}

static void usage(void)
{
	printf("usage: jh7110_uboot_spl u-boot-spl.bin version\n");
	printf("u-boot-spl.bin is path to input U-Boot SPL file\n");
	printf("version shall be 0x01010101\n");
	printf("output will be written to u-boot-spl.bin.out\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int fd;
	uint32_t v;
	size_t sz;

	if (argc < 3) usage();

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) xerror(errno, argv[1]);

	v = (uint32_t)strtoul(argv[2], NULL, 16);
	v = htole32(v);
	ubsplhdr.vers = v;

	sz = (size_t)read(fd, ubootspl, sizeof(ubootspl));
	if (sz == NOSIZE) xerror(errno, argv[1]);
	if (sz >= (sizeof(ubootspl) - sizeof(struct ubootsplhdr)))
		xerror(0, "File too large! Please rebuild your SPL with -Os. Maximum allowed size is 130048 bytes.");
	v = htole32((uint32_t)sz);
	ubsplhdr.fsiz = v;

	close(fd);
	snprintf(outpath, sizeof(outpath), "%s.out", argv[1]);
	fd = creat(outpath, 0666);
	if (fd == -1) xerror(errno, outpath);

	v = crc32(~0U, 0x04c11db7U, ubootspl, sz);
	v = crc32_final(v);
	v = htole32(v);
	ubsplhdr.crcs = v;

	write(fd, &ubsplhdr, sizeof(struct ubootsplhdr));
	write(fd, ubootspl, sz);

	close(fd);

	printf("SPL written to %s successfully.\n", outpath);

	return 0;
}
