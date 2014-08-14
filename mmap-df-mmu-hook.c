#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#define DF_MMU_DEV_PATH "/dev/df_mmu"

static int df_mmu_fd = -1;

static void
close_df_mmu(void)
{
	if (df_mmu_fd != -1) {
		(void) close(df_mmu_fd);	/* someone else may have closed it */
		df_mmu_fd = -1;
	}
}

static int
open_df_mmu(void)
{
	int ret = 0;

	if ((df_mmu_fd = open(DF_MMU_DEV_PATH, O_RDWR)) == -1)
		return -1;

        return ret;
}
void usage()
{
	fprintf(stderr, "Usage: mmap [-u -s delay -t -T] <filename>\n");
}


int main(int argc, char *argv[])
{

	int fd;
	struct stat fs;
	size_t size;

	int opt;
	int disable_thp = -1;
	int unmap = 0;
	unsigned int delay = 0;

	char *src, *dst;

	while ((opt = getopt(argc, argv, "utTs:")) != -1) {
		switch (opt) {
			case 't':
				disable_thp = 1;
				break;
			case 'T':
				disable_thp = 0;
				break;
			case 'u':
				unmap = 1;
				break;
			case 's':
				delay = atoi(optarg);
				break;
			default:
				usage();
				exit(EXIT_FAILURE);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 1) {
		usage();
		exit(EXIT_FAILURE);
	}

	if ((fd = open(*argv, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", *argv, strerror(errno));
		exit(EXIT_FAILURE);
	}

	open_df_mmu();
	
	if (fstat(fd, &fs) == -1) {
		fprintf(stderr, "Failed to stat %s: %s\n", *argv, strerror(errno));
		exit(EXIT_FAILURE);
        }

	size = fs.st_size;
	
	src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd , 0);
	
	if (src == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap file %s: %s\n", *argv, strerror(errno));
		exit(EXIT_FAILURE);
	}

	close(fd);

	dst = mmap(NULL, size, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 , 0);

	if (dst == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap anon memory: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (disable_thp == 0) {
		madvise(src, size, MADV_HUGEPAGE);
		madvise(dst, size, MADV_HUGEPAGE);
	} else if (disable_thp == 1) {
		madvise(src, size, MADV_NOHUGEPAGE);
		madvise(dst, size, MADV_NOHUGEPAGE);
	}
		
	memcpy(dst, src, size);

	if (unmap) {
		munmap(src, size);
		munmap(dst, size);
	}

	if (delay)
		sleep(delay);

	close_df_mmu();
	return 0;
}
