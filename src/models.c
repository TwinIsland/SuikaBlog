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

void free_postInfo(PostInfo *postInfo)
{
    if (postInfo != NULL)
    {
        free(postInfo->Title);
        free(postInfo->Banner);
        free(postInfo->Excerpts);

        postInfo->Title = NULL;
        postInfo->Banner = NULL;
        postInfo->Excerpts = NULL;
    }
}

void free_indexData(IndexData *indexData)
{
    if (indexData != NULL)
    {
        free_postInfo(&indexData->CoverArticleInfo);

        for (int i = 0; i < INDEX_DATA_POST_N; ++i)
            free_postInfo(&indexData->NormalArticleInfos[i]);

        free(indexData->Notice.content);
        free(indexData->Notice.title);

        for (int i = 0; i < indexData->Archieves.size; ++i)
        {
            free(indexData->Archieves.data[i]);
            indexData->Archieves.data[i] = NULL;
        }

        free(indexData->Archieves.data);

        for (int i = 0; i < indexData->Tags.size; ++i)
        {
            free(indexData->Tags.data[i]);
            indexData->Tags.data[i] = NULL;
        }
        free(indexData->Tags.data);
    }
}