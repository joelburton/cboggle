#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "boggle.h"


/** Read dictionary file.
 *
 * Reads DAWG into memory.
 */

void read_all() {
  const int fd = open(WORDS_PATH, O_RDONLY);
  if (fd < 0)
    FATAL("Cannot open dict at " WORDS_PATH);
    // fatal("Cannot open " WORDS_PATH);

  int32_t nelems;
  if (read(fd, &nelems, 4) < 4)
    FATAL("Cannot get size of " WORDS_PATH);

  int32_t *f = mmap(NULL, (size_t) nelems * 4, PROT_READ, MAP_PRIVATE, fd, 0);
  if (f == MAP_FAILED)
    FATAL("Cannot read dict at " WORDS_PATH);

  // Skip over the first integer, which was the # of dawg items
  dawg = f + 1;
}
