#ifndef MODELS_H
#define MODELS_H

#include <stdint.h>
#include <time.h>

#define INDEX_DATA_POST_N 5 // Number of Post shown on index page

typedef struct
{
    int32_t PostID;
    char *Title;
    char *Banner;
    char *Excerpts;
    char *Content;
    int32_t IsPage;
    time_t CreateDate;
    time_t DateModified;
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
    time_t CreateDate;
    time_t DateModified;
    int32_t UpVoted;
    int32_t Views;
} PostInfo ;

typedef struct {
    char *title;
    char *content;
} Notice;

typedef char* Archieve;
typedef char* Tag;

typedef struct {
    Tag *data;
    size_t size;
} Tags;

typedef struct
{
    Archieve *data;
    size_t size;
} Archieves;


typedef struct
{
    PostInfo CoverArticleInfo;
    Notice Notice;
    Tags Tags;
    Archieves Archieves;
    PostInfo NormalArticleInfos[INDEX_DATA_POST_N]; 
} IndexData;

void free_post(Post *post);
void free_postInfo(PostInfo *postInfo);
void free_indexData(IndexData *indexData);

#endif // MODELS_H