/*****************************************************************************
 *  Copyright (C) 2007-2010 Lawrence Livermore National Security, LLC.
 *  This module written by Jim Garlick <garlick@llnl.gov>.
 *  UCRL-CODE-232438
 *  All Rights Reserved.
 *
 *  This file is part of Lustre Monitoring Tool, version 2.
 *  Authors: H. Wartens, P. Spencer, N. O'Neill, J. Long, J. Garlick
 *  For details, see http://github.com/chaos/lmt.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA or see
 *  <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <dirent.h> 
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>
#include <sys/utsname.h>
#include <inttypes.h>
#include <math.h>

#include "list.h"
#include "hash.h"
#include "error.h"

#include "proc.h"
#include "stat.h"
#include "meminfo.h"
#include "lustre.h"

#include "lcmt.h"
#include "client.h"
#include "util.h"

static int
_get_mem_usage (pctx_t ctx, double *fp)
{
    uint64_t kfree, ktot;

    if (proc_meminfo (ctx, &ktot, &kfree) < 0) {
        err ("error reading memory usage from proc");
        return -1;
    }
    *fp = ((double)(ktot - kfree) / (double)(ktot)) * 100.0;
    return 0;
}

int
lmt_client_string (pctx_t ctx, char *s, int len)
{
    static uint64_t cpuused = 0, cputot = 0;
    struct utsname uts;
    double cpupct, mempct;
    int n, retval = -1;
    hash_t stats_hash = NULL;
    uint64_t read_bytes, write_bytes;
    uint64_t read_count, write_count;
    uint64_t open_count, close_count;
    uint64_t getattr_count, setattr_count;
    uint64_t seek_count, fsync_count;
    uint64_t dhits_count, dmisses_count;
    char name[256];
    DIR  *d;
    struct dirent *dir;

    d = opendir("/proc/fs/lustre/llite");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strncmp(dir->d_name, ".", 1)) continue;
            strcpy(name, dir->d_name);
        }
    }
    closedir(d);

    if (uname (&uts) < 0) {
        err ("uname");
        goto done;
    }
    if (proc_stat2 (ctx, &cpuused, &cputot, &cpupct) < 0) {
        err ("error reading cpu usage from proc");
        goto done;
    }
    if (_get_mem_usage (ctx, &mempct) < 0)
        goto done;
    if (proc_lustre_hashstats (ctx, name, &stats_hash) < 0) {
        err ("error reading lustre %s stats from proc", name);
        goto done;
    }
    proc_lustre_parsestat (stats_hash, "read_bytes", NULL, NULL, NULL,
                           &read_bytes, NULL);
    proc_lustre_parsestat (stats_hash, "read_bytes", &read_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "write_bytes", NULL, NULL, NULL,
                           &write_bytes, NULL);
    proc_lustre_parsestat (stats_hash, "write_bytes", &write_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "open", &open_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "close", &close_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "getattr", &getattr_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "setattr", &setattr_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "seek", &seek_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "fsync", &fsync_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "dirty_pages_hits", &dhits_count, NULL, NULL,
                           NULL, NULL);
    proc_lustre_parsestat (stats_hash, "dirty_pages_misses", &dmisses_count, NULL, NULL,
                           NULL, NULL);
    n = snprintf (s, len, "%f;%f"
                  ";%"PRIu64";%"PRIu64";%"PRIu64";%"PRIu64
                  ";%"PRIu64";%"PRIu64";%"PRIu64";%"PRIu64
                  ";%"PRIu64";%"PRIu64";%"PRIu64";%"PRIu64,
		  cpupct, mempct,
                  read_bytes, read_count, write_bytes, write_count,
                  open_count, close_count, getattr_count, setattr_count,
                  seek_count, fsync_count, dhits_count, dmisses_count);
    if (n >= len) {
        msg ("string overflow");
        goto done;
    }
    retval = 0;
done:
    return retval;
}

