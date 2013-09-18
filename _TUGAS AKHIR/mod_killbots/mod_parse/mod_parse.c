#include "httpd.h"
#include "http_protocol.h"
#include "http_connection.h"
#include "http_log.h"
#include "http_request.h"
#include "http_config.h"
#include "http_core.h"
#include "http_main.h"
#include "apr_strings.h"
#include "util_script.h"
#include "apr_md5.h"
#include "apr_sha1.h"
#include "apr_hash.h"
#include "apr_time.h"
#include "apr_base64.h"
#include "apr_strings.h"

#include <ctype.h>
#include <string.h>

#define DEFAULT_ENCTYPE "application/x-www-form-urlencoded"

static int util_read(request_rec *r, const char **rbuf)
{
    int rc;
    if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK) {
        return rc;
    }
    
    if (ap_should_client_block(r)) {
        char argsbuffer[HUGE_STRING_LEN];
        int rsize, len_read, rpos=0;
        long length = r->remaining;
        
        *rbuf = apr_pcalloc(r->pool, length + 1);
                
        while ((len_read = ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) { 
        
            if ((rpos + len_read) > length) {
                rsize = length - rpos;
            }
            else {
                rsize = len_read;
            }
            memcpy((char*)*rbuf + rpos, argsbuffer, rsize);
            rpos += rsize;
        }

    }
    return rc;
}

static int read_post(request_rec *r, apr_table_t **tab)
{
    const char *data;
    const char *key, *val, *type;
    int rc = OK;
    
    if(r->method_number != M_POST) {
        return rc;
    }
    
    type = apr_table_get(r->headers_in, "Content-Type");
    if(apr_strnatcmp(type, DEFAULT_ENCTYPE) != 0) {
        return DECLINED;
    }
    
    if((rc = util_read(r, &data)) != OK) {
        return rc;
    }
    
    if(*tab) {
         apr_table_clear(*tab);
    }
    else {
        *tab = apr_table_make(r->pool, 8);
    }
    
    while(*data && (val = ap_getword(r->pool, &data, '&'))) { 
        key = ap_getword(r->pool, &val, '=');
        
        ap_unescape_url((char*)key);
        ap_unescape_url((char*)val);
        
        apr_table_merge(*tab, key, val);
    }
    
    return OK;
}

static apr_table_t *qos_get_query_table(request_rec *r) {
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

static int printitem(void* rec, const char* key, const char* value){
    request_rec* r = rec ;
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : [%s = %s]", ap_escape_html(r->pool, key), ap_escape_html(r->pool, value));
        
    return 1;
}

static int quick_handler(request_rec *r){
    apr_table_do(printitem, r, r->headers_in, NULL);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "=================");
    apr_table_do(printitem, r, r->headers_out, NULL);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "-----------------");
    
    return DECLINED;
}

static int xhandler(request_rec *r){
//    if ( !r->handler || apr_strnatcmp(r->handler, "mod_parse") ) {
//        return DECLINED ;   /* none of our business */
//    }
//    if ( r->method_number != M_GET ) {
//        return HTTP_METHOD_NOT_ALLOWED ;  /* Reject other methods */
//    }
//    apr_table_t *xtab = qos_get_query_table(r);
//    
//    char *temp = apr_table_get(xtab, "submit_btn");
//    char *xtemp;
//    
//    
//    ap_set_content_type(r, "text/html;charset=ascii") ;
//    ap_rputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n", r) ;
//    ap_rputs("<html>", r);
//    ap_rputs("<body>", r);
//    if (temp){
//        xtemp = apr_table_get(xtab, "input_txt");
//        ap_rprintf(r, "<h3><i>%s</i></h3>", xtemp);
//    }
//    ap_rputs("<form action=\"#\" method=\"get\">", r);
//    ap_rputs("<h3>Please Write The Number (Only) Below</h3>", r);
//    ap_rputs("<h2>1233</h2>", r);
//    ap_rputs("<input type=\"text\" name=\"input_txt\" /><br/>", r);
//    ap_rputs("<input type=\"submit\" name=\"submit_btn\" /><br/>", r);
//    ap_rputs("</form>", r);
//    ap_rputs("</body>", r);
//    ap_rputs("</html>", r);
    
    
    const char *note = apr_table_get(r->notes, "mod_parse");
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", note);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", r->handler);
    
    return DECLINED;
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

char *get_cookie(char *cookie_name, char *cookies, request_rec *r){
    unsigned char *token;

    apr_table_t *cookie_tab = apr_table_make(r->pool, 100);

    char *pair;
    char *last = NULL;
    char *del = ";";

    //--Masukkan Cookie Ke Cookie Table
    for ( pair = apr_strtok(cookies, del, &last) ; pair ;
          pair = apr_strtok(NULL, del, &last) ) {

        char *xlast = NULL;
        char *xcookie = strdup(trim(pair));
        char *xtoken, *key, *value;

        xtoken = apr_strtok(xcookie, "=", &xlast);
        key = strdup(xtoken);

        xtoken = apr_strtok(NULL, "=", &xlast);
        value = strdup(xtoken);

        apr_table_set(cookie_tab, key, value);
    }

    //--Ambil Cookie yang bersangkutan
    return apr_table_get(cookie_tab, cookie_name);
}

static int post_read_request_handler(request_rec *r){
    
//    apr_table_t *xtab = qos_get_query_table(r);
//    
//    char *temp = apr_table_get(xtab, "input_txt");
//    
//    if (temp){
//        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", temp);
//    }
    
    unsigned char *xchar = "11110000000000000000000000191555555555555555555555555555555555";
    unsigned char *puzzle_id, *xtime, *xhash, *random;
    puzzle_id = apr_pstrmemdup(r->pool, &xchar[0], 4*sizeof(char));
    random = apr_pstrmemdup(r->pool, &xchar[4], 22*sizeof(char));
    xtime = apr_pstrmemdup(r->pool, &xchar[26], 4*sizeof(char));
    xhash = apr_pstrmemdup(r->pool, &xchar[30], (strlen(xchar)-30)*sizeof(char));
   
//    struct tm *ftime;
//    time_t now;
//    now = time(NULL);
//    ftime = localtime(&now);
//    
    int hour, minute;
    
    hour = atoi(apr_pstrmemdup(r->pool, &xtime[0], 2*sizeof(char)));
    minute = atoi(apr_pstrmemdup(r->pool, &xtime[2], 2*sizeof(char)));

    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %d", hour);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %d", minute);
    
    struct tm *ftime;
    time_t raw_time;
    
    time(&raw_time);
    ftime = localtime(&raw_time);
    ftime->tm_hour = hour;
    ftime->tm_min = minute;
    
    double sec = difftime(time(NULL), mktime(ftime));
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %f", sec);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %f", sec/60);
    
//    
//    ftime->tm_hour = hour;
//    ftime->tm_min =  minute;
//    
//    double sec = difftime(time(NULL), mktime(&ftime));
//    
//    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %f", sec);
//    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %f", sec/60);
//    if (sec > (60*4)){
//        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : LEBUH");
//    }
    
//    apr_time_exp_t xxtime;
//    apr_time_t t2;
//    
//    apr_time_exp_gmt(&xxtime, apr_time_now());
//    
//    xxtime.tm_hour = hour;
//    xxtime.tm_min = minute;
//    
//    apr_time_exp_gmt_get(&t2, &xxtime);
//    
//    double xsec = apr_time_now() - t2;
        
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %f", sec);
    
    
    //tm->tm_hour = 0;
    
//    apr_time_exp_gmt_get(&t2, &tm);
    
    
    
//    if (difftime(apr_time_now(), t2) > 0){
//        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", "asdsd");
//    }
    
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", puzzle_id);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", xtime);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", random);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", xhash);
    
    unsigned char *cookie = apr_table_get(r->headers_in, "Cookie");

    if (cookie){
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", cookie);
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", cookie);
        unsigned char *del = ";";
        unsigned char *token;
        
        char *pair;
        char *last = NULL;
        
        for ( pair = apr_strtok(cookie, del, &last) ; pair ;
        pair = apr_strtok(NULL, del, &last) ) {
            char *xlast = NULL;
            char *xcookie = strdup(trim(pair));
            char *xtoken, *key, *value;

            xtoken = apr_strtok(xcookie, "=", &xlast);
            key = strdup(xtoken);

            xtoken = apr_strtok(NULL, "=", &xlast);
            value = strdup(xtoken);
            
            if (apr_strnatcmp(key, "temp5") == 0){
                if (apr_strnatcmp(value, md5_digest("mangana")) == 0){
                    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", "MANGAN");
                }
            }
            
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", trim(pair));
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : Key=%s, value=%s (%d)", key, value, strlen(value));
        }
//        while ((token = strsep(&cookie, del)) != NULL){
//            
//            if (apr_strnatcmp(token, apr_pstrcat(r->pool, "temp5=", md5_digest("mangan"))) == 0){
//                ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", "MANGAN");
//            }
//            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", trim(token));
//            
//        }
    }
    else{
        char *ttime[MAX_STRING_LEN];
        apr_ctime(ttime, apr_time_now());
        
        apr_table_addn(r->headers_out, "Set-Cookie", "temp=1000");
        apr_table_addn(r->headers_out, "Set-Cookie", "temp=3215135");
        apr_table_addn(r->headers_out, "Set-Cookie", "temp3=1000");
        apr_table_addn(r->headers_out, "Set-Cookie", "temp4=1000");
        apr_table_addn(r->headers_out, "Set-Cookie", "temp4=1000");
        apr_table_addn(r->headers_out, "Set-Cookie", apr_pstrcat(r->pool, "temp5=", md5_digest("manga21321564sd5467cdxsasdfd45645dn"), NULL));
        
    }
    
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", r->uri);
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "Custom : %s", 
                                get_cookie("temp", apr_table_get(r->headers_in, "Cookie"), r));
    
    
//    apr_table_set(r->notes, "mod_parse", "puzzle");
    
    
    
    return OK;
}

static void register_hooks(apr_pool_t *pool){
    ap_hook_post_read_request(post_read_request_handler, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(xhandler, NULL, NULL, APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA parse_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,    /* per-directory config creator */
    NULL,     /* dir config merger */
    NULL, /* server config creator */
    NULL,  /* server config merger */
    NULL,                 /* command table */
    register_hooks,       /* set up other request processing hooks */
};
