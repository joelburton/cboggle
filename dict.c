#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "boggle.h"

const int32_t *dawg;

/** Read dictionary file.
 *
 * Reads DAWG into memory.
 *
 * @params
 *    local: is this locally-run or globally?
 */

void read_all(bool local) {
    const char *path = local ? WORDS_LOCAL_PATH : WORDS_PATH;
    const int fd = open(path, O_RDONLY);
    if (fd < 0)
        FATAL2("Cannot open dict at", path);
    // fatal("Cannot open " WORDS_PATH);

    int32_t nelems;
    if (read(fd, &nelems, 4) < 4)
        FATAL2("Cannot get size of", path);

    int32_t *f = mmap(NULL, (size_t) nelems * 4, PROT_READ, MAP_PRIVATE, fd, 0);
    if (f == MAP_FAILED)
        FATAL2("Cannot read dict at", path);

    // Skip over the first integer, which was the # of dawg items
    dawg = f + 1;
}
