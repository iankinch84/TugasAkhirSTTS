
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
#include "apr_md5.h"

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

#include "mod_killbots.h"
#include "BloomFilter.c"
#include "des.c"
#include "string.h"


/*********************
 * Initiate Function *
 *********************/

void set_filter_size(int size);
void init_bloom_filter();

void set_des_key(unsigned char *value);
void set_key_sets(unsigned char *xdes_key);
void encrypt_des(unsigned char *plain, unsigned char *xdes_key, key_set *xks, unsigned char *digest);
void decrypt_des(unsigned char *digest, unsigned char *xdes_key, key_set *xks, unsigned char *plain);

void insert_data_LL(unsigned char *xip_address, unsigned char *xhash, time_t xtime);
void show_data_LL();

double server_load();

/*********************
 * Initiate Variable * 
 *********************/
//--Load Server
static pid_t child_pid;
int server_limit, thread_limit;

//--Bloom Filter--
unsigned char xfilter[20];

//--DES--
unsigned char *des_key;
key_set *key_sets;

//--Linked List--
typedef struct node node;
node *head;

struct node{
    unsigned char ip_address[18];
    unsigned char hash[100];
    time_t timer;
    node *next;
};

//Module Name
module AP_MODULE_DECLARE_DATA KillBots_module;

/************
 * Function * 
 ************/
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

void init_bloom_filter(){
    int i;
    for (i=0; i < FILTER_SIZE; i++){
        xfilter[i] = 0;
    }
    
    if (FILTER_SIZE > MAX_BIT){
        BIT_SHIFT = 0;
    }
    else{
        BIT_SHIFT = MAX_BIT - FILTER_SIZE;
    }    
/*
    
    FILTER_BYTE_SIZE = ((FILTER_SIZE - 3));    
    FILTER_BYTE_SIZE = 1 << FILTER_BYTE_SIZE;
*/
    
    FILTER_BITMASK = 0;
    //FILTER_BITMASK = ((1 << FILTER_SIZE) - 1);
}

void set_des_key(unsigned char *value){
    des_key = value;
    
    set_key_sets(des_key);
}

void set_key_sets(unsigned char *xdes_key){
    key_sets = (key_set *)malloc(17 * sizeof(key_set));
    generate_sub_keys(xdes_key, key_sets);
}

void encrypt_des(unsigned char *plain, unsigned char *xdes_key, key_set *xks, unsigned char *digest){
    unsigned char* data_block = (unsigned char*) malloc(strlen(plain)*sizeof(char));
    unsigned char* processed_block = (unsigned char*) malloc(strlen(plain)*sizeof(char));
        
    strcpy(data_block, plain);
    
    process_message(data_block, processed_block, xks, ENCRYPTION_MODE);
    
    strcpy(digest, processed_block);
    free(data_block);
    free(processed_block);
}

void decrypt_des(unsigned char *digest, unsigned char *xdes_key, key_set *xks, unsigned char *plain){
    unsigned char* data_block = (unsigned char*) malloc(strlen(plain)*sizeof(char));
    unsigned char* processed_block = (unsigned char*) malloc(strlen(plain)*sizeof(char));
    strcpy(data_block, digest);
    
    process_message(data_block, processed_block, xks, DECRYPTION_MODE);
    
    strcpy(plain, processed_block);
    free(data_block);
    free(processed_block);
}

void insert_data_LL(unsigned char *xip_address, unsigned char *xhash, time_t xtime){
    node *temp = (node *) malloc (sizeof(node));
    strcpy(temp->ip_address, xip_address);
    strcpy(temp->hash, xhash);
    temp->timer = xtime;
       
    temp->next = head;
    head = temp;
}

/**************************
 * Apache Module Function * 
 **************************/

static void init_table(apr_pool_t *p, server_rec *s) {
    init_bloom_filter();
    
}

static int post_read_request_handler(request_rec *r){
    //bf_insert(xfilter, r->connection->remote_ip);
    if (bf_is_any(xfilter, r->connection->remote_ip) == BF_NOT_FOUND){
        bf_insert(xfilter, r->connection->remote_ip);
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Inserting to BloomFilter");
    }
    else{
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Data Found in BloomFilter");
    }
    
    return HTTP_NOT_FOUND;
}

#ifdef HAVE_TIMES
static void status_child_init(apr_pool_t *p, server_rec *s)
{
    child_pid = getpid();
}
#endif

static int status_init(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp,
                       server_rec *s)
{
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_THREADS, &thread_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);
    
    init_bloom_filter();
    
    return OK;
}

static void register_hooks(apr_pool_t *pool){
    ap_hook_post_read_request(post_read_request_handler, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_post_config(status_init, NULL, NULL, APR_HOOK_MIDDLE);
#ifdef HAVE_TIMES
    ap_hook_child_init(status_child_init, NULL, NULL, APR_HOOK_MIDDLE);
#endif
}

module AP_MODULE_DECLARE_DATA KillBots_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,    /* per-directory config creator */
    NULL,     /* dir config merger */
    NULL, /* server config creator */
    NULL,  /* server config merger */
    NULL,                 /* command table */
    register_hooks,       /* set up other request processing hooks */
};