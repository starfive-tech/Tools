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

/* all uint32_t ends up little endian in output header */
struct __attribute__((__packed__)) ubootsplhdr {
	uint32_t sofs;		/* offset of "second?" header: 0x240, dunno */
	uint32_t bofs;		/* SBL_BAK_OFFSET: Offset of backup SBL from Flash info start (from input_sbl_normal.cfg) */
	uint8_t  zro2[636];
	uint32_t vers;		/* version: shall be 0x01010101
				 * (from https://doc-en.rvspace.org/VisionFive2/SWTRM/VisionFive2_SW_TRM/create_spl.html) */
	uint32_t fsiz;		/* u-boot-spl.bin size in bytes */
	uint32_t res1;		/* unknown, 0x400 (00 04 00 00) currently */
	uint32_t crcs;		/* CRC32 of u-boot-spl.bin */
	uint8_t  zro3[364];
};

static struct ubootsplhdr ubsplhdr;

static char ubootspl[131072-sizeof(struct ubootsplhdr)+1];
static char outpath[PATH_MAX];

#define DEFVERSID 0x01010101

static void xerror(int errnoval, const char *s)
{
	if (errnoval) perror(s);
	else fprintf(stderr, "%s\n", s);
	exit(2);
}

static void usage(void)
{
	printf("usage: jh7110_uboot_spl u-boot-spl.bin [version]\n");
	printf("u-boot-spl.bin is path to input U-Boot SPL file\n");
	printf("if version specified, shall be 0x01010101\n");
	printf("output will be written to u-boot-spl.bin.out\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int fd;
	uint32_t v;
	size_t sz;

	if (argc < 2) usage();

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) xerror(errno, argv[1]);

	ubsplhdr.sofs = htole32(0x240U);
	ubsplhdr.bofs = htole32(0x200000U);
	ubsplhdr.res1 = htole32(0x400U);

	if (argc >= 3) {
		v = (uint32_t)strtoul(argv[2], NULL, 16);
		v = htole32(v);
		ubsplhdr.vers = v;
	}
	else ubsplhdr.vers = htole32(DEFVERSID);

	sz = (size_t)read(fd, ubootspl, sizeof(ubootspl));
	if (sz == NOSIZE) xerror(errno, argv[1]);
	if (sz >= (sizeof(ubootspl)))
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
