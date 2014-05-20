typedef struct {
    char *key;
    char *val;
} shash_t;

int proc_lustre_parsestat (hash_t stats, const char *key, uint64_t *countp,
                           uint64_t *minp, uint64_t *maxp,
                           uint64_t *sump, uint64_t *sumsqp);

int proc_lustre_hashstats (pctx_t ctx, char *name, hash_t *hp);

