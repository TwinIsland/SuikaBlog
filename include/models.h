#pragma once

#include <stdint.h>
#include <time.h>
#include <mongoose.h>

#include "config.h"

#define SELECT_ARTICLE_N 3

typedef struct
{
    int32_t PostID;
    char *Title;
    char *Banner;
    char *Excerpts;
    char *Content;
    int32_t IsPage;
    char CreateDate[20];
    char DateModified[20];
    int32_t UpVoted;
    int32_t Views;
} Post;

typedef struct
{
    int32_t PostID;
    char *Title;
    char *Banner;
    char *Excerpts;
    int32_t IsPage;
    char CreateDate[20];
    char DateModified[20];
    int32_t UpVoted;
    int32_t Views;
} PostInfo;

typedef struct
{
    int32_t PostID;
    int32_t Views;
} View;

typedef struct
{
    View *data;
    size_t size;
} Views;

typedef struct
{
    PostInfo *data;
    size_t size;
} PostInfos;

typedef struct
{
    char *title;
    char *content;
} Notice;

typedef struct
{
    int year;
    size_t articleCount;
    char **ArticleTitles;
} Archieve;

typedef struct
{
    Archieve *data;
    size_t size;
} Archieves;

typedef char *Tag;

typedef struct
{
    Tag *data;
    size_t size;
} Tags;

typedef struct
{
    PostInfos SelectedArticleInfos;
    Notice Notice;
    Tags Tags;
    Archieves Archieves;
    PostInfos NormalArticleInfos;
} IndexData;

typedef struct
{
    int32_t CommentID;
    int32_t PostID;
    char *AuthorName;
    char *CreateDate;
    char *Content;
    int32_t UpVoted;
} Comment;

void free_post(Post *post);
void free_postInfo(PostInfo *postInfo);
void free_tags(Tags *tags);
void free_views(Views *views);
void free_archieves(Archieves *archieves);
void free_PostInfos(PostInfos *post_infos);
void free_indexData(IndexData *indexData);

char *notice_to_json(Notice *notice);
char *tags_to_json(Tags *tags);
char *post_to_json(Post *post);
char *views_to_json(Views *views);
char *postInfo_to_json(PostInfo *post_info);
char *archieves_to_json(Archieves *archieves);
char *postInfos_to_json(PostInfos *post_infos);
char *indexData_to_json(IndexData *index_data);
