#include <stdlib.h>
#include "models.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

void free_post(Post *post)
{
    if (post != NULL)
    {
        if (post->Title != NULL)
        {
            free(post->Title);
            post->Title = NULL;
        }
        if (post->Banner != NULL)
        {
            free(post->Banner);
            post->Banner = NULL;
        }
        if (post->Excerpts != NULL)
        {
            free(post->Excerpts);
            post->Excerpts = NULL;
        }
        if (post->Content != NULL)
        {
            free(post->Content);
            post->Content = NULL;
        }
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

void ViewsMap_Destroyer(void *cache)
{
    hmfree(((ViewsCache *)cache)->viewsMap);
    free(((ViewsCache *)cache)->json_str);
    free((ViewsCache *)cache);
}

void free_views(Views *views)
{
    if (views == NULL)
        return;

    free(views->data);
}

void free_archieves(Archieves *archieves)
{
    for (size_t i = 0; i < archieves->size; ++i)
    {
        for (size_t j = 0; j < archieves->data[i].articleCount; ++j)
        {
            free(archieves->data[i].ArticleTitles[j]);
        }
        free(archieves->data[i].ArticleTitles);
    }
    free(archieves->data);
}

void free_PostInfos(PostInfos *post_infos)
{
    for (int i = 0; i < post_infos->size; ++i)
        free_postInfo(&post_infos->data[i]);
    free(post_infos->data);
}

void free_indexData(IndexData *indexData)
{
    if (indexData == NULL)
        return;

    free_PostInfos(&indexData->SelectedArticleInfos);
    free_PostInfos(&indexData->NormalArticleInfos);

    free(indexData->Notice.content);
    free(indexData->Notice.title);

    free_tags(&indexData->Tags);
    free_archieves(&indexData->Archieves);
}

void free_visitor(Visitor *visitor)
{
    free(visitor->email);
    free(visitor->name);
    free(visitor->website);
}

char *format_time(time_t t, char *timestamp_buf)
{
    printf("Raw time_t value: %ld\n", t);

    struct tm *tm_info;
    tm_info = localtime(&t);
    strftime(timestamp_buf, 20, "%Y-%m-%d", tm_info);
    return timestamp_buf;
}

char *notice_to_json(Notice *notice)
{
    char *json = mg_mprintf("{%m: %m, %m: %m}",
                            MG_ESC("title"), MG_ESC(notice->title),
                            MG_ESC("content"), MG_ESC(notice->content));

    return json;
}

char *tags_to_json(Tags *tags)
{
    if (tags->size == 0)
    {
        return strdup("[]");
    }

    size_t buffer_size = 256;
    char *json_str = malloc(buffer_size);

    size_t index = 0;
    json_str[index++] = '[';
    json_str[index++] = '"';

    for (size_t i = 0; i < tags->size; ++i)
    {
        size_t tag_len = strlen(tags->data[i]);
        size_t needed_capacity = index + tag_len + 4; // Include space for comma, double quotes, or closing bracket

        if (needed_capacity > buffer_size)
        {
            while (needed_capacity > buffer_size)
            {
                buffer_size *= 2; // Double the buffer size to accommodate
            }
            char *new_json_str = realloc(json_str, buffer_size);

            json_str = new_json_str;
        }

        if (i > 0)
        {
            json_str[index++] = ',';
            json_str[index++] = '"';
        }
        memcpy(json_str + index, tags->data[i], tag_len);
        index += tag_len;
        json_str[index++] = '"';
    }

    json_str[index++] = ']';
    json_str[index] = '\0';

    return json_str;
}

char *views_to_json(Views *views, ViewsCache *cache)
{
    if (views->size == 0)
    {
        return strdup("{}");
    }

    char *json_str = malloc(views->size * 64);
    json_str[0] = '{';

    size_t index = 1;
    cache->viewsMap = NULL;

    for (size_t i = 0; i < views->size; ++i)
    {
        if (i > 0)
        {
            json_str[index++] = ',';
        }

        index += snprintf(json_str + index, 64, "\"%d\":", views->data[i].PostID);
        hmput(cache->viewsMap, views->data[i].PostID, index);

        char view_count_str[VIEW_COUNT_ALIGN + 1];
        int view_count_length = snprintf(view_count_str, sizeof(view_count_str), "%d", views->data[i].Views);

        if (view_count_length < VIEW_COUNT_ALIGN)
        {
            int padding = VIEW_COUNT_ALIGN - view_count_length;
            memmove(view_count_str + padding, view_count_str, view_count_length + 1);
            memset(view_count_str, ' ', padding);
        }

        index += snprintf(json_str + index, VIEW_COUNT_ALIGN + 1, "%s", view_count_str);
    }

    json_str[index++] = '}';
    json_str[index] = '\0';

    return json_str;
}

char *post_to_json(Post *post)
{
    char *json = mg_mprintf("{%m: %d, %m: %m, %m: %m, %m: %m, %m: %m, %m: %d, %m: %m, %m: %m, %m: %d, %m: %d}",
                            MG_ESC("PostID"), post->PostID,
                            MG_ESC("Title"), MG_ESC(post->Title),
                            MG_ESC("Banner"), MG_ESC(post->Banner),
                            MG_ESC("Excerpts"), MG_ESC(post->Excerpts),
                            MG_ESC("Content"), MG_ESC(post->Content),
                            MG_ESC("IsPage"), post->IsPage,
                            MG_ESC("CreateDate"), MG_ESC(post->CreateDate),
                            MG_ESC("DateModified"), MG_ESC(post->DateModified),
                            MG_ESC("UpVoted"), post->UpVoted,
                            MG_ESC("Views"), post->Views);

    return json;
}

char *postInfo_to_json(PostInfo *post_info)
{

    char *json = mg_mprintf("{%m: %d, %m: %m, %m: %m, %m: %m, %m: %d, %m: %m, %m: %m, %m: %d, %m: %d}",
                            MG_ESC("PostID"), post_info->PostID,
                            MG_ESC("Title"), MG_ESC(post_info->Title),
                            MG_ESC("Banner"), MG_ESC(post_info->Banner),
                            MG_ESC("Excerpts"), MG_ESC(post_info->Excerpts),
                            MG_ESC("IsPage"), post_info->IsPage,
                            MG_ESC("CreateDate"), MG_ESC(post_info->CreateDate),
                            MG_ESC("DateModified"), MG_ESC(post_info->DateModified),
                            MG_ESC("UpVoted"), post_info->UpVoted,
                            MG_ESC("Views"), post_info->Views);

    return json;
}

char *archieves_to_json(Archieves *archieves)
{
    if (archieves->size == 0)
    {
        return strdup("[]");
    }

    size_t buffer_size = 1024;
    char *json = malloc(buffer_size);

    size_t index = 0;
    index += sprintf(json, "[");

    for (size_t i = 0; i < archieves->size; ++i)
    {
        Archieve *archive = &archieves->data[i];

        index += mg_snprintf(json + index, buffer_size - index, "{%m: %d, %m: %d, %m: [",
                             MG_ESC("year"), archive->year,
                             MG_ESC("articleCount"), archive->articleCount,
                             MG_ESC("ArticleTitles"));

        for (size_t j = 0; j < archive->articleCount; ++j)
        {
            index += mg_snprintf(json + index, buffer_size - index, "%s\"%s\"", j == 0 ? "" : ", ", archive->ArticleTitles[j]);

            // Reallocate buffer if necessary
            if (buffer_size - index < 512)
            {
                buffer_size *= 2;
                char *new_json = realloc(json, buffer_size);
                if (!new_json)
                {
                    free(json);
                    return NULL;
                }
                json = new_json;
            }
        }

        index += mg_snprintf(json + index, buffer_size - index, "]}%s", i < archieves->size - 1 ? ", " : "");
    }

    index += mg_snprintf(json + index, buffer_size - index, "]");
    return json;
}

char *postInfos_to_json(PostInfos *post_infos)
{
    size_t buffer_size = 512; // Initial buffer size
    char *json = malloc(buffer_size);
    if (!json)
    {
        return NULL;
    }

    size_t index = mg_snprintf(json, buffer_size, "[");

    for (size_t i = 0; i < post_infos->size; ++i)
    {
        char *post_info_str = postInfo_to_json(&post_infos->data[i]);
        size_t needed_size = strlen(post_info_str) + 10; // Additional space for commas and potential padding

        if (buffer_size - index < needed_size)
        {
            buffer_size += needed_size + 512;
            char *new_json = realloc(json, buffer_size);
            if (!new_json)
            {
                free(json);
                free(post_info_str);
                return NULL;
            }
            json = new_json;
        }
        index += mg_snprintf(json + index, buffer_size - index, "%s%s",
                             post_info_str, i < post_infos->size - 1 ? ", " : "");

        free(post_info_str);
    }
    mg_snprintf(json + index, buffer_size - index, "]");

    return json;
}

char *indexData_to_json(IndexData *index_data)
{
    // Convert components to JSON strings
    char *tags_str = tags_to_json(&index_data->Tags);
    char *selectedArticleInfos_str = postInfos_to_json(&index_data->SelectedArticleInfos);
    char *archieves_str = archieves_to_json(&index_data->Archieves);
    char *notice_str = notice_to_json(&index_data->Notice);
    char *normalArticleInfos_str = postInfos_to_json(&index_data->NormalArticleInfos);

    if (!tags_str || !selectedArticleInfos_str || !archieves_str || !notice_str || !normalArticleInfos_str)
    {
        free(tags_str);
        free(selectedArticleInfos_str);
        free(archieves_str);
        free(notice_str);
        free(normalArticleInfos_str);
        return NULL;
    }

    // Calculate initial buffer size
    size_t buffer_size = strlen(tags_str) + strlen(selectedArticleInfos_str) +
                         strlen(archieves_str) + strlen(notice_str) +
                         strlen(normalArticleInfos_str) + 512; // extra space for JSON formatting

    char *json = malloc(buffer_size);

    // Compose the final JSON string
    mg_snprintf(json, buffer_size, "{%m: %s, %m: %s, %m: %s, %m: %s, %m: %s}",
                MG_ESC("cover_article"), selectedArticleInfos_str,
                MG_ESC("tags"), tags_str,
                MG_ESC("notice"), notice_str,
                MG_ESC("archives"), archieves_str,
                MG_ESC("normal_article"), normalArticleInfos_str);

    // Free intermediate JSON strings
    free(tags_str);
    free(selectedArticleInfos_str);
    free(archieves_str);
    free(notice_str);
    free(normalArticleInfos_str);

    return json;
}

char *visitor_to_json(Visitor *visitor)
{
    char *json = mg_mprintf("{%m: %m, %m: %m, %m: %m, %m: %d",
                            MG_ESC("name"), MG_ESC(visitor->name),
                            MG_ESC("email"), MG_ESC(visitor->email),
                            MG_ESC("website"), MG_ESC(visitor->website),
                            MG_ESC("banned"), MG_ESC(visitor->banned));

    return json;
}