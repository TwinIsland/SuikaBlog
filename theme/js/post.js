

document.addEventListener('scroll', textScrollHandler);

new QRCode(document.getElementById("page-qr"), {
    text: window.location.href,
    width: 128,
    height: 128,
    colorDark: "#B98A82",
    correctLevel: QRCode.CorrectLevel.H
});

function getPostIdFromUri() {
    const path = window.location.pathname;
    const idPattern = /\/post\/(\d+)$/;
    const match = path.match(idPattern);
    return match ? match[1] : null;
};

function renderArticleTitle(articleSON) {
    document.querySelector(".scroll-text-decoration").textContent = articleSON.Title;
    return `
        ${articleSON.Title}
    `
}

function renderArticleMeta(articleJSON) {
    return `
        ${formatTimestamp(articleJSON.CreateDate)} • ${articleJSON.Views} Views
    `
}

function renderArticleBody(articleJSON) {
    return `
        ${converter.makeHtml(articleJSON.Content)}
    `
}

function renderLikeCount(articleJSON) {
    return `${articleJSON.UpVoted}`
}

fetchDataWithCache('/api/post/' + getPostIdFromUri(), `post/${getPostIdFromUri()}`, 1)
    .then(data => {
        const renderPromises = [
            renderWrapper(document.getElementById("article-title"), renderArticleTitle(data)),
            renderWrapper(document.getElementById("article-meta"), renderArticleMeta(data)),
            renderWrapper(document.getElementById("article-body"), renderArticleBody(data)),
            renderWrapper(document.getElementById("likeCount"), renderLikeCount(data))
        ];

            return Promise.all(renderPromises);
        })
    .then(() => {
        Prism.highlightAll();
        registerLikeButton();
        return updateTOC(".article", "#toc-body");
    })
    .catch(error => {
        console.error('There was a problem with your fetch operation:', error);
        navigateTo("/err");
    });


// Cleanup function
window.currentCleanup = function () {
    document.removeEventListener('scroll', textScrollHandler);
    return "/post listeners";
}

