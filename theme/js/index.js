/*
    The module to rendering the index page
    block when loading html

*/



function renderCoverArticle(coverArticleJSON, normalArticlesJSON) {
  const coverArticle = coverArticleJSON.length ? coverArticleJSON[0] : normalArticlesJSON[0];
  if (!coverArticle) return "";

  if (coverArticleJSON.length == 0) {
    updateSelectBlock("select-block-0", coverArticle);
  } else {
    if (coverArticleJSON.length >= 1) {
      updateSelectBlock("select-block-0", coverArticleJSON[0]);
    }
    if (coverArticleJSON.length >= 2) {
      updateSelectBlock("select-block-1", coverArticleJSON[1]);
    }
    if (coverArticleJSON.length >= 3) {
      updateSelectBlock("select-block-2", coverArticleJSON[2]);
    }
  }

  return `
  <a href="/post/${coverArticle.PostID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <h1 class="inline-text">${coverArticle.Title}</h1>
    <span>${coverArticle.CreateDate}</span>
    <span style="color: rgb(255,193,7);">${coverArticle.Views} Views</span>
    <p>${coverArticle.Excerpts}</p>
  </a>
  `;
}


function renderNormalArticle(normalArticlesJSON) {
  return normalArticlesJSON.map(normalArticleJSON => `
  <a href="/post/${normalArticleJSON.PostID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <div class="card normal-article" style="background-image: linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('${normalArticleJSON.Banner === "" ? '/img/banner.webp' : normalArticleJSON.Banner}')";>
        <h2 class="inline-text" style="padding: 10px 0 10px 0;">${normalArticleJSON.Title}</h2>
        <div style="margin-bottom: 45px;">
            <span>${normalArticleJSON.CreateDate}</span>
            <span style="color: rgb(255,193,7);">${normalArticleJSON.Views} Views</span>
        </div>
        <div>
            ${normalArticleJSON.Excerpts}
        </div>
    </div>
  </a>
  `).join('')
}

function renderNotice(noticeJSON) {
  return `
    <header>${noticeJSON.title}</header>
    <p>${noticeJSON.content}</p>
  `
}

function renderTags(tagsJSON) {
  return tagsJSON.map(tagJSON => `
    <button href="/tag/${tagJSON.uri}" onclick="route()">${tagJSON}</button>
  `).join('')
}

function renderArchives(archivesJSON) {
  return archivesJSON.map(archiveJSON => `
    <li>
      <a href="/archives/${archiveJSON.year}" onclick="route()">${archiveJSON.year}的文章：${archiveJSON.articleCount}</a>
    </li>  
  `).join('')
}

var selected_block_n = 0

function focusOnBlock(index) {
  if (selected_block_n == index) return;

  const newBlock = document.getElementById(`select-block-${index}`);
  const oldBlock = document.getElementById(`select-block-${selected_block_n}`);
  const coverArticleContainer = document.getElementById("cover-article-container");

  if (newBlock) {
    newBlock.classList.add("highlight");
    if (oldBlock) {
      oldBlock.classList.remove("highlight");
    }

    const postID = newBlock.getAttribute("data-postid");
    const title = newBlock.getAttribute("data-title");
    const createDate = newBlock.getAttribute("data-createdate");
    const views = newBlock.getAttribute("data-views");
    const excerpts = newBlock.getAttribute("data-excerpts");

    if (coverArticleContainer) {
      coverArticleContainer.innerHTML = `
        <a href="/post/${postID}" class="float-up" onclick="route()" style="text-decoration: none; color: inherit;">
          <h1 class="inline-text">${title}</h1>
          <span>${createDate}</span>
          <span style="color: rgb(255,193,7);">${views} Views</span>
          <p>${excerpts}</p>
        </a>
      `;
    }

    selected_block_n = index;
  }
}

var blockSwitchIntervalId = setInterval(() => {
  focusOnBlock((selected_block_n + 1) % 3);
}, 5000);


// END RENDERING HELPERS
Promise.all([
  fetchDataWithCache('/api/index', "index", true),
  fetchDataWithCache('/api/views', "views", false)
])
  .then(([data, views]) => {
    // Merge views data into the articles data
    data.cover_article.forEach(article => {
      article.Views = views[article.PostID] || article.Views;
    });
    data.normal_article.forEach(article => {
      article.Views = views[article.PostID] || article.Views;
    });

    const renderPromises = [
      renderWrapper(document.getElementById('cover-article-container'), renderCoverArticle(data['cover_article'], data['normal_article'])),
      renderWrapper(document.getElementById('normal-article-container'), renderNormalArticle(data['normal_article'], data['cover_article'])),
      renderWrapper(document.getElementById('notice-container'), renderNotice(data['notice'])),
      renderWrapper(document.getElementById('tags-container'), renderTags(data['tags'])),
      renderWrapper(document.getElementById('archives-container'), renderArchives(data['archives']))
    ];

    postOffset = data.normal_article.length + 1;
    return Promise.all(renderPromises)
  })
  .then(() => {
    putLoadMoreBtn()
  })
.catch(error => {
  if (error.code && error.msg) {
    navigateTo(`/err?code=${error.code}&msg=${encodeURIComponent(error.msg)}`);
  } else {
    navigateTo("/err");
  }
  throw error
});

// Cleanup function
window.currentCleanup = function () {
  clearInterval(blockSwitchIntervalId);
  return "/index listeners";
}