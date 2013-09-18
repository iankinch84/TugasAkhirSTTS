#define CORE_PRIVATE
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_main.h"
#include "http_request.h"
#include "ap_mpm.h"
#include "util_script.h"
#include <time.h>
#include "scoreboard.h"
#include "http_log.h"
#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif
#define APR_WANT_STRFUNC
#include "apr_want.h"
#include "apr_strings.h"

#ifdef NEXT
#if (NX_CURRENT_COMPILER_RELEASE == 410)
#ifdef m68k
#define HZ 64
#else
#define HZ 100
#endif
#else
#include <machine/param.h>
#endif
#endif /* NEXT */

#define KBYTE 1024
#define MBYTE 1048576L
#define GBYTE 1073741824L

/*******************
 * Global Variable *
 *******************/
static pid_t child_pid;
int server_limit, thread_limit;

/**************************
 * APACHE Module Function *
 **************************/
double server_load(){
#ifdef HAVE_TIMES
    float tick;
    int times_per_thread = getpid() != child_pid;
#endif
    
    
#ifdef HAVE_TIMES
#ifdef _SC_CLK_TCK
    tick = sysconf(_SC_CLK_TCK);
#else
    tick = HZ;
#endif
#endif
    
    worker_score *ws_record;
    process_score *ps_record;

    clock_t tu, ts, tcu, tcs;
    apr_time_t nowtime;
    
    nowtime = apr_time_now();
    tu = ts = tcu = tcs = 0;
    
    int i, j, res;
    int ready;
    int busy;
    
    unsigned long count;
    unsigned long lres, my_lres, conn_lres;
    
    apr_off_t bytes, my_bytes, conn_bytes;
    apr_off_t bcount, kbcount;
    
    apr_interval_time_t up_time;
    
    for (i = 0; i < server_limit; ++i) {
#ifdef HAVE_TIMES
        clock_t proc_tu = 0, proc_ts = 0, proc_tcu = 0, proc_tcs = 0;
        clock_t tmp_tu, tmp_ts, tmp_tcu, tmp_tcs;
#endif        
        ps_record = ap_get_scoreboard_process(i);
        
        for (j = 0; j < thread_limit; ++j) {
            int indx = (i * thread_limit) + j;

            ws_record = ap_get_scoreboard_worker(i, j);
            res = ws_record->status;
            
            if (!ps_record->quiescing
                && ps_record->pid) {
                if (res == SERVER_READY
                    && ps_record->generation == ap_my_generation)
                    ready++;
                else if (res != SERVER_DEAD &&
                         res != SERVER_STARTING &&
                         res != SERVER_IDLE_KILL)
                    busy++;
            }
            
            if (ap_extended_status) {
                lres = ws_record->access_count;
                bytes = ws_record->bytes_served;

                if (lres != 0 || (res != SERVER_READY && res != SERVER_DEAD)) {

#ifdef HAVE_TIMES
                    tmp_tu = ws_record->times.tms_utime;
                    tmp_ts = ws_record->times.tms_stime;
                    tmp_tcu = ws_record->times.tms_cutime;
                    tmp_tcs = ws_record->times.tms_cstime;

                    if (times_per_thread) {
                        proc_tu += tmp_tu;
                        proc_ts += tmp_ts;
                        proc_tcu += tmp_tcu;
                        proc_tcs += proc_tcs;
                    }
                    else {
                        if (tmp_tu > proc_tu ||
                            tmp_ts > proc_ts ||
                            tmp_tcu > proc_tcu ||
                            tmp_tcs > proc_tcs) {
                            proc_tu = tmp_tu;
                            proc_ts = tmp_ts;
                            proc_tcu = tmp_tcu;
                            proc_tcs = proc_tcs;
                        }
                    }
#endif /* HAVE_TIMES */
                    
                    count += lres;
                    bcount += bytes;

                    if (bcount >= KBYTE) {
                        kbcount += (bcount >> 10);
                        bcount = bcount & 0x3ff;
                    }
                }
            }
        }
        
        tu += proc_tu;
        ts += proc_ts;
        tcu += proc_tcu;
        tcs += proc_tcs;
    }
    
    up_time = (apr_uint32_t) apr_time_sec(nowtime -
                               ap_scoreboard_image->global->restart_time);
    
    return (tu + ts + tcu + tcs) / tick / up_time * 100.;
}

#ifdef HAVE_TIMES
static void status_child_init(apr_pool_t *p, server_rec *s)
{
    child_pid = getpid();
}
#endif

static int status_handler(request_rec *r){
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Server Load : %.3g%%", server_load());
    
    return DECLINED;
}

static int status_init(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp,
                       server_rec *s)
{
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_THREADS, &thread_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);
    return OK;
}

static void register_hooks(apr_pool_t *p)
{
    ap_hook_access_checker(status_handler, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_post_config(status_init, NULL, NULL, APR_HOOK_MIDDLE);
#ifdef HAVE_TIMES
    ap_hook_child_init(status_child_init, NULL, NULL, APR_HOOK_MIDDLE);
#endif
}

module AP_MODULE_DECLARE_DATA sload_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,                       /* dir config creater */
    NULL,                       /* dir merger --- default is to override */
    NULL,                       /* server config */
    NULL,                       /* merge server config */
    NULL,         /* command table */
    register_hooks              /* register_hooks */
};
