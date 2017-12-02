#include "parse.h"
#include <memory.h>

hlsparse_malloc_callback hls_malloc = (hlsparse_malloc_callback) malloc;
hlsparse_free_callback hls_free = (hlsparse_free_callback) free;

HLSCode hlsparse_global_init(void)
{
    hls_malloc = (hlsparse_malloc_callback) malloc;
    hls_free = (hlsparse_free_callback) free;

    return HLS_OK;
}

HLSCode hlsparse_init_mem(hlsparse_malloc_callback m, hlsparse_free_callback f)
{
    if(!m || !f) {
        return HLS_ERROR;
    }

    hls_malloc = m;
    hls_free = f;
    return HLS_OK;
}

HLSCode hlsparse_master_init(master_t *dest)
{
    if(!dest) {
        return HLS_ERROR;
    }

    memset(dest, 0, sizeof(master_t));
    return HLS_OK;
}

HLSCode hlsparse_media_playlist_init(media_playlist_t *dest)
{
    if(!dest) {
        return HLS_ERROR;
    }
    
    memset(dest, 0, sizeof(media_playlist_t));
    return HLS_OK;
}

HLSCode hlsparse_master_term(master_t *dest)
{
    if(!dest) {
        return HLS_ERROR;
    }

    char **params[] = {
        &dest->uri
    };

    parse_param_term(params, 1);
    parse_string_list_term(&dest->custom_tags);
    parse_session_data_list_term(&dest->session_data);
    parse_media_list_term(&dest->media);
    parse_stream_inf_list_term(&dest->stream_infs);
    parse_iframe_stream_inf_list_term(&dest->iframe_stream_infs);

    return HLS_OK;
}

HLSCode hlsparse_media_playlist_term(media_playlist_t *dest)
{
    if(!dest) {
        return HLS_ERROR;
    }
    char **params[] = {
        &dest->uri
    };

    parse_param_term(params, 1);
    parse_string_list_term(&dest->custom_tags);
    parse_segment_list_term(&dest->segments);
    
    return HLS_OK;
}

int hlsparse_master(const char *src, size_t size, master_t *dest)
{
    int res = 0;

    // make sure we have some data
    if (src && *src != '\0' && src < &src[size]) {
        // go through each line parsing the tags
        const char *pt = src;
        while (*pt != '\0') {
            if (*pt == '#') {
                ++pt;
                pt += parse_master_tag(pt, size - (pt - src), dest);
            } else {
                ++pt;
            }
        }

        res = pt - src;
    }

    return res;
}

int hlsparse_media_playlist(const char *src, size_t size, media_playlist_t *dest)
{
    int res = 0;

    if(dest) {
        // reset the duration
        dest->duration = 0;
        // reset the segment byte range
        dest->next_segment_byterange.n = dest->next_segment_byterange.o = 0;
    }

    // make sure we have some data
    if(src && (src[0] != '\0') && size > 0) {
        // go through each line parsing the tags
        const char* pt = &src[0];
        while(*pt != '\0' && pt < &src[size]) {
            if(*pt == '#') {
                ++pt;
                pt += parse_media_playlist_tag(pt, size - (pt - src), dest);
            } else {
                ++pt;
            }
        }

        res = pt - src;
    }

    return res;
}
