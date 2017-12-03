/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */
#ifndef _HLSPARSE_H
#define _HLSPARSE_H

#include <stdlib.h>
#include <stdint.h>

// HLSCode values
#define HLS_OK      (0)
#define HLS_ERROR   (1)

// bool_t values
#define HLS_TRUE    (1)
#define HLS_FALSE   (0)

// HLS tags
#define EXTM3U                      "EXTM3U"
#define EXTXVERSION                "EXT-X-VERSION"
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
#define DATAID                      "DATA-ID"
#define VALUE                       "VALUE"
#define TIMEOFFSET                  "TIME-OFFSET"
#define PRECISE                     "PRECISE"
#define VOD                         "VOD"
#define EVENT                       "EVENT"

// HLS tag enums
#define KEY_METHOD_NONE             0
#define KEY_METHOD_AES128           1
#define KEY_METHOD_SAMPLEAES        2
#define KEY_METHOD_INVALID          3

#define MEDIA_TYPE_NONE             0
#define MEDIA_TYPE_VIDEO            1
#define MEDIA_TYPE_AUDIO            2
#define MEDIA_TYPE_SUBTITLES        3
#define MEDIA_TYPE_CLOSEDCAPTIONS   4
#define MEDIA_TYPE_INVALID          5

#define MEDIA_INSTREAMID_NONE       0
#define MEDIA_INSTREAMID_CC1        1
#define MEDIA_INSTREAMID_CC2        2
#define MEDIA_INSTREAMID_CC3        3
#define MEDIA_INSTREAMID_CC4        4
#define MEDIA_INSTREAMID_SERVICE    5
#define MEDIA_INSTREAMID_INVALID    6

#define PLAYLIST_TYPE_VOD           0
#define PLAYLIST_TYPE_EVENT         1
#define PLAYLIST_TYPE_INVALID       2

#define PARAM_TYPE_NONE             0
#define PARAM_TYPE_DATA             1
#define PARAM_TYPE_STRING           2
#define PARAM_TYPE_FLOAT            3

#ifdef __cplusplus
extern "C" {
#endif

typedef int         HLSCode;
typedef int         bool_t;
typedef uint64_t    timestamp_t;

typedef void* (*hlsparse_malloc_callback)(size_t);
typedef void (*hlsparse_free_callback)(void *);

typedef struct string_list {
    char *data;
    struct string_list *next;
} string_list_t;

typedef int param_type_t;

typedef struct param_list {
    char *key;                  // name of the a parameter entry.
    union {
        char *data;             // string or byte data value.
        float number;           // floating point number value.
    } value;
    param_type_t value_type;    // type of content stored in value.
    struct param_list *next;    // next item in linked list
} param_list_t;

typedef struct {
    int n, o;
} byte_range_t;

typedef struct {
    float duration;
    char *title;
} ext_inf_t;

typedef struct {
    int width;
    int height;
} resolution_t;

typedef struct {
    int method;
    char *uri;
    char *iv;
    char *key_format;
    char *key_format_versions;
} hls_key_t;

typedef struct {
    int program_id;
    float bandwidth;
    float avg_bandwidth;
    float frame_rate;
    char *codecs;
    char *video;
    char *uri;
    resolution_t resolution;
} iframe_stream_inf_t;

typedef struct {
    int program_id;
    float bandwidth;
    float avg_bandwidth;
    float frame_rate;
    char *codecs;
    char *video;
    char *audio;
    char *uri;
    char *subtitles;
    char *closed_captions;
    resolution_t resolution;
} stream_inf_t;

typedef struct {
    char *uri;
    byte_range_t byte_range;
} map_t;

typedef struct {
    char *id;
    char *klass;
    timestamp_t start_date;
    timestamp_t end_date;
    float duration;
    float planned_duration;
    param_list_t client_attributes;
    char *scte35_cmd;
    char *scte35_out;
    char *scte35_in;
    bool_t end_on_next;
} daterange_t;

typedef struct {
    int type;
    int instream_id;
    int service_n;
    bool_t forced;
    bool_t is_default;
    bool_t auto_select;
    char *name;
    char *group_id;
    char *language;
    char *assoc_language;
    char *uri;
    char *characteristics;
} media_t;

typedef struct {
    int sequence_num;
    int key_index;
    int map_index;
    float duration;
    char *title;
    char *uri;
    bool_t discontinuity;
    bool_t pdt_discontinuity;
    timestamp_t pdt;
    timestamp_t pdt_end;
    byte_range_t byte_range;
    string_list_t custom_tags;
} segment_t;

typedef struct {
    char *data_id;
    char *value;
    char *uri;
    char *language;
} session_data_t;

typedef struct {
    float time_offset;
    bool_t precise;
} start_t;

typedef struct segment_list {
    segment_t                       *data;
    struct segment_list             *next;
} segment_list_t;

typedef struct session_data_list {
    session_data_t                  *data;
    struct session_data_list        *next;
} session_data_list_t;

typedef struct key_list {
    hls_key_t                       *data;
    struct key_list                 *next;
} key_list_t;

typedef struct stream_inf_list {
    stream_inf_t                    *data;
    struct stream_inf_list          *next;
} stream_inf_list_t;

typedef struct iframe_stream_inf_list {
    iframe_stream_inf_t *data;
    struct iframe_stream_inf_list   *next;
} iframe_stream_inf_list_t;

typedef struct media_list {
    media_t                         *data;
    struct media_list               *next;
} media_list_t;

typedef struct map_list {
    map_t                           *data;
    struct map_list                 *next;
} map_list_t;

typedef struct daterange_list {
    daterange_t                     *data;
    struct daterange_list           *next;
} daterange_list_t;

typedef struct {
    int                         version;
    char                        *uri;
    bool_t                      m3u;
    bool_t                      independent_segments;
    start_t                     start;
    session_data_list_t         session_data;
    media_list_t                media;
    stream_inf_list_t           stream_infs;
    iframe_stream_inf_list_t    iframe_stream_infs; 
    string_list_t               custom_tags;
} master_t;

typedef struct {
    int                         version;
    int                         media_sequence;
    int                         next_segment_media_sequence;
    int                         nb_segments;
    int                         nb_keys;
    int                         nb_maps;
    int                         nb_dateranges;
    int                         nb_custom_tags;
    int                         playlist_type;
    int                         discontinuity_sequence;
    float                       target_duration;
    float                       duration;
    char                        *uri;
    bool_t                      m3u;
    bool_t                      independent_segments;
    bool_t                      allow_cache;
    bool_t                      iframes_only;
    bool_t                      next_segment_discontinuity;
    bool_t                      end_list;
    bool_t                      discontinuity;
    timestamp_t                 next_segment_pdt;
    start_t                     start;
    byte_range_t                next_segment_byterange;
    segment_list_t              segments;
    key_list_t                  keys;
    map_list_t                  maps;
    daterange_list_t            dateranges;
    string_list_t               custom_tags;
    segment_t                   *last_segment;                  
} media_playlist_t;

HLSCode hlsparse_global_init(void);
HLSCode hlsparse_global_init_mem(hlsparse_malloc_callback m, hlsparse_free_callback f);

/**
 * Initializes a master_t object
 *
 * @param dest The object to initialize
 */
HLSCode hlsparse_master_init(master_t *dest);

/**
 * Initializes a media_playlist_t object
 *
 * @param dest The object to initialize
 */
HLSCode hlsparse_media_playlist_init(media_playlist_t *dest);

/**
 * Cleans up a master_t object freeing any resources.
 * The master_t object itself is not destroyed in the process.
 * Call parse_master_init to reinitialize the master_t after if has been destroyed
 *
 * @param dest The master_t object to destroy
 */
HLSCode hlsparse_master_term(master_t *dest);

/**
 * Cleans up a media_playlist_t object freeing any resources.
 * The media_playlist_t object itself is not destroyed in the process.
 * Call parse_media_playlist_init to reinitialize the meida_playlisy_t after it has
 * been destroyed
 *
 * @param dest The media_playlist_t object to destroy
 */
HLSCode hlsparse_media_playlist_term(media_playlist_t *dest);

/**
 * parses an HLS string of data into a media_playlist_t struct
 *
 * @param src The raw string of data that represents an HLS master playlist
 * @param size The length of src
 * @param dest The master_t to parse the source text into
 */
int hlsparse_master(const char *src, size_t size, master_t *dest);

/**
 * parses an HLS string of data into a media_playlist_t struct
 *
 * @param src The raw string of data that represents an HLS media playlist
 * @param size The length of src
 * @param dest The media_playlist_t to parse the source text into
 */
int hlsparse_media_playlist(const char *src, size_t size, media_playlist_t *dest);

#ifdef __cplusplus
}
#endif

#endif // _HLSPARSE_H