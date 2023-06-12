//
//  Linklist.h
//  CList
//
//  Created by xuzhe on 2020/6/9.
//

#ifndef Linklist_h
#define Linklist_h

#include "kingcstd.h"

typedef struct _linknode
{
    struct _linknode *_next;
    void *_data;
}linknode;

typedef struct _linkList
{
    linknode *_phead;
    int _count;
}linkList;

linkList *create_linklist(void);
int destory_linklist(linkList *plist);
int linklist_is_empty(linkList *plist);
int linklist_size(linkList *plist);
void *linklist_get(linkList *plist, int index);
void *linklist_get_first(linkList *plist);
void *linklist_get_last(linkList *plist);
int linklist_insert(linkList *plist, int index, void *pdata);
int linklist_insert_first(linkList *plist, void *pdata);
int linklist_append_last(linkList *plist, void *pdata);
int linklist_delete(linkList *plist, int index);
int linklist_delete_first(linkList *plist);
int linklist_delete_last(linkList *plist);
void print_linklist(linkList *plist);

#endif /* Linklist_h */
