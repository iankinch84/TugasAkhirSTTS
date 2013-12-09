#include "httpd.h"
#include "http_config.h"
#include "http_connection.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_vhost.h"

#include "apr_strings.h"
#include "apr_hash.h"
#include "apr_time.h"
#include "apr_want.h"

#include "ap_mpm.h"
#include "scoreboard.h"
#include <time.h>


/********************
 * Defined Variable *
 ********************/
#define SUSPECTED_MODE 1
#define NORMAL_MODE 0
#define QUESTION_COUNT 20
#define WRONG_COUNT 32
#define KEY "killbots"
#define COOKIE_NAME "mdklbs"

#define FILTER_FOUND 1
#define FILTER_NOT_FOUND 0
#define FILTER_SIZE 20
#define NUM_HASHES 2

#define FILTER_BYTE_SIZE (1 << (FILTER_SIZE - 3)) //-- 131 072 Bytes
#define FILTER_BITMASK ((1 << FILTER_SIZE) - 1) //-- 1 048 576 Bits


/*******************
 * Global Variable *
 *******************/
//-- Server Status
int server_status;

//-- Load Server
static pid_t child_pid;
int server_limit, thread_limit;

//-- Table
apr_table_t *question_puzzle_tab;
apr_table_t *answer_puzzle_tab;
apr_hash_t *cookie_tab;
apr_hash_t *puzzle_dump_tab;

//-- Bloom Filter
static unsigned char bloom_filter[FILTER_BYTE_SIZE];


/*******************************
 * Initialize Another Function *
 *******************************/
unsigned int RSHash(unsigned char *str, unsigned int len);
unsigned int JSHash(unsigned char *str, unsigned int len);

apr_table_t *get_query_table(request_rec *r);
void *init_puzzle_table(apr_table_t **q_tab, apr_table_t **a_tab, apr_pool_t *pool, int size);

double get_server_load();
char *randomize_char(int size, request_rec *r);
char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

int is_valid_token(char *token, request_rec *r, char *key, int puzzle);
const char *get_cookie(const char *cookie_name, char *cookies, request_rec *r);
char *get_update_token(char *token, request_rec *r, char *key);
char *new_token(unsigned char *puzzle_id, request_rec *r, char *key);
char *get_puzzle_id(char *token, request_rec *r);

void *create_puzzle(request_rec *r, char *puzzle_id);

void filter_insert(unsigned char filter[], char *str);
int filter_is_any(unsigned char filter[], char *str);
void filter_get_hash(unsigned int hash[], char *str);


/********************
 * Another Function *
 ********************/
unsigned int RSHash(unsigned char *str, unsigned int len)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = hash * a + (*str);
      a    = a * b;
   }

   return (hash);
}

unsigned int JSHash(unsigned char *str, unsigned int len)
{
   unsigned int hash = 1315423911;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash ^= ((hash << 5) + (*str) + (hash >> 2));
   }

   return hash;
}

void *create_puzzle(request_rec *r, char *puzzle_id){
    
    char *xtoken = apr_psprintf(r->pool, "<input type=\"hidden\" name=\"token\" value=\"%s\"/>", 
                                                                        new_token(puzzle_id, r, KEY));
    char *puzzle_text = apr_table_get(question_puzzle_tab, puzzle_id);
    char *js0 = apr_psprintf(r->pool, 
        "<script language=\"javascript\" type=\"text/javascript\" src=\"http://%s/jquery.js\"></script>", r->hostname);
    char *js1 = apr_psprintf(r->pool, 
        "<script language=\"javascript\" type=\"text/javascript\" src=\"http://%s/index.js\"></script>", r->hostname);
    char *js2 = apr_psprintf(r->pool, 
        "<script language=\"javascript\" type=\"text/javascript\">$(document).ready(function(){ $(\"#container\").html(generateCaptcha(\"%s\")) });</script>", 
        puzzle_text);

    ap_set_content_type(r, "text/html;charset=ascii") ;
    ap_rputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n", r) ;
    ap_rputs("<html>", r);
    ap_rputs("<head>", r);
    ap_rputs(js0, r);
    ap_rputs(js1, r);
    ap_rputs(js2, r);
    ap_rputs("</head>", r);
    ap_rputs("<body>", r);
    ap_rputs("<form action=\"#\" method=\"get\" style=\"text-align:center\">", r);
    ap_rputs("<h2>Our Website is Experiencing Unusually High Load.</h2>", r);
    ap_rputs("<h2>To Restrict Automated Access We Require Code Verification.</h2>", r);
    ap_rputs("<h3>Please Enter The Code (<i>Number Only</i>) Below.</h3>", r);
    ap_rputs("<input type=\"text\" name=\"answer\" />", r);
    ap_rputs(xtoken, r);    
    ap_rputs("<input type=\"submit\" name=\"submit\" /><br/>", r);
    ap_rputs("<br/>", r);
    ap_rputs("<div id=\"container\" style=\"font-family: 'courier new'; font-size: 5pt; line-height: 80%;\">", r);        
    ap_rputs("</div>", r);
    ap_rputs("<br/>", r);
    ap_rputs("</form>", r);
    ap_rputs("</body>", r);
    ap_rputs("</html>", r);
}

char *get_puzzle_id(char *token, request_rec *r)
{
    return apr_pstrmemdup(r->pool, (const char *)&token[0], 4*sizeof(char));
}

char *new_token(unsigned char *puzzle_id, request_rec *r, char *key)
{
    unsigned char *xtime, *xhash, *random, *body;
    unsigned char *temp = (unsigned char *)apr_palloc(r->pool, 4*sizeof(char));
    
    temp = (unsigned char *)apr_pstrdup(r->pool,(puzzle_id));
    
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
    
    xtime = (unsigned char *)apr_psprintf(r->pool, "%s%s", hour, minute);
    random = (unsigned char *)randomize_char(22, r);
    body = (unsigned char *)apr_pstrcat(r->pool, temp, random, xtime, key, NULL);
    xhash = apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));    
        
    return apr_pstrcat(r->pool, temp, random, xtime, xhash, NULL);
}

char *get_update_token(char *token, request_rec *r, char *key)
{
    if (!token){
        return NULL;
    }
    unsigned char *puzzle_id, *xtime, *xhash, *random, *body;
    puzzle_id = (unsigned char *)apr_pstrmemdup(r->pool, &token[0], 4*sizeof(char));
    random = (unsigned char *)apr_pstrmemdup(r->pool, &token[4], 22*sizeof(char));
      
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
    
    xtime = (unsigned char *)apr_psprintf(r->pool, "%s%s", hour, minute);    
    body = (unsigned char *)apr_pstrcat(r->pool, puzzle_id, random, xtime, key, NULL);
    
    xhash = (unsigned char *)apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));
    
    return apr_pstrcat(r->pool, puzzle_id, random, xtime, xhash, NULL);
}

const char *get_cookie(const char *cookie_name, char *cookies, request_rec *r)
{
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

        apr_table_set(cookie_tab, (const char *)key, (const char *)value);
    }

    //--Ambil Cookie yang bersangkutan
    return apr_table_get(cookie_tab, cookie_name);
}

int is_valid_token(char *token, request_rec *r, char *key, int puzzle)
{
    if (!token){
        return 0;
    }
    
    unsigned char *puzzle_id, *xtime, *xhash, *random, *body;
    puzzle_id = (unsigned char *)apr_pstrmemdup(r->pool, &token[0], 4*sizeof(char));
    random = (unsigned char *)apr_pstrmemdup(r->pool, &token[4], 22*sizeof(char));
    xtime = (unsigned char *)apr_pstrmemdup(r->pool, &token[26], 4*sizeof(char));
    xhash = (unsigned char *)apr_pstrmemdup(r->pool, &token[30], (strlen(token)-30)*sizeof(char));
    
    body = (unsigned char *)apr_pstrcat(r->pool, puzzle_id, random, xtime, key, NULL);
    
    unsigned char *new_hash = apr_psprintf(r->pool, "%u", RSHash(body, strlen(body)));
    
    if (apr_strnatcmp(new_hash, xhash) == 1){
        return 0;
    }

    //-- Check Waktu Respond Form Data
    int hour, minute;
    
    hour = atoi(apr_pstrmemdup(r->pool, (const char *)&xtime[0], 2*sizeof(char)));
    minute = atoi(apr_pstrmemdup(r->pool, (const char *)&xtime[2], 2*sizeof(char)));

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

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

void *init_puzzle_table(apr_table_t **q_tab, apr_table_t **a_tab, apr_pool_t *pool, int size)
{
    
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
    
    name = (unsigned char *)apr_palloc(pool, size_puzzle*sizeof(char));
    value = (unsigned char *)apr_palloc(pool, size_puzzle*sizeof(char));

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
                name[j] = (unsigned char *)((rand()%25)+97);
                
            }
        }
        
        char key[size_puzzle];
        sprintf(key, "%i", i);
        apr_table_set(*q_tab, (const char *)key, (const char *)name);
        apr_table_set(*a_tab, (const char *)key, (const char *)value);
        
    }
    
}

apr_table_t *get_query_table(request_rec *r) 
{
  apr_table_t *av = apr_table_make(r->pool, 2);
  if(r->parsed_uri.query) {
      
    const char *q = (const char *)apr_pstrdup(r->pool, (const char *)r->parsed_uri.query);
    
    while(q && q[0]) {
        
      const char *t = ap_getword(r->pool, &q, '&');
      const char *name = ap_getword(r->pool, &t, '=');
      const char *value = apr_pstrdup(r->pool, t);
      
      if(name && (strlen(name) > 0)) {
        if(value && (strlen(value) > 0)) {
            
          apr_table_add(av, name, value);
          
        } 
        else if((strlen(name) > 0)) {
            
          apr_table_add(av, name, "");
          
        }
      }
    }
  }
  
  return av;
}

char *randomize_char(int size, request_rec *r)
{
    int i, count;
    char *asc = (char *) apr_palloc(r->pool, size*sizeof(char));
    srand(apr_time_now());
    for (i=0; i<size; i++){
        count = rand()%9;
        asc[i] = '0'+count;
    }
    
    return asc;
}

double get_server_load()
{
    worker_score *ws_record;
    process_score *ps_record;
    int i, j, res;
    int ready;
    int busy;

    busy = 0;
    ready = 0;

    for (i = 0; i < server_limit; ++i) {
        ps_record = ap_get_scoreboard_process(i);

        for (j = 0; j < thread_limit; ++j) {

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
        }
    }


    return (busy * 100)/ (server_limit * thread_limit);
}

void filter_get_hash(unsigned int hash[], char *str)
{
    unsigned char *str_in = (unsigned char *)str;
    int pos = strlen(str_in);

    hash[0] = RSHash (str_in, pos);
    hash[1] = JSHash (str_in, pos);
}

int filter_is_any(unsigned char filter[], char *str)
{
    unsigned int hash[NUM_HASHES];
    int i;

    filter_get_hash(hash, str);

    for (i = 0; i < NUM_HASHES; i++) {
            hash[i] = (hash[i] >> FILTER_SIZE) ^ (hash[i] & FILTER_BITMASK);
            
            if (!(filter[hash[i] >> 3] & (1 << (hash[i] & 7)))){
                return 0;
            }                    
    }

    return 1;
}

void filter_insert(unsigned char filter[], char *str)
{
    unsigned int hash[NUM_HASHES];
    int i;

    filter_get_hash(hash, str);

    for (i = 0; i < NUM_HASHES; i++) {
        /* xor-fold the hash into FILTER_SIZE bits */
        hash[i] = (hash[i] >> FILTER_SIZE) ^ 
                  (hash[i] & FILTER_BITMASK);
        /* set the bit in the filter */
        filter[hash[i] >> 3] |= 1 << (hash[i] & 7);

    }
}

void filter_init(unsigned char filter[])
{
    int i;
    for (i=0; i < FILTER_BYTE_SIZE; i++){
        filter[i] = 0;
    }
}


/**************************
 * Apache Module Function *
 **************************/
static int post_read_request_handler(request_rec *r){
    const char *cookie;
    const char *input, *submit, *token, *answer, *q_number;
    const char *answer_name, *submit_name, *token_name, *note_name;
    int counter;
    char *ip = r->connection->remote_ip;
    char *hashed_ip = apr_psprintf(r->pool, "%u", RSHash((unsigned char *)ip, strlen(ip)));
    char *dump_counter;
    apr_table_t *query_tab;
        
    query_tab = get_query_table(r);
        
    answer_name = "answer";
    submit_name = "submit";
    token_name = "token";
    note_name = "mod_parse";
    
    input = apr_table_get(query_tab, answer_name);
    submit = apr_table_get(query_tab, submit_name);
    token = apr_table_get(query_tab, token_name);

    //-- Check Status - Start
    if (server_status == NORMAL_MODE){
        if (get_server_load() >= 70){
            server_status = SUSPECTED_MODE;
        }        
    }
    else if(server_status == SUSPECTED_MODE){
        if (get_server_load() <= 50){
            server_status = NORMAL_MODE;
        }
    }
    
    if (server_status == NORMAL_MODE){
        filter_init(bloom_filter);
        apr_hash_clear(puzzle_dump_tab);
    }
    
    //--Jika ada dalam bloom filter
    if (filter_is_any(bloom_filter, hashed_ip)){
        counter = atoi(apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING));
        if (counter > WRONG_COUNT){
            return HTTP_FORBIDDEN;
        }
    }
    
    //-- Ketika suspected mode tetapi tidak sedang menjawab    
    if (server_status == SUSPECTED_MODE && !input && !submit && !token){
        cookie = get_cookie((const char *)COOKIE_NAME, apr_table_get(r->headers_in, (const char *)"Cookie"), r);
        if (cookie == NULL){
            apr_table_set(r->notes, note_name, (const char *)"create_puzzle");

            return DECLINED;
        }

        if (is_valid_token((char *)cookie, r, KEY, 0) == 0){
            apr_table_set(r->notes, note_name, (const char *)"create_puzzle");

            return DECLINED;
        }
    }    
    
    //-- Tambahan
    char *x = (char *)apr_table_get(query_tab, (const char *)"x");
    if (x){
        apr_table_set(r->notes, note_name, (const char *)"create_puzzle");
        
        return DECLINED;
    }
    //-- Tambahan - End

    if (input && submit && token){
        
        ap_unescape_url((char *)token);
        
        if (!token){
            return DECLINED;
        }
        
        //--Token Salah
        if (is_valid_token((char *)token, r, KEY, 1) == 0){
             apr_table_set(r->notes, note_name, (const char *)"create_puzzle");
        
             return DECLINED;
        }
        
        q_number = apr_psprintf(r->pool, "%i", atoi(get_puzzle_id((char *)token, r)));
        answer = apr_table_get(query_tab, answer_name);
        
        counter = 0;
        //-- Jawaban Salah
        if (apr_strnatcmp(apr_table_get(answer_puzzle_tab, q_number), answer) != 0){
            
            if (filter_is_any(bloom_filter, hashed_ip)){
                counter = atoi(apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING));
            }
            else{
                filter_insert(bloom_filter, apr_pstrdup(r->server->process->pconf, hashed_ip));
                
            }
            
            counter++;
            
            //-- Masukkan ke dalam puzzle dump
            apr_hash_set(puzzle_dump_tab, 
                    apr_pstrdup(r->server->process->pconf, hashed_ip), 
                    APR_HASH_KEY_STRING, apr_pstrdup(r->server->process->pconf, 
                    apr_psprintf(r->server->process->pconf, "%i", counter)));
            
            apr_table_set(r->notes, note_name, (const char *)"create_puzzle");
        
            return DECLINED;
        }        
        
        
        if (filter_is_any(bloom_filter, hashed_ip)){
            counter = atoi(apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING));
            counter--;
            
            if (counter < 0){
                counter = 0;
            }
            
            apr_hash_set(puzzle_dump_tab, 
                    apr_pstrdup(r->server->process->pconf, hashed_ip), 
                    APR_HASH_KEY_STRING, apr_pstrdup(r->server->process->pconf, 
                    apr_psprintf(r->server->process->pconf, "%i", counter)));
        }
        
        apr_table_addn(r->headers_out, (const char *)"Set-Cookie", (const char *)apr_pstrcat(r->pool, COOKIE_NAME, "=", 
                                                    get_update_token((char *)token, r, KEY), ";Path=/", NULL));
        
        apr_table_set(r->notes, note_name, (const char *)"redirect");
        return DECLINED;
    }
    
    return DECLINED;
}

static int content_handler(request_rec *r)
{
    int counter;
    char *ip = r->connection->remote_ip;
    char *hashed_ip = apr_psprintf(r->pool, "%u", RSHash((unsigned char *)ip, strlen(ip)));
    const char *note_name = "mod_parse";
    char *notes = (char *)apr_table_get(r->notes, note_name);
    
    if (notes){
        counter = 0;
        if (apr_strnatcmp(notes, "create_puzzle") == 0){
            srand(apr_time_now());
            create_puzzle(r, apr_psprintf(r->pool, "%i", rand()%QUESTION_COUNT));
            
            if (filter_is_any(bloom_filter, hashed_ip)){
                counter = atoi(apr_hash_get(puzzle_dump_tab, hashed_ip, APR_HASH_KEY_STRING));
            }
            else{
                filter_insert(bloom_filter, apr_pstrdup(r->server->process->pconf, hashed_ip));
                
            }
            
            counter++;
            
            //-- Masukkan ke dalam puzzle dump
            apr_hash_set(puzzle_dump_tab, 
                    apr_pstrdup(r->server->process->pconf, hashed_ip), 
                    APR_HASH_KEY_STRING, apr_pstrdup(r->server->process->pconf, 
                    apr_psprintf(r->server->process->pconf, "%i", counter)));
            
            apr_table_set(r->notes, note_name, (const char *)"create_puzzle");
            
            return OK;
        }
        else if (apr_strnatcmp((char *)notes, "redirect") == 0){
            
            char *meta = apr_psprintf(r->pool, "<meta http-equiv=\"refresh\" content=\"5; url=%s\">", r->uri);
            
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
    
    return DECLINED;
        
}

static int x_post_config(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
    server_status = NORMAL_MODE;
    
    init_puzzle_table(&question_puzzle_tab, &answer_puzzle_tab, pconf, QUESTION_COUNT);
    
    cookie_tab = apr_hash_make(pconf);
    puzzle_dump_tab = apr_hash_make(pconf);
    
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_THREADS, &thread_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);
    
    filter_init(bloom_filter);
    
    return OK;
}

#ifdef HAVE_TIMES
static void status_child_init(apr_pool_t *p, server_rec *s)
{
    child_pid = getpid();
}
#endif

static void register_hooks(apr_pool_t *pool){
    ap_hook_post_read_request(post_read_request_handler, NULL, NULL, APR_HOOK_FIRST);
    ap_hook_post_config(x_post_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(content_handler, NULL, NULL, APR_HOOK_FIRST);
#ifdef HAVE_TIMES
    ap_hook_child_init(status_child_init, NULL, NULL, APR_HOOK_MIDDLE);
#endif
}

module AP_MODULE_DECLARE_DATA killbots_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,    /* per-directory config creator */
    NULL,     /* dir config merger */
    NULL, /* server config creator */
    NULL,  /* server config merger */
    NULL,                 /* command table */
    register_hooks,       /* set up other request processing hooks */
};
