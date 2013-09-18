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
#include <ctype.h>
#include <string.h>

#include "scoreboard.h"
#include "http_log.h"

#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif
#define APR_WANT_STRFUNC
#include "apr_want.h"
#include "apr_strings.h"
#include "apr_hash.h"
#include "apr_time.h"
#include "apr_base64.h"
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

#include "BF.c"
#include "des.c"

#define SUSPECTED_MODE 1
#define NORMAL_MODE 0
#define COOKIE_NAME "mdklbs"
#define QUESTION_COUNT 20
#define KEY "killbots"
#define WRONG_COUNT 10

/*******************
 * Global Variable *
 *******************/
//--Server Status
int server_status;

//--Load Server
static pid_t child_pid;
int server_limit, thread_limit;

//--BF
unsigned char *tempchar;
unsigned char *bloom_filter;

//--DES
unsigned char *des_key;
key_set *key_sets;

//--Linked List--
typedef struct node node;
node *head;

//--Puzzle Status Handler
int puzzle_mode;

//--Linked List
struct node{
    unsigned char ip_address[18];
    unsigned char hash[100];
    time_t timer;
    node *next;
};

node *head;

//--Table
apr_table_t *question_puzzle_tab;
apr_table_t *answer_puzzle_tab;
apr_hash_t *cookie_tab;
apr_hash_t *puzzle_dump_tab;

/*******************************
 * Initialize Another Function *
 *******************************/
void set_des_key(unsigned char *value, apr_pool_t *pool);
void set_key_sets(unsigned char *xdes_key, apr_pool_t *pool);
void encrypt_des(unsigned char *plain, unsigned char *xdes_key, key_set *xks, unsigned char *digest);
void decrypt_des(unsigned char *digest, unsigned char *xdes_key, key_set *xks, unsigned char *plain);

double server_load();

unsigned char *randomize_char(int size);

apr_table_t *get_query_table(request_rec *r);
void *init_puzzle_table(apr_table_t **q_tab, apr_table_t **a_tab, apr_pool_t *pool, int size);

char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

char *md5_digest(unsigned char *plain);

int is_valid_token(char *token, request_rec *r, char *key, int puzzle);
char *get_cookie(char *cookie_name, char *cookies, request_rec *r);
char *get_update_token(char *token, request_rec *r, char *key);
char *new_token(unsigned char *puzzle_id, request_rec *r, char *key);
char *get_puzzle_id(char *token, request_rec *r);
/********************
 * Another Function *
 ********************/
void *create_puzzle(request_rec *r, char *puzzle_id){
    
    char *xtoken = apr_psprintf(r->pool, "<input type=\"hidden\" name=\"token\" value=\"%s\"/>", 
                                                                        new_token(puzzle_id, r, KEY));
    char *puzzle_text = apr_table_get(question_puzzle_tab, puzzle_id);
    char *xpuzzle = apr_psprintf(r->pool, "<h2>= %s =</h2>", puzzle_text);
    
    ap_set_content_type(r, "text/html;charset=ascii") ;
    ap_rputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n", r) ;
    ap_rputs("<html>", r);
    ap_rputs("<body>", r);
    ap_rputs("<form action=\"#\" method=\"get\" style=\"text-align:center\">", r);
    ap_rputs("<h2>Our Website is Experiencing Unusually High Load.</h2>", r);
    ap_rputs("<h2>To Restrict Automated Access We Require Code Verification.</h2>", r);
    ap_rputs("<h3>Please Enter The Code (<i>Number Only</i>) Below.</h3>", r);
    ap_rputs("<input type=\"text\" name=\"answer\" />", r);
    ap_rputs(xtoken, r);    
    ap_rputs("<input type=\"submit\" name=\"submit\" /><br/>", r);
    ap_rputs(xpuzzle, r);
    ap_rputs("</form>", r);
    ap_rputs("</body>", r);
    ap_rputs("</html>", r);
}

char *get_puzzle_id(char *token, request_rec *r){
    return apr_pstrmemdup(r->pool, &token[0], 4*sizeof(char));
}

char *new_token(unsigned char *puzzle_id, request_rec *r, char *key){
    unsigned char *xtime, *xhash, *random, *body;
    unsigned char *temp = apr_palloc(r->pool, 4*sizeof(char));
    
    temp = apr_pstrdup(r->pool,(puzzle_id));
    
    int padding = 4 - strlen(puzzle_id);
    if (padding < 0){
        return NULL;
    }
    
    while (strlen(temp) < 4){
        temp = apr_psprintf(r->pool, "0%s", temp);
    }
    
    struct tm *ftime;
    time_t raw_time;

    time(&raw_time);
    ftime = localtime(&raw_time);
    
    char *hour, *minute;
    hour = apr_psprintf(r->pool, "%i", ftime->tm_hour);
    if (strlen(hour) < 2){
        hour = apr_psprintf(r->pool, "0%s", hour);
    }
    
    minute = apr_psprintf(r->pool, "%i", ftime->tm_min);
    if (strlen(minute) < 2){
        minute = apr_psprintf(r->pool, "0%s", minute);
    }
    
    xtime = apr_psprintf(r->pool, "%s%s", hour, minute);
    random = randomize_char(22);
    body = apr_pstrcat(r->pool, temp, random, xtime, key, NULL);
    xhash = apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));    
        
    return apr_pstrcat(r->pool, temp, random, xtime, xhash, NULL);
}

char *get_update_token(char *token, request_rec *r, char *key){
    if (!token){
        return NULL;
    }
    unsigned char *puzzle_id, *xtime, *xhash, *random, *body;
    puzzle_id = apr_pstrmemdup(r->pool, &token[0], 4*sizeof(char));
    random = apr_pstrmemdup(r->pool, &token[4], 22*sizeof(char));
      
    struct tm *ftime;
    time_t raw_time;

    time(&raw_time);
    ftime = localtime(&raw_time);
    
    char *hour, *minute;
    hour = apr_psprintf(r->pool, "%i", ftime->tm_hour);
    if (strlen(hour) < 2){
        hour = apr_psprintf(r->pool, "0%s", hour);
    }
    
    minute = apr_psprintf(r->pool, "%i", ftime->tm_min);
    if (strlen(minute) < 2){
        minute = apr_psprintf(r->pool, "0%s", minute);
    }
    
    xtime = apr_psprintf(r->pool, "%s%s", hour, minute);    
    body = apr_pstrcat(r->pool, puzzle_id, random, xtime, key, NULL);
    
    xhash = apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));
    
    return apr_pstrcat(r->pool, puzzle_id, random, xtime, xhash, NULL);
}

char *get_cookie(char *cookie_name, char *cookies, request_rec *r){
    if (!cookies){
        return NULL;
    }
    
    unsigned char *token;

    apr_table_t *cookie_tab = apr_table_make(r->pool, 100);

    char *pair;
    char *last = NULL;
    char *del = ";";

    //--Masukkan Cookie Ke Cookie Table
    for ( pair = apr_strtok(cookies, del, &last) ; pair ;
          pair = apr_strtok(NULL, del, &last) ) {

        char *xlast = NULL;
        char *xcookie = apr_pstrdup(r->pool, trim(pair));
        char *xtoken, *key, *value;

        xtoken = apr_strtok(xcookie, "=", &xlast);
        key = apr_pstrdup(r->pool, xtoken);

        xtoken = apr_strtok(NULL, "=", &xlast);
        value = apr_pstrdup(r->pool, xtoken);

        apr_table_set(cookie_tab, key, value);
    }

    //--Ambil Cookie yang bersangkutan
    return apr_table_get(cookie_tab, cookie_name);
}

int is_valid_token(char *token, request_rec *r, char *key, int puzzle){
    if (!token){
        return 0;
    }
    
    unsigned char *puzzle_id, *xtime, *xhash, *random, *body;
    puzzle_id = apr_pstrmemdup(r->pool, &token[0], 4*sizeof(char));
    random = apr_pstrmemdup(r->pool, &token[4], 22*sizeof(char));
    xtime = apr_pstrmemdup(r->pool, &token[26], 4*sizeof(char));
    xhash = apr_pstrmemdup(r->pool, &token[30], (strlen(token)-30)*sizeof(char));
    
    body = apr_pstrcat(r->pool, puzzle_id, random, xtime, key, NULL);
    
    unsigned char *new_hash = apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));
    
    
    
    if (apr_strnatcmp(new_hash, xhash) == 1){
        return 0;
    }

    //-- Check Waktu Respond Form Data
    int hour, minute;
    
    hour = atoi(apr_pstrmemdup(r->pool, &xtime[0], 2*sizeof(char)));
    minute = atoi(apr_pstrmemdup(r->pool, &xtime[2], 2*sizeof(char)));

    struct tm *ftime;
    time_t raw_time;

    time(&raw_time);
    ftime = localtime(&raw_time);
    ftime->tm_hour = hour;
    ftime->tm_min = minute;

    double sec = difftime(time(NULL), mktime(ftime));  
    
    //-- puzzle = 1 -> Token untuk puzzle
    //-- puzzle = 0 -> Token untuk autentikasi
    int limit_time = 4;
    if (puzzle == 0){
        limit_time = 30;
    }

    if (sec / 60 >= limit_time){
        return 0;
    }
    
    return 1;
}

char *md5_digest(unsigned char *plain){
    srand(apr_time_now());
    
    char bsalt[64];/* random number(binary) for salt */
    char salt[128];
    char digest[128];
    
    apr_generate_random_bytes(bsalt, sizeof(bsalt));
    apr_base64_encode(salt, bsalt, sizeof(bsalt));
    
    apr_md5_encode(plain, salt, digest, sizeof(digest));
    
    return digest;
}

char *ltrim(char *s){
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s){
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s){
    return rtrim(ltrim(s)); 
}

void *init_puzzle_table(apr_table_t **q_tab, apr_table_t **a_tab, apr_pool_t *pool, int size){
    
    if (*q_tab){
        apr_table_clear(*q_tab);
    }
    else{
        *q_tab = apr_table_make(pool, size);
    }
    
    if (*a_tab){
        apr_table_clear(*a_tab);
    }
    else{
        *a_tab = apr_table_make(pool, size);
    }
    
    int i, j, count, data, ival, size_puzzle;
    unsigned char *name, *value;
    
    size_puzzle = 10;
    
    name = apr_palloc(pool, size_puzzle*sizeof(char));
    value = apr_palloc(pool, size_puzzle*sizeof(char));

    for(i=0; i<size; i++){
        ival = 0;
        
        for (j=0; j<size_puzzle; j++){
            name[j] = 0;
            value[j] = 0;
        }        
        
        for (j=0; j<size_puzzle; j++){
            srand(apr_time_now());
            count = rand()%255;
                        
            srand(apr_time_now());
            if (count % 2 == 0){
                data = rand()%9;
                name[j] = '0'+data;
                value[ival] = '0'+data;
                ival ++;
            }
            else{
                name[j] = (unsigned char)((rand()%25)+97);
            }
        }
        
        char key[10];
        sprintf(key, "%i", i);
        
        apr_table_merge(*q_tab, key, name);
        apr_table_add(*a_tab, key, value);
        
    }
    
}

apr_table_t *get_query_table(request_rec *r) {
  apr_table_t *av = apr_table_make(r->pool, 2);
  if(r->parsed_uri.query) {
    const char *q = apr_pstrdup(r->pool, r->parsed_uri.query);
    while(q && q[0]) {
      const char *t = ap_getword(r->pool, &q, '&');
      const char *name = ap_getword(r->pool, &t, '=');
      const char *value = t;
      if(name && (strlen(name) > 0)) {
        if(value && (strlen(value) > 0)) {
          apr_table_add(av, name, value);
        } else if((strlen(name) > 0)) {
          apr_table_add(av, name, "");
        }
      }
    }
  }
  return av;
}

unsigned char *randomize_char(int size){
    int i, count;
    unsigned char *asc = (unsigned char *) calloc(size, sizeof(char));
    srand(apr_time_now());
    for (i=0; i<size; i++){
        count = rand()%9;
        asc[i] = '0'+count;
    }
    
    return asc;
}

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

void set_des_key(unsigned char *value, apr_pool_t *pool){
    des_key = value;
    
    set_key_sets(des_key, pool);
}

void set_key_sets(unsigned char *xdes_key, apr_pool_t *pool){
    key_sets = (key_set *)apr_palloc(pool, 17 * sizeof(key_set));
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

/**************************
 * Apache Module Function *
 **************************/
static int x_post_config(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
    server_status = NORMAL_MODE;
    
    init_puzzle_table(&question_puzzle_tab, &answer_puzzle_tab, pconf, QUESTION_COUNT);
    cookie_tab = apr_hash_make(pconf);
    puzzle_dump_tab = apr_hash_make(pconf);
    
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_THREADS, &thread_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);
    
    tempchar = (unsigned char *) apr_palloc(pconf, FILTER_SIZE_BYTES * sizeof(char));
    bloom_filter = (unsigned char *) apr_palloc(pconf, FILTER_SIZE_BYTES * sizeof(char));
    
    int i;
    for (i=0; i < FILTER_SIZE_BYTES; i++){
        bloom_filter[i] = 0;
    }
    
    head = NULL;
    
    set_des_key("temamets", pconf);
    
    return OK;
}

#ifdef HAVE_TIMES
static void status_child_init(apr_pool_t *p, server_rec *s)
{
    child_pid = getpid();
}
#endif

static int post_read_request_handler(request_rec *r){
    char *cookie, *input, *submit, *token, *answer, *q_number;
    int counter;
    char *ip = r->connection->remote_ip;
    char *hashed_ip = apr_psprintf(r->pool, "%i", RSHash(ip, strlen(ip)));
    char *dump_counter;
    
    
    //-- Check Status - Start
    if (server_status == NORMAL_MODE){
        if (server_load() >= 70){
            server_status = SUSPECTED_MODE;
        }        
    }
    else if(server_status == SUSPECTED_MODE){
        if (server_load() <= 50){
            server_status = NORMAL_MODE;
        }
    }
    
//    if (server_status == NORMAL_MODE){
//        int i;
//        for (i=0; i < FILTER_SIZE_BYTES; i++){
//            bloom_filter[0] = 0;
//        }
//    }
    
    if (bf_is_any(bloom_filter, hashed_ip) == BF_FOUND){
        dump_counter = apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING);
        counter = atoi(dump_counter);

        if (counter > WRONG_COUNT){            
            return HTTP_BAD_REQUEST;
        }
    }
        
    //-- Server Status - SUSPECTED
    if (server_status == SUSPECTED_MODE){
        cookie = get_cookie(COOKIE_NAME, apr_table_get(r->headers_in, "Cookie"), r);
        if (cookie == NULL){
            apr_table_set(r->notes, "mod_parse", "create_puzzle");

            return DECLINED;
        }

        if (is_valid_token(cookie, r, KEY, 0) == 0){
            apr_table_set(r->notes, "mod_parse", "create_puzzle");

            return DECLINED;
        }
        else{
            char *xc = apr_hash_get(cookie_tab, cookie, APR_HASH_KEY_STRING);
            counter = 0;
            
            if (xc){
                counter = atoi(xc);
            }
            
            if (xc <= 8){
                counter++;
                apr_hash_set(cookie_tab, apr_pstrdup(r->pool, cookie), 
                        APR_HASH_KEY_STRING, apr_pstrdup(r->pool, apr_psprintf(r->pool, "%i", counter)));
            }
            else{
                return HTTP_BAD_REQUEST;
            }
        }
    }    
    
    apr_table_t *query_tab = get_query_table(r);
    
    //-- Tambahan
    char *x = apr_table_get(query_tab, "x");
    if (x){
        apr_table_set(r->notes, "mod_parse", "create_puzzle");
        
        return DECLINED;
    }
    //-- Tambahan - End
    
    input = apr_table_get(query_tab, "answer");
    submit = apr_table_get(query_tab, "submit");
    token = apr_table_get(query_tab, "token");
    
    if (input && submit && token){
        
        // -- Cek Jawaban                
        ap_unescape_url(token);
        if (!token){
            return DECLINED;
        }
        
        //--Token Salah
        if (is_valid_token(token, r, KEY, 1) == 0){
             apr_table_set(r->notes, "mod_parse", "create_puzzle");
        
             return DECLINED;
        }
        
        q_number = apr_psprintf(r->pool, "%i", atoi(get_puzzle_id(token, r)));
        answer = apr_table_get(query_tab, "answer");
        
        
        
        //-- Jawaban Salah
        if (apr_strnatcmp(apr_table_get(answer_puzzle_tab, q_number), answer) != 0){
            
            if (bf_is_any(bloom_filter, hashed_ip) == BF_FOUND){
                dump_counter = apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING);
                counter = atoi(dump_counter);
                                
                if (counter > WRONG_COUNT){                    
                    return HTTP_BAD_REQUEST;
                }
                
                counter++;
                apr_hash_set(puzzle_dump_tab, apr_pstrdup(r->pool, hashed_ip), 
                        APR_HASH_KEY_STRING, apr_pstrdup(r->pool, apr_psprintf(r->pool, "%i", counter)));
            }
            else{
                bf_insert(bloom_filter, hashed_ip);
                counter = 1;
                apr_hash_set(puzzle_dump_tab, apr_pstrdup(r->pool, hashed_ip), 
                        APR_HASH_KEY_STRING, apr_pstrdup(r->pool, apr_psprintf(r->pool, "%i", counter)));
            }
            
            apr_table_set(r->notes, "mod_parse", "create_puzzle");
        
            return DECLINED;
        }
        
        if (bf_is_any(bloom_filter, hashed_ip) == BF_FOUND){
            dump_counter = apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING);
            counter = atoi(dump_counter);
            if (counter > 0){
                counter--;
            }
            apr_hash_set(puzzle_dump_tab, apr_pstrdup(r->pool, hashed_ip), 
                        APR_HASH_KEY_STRING, apr_pstrdup(r->pool, apr_psprintf(r->pool, "%i", counter)));
        }
        
        apr_table_addn(r->headers_out, "Set-Cookie", apr_pstrcat(r->pool, COOKIE_NAME, "=", 
                                                    get_update_token(token, r, KEY), ";Path=/", NULL));
        
        apr_table_set(r->notes, "mod_parse", "redirect");
        return DECLINED;
    }
    
    return DECLINED;
}

static int content_handler(request_rec *r){
    char *notes = apr_table_get(r->notes, "mod_parse");
    
    if(r->prev){
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom: %s", apr_table_get(r->prev->notes, "mod_parse"));
    }    
    
    if (notes){
        if (apr_strnatcmp(notes, "create_puzzle") == 0){
            srand(apr_time_now());
            create_puzzle(r, apr_psprintf(r->pool, "%i", rand()%QUESTION_COUNT));
            return OK;
        }
        else if (apr_strnatcmp(notes, "redirect") == 0){
            
            char *meta = apr_psprintf(r->pool, "<meta http-equiv=\"refresh\" content=\"0; url=%s\">", r->uri);
            
            ap_set_content_type(r, "text/html;charset=ascii") ;
            ap_rputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n", r);
            ap_rputs(apr_psprintf(r->pool, "<header>%s</header>", meta), r);
            ap_rputs("<html>", r);
            ap_rputs("<body>", r);
            ap_rputs("<p>Your page will be redirected immediately</p>", r);
            ap_rputs("</body>", r);
            ap_rputs("</html>", r);
            
            return OK;
        }
    }

    char *cookie = get_cookie(COOKIE_NAME, apr_table_get(r->headers_in, "Cookie"), r);
    char *xc = apr_hash_get(cookie_tab, cookie, APR_HASH_KEY_STRING);
    int counter = 0;

    if (xc){
        counter = atoi(xc);
    }
    
    if (counter > 0){
        counter--;
    }    
    apr_hash_set(cookie_tab, apr_pstrdup(r->pool, cookie), 
            APR_HASH_KEY_STRING, apr_pstrdup(r->pool, apr_psprintf(r->pool, "%i", counter)));
    
    return DECLINED;
        
}

void try(request_rec *r){
    if (bf_is_any(tempchar, r->connection->remote_ip) == BF_NOT_FOUND){
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : Inserting to Bloom Filter");
        bf_insert(tempchar, r->connection->remote_ip);
    }
    else{
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : Found in Bloom Filter");
    }
    
    char *digest = (unsigned char *) apr_palloc(r->pool, 100*sizeof(char));
    char *plain = (unsigned char *) apr_palloc(r->pool, 100*sizeof(char));

    encrypt_des("ken", des_key, key_sets, digest);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : DES chiper %s", digest);
    decrypt_des(digest, des_key, key_sets, plain);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : DES plain %s", plain);
}

static void register_hooks(apr_pool_t *pool){
    ap_hook_post_read_request(post_read_request_handler, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_post_config(x_post_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(content_handler, NULL, NULL, APR_HOOK_MIDDLE);
#ifdef HAVE_TIMES
    ap_hook_child_init(status_child_init, NULL, NULL, APR_HOOK_MIDDLE);
#endif
}

module AP_MODULE_DECLARE_DATA variable_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,    /* per-directory config creator */
    NULL,     /* dir config merger */
    NULL, /* server config creator */
    NULL,  /* server config merger */
    NULL,                 /* command table */
    register_hooks,       /* set up other request processing hooks */
};
