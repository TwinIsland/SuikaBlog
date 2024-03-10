#include <stdlib.h>

#include "models.h"


void free_post(Post *post)
{
    if (post != NULL)
    {
        free(post->Title);
        free(post->Banner);
        free(post->Excerpts);
        free(post->Content);

        post->Title = NULL;
        post->Banner = NULL;
        post->Excerpts = NULL;
        post->Content = NULL;
    }
}