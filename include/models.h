#ifndef MODELS_H
#define MODELS_H

#include <stdint.h>
#include <time.h>

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

void free_post(Post *post);

#endif // MODELS_H