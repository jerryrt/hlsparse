#include <hlsparse.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define ALOGD(...) printf(__VA_ARGS__)
#define ALOGI(...) printf(__VA_ARGS__)
#define ALOGW(...) printf(__VA_ARGS__)
#define ALOGE(...) printf(__VA_ARGS__)

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        ALOGD("Failed to open file '%s'\n", filename);
        return NULL;
    }   

    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(sizeof(char) * (file_size + 1));
    if (buffer == NULL) {
        fclose(file);
        ALOGD("Failed to allocate memory for file '%s'\n", filename);
        return NULL;
    }   

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read != file_size) {
        fclose(file);
        free(buffer);
        ALOGD("Failed to read file '%s'\n", filename);
        return NULL;
    }   

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}


static int get_value_by_name(const char *search_str, const char *param, char *value) {
	if (search_str == NULL || param == NULL || value == NULL) return -1;

	char *p_start , *p_end;
	char flag = 0;

	int paramLen = strlen(param);

	if (paramLen == 0) return -1;

	p_start = strstr(search_str, param);

	if (!p_start) return 0;

	p_start += paramLen;

	while (*p_start == '=' || *p_start == '\"' || *p_start == '\'') {
		if (*p_start == '\"' || *p_start == '\'') flag = *p_start;
		p_start++;
	}
	p_end = p_start;
	flag = flag != 0 ? flag : ',';

	while (*p_end != flag) p_end++;

	if (!p_end) return 0;

	strncpy(value, p_start, p_end - p_start);

	return strlen(value);
}



/*
 * matchResults are pointers to the outputBuf, make sure outputBuf is big enough to store all the results.
 */
static int regex_match(const char * source, const char * regexString, size_t maxGroups, char * outputBuf, const char *matchResults[]) {
    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];

    if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
        ALOGW("Could not compile regular expression.\n");
        return 1;
    }

    int exec_ret = regexec(&regexCompiled, source, maxGroups, groupArray, 0);
    if (exec_ret != 0) {
        ALOGW("Regular expression execution failure: %d.\n", exec_ret);
        ALOGW("Input: %s, Regex: %s\n", source, regexString);
        regfree(&regexCompiled);
        return exec_ret;
    }

    unsigned int g = 0;
    char * bufPos = outputBuf;
    for (g = 0; g < maxGroups; g++) {
        regmatch_t m = groupArray[g];
        if (m.rm_so == (size_t)-1)
            break;  // No more groups

        int matchBeg = m.rm_so;
        int matchEnd = m.rm_eo;
        int matchLen = matchEnd - matchBeg;

        matchResults[g] = bufPos;
        memcpy(bufPos, source+matchBeg, matchLen);
        bufPos += matchLen;
        *bufPos = '\0';
        bufPos += 1;

        //printf("Group %u: [%2u-%2u]: %s\n", g, matchBeg, matchEnd, matchResults[g]);
    }

    regfree(&regexCompiled);

    return 0;
}

static int parser_g_init = 0;
static HLSCode hls_parser_global_init_once() {
    if (!parser_g_init) {
        HLSCode res = hlsparse_global_init();
        if(res != HLS_OK) {
            ALOGW("failed to initialize hlsparse\n");
            return -1;
        }
        parser_g_init = 1;
    }

    return HLS_OK;
}

int parse_m3u8_master(master_t *m3u8_obj, const char *m3u8_txt) {
    if (NULL == m3u8_obj) {
        return -1;
    }

    HLSCode res = hls_parser_global_init_once();
    if (res != HLS_OK) {
      ALOGW("global init failure.");
      return -1;
    }

    res = hlsparse_master_init(m3u8_obj);
    if(res != HLS_OK) {
        ALOGW("failed to initialize master m3u8 structure\n");
        return -1;
    }

    char *null_str = malloc(4);
    null_str[0] = '\0';
    m3u8_obj->uri = null_str;

    // parse the playlist information into our master structure
    int read = hlsparse_master(m3u8_txt, strlen(m3u8_txt), m3u8_obj);

    ALOGD("num of streams: %d, num of iframe_streams, %d, num of keys: %d\n", m3u8_obj->nb_stream_infs, m3u8_obj->nb_iframe_stream_infs, m3u8_obj->nb_session_keys);

    return read;
}


int parse_m3u8_playlist(media_playlist_t *m3u8_obj, const char *m3u8_txt) {
    if (NULL == m3u8_obj) {
        return -1;
    }

    HLSCode res = hls_parser_global_init_once();
    if (res != HLS_OK) {
      ALOGW("global init failure.");
      return -1;
    }

    res = hlsparse_media_playlist_init(m3u8_obj);
    if(res != HLS_OK) {
        ALOGW("failed to initialize playlist m3u8 structure\n");
        return -1;
    }

    // parse the playlist information into our master structure
    int read = hlsparse_media_playlist(m3u8_txt, strlen(m3u8_txt), m3u8_obj);

    ALOGD("num of segments: %d, duration: %f, custom tags: %d, uri: %s\n", m3u8_obj->nb_segments, m3u8_obj->duration, m3u8_obj->nb_custom_tags, m3u8_obj->uri);

    return read;
}

HLSCode write_m3u8_master(master_t *m3u8_obj, char **m3u8_buf, int *buf_len, const stream_inf_list_t *stream_infs, const media_list_t *medias) {
    if (NULL == m3u8_obj) {
        return -1;
    }
    
    HLSCode res = hls_parser_global_init_once();
    if (res != HLS_OK) {
      ALOGW("global init failure.");
      return -1;
    }

    res = hlsparse_master_init(m3u8_obj);
    if(res != HLS_OK) {
        ALOGW("failed to initialize master playlist structure\n");
        return -1;
    }
    m3u8_obj->version = 4;
    m3u8_obj->independent_segments = HLS_TRUE;
    char *null_str = malloc(4);
    null_str[0] = '\0';
    m3u8_obj->uri = null_str;

    if (stream_infs) m3u8_obj->stream_infs = *stream_infs;
    if (medias) m3u8_obj->media = *medias;

    res = hlswrite_master(m3u8_buf, buf_len, m3u8_obj);
    if(res != HLS_OK) {
        ALOGW("failed to write the m3u8\n");
        return -1;
    }

    return res;
}

const char* _stristr(const char* haystack, const char* needle) {
  do {
    const char* h = haystack;
    const char* n = needle;
    while (tolower((unsigned char) *h) == tolower((unsigned char ) *n) && *n) {
      h++;
      n++;
    }
    if (*n == 0) {
      return haystack;
    }
  } while (*haystack++);
  return 0;
}

int find_stream_inf_of(const master_t *m3u8_obj, const stream_inf_list_t **out, const char * codec, const int width, const int height) {
    if (NULL == m3u8_obj) {
        return -1;
    }

    const stream_inf_list_t *streamInf = &m3u8_obj->stream_infs;
    int idx = 0;
    while(streamInf && streamInf->data) {
        int match = 1;
        if (width > 0 && width != streamInf->data->resolution.width) {
            match = 0;
        }
        if (height > 0 && height != streamInf->data->resolution.width) {
            match = 0;
        }
        if (codec && !_stristr(streamInf->data->codecs, codec)) {
            match = 0;
        }
        if (match) {
            *out = streamInf;
            ALOGD("found matching stream: %p at pos: %d\n", *out, idx);
            return idx;
        }
        idx++;
        streamInf = streamInf->next;
    }
    return -1;
}

int find_media_of(const master_t *m3u8_obj, const media_list_t **out, const char * group_id) {
    if (NULL == m3u8_obj || NULL == group_id) {
        return -1;
    }
    const media_list_t *mediaInf = &m3u8_obj->media;
    int idx = 0;
    while (mediaInf && mediaInf->data) {
        if (!strcmp(mediaInf->data->group_id, group_id)) {
            *out = mediaInf;
            ALOGD("found matching media: %p at pos: %d\n", *out, idx);
            return idx;
        }
        idx++;
        mediaInf = mediaInf->next;
    }
    return -1;
}

// "mlhls://localhost/itag/234/mediadata.m3u8" => "234"
static int itag_id_in_ytb_uri(const char * uri, char *buf, int buf_size) {
    if (uri == NULL || buf == NULL) {
        return 0;
    }
    const char * p1 = NULL;
    const char * p2 = NULL;
    if ((p1=strstr(uri, "/itag/")) != NULL && (p2=strstr(p1, "/mediadata.m3u8")) != NULL) {
        memset(buf, 0, buf_size);
        p1 = p1 + strlen("/itag/");
        int i = 0;
        while (p1 < p2) {
            buf[i] = *p1;
            p1++;
            i++;
        }
        return strlen(buf);
    } else {
        ALOGD("no ytb tag found.\n");
        return 0;
    }
}

// "...mlhls://localhost/itag/234/mediadata.m3u8..." => "...mlhls://localhost/itag/234_mediadata.m3u8..."
static char * uri_modify_ytb_master_itag_path(char * buf) {
    if (buf == NULL) {
        return NULL;
    }

    const char *search = "/mediadata.m3u8";
    int sLen = strlen(search);
    char *p1 = strstr(buf, search);
    while (p1 != NULL) {
        *p1='_';
        p1 = strstr(p1+sLen, search);
    }

    return buf;
}

static void test_master_m3u8_update() {
    char *m3u8 = read_file("yt_master.m3u8");
    ALOGD("test_master.m3u8\n%s", m3u8);

    // create a master playlist structure
    master_t myMaster;
    // parse the playlist information into our master structure
    int read = parse_m3u8_master(&myMaster, m3u8);
    ALOGD("read a total of %d bytes parsing the master source\n", read);

    // print out all the StreamInf bitrates that were found
    stream_inf_list_t *streamInf = &myMaster.stream_infs;
    int count = 0;
    while(streamInf && streamInf->data) {
        ALOGD("StreamInf %d Uri: %s\n", count, streamInf->data->uri);
        ALOGD("StreamInf %d Bandwidth: %f\n", count, streamInf->data->bandwidth);
        ALOGD("StreamInf %d Codec: %s\n", count, streamInf->data->codecs);
        ALOGD("StreamInf %d Audio: %s\n", count, streamInf->data->audio);
        ALOGD("StreamInf %d Video: %s\n", count, streamInf->data->video);
        ALOGD("StreamInf %d WxH: %dx%d\n", count, streamInf->data->resolution.width, streamInf->data->resolution.height);
        ++count;
        streamInf = streamInf->next;
    }

    media_list_t *mediaInf = &myMaster.media;
    count = 0;
    while (mediaInf && mediaInf->data) {
        ALOGD("Media %d Uri: %s\n", count, mediaInf->data->uri);
        ++count;
        mediaInf = mediaInf->next;
    }

    session_data_list_t *sessInf= &myMaster.session_data;
    count = 0;
    while (sessInf && sessInf->data) {
        ALOGD("Sess %d Uri: %s\n", count, sessInf->data->uri);
        ++count;
        sessInf = sessInf->next;
    }

    
    // master_t outMaster;
    
    // const stream_inf_list_t *stream_search = NULL;
    // stream_inf_list_t stream_write;
    // stream_inf_list_t *stream_write_ptr = NULL;
    // const int res_search_arr[] = {1920, 1280, 854, 640, 420, 256};
    // for (int c=0; c<sizeof(res_search_arr); c++) {
    //     int resolution = res_search_arr[c];
    //     if (find_stream_inf_of(&myMaster, &stream_search, "avc1.", resolution, 0)>=0 && stream_search) {
    //         stream_write = *stream_search;
    //         stream_write.next = NULL;
    //         stream_write_ptr = &stream_write;
    //         break;
    //     }
    // }

    // const media_list_t *media_search = NULL;
    // media_list_t media_write;
    // media_list_t *media_write_ptr = NULL;
    // if (stream_write_ptr && find_media_of(&myMaster, &media_search, stream_write_ptr->data->audio)>=0 && media_search) {
    //     media_write = *media_search;
    //     media_write.next = NULL;
    //     media_write_ptr = &media_write;
    // }

    // if (stream_write_ptr == NULL || media_write_ptr == NULL) {
    //     ALOGW("no available video+audio track.");
    // }

    // char itag_buf[32] = {'0','\0'};
    // int len = itag_id_in_ytb_uri(stream_write_ptr->data->uri, itag_buf, sizeof(itag_buf));
    // if (len > 0) ALOGD("extracted ytb tag: %s\n", itag_buf);

    // char *out = NULL;
    // int size = 0;
    // HLSCode res = write_m3u8_master(&outMaster, &out, &size, stream_write_ptr, media_write_ptr);
    // ALOGD("write m3u8 result: %d\n", res);
    // if (out) {
    //     ALOGD("content dump(copy a):\n%s\n", out);
    //     out = uri_modify_ytb_master_itag_path(out);
    //     ALOGD("content dump(copy b):\n%s\n", out);
    // }

    // char test_str[8];
    // printf("dump address on stack: %p\n", test_str);
    // hlsparse_master_term(&outMaster);
    hlsparse_master_term(&myMaster);
    
    if (m3u8) free(m3u8);
    // if (out) free(out);

}

static int parse_yt_ext_tag_data(const char * ytExtTag, char * dataBuf, const char** results) {
    const char * regexString = "YT-EXT-CONDENSED-URL:BASE-URI=\"(.+)\",PARAMS=\"(.+)\",PREFIX=\"(.+)\"";
    size_t maxGroups = 4;
    return regex_match(ytExtTag, regexString, maxGroups, dataBuf, results);
}

static int split_yt_str_by_delim(char * input, const char * delim, const char** outStrArr) {
    char * ypPtr = NULL;
    int ypNum = 0;

    for (outStrArr[ypNum]=strtok_r(input, delim, &ypPtr); outStrArr[ypNum]!=NULL; ) {
        ypNum++;
        outStrArr[ypNum]=strtok_r(NULL, delim, &ypPtr);
    }

    return ypNum;
}

static void format_yt_uri_with_params(char * newUri, const char * baseUrl, const char **yt_p_arr, const int yt_p_n, const char **yt_u_arr, const int yt_u_n) {
    if (yt_u_n < 1 || yt_u_n != yt_p_n+1) {
        ALOGW("[%s:%d]params length invalid(%d:%d), abort.\n",  __func__, __LINE__, yt_p_n, yt_u_n);
        return;
    }

    switch (yt_p_n) {
        case 0:
            sprintf(
                newUri, "%s", baseUrl
            );
            break;
        case 1:
            sprintf(
                newUri, "%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1]
            );
            break;
        case 2:
            sprintf(
                newUri, "%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2]
            );
            break;
        case 3:
            sprintf(
                newUri, "%s/%s/%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2],
                yt_p_arr[2], yt_u_arr[3]
            );
            break;
        case 4:
            sprintf(
                newUri, "%s/%s/%s/%s/%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2],
                yt_p_arr[2], yt_u_arr[3],
                yt_p_arr[3], yt_u_arr[4]
            );
            break;
        case 5:
            sprintf(
                newUri, "%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2],
                yt_p_arr[2], yt_u_arr[3],
                yt_p_arr[3], yt_u_arr[4],
                yt_p_arr[4], yt_u_arr[5]
            );
            break;
        case 6:
            sprintf(
                newUri, "%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2],
                yt_p_arr[2], yt_u_arr[3],
                yt_p_arr[3], yt_u_arr[4],
                yt_p_arr[4], yt_u_arr[5],
                yt_p_arr[5], yt_u_arr[6]
            );
            break;
        case 7:
            sprintf(
                newUri, "%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s", baseUrl, 
                yt_p_arr[0], yt_u_arr[1],
                yt_p_arr[1], yt_u_arr[2],
                yt_p_arr[2], yt_u_arr[3],
                yt_p_arr[3], yt_u_arr[4],
                yt_p_arr[4], yt_u_arr[5],
                yt_p_arr[5], yt_u_arr[6],
                yt_p_arr[6], yt_u_arr[7]
            );
            break;
        default:
            ALOGW("[%s:%d]too many params, broken logic.\n", __func__, __LINE__);
    }
}

static void test_playlist_m3u8_update() {
    char *m3u8 = read_file("yt_input_001.m3u8");
    ALOGD("yt_input_001.m3u8\n%s", m3u8);

    // create a media playlist structure
    media_playlist_t myPlaylist;
    // parse the text M3U8
    int read = parse_m3u8_playlist(&myPlaylist, m3u8);
    ALOGD("read a total of %d bytes parsing the playlist source\n", read);

    // print out all the StreamInf bitrates that were found
    segment_list_t *firstSeg = &myPlaylist.segments;
    segment_list_t *seg = firstSeg;


    const int line_len = 2048;
    char * regexBuf = malloc(line_len);
    const char* regexMatches[8];
    const char* baseUrl = NULL;
    const char* ytParams = NULL;
    const char* ytPrefix = NULL;
    if (firstSeg && firstSeg->data 
        && firstSeg->data->custom_tags.data) {
        const char *cust_tag_data = firstSeg->data->custom_tags.data;
        
        int good = parse_yt_ext_tag_data(cust_tag_data, regexBuf, regexMatches);
        baseUrl = regexMatches[1];
        ytParams = regexMatches[2];
        ytPrefix = regexMatches[3];
        free(firstSeg->data->custom_tags.data);
        firstSeg->data->custom_tags.data = NULL;
    }

    ALOGD("base url: %s\n", baseUrl);
    ALOGD("yt params: %s\n", ytParams);
    ALOGD("yt prefix: %s\n", ytPrefix);

    int count = 0;
    while(seg && seg->data) {
        ALOGD("Segment %d Uri: %s\n", seg->data->sequence_num, seg->data->uri);
        ++count;
        seg = seg->next;
    }

    const int YT_P_SIZE = 8;
    const char *yt_p_arr[YT_P_SIZE];
    memset(yt_p_arr, 0, sizeof(yt_p_arr));
    const int yt_p_n = split_yt_str_by_delim((char *) ytParams, ",", yt_p_arr);

    char * lineBuf = malloc(line_len*(count+1));
    memset(lineBuf, 0, line_len*(count+1));

    count = 0;
    seg = firstSeg;

    while(seg && seg->data) {
        char * oldUri = seg->data->uri;
        seg->data->uri = NULL;

        char * newUri = lineBuf+(line_len*count);

        const char *yt_u_arr[YT_P_SIZE];
        memset(yt_u_arr, 0, sizeof(yt_u_arr));
        const int yt_u_n = split_yt_str_by_delim(oldUri, "/", yt_u_arr);
        
        format_yt_uri_with_params(newUri, baseUrl, yt_p_arr, yt_p_n, yt_u_arr, yt_u_n);
        seg->data->uri = newUri;

        ALOGD("Segment %d Uri: %s\n", seg->data->sequence_num, seg->data->uri);
        seg = seg->next;
        free(oldUri);
        ++count;
    }

    int outputBufLen = 0;
    char * outputBuf = NULL;

    myPlaylist.uri = "\0";
    HLSCode res = hlswrite_media(&outputBuf, &outputBufLen, &myPlaylist);
    if(res != HLS_OK) {
        ALOGW("failed to write the m3u8\n");
    } else {
        ALOGW("M3U8 output length: %d\n", outputBufLen);
    }

    ALOGD("Dumping playlist:\n%s\n", outputBuf);
    if (outputBuf) free(outputBuf);

    free(m3u8);
    free(regexBuf);
    free(lineBuf);

    // hlsparse_media_playlist_term(&myPlaylist);
}

static int test_regex() {
    const char * source = "___ abc123def ___ ghi456 ___";
    const char * regexString = "[a-z]*([0-9]+)([a-z]*)";
    size_t maxGroups = 3;

    char outputBuf[1024];
    const char* results[maxGroups];
    return regex_match(source, regexString, 3, outputBuf, results);
}


int main() {
    test_master_m3u8_update();

    // test_playlist_m3u8_update();

    // test_regex();

    return 0;
}
