/* standard */
#include <stdio.h>
#include <stdlib.h>

/* thirdparty */
#include <clog.h>

/* pcaio */
#include <pcaio/pcaio.h>


int
main() {
    pcaio_init(NULL);
    pcaio();
    pcaio_deinit();
    return EXIT_SUCCESS;
}
