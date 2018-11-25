#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

char *devname = "/dev/spidev0.0";
static __u8 ch = 0;
static int nch = 3;
static int verbose = 0;

static void printu8(__u8 val, char sep)
{
	unsigned b = 0x80;
	char bit;

	for (b = 0x80; b; b >>= 1, putc(bit, stderr))
		bit = val & b ? '1' : '0';
	putc(sep, stderr);
}

/*
 * SPI is byte oriented while MCP3008 is bit-oriented, so SPI shifts
 * 24 bits in 3 bytes. Channel value is the last 10 bits
 * See section 6.1 of MCP3008 data sheet for specifics
 */
static int do_readch(int fd, __u8 ch)
{
	struct spi_ioc_transfer	xfer;
	__u8 buf[3];
	int ret;

	memset(&xfer, 0, sizeof(xfer));

	buf[0] = 0x1;
	buf[1] = (8+ch) << 4;
	buf[2] = 0x0;	/* delay to allow adc */
	if (verbose) {
		fprintf(stderr, "ch%d ", ch);
		printu8(buf[0], ' ');
		printu8(buf[1], ' ');
		printu8(buf[2], ' ');
		fputs("-> ", stderr);
	}
	xfer.tx_buf = (unsigned long)buf;
	xfer.rx_buf = (unsigned long)buf;
	xfer.len = sizeof(buf);

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (ret >= 0)
		ret = (buf[1] & 0x3) << 8 | buf[2];
	if (verbose) {
		printu8(buf[0], ' ');
		printu8(buf[1], ' ');
		printu8(buf[2], '\n');
	}
	return ret;
}

void usage(char *cmd)
{
	fprintf(stderr, "usage: %s [-d devname ] [-v] [ channels .. ]\n",
		cmd);
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;
	int ret;
	int fd;
	char *stdchannels[] = { "0", "1", "2", NULL};
	char **channels = stdchannels;

	while ((opt = getopt(argc, argv, "d:v")) != EOF) {
		switch (opt) {
		case 'd':
			devname = optarg;
			continue;
		case 'v':
			verbose++;
			continue;
		default:
usage: 			usage(argv[0]);
			return 1;
		}
	}
	if (argc > optind)
		channels = argv+optind;
	fd = open(devname, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	for (; *channels; channels++) {
		ret = do_readch(fd, atoi(*channels));
		if (ret < 0)
			perror(*channels);
		printf("%d ", ret);
	}
	putchar('\n');
	close(fd);
	return 0;
}
