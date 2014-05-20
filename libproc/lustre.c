#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "list.h"
#include "hash.h"
#include "error.h"

#include "proc.h"
#include "lustre.h"

#define STATS_HASH_SIZE                 64

#define NONFATAL_DUPLICATE_STATS_HASHKEY 1

static void
_destroy_shash (shash_t *s)
{
    if (s) {
        if (s->key)
            free (s->key);
        if (s->val)
            free (s->val);
        free (s);
    }
}

static shash_t *
_create_shash (char *key, char *val)
{
    shash_t *s;

    if (!(s = malloc (sizeof (shash_t))))
        msg_exit ("out of memory");
    memset (s, 0, sizeof (shash_t));
    if (!(s->key = strdup (key)))
        msg_exit ("out of memory");
    if (!(s->val = strdup (val)))
        msg_exit ("out of memory");
    return s;
}

#if NONFATAL_DUPLICATE_STATS_HASHKEY
static void *
_hash_insert_lastinwins (hash_t h, const void *key, void *data, hash_del_f del)
{
    void *res = NULL, *old = NULL;

    if (!(res = hash_insert (h, key, data))) {
        if (errno != EEXIST)
            goto done;
        old = hash_remove (h, key);
        if (old)
            del (old);
        res = hash_insert (h, key, data);
    }
done: 
    return res;
}
#endif

static int
_parse_stat (char *s, shash_t **itemp)
{
    char *key = s;

    while (*s && !isspace (*s))
        s++;
    if (!*s) {
        errno = EIO;
        return -1;
    }
    *s++ = '\0';
    while (*s && isspace (*s))
        s++;
    if (!*s) {
        errno = EIO;
        return -1;
    }
    *itemp = _create_shash (key, s);
    return 0;
}

static int
_hash_stats (pctx_t ctx, hash_t h)
{
    char line[256];
    shash_t *s;
    int ret;

    errno = 0;
    while ((ret = proc_gets (ctx, NULL, line, sizeof (line))) >= 0) {
        if ((ret = _parse_stat (line, &s)) < 0)
            break;
#if NONFATAL_DUPLICATE_STATS_HASHKEY
        if (!_hash_insert_lastinwins (h, s->key, s,
                                     (hash_del_f)_destroy_shash)) {
#else
        if (!hash_insert (h, s->key, s)) {
#endif
            _destroy_shash (s);
            ret = -1;
            break;
        }
    }
    if (ret == -1 && errno == 0) /* treat EOF as success */
        ret = 0;
    return ret;
}


int
proc_lustre_hashstats (pctx_t ctx, char *name, hash_t *hp)
{
    hash_t h = NULL;
    int ret = -1;
    char *stats;

    stats = strdup("fs/lustre/llite/%s/stats");

    h = hash_create (STATS_HASH_SIZE, (hash_key_f)hash_key_string,
                    (hash_cmp_f)strcmp, (hash_del_f)_destroy_shash);

    ret = proc_openf (ctx, stats, name);
    ret = _hash_stats (ctx, h);
    proc_close (ctx);
    free (stats);

    if (ret == 0)
        *hp = h;                           
    else if (h)
        hash_destroy (h);
    return ret;
}

static int
_parse_stat_node (shash_t *node, uint64_t *countp, uint64_t *minp,
                  uint64_t *maxp, uint64_t *sump, uint64_t *sumsqp)
{
    uint64_t count = 0, min = 0, max = 0, sum = 0, sumsq = 0;
    int ret = -1;

    assert (node->val);
    if (sscanf (node->val,
                "%"PRIu64" samples %*s %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64,
                &count, &min, &max, &sum, &sumsq) < 1) {
        errno = EIO;
        goto done;
    }
    if (countp)
        *countp = count;
    if (minp)
        *minp = min;
    if (maxp)
        *maxp = max;
    if (sump)
        *sump = sum;
    if (sumsqp)
        *sumsqp = sumsq;
    ret = 0;
done:
    return ret;
}


/* stat format is:  <key>   <count> samples [<unit>] <min> <max> <sum> <sumsq> 
 * minimum is:      <key>   <count> samples [<unit>] 
 */
int
proc_lustre_parsestat (hash_t stats, const char *key, uint64_t *countp,
                       uint64_t *minp, uint64_t *maxp,
                       uint64_t *sump, uint64_t *sumsqp)
{
    shash_t *s;
    int ret = -1;

    /* Zero the counters here to avoid returning uninitialized values
       if the requested key doesn't exist in Lustre stats. */
    if (countp)
        *countp = 0;
    if (minp)
        *minp = 0;
    if (maxp)
        *maxp = 0;
    if (sump)
        *sump = 0;
    if (sumsqp)
        *sumsqp = 0;

    if (!(s = hash_find (stats, key))) {
        errno = EINVAL;
        goto done;
    }
    ret = _parse_stat_node (s, countp, minp, maxp, sump, sumsqp);
done:
    return ret;
}

