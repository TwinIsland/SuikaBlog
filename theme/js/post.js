

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
        ${articleJSON.CreateDate} • ${articleJSON.Views} Views
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

fetchDataWithCache('/api/post/' + getPostIdFromUri(), `post/${getPostIdFromUri()}`)
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
        registerLikeButton(getPostIdFromUri());
        updateTOC("#article-body", "#toc-body");

        document.getElementById("article-body").innerHTML += `
        <div id="copyright">
            <h3 style="margin-top: 0; font-weight: bold;">CC BY-ND</h3>      
            <p style="margin: 0; font-weight: normal; /* Regular font weight */">This license enables reusers to copy and distribute the material in any medium or format in unadapted form only, and only so long as attribution is given to the creator. The license allows for commercial use.</p>
            <a href="https://creativecommons.org/licenses/by-nd/4.0/" target="_blank">
                <img src="/img/by-nd.webp" width="117" height="41" style="margin-left: 0; margin-top: 20px;">
            </a>  
        </div>
        `

        loadJsr('https://cdn.bootcdn.net/ajax/libs/KaTeX/0.16.9/katex.min.js', function () {
            loadJsr('https://cdn.bootcdn.net/ajax/libs/KaTeX/0.16.9/contrib/auto-render.min.js', function () {
                try {
                    renderMathInElement(document.getElementById("article-body"), {
                        delimiters: [
                            { left: '$$', right: '$$', display: true },
                            { left: '$', right: '$', display: true },
                        ],
                        throwOnError: false
                    });
                } catch (err) {
                    console.error(err)
                }
            })
        })
    })
    .catch(error => {
        console.error(error)
        console.log(error.code)
        if (error.code && error.msg) {
            navigateTo(`/err?code=${error.code}&msg=${encodeURIComponent(error.msg)}`);
        } else {
            navigateTo("/err");
        }
    });


// Cleanup function
window.currentCleanup = function () {
    document.removeEventListener('scroll', textScrollHandler);
    return "/post listeners";
}

