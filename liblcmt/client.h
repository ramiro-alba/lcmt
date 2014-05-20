int lmt_client_string (pctx_t ctx, char *s, int len);
int
lmt_client_decode (const char *s, char **ostnamep,
                           uint64_t *read_bytesp, uint64_t *write_bytesp,
                           uint64_t *kbytes_freep, uint64_t *kbytes_totalp,
                           uint64_t *inodes_freep, uint64_t *inodes_totalp,
                           uint64_t *iopsp, uint64_t *num_exportsp,
                           uint64_t *lock_countp, uint64_t *grant_ratep,
                           uint64_t *cancel_ratep,
                           uint64_t *connectp, uint64_t *reconnectp,
                           char **recov_statusp);
