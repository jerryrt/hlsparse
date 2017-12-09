/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#ifndef _PARSER_TAGS_H
#define _PARSER_TAGS_H

#include "hlsparse.h"

// HLS tags
#define EXTM3U                      "EXTM3U"
#define EXTXVERSION                 "EXT-X-VERSION"
#define EXTINF                      "EXTINF"
#define EXTXBYTERANGE               "EXT-X-BYTERANGE"
#define EXTXDISCONTINUITY           "EXT-X-DISCONTINUITY"
#define EXTXKEY                     "EXT-X-KEY"
#define EXTXMAP                     "EXT-X-MAP"
#define EXTXDATERANGE               "EXT-X-DATERANGE"
#define EXTXPROGRAMDATETIME         "EXT-X-PROGRAM-DATE-TIME"
#define EXTXTARGETDURATION          "EXT-X-TARGETDURATION"
#define EXTXMEDIASEQUENCE           "EXT-X-MEDIA-SEQUENCE"
#define EXTXDISCONTINUTITYSEQUENCE  "EXT-X-DISCONTINUITY-SEQUENCE"
#define EXTXENDLIST                 "EXT-X-ENDLIST"
#define EXTXPLAYLISTTYPE            "EXT-X-PLAYLIST-TYPE"
#define EXTXIFRAMESONLY             "EXT-X-I-FRAMES-ONLY"
#define EXTXMEDIA                   "EXT-X-MEDIA"
#define EXTXSTREAMINF               "EXT-X-STREAM-INF"
#define EXTXIFRAMESTREAMINF         "EXT-X-I-FRAME-STREAM-INF"
#define EXTXSESSIONDATA             "EXT-X-SESSION-DATA"
#define EXTXSESSIONKEY              "EXT-X-SESSION-KEY"
#define EXTXINDEPENDENTSEGMENTS     "EXT-X-INDEPENDENT-SEGMENTS"
#define EXTXSTART                   "EXT-X-START"
#define EXTXALLOWCACHE              "EXT-X-ALLOW-CACHE"
#define EXTXDISCONTINUITYSEQ        "EXT-X-DISCONTINUITY-SEQUENCE"

// HLS tag properties
#define PROGRAMID                   "PROGRAM-ID"
#define BANDWIDTH                   "BANDWIDTH"
#define AVERAGEBANDWIDTH            "AVERAGE-BANDWIDTH"
#define CODECS                      "CODECS"
#define RESOLUTION                  "RESOLUTION"
#define VIDEO                       "VIDEO"
#define URI                         "URI"
#define FRAMERATE                   "FRAME-RATE"
#define HDCPLEVEL                   "HDCP-LEVEL"
#define AUDIO                       "AUDIO"
#define SUBTITLES                   "SUBTITLES"
#define CLOSEDCAPTIONS              "CLOSED-CAPTIONS"
#define METHOD                      "METHOD"
#define KEY_IV                      "IV"
#define KEYFORMAT                   "KEYFORMAT"
#define KEYFORMATVERSIONS           "KEYFORMATVERSIONS"
#define BYTERANGE                   "BYTERANGE"
#define ID                          "ID"
#define CLASS                       "CLASS"
#define STARTDATE                   "START-DATE"
#define ENDDATE                     "END-DATE"
#define DURATION                    "DURATION"
#define PLANNEDDURATION             "PLANNED-DURATION"
#define XCLIENT                     "X-"
#define SCTE35CMD                   "SCTE35-CMD"
#define SCTE35OUT                   "SCTE35-OUT"
#define SCTE35IN                    "SCTE35-IN"
#define ENDONNEXT                   "END-ON-NEXT"
#define TYPE                        "TYPE"
#define GROUPID                     "GROUP-ID"
#define NAME                        "NAME"
#define AUTOSELECT                  "AUTOSELECT"
#define FORCED                      "FORCED"
#define DEFAULT                     "DEFAULT"
#define YES                         "YES"
#define NO                          "NO"
#define LANGUAGE                    "LANGUAGE"
#define ASSOCLANGUAGE               "ASSOC-LANGUAGE"
#define INSTREAMID                  "INSTREAM-ID"
#define CC1                         "CC1"
#define CC2                         "CC2"
#define CC3                         "CC3"
#define CC4                         "CC4"
#define SERVICE                     "SERVICE"
#define CHARACTERISTICS             "CHARACTERISTICS"
#define CHANNELS                    "CHANNELS"
#define DATAID                      "DATA-ID"
#define VALUE                       "VALUE"
#define TIMEOFFSET                  "TIME-OFFSET"
#define PRECISE                     "PRECISE"
#define VOD                         "VOD"
#define EVENT                       "EVENT"
#define TYPE0                       "TYPE-0"
#define NONE                        "NONE"
#define AES128                      "AES-128"
#define SAMPLEAES                   "SAMPLE-AES"

// compares a char* with a string literal HLS tag increasing the ptr by the length
// of the tag if successful
#define EQUAL(a,b)  (0 == strncmp((a), (b), sizeof(b) - 1) && (a += sizeof(b) - 1))

#ifdef __cplusplus
extern "C" {
#endif

// Memory Allocator
extern hlsparse_malloc_callback hls_malloc;
extern hlsparse_free_callback hls_free;

// Utils
char *str_utils_dup(const char *str);
char *str_utils_ndup(const char *str, size_t size);
char *str_utils_append(char *str, const char *append);
char *str_utils_nappend(char *str, const char *append, size_t size);
char *str_utils_join(const char *str, const char *join);
char *str_utils_njoin(const char *str, const char *join, size_t size);
char *path_combine(char **dest, const char *base, const char *path);

// Tag parsing
int parse_line_to_str(const char *src, char **dest, size_t size);
int parse_str_to_int(const char *src, int *dest, size_t size);
int parse_str_to_float(const char *str, float *dest, size_t size);
int parse_date(const char *src, uint64_t *dest, size_t size);
int parse_attrib_str(const char *src, char **dest, size_t size);
int parse_attrib_data(const char *src, char **dest, size_t size);
int parse_line_to_str(const char *src, char **dest, size_t size);
int parse_str_to_int(const char *src, int *dest, size_t size);
int parse_str_to_float(const char *str, float *dest, size_t size);
int parse_date(const char *src, uint64_t *dest, size_t size);
int parse_attrib_str(const char *src, char **dest, size_t size);
int parse_attrib_data(const char *src, char **dest, size_t size);
int parse_master_tag(const char *src, size_t size, master_t *dest); 
int parse_media_playlist_tag(const char *src, size_t size, media_playlist_t *dest);
void parse_byte_range_init(byte_range_t *byte_range);
void parse_ext_inf_init(ext_inf_t *ext_inf);
void parse_resolution_init(resolution_t *resolution);
void parse_iframe_stream_inf_init(iframe_stream_inf_t *stream_inf);
void parse_stream_inf_init(stream_inf_t *stream_inf);
void parse_key_init(hls_key_t *key);
void parse_map_init(map_t *map);
void parse_daterange_init(daterange_t *daterange);
void parse_media_init(media_t *media);
void parse_segment_init(segment_t *segment);
void parse_session_data_init(session_data_t *session_data);
void parse_start_init(start_t *start);
void parse_segment_list_init(segment_list_t *list);
void parse_session_data_list_init(session_data_list_t *list);
void parse_key_list_init(key_list_t *list);
void parse_stream_inf_list_init(stream_inf_list_t *list);
void parse_iframe_stream_inf_list_init(iframe_stream_inf_list_t *list);
void parse_media_list_init(media_list_t *list);
void parse_map_list_init(map_list_t *list);
void parse_string_list_init(string_list_t *list);
void parse_daterange_list_init(daterange_list_t *list);
void parse_param_list_init(param_list_t *list);
void parse_param_term(char ***params, int size);
void parse_ext_inf_term(ext_inf_t *ext_inf);
void parse_iframe_stream_inf_term(iframe_stream_inf_t *stream_inf);
void parse_stream_inf_term(stream_inf_t *stream_inf);
void parse_key_term(hls_key_t *key);
void parse_map_term(map_t *map);
void parse_daterange_term(daterange_t *daterange);
void parse_media_term(media_t *media);
void parse_segment_term(segment_t *segment);
void parse_session_data_term(session_data_t *session_data);
void parse_segment_list_term(segment_list_t *list);
void parse_session_data_list_term(session_data_list_t *list);
void parse_key_list_term(key_list_t *list);
void parse_media_list_term(media_list_t *list);
void parse_map_list_term(map_list_t *list);
void parse_daterange_list_term(daterange_list_t *list);
void parse_iframe_stream_inf_list_term(iframe_stream_inf_list_t *list);
void parse_stream_inf_list_term(stream_inf_list_t *list);
void parse_string_list_term(string_list_t *list);
void parse_param_list_term(param_list_t *list);
int parse_byte_range(const char *src, size_t size, byte_range_t *dest);
int parse_iframe_stream_inf(const char *src, size_t size, iframe_stream_inf_t *dest);
int parse_iframe_stream_inf_tag(const char *src, size_t size, iframe_stream_inf_t *dest);
int parse_stream_inf(const char *src, size_t size, stream_inf_t *dest);
int parse_stream_inf_tag(const char *src, size_t size, stream_inf_t *dest);
int parse_resolution(const char *src, size_t size, resolution_t *dest);
int parse_key(const char *src, size_t size, hls_key_t *key);
int parse_key_tag(const char *src, size_t size, hls_key_t *key);
int parse_map(const char *src, size_t size, map_t *map);
int parse_daterange_tag(const char *src, size_t size, daterange_t *daterange);
int parse_daterange(const char *src, size_t size, daterange_t *daterange);
int parse_map_tag(const char *src, size_t size, map_t *map);
int parse_media(const char *src, size_t size, media_t *media);
int parse_media_tag(const char *src, size_t size, media_t *media);
int parse_segment(const char *src, size_t size, segment_t *segment);
int parse_segment_tag(const char *src, size_t size, segment_t *segment);
int parse_session_data(const char *src, size_t size, session_data_t *session_data);
int parse_session_data_tag(const char *src, size_t size,session_data_t *session_data);
int parse_start(const char *src, size_t size, start_t *start);

#ifdef __cplusplus
}
#endif

#endif // _PARSER_TAGS_H
