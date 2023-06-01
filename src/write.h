/*
 * Copyright 2017 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#ifndef _WRITE_H
#define _WRITE_H

#include <stdarg.h>

typedef struct page {
    char *buffer;
    int size;
    char *cur;
    struct page *next;
} page_t;

page_t* create_page(page_t *page);
void free_page_root(page_t *root);

page_t* write_to_page(page_t *page, const char *buffer, int size);
page_t* pgprintf(page_t *page, const char *format, ...);
void page_to_str(page_t *page, char **dest, int *dest_size);
const char* find_relative_path(const char *path, const char *base);

#endif // _WRITE_G