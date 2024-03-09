#include <stdlib.h>

#include "models.h"


void destroy_post(Post *post)
{
    if (post != NULL)
    {
        if (post->Content != NULL)
        {
            free(post->Content);
            post->Content = NULL;
        }
    }
}