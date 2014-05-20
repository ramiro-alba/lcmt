#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "util.h"
#include "error.h"
#include "lcmtcerebro.h"

int
main (int argc, char *argv[]) {
    int exitval = -1;

    if (lmt_cbr_print_client_metrics () < 0)
	goto done;

    exitval = 0;

done:
    exit(exitval);
}
