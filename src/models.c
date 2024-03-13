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
    if (postInfo == NULL)
        return;

    free(postInfo->Title);
    free(postInfo->Banner);
    free(postInfo->Excerpts);

    postInfo->Title = NULL;
    postInfo->Banner = NULL;
    postInfo->Excerpts = NULL;
}

void free_tags(Tags *tags)
{
    if (tags == NULL)
        return;
    for (int i = 0; i < tags->size; ++i)
    {
        free(tags->data[i]);
        tags->data[i] = NULL;
    }
    free(tags->data);
}

void free_archieves(Archieves *archieves)
{
    if (archieves == NULL)
        return;
    for (int i = 0; i < archieves->size; ++i)
    {
        for (int j = 0; j < archieves->data[i].articleCount; ++j)
        {
            free_postInfo(&archieves->data[i].posts[j]);
        }
        free(archieves->data[i].posts);
    }
    free(archieves->data);
    archieves->data = NULL;
}

void free_indexData(IndexData *indexData)
{
    if (indexData == NULL)
        return;

    free_postInfo(&indexData->CoverArticleInfo);

    for (int i = 0; i < INDEX_DATA_POST_N; ++i)
        free_postInfo(&indexData->NormalArticleInfos[i]);

    free(indexData->Notice.content);
    free(indexData->Notice.title);

    free_tags(&indexData->Tags);
    free_archieves(&indexData->Archieves);
}