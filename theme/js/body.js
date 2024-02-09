import { getBlogInfo } from './blog.js';
import { getVisitorInfo } from './visitor.js';
import { fetchArticles, wrapArticleUrl } from './post.js';
import { displayMsgBox, Color } from './utils.js'
import {
    MSG_BOX_DISPLAY_TIME, MSG_BOX_ERRMSG_DISPLAY_TIME, LOADING_ERR_MSG, LOADING_TIMEOUT,
    NO_MORE_POST_MSG, INIT_RENDER_POST, RENDER_POST_PER_TIME, SOCIAL_MEDIA_BILIBILI,
    SOCIAL_MEDIA_GITHUB, SOCIAL_MEDIA_EMAIL_TRIM
} from './config.js'

document.addEventListener("DOMContentLoaded", function () {
    // render featured post
    const featuredPostContent = getBlogInfo('featured_post');

    const featuredPostBody = document.getElementById('featured-post');
    if (featuredPostContent.cover_img !== "") {
        featuredPostBody.style.backgroundImage = `url('${featuredPostContent.cover_img}')`;
    }
    
    document.getElementById('featured-post-title').textContent = featuredPostContent.title;
    document.getElementById('featured-post-content').textContent = featuredPostContent.leadContent;
    
    const postLinkElement = document.getElementById('featured-post-link');
    postLinkElement.href = wrapArticleUrl(featuredPostContent.id);
    
    // render visitor info
    const visitorInfo = getVisitorInfo();
    document.getElementById('userInfo').textContent = visitorInfo['name']

    // render initial articles 
    document.querySelector('.skeleton-container-2').style.display = 'block';
    const fetchPromise = fetchArticles(0, INIT_RENDER_POST);
    const timeoutPromise = new Promise((_, reject) => {
        setTimeout(() => reject(new Error(LOADING_ERR_MSG)), LOADING_TIMEOUT);
    });

    Promise.race([fetchPromise, timeoutPromise])
        .then(newArticles => {
            // Once data is fetched, hide skeleton
            document.querySelector('.skeleton-container-2').style.display = 'none';
            newArticles.forEach(article => {
                renderArticle(article);
            });
        })
        .catch(error => {
            displayMsgBox(error.message, MSG_BOX_ERRMSG_DISPLAY_TIME, Color.RED);
        });

    // render about
    document.getElementById('about-body').textContent = getBlogInfo("about")

    // render tag group
    const tagGroup = document.getElementById('tag-group');
    const tagTemplate = document.getElementById('tagTemplate').content;
    const tags = getBlogInfo("tags");

    tags.forEach(tag => {
        const tagClone = document.importNode(tagTemplate, true);
        tagClone.querySelector('button').innerText = tag;
        tagGroup.appendChild(tagClone);
    });

    // render trend post
    const trendPostsContainer = document.getElementById('trendPostsContainer');
    const postTemplate = document.getElementById('postTemplate').content;
    const trendPostsData = getBlogInfo('trend_post');

    trendPostsData.forEach(post => {
        const postClone = document.importNode(postTemplate, true);

        postClone.querySelector('.post-title').innerText = post.title;
        postClone.querySelector('.post-date').innerText = post.meta.date;
        postClone.querySelector('.post-description').innerText = post.description;

        const postLinkElement = postClone.getElementById('trend-link');
        postLinkElement.href = wrapArticleUrl(post.id);

        if (post.meta.cover_img && post.meta.cover_img.trim() !== "") {
            postClone.querySelector('.post-image').src = post.meta.cover_img;
        }

        // Append the populated clone to the container
        trendPostsContainer.appendChild(postClone);
    });


    // render the archives
    const archiveContainer = document.querySelector('#archiveContainer');
    const archiveItemTemplate = document.getElementById('archiveItemTemplate').content;
    const archives = getBlogInfo("archives");

    archives.forEach(archive => {
        const archiveItemClone = document.importNode(archiveItemTemplate, true);

        const archiveLink = archiveItemClone.querySelector('a');
        archiveLink.innerText = archive.k;
        archiveLink.href = archive.link;

        archiveContainer.appendChild(archiveItemClone);
    });

    // render social medias
    document.getElementById("github-link").href = SOCIAL_MEDIA_GITHUB
    document.getElementById("email-p1").textContent = SOCIAL_MEDIA_EMAIL_TRIM.split(' ')[0]
    document.getElementById("email-p2").textContent = SOCIAL_MEDIA_EMAIL_TRIM.split(' ')[1]
    document.getElementById("email-p3").textContent = SOCIAL_MEDIA_EMAIL_TRIM.split(' ')[2]
    document.getElementById("bilibili-link").href = SOCIAL_MEDIA_BILIBILI
});

function renderArticle(article) {
    const articleTemplate = document.getElementById('articleTemplate').content;
    const articleClone = document.importNode(articleTemplate, true);

    articleClone.querySelector('.blog-post-title').innerText = article.title;
    articleClone.querySelector('.blog-post-meta > span:first-child').innerText = article.meta.date;
    articleClone.querySelector('.blog-post-meta > span.text-warning').innerText = `  ${article.meta.word_count}`;
    articleClone.querySelector('.blog-post-content').innerText = article.description;

    const articleElement = articleClone.querySelector('article');
    articleElement.setAttribute('article-id', article.id);

    if (article.meta.cover_img !== "") {
        articleElement.style.backgroundImage = `linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('${article.meta.cover_img}')`;
    }

    document.querySelector('.article_box').appendChild(articleClone);
}

let canLoadMore = true;     // flag indicate if left post to be loaded
let isLoading = false       // mutex to ensure next loading happend after cur loading finish
let renderedPostsCount = INIT_RENDER_POST;

function loadMoreContent() {
    if (!canLoadMore) {
        displayMsgBox(NO_MORE_POST_MSG, MSG_BOX_DISPLAY_TIME);
        return;
    };

    if (isLoading) return
    isLoading = true

    // Show the skeleton screen
    document.querySelector('.skeleton-container').style.display = 'block';

    const fetchPromise = fetchArticles(renderedPostsCount, renderedPostsCount + RENDER_POST_PER_TIME);

    const timeoutPromise = new Promise((_, reject) => {
        setTimeout(() => reject(new Error(LOADING_ERR_MSG)), LOADING_TIMEOUT);
    });

    Promise.race([fetchPromise, timeoutPromise])
        .then(newArticles => {
            // Once data is fetched, hide skeleton
            document.querySelector('.skeleton-container').style.display = 'none';

            // nothing to be loaded
            if (newArticles.length === 0) {
                canLoadMore = false;
                displayMsgBox(NO_MORE_POST_MSG, MSG_BOX_DISPLAY_TIME);
            } else {
                // Render the new articles
                newArticles.forEach(article => {
                    renderArticle(article);
                });

                renderedPostsCount += newArticles.length;
            }

            isLoading = false
        })
        .catch(error => {
            document.querySelector('.skeleton-container').style.display = 'none';
            displayMsgBox(error.message, MSG_BOX_ERRMSG_DISPLAY_TIME, Color.RED);
            isLoading = false
        });
}

window.addEventListener('scroll', function () {
    // Check if user has scrolled to the bottom of the page
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight) {
        loadMoreContent();
    }
});

document.getElementById('subscribeForm').addEventListener('submit', function (e) {
    e.preventDefault(); // Prevent default form submission.

    const emailInput = document.querySelector('#email-address');

    if (emailInput.value && emailInput.validity.valid) {
        // TODO: subscribe email trigger
        console.log('Email is valid and ready to be submitted: ', emailInput.value);
    }
});

