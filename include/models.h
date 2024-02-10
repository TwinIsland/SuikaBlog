#ifndef MODELS_H
#define MODELS_H

#include <stdint.h>
#include <time.h>

typedef struct
{
    int32_t PostID;
    char Title[256];
    char *Excerpts;
    char *Content;
    time_t DatePublished;
    time_t DateModified;
    int32_t UpVoted;
    int32_t Views;
} Post;

typedef struct {
    int32_t VisitorID;
    char *Name;
    char *Email; 
    char *Website; 
    char IP[46]; // Maximum length for IPv6 addresses including null terminator
} Visitor;

typedef struct {
    int32_t CommentID;
    int32_t PostID;
    int32_t VisitorID;
    char *Content;
    int32_t UpVoted;
    int32_t DownVoted;
} Comment;

typedef struct {
    int32_t ActivityID;
    int32_t VisitorID; 
    char *Type;
    time_t CreateDate; 
} Activity;

typedef struct {
    int32_t TagID;
    char *TagName; 
} Tag;

typedef struct {
    int32_t CategoryID;
    char *CategoryName; 
} Category;

typedef struct {
    int32_t PostID;
    int32_t TagID;
} PostTag;

typedef struct {
    int32_t PostID;
    int32_t CategoryID;
} PostCategory;


void destroy_post(Post *post);

#endif // MODELS_H