/*
    The module to rendering the index page
    block when loading html

*/



function renderCoverArticle(coverArticleJSON) {
  return `
  <a href="/post/${coverArticleJSON.PostID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <h1 class="inline-text">${coverArticleJSON.Title}</h1>
    <p>${coverArticleJSON.Excerpts}</p>
  </a>
    `
}

function renderNormalArticle(normalArticlesJSON, coverArticalJSON) {
  let ret = coverArticalJSON && coverArticalJSON.PostID != -1 ? `
  <div class="full none-800">
  <a href="/post/${coverArticalJSON.PostID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <div class="card normal-article" style="background-image: linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('${coverArticalJSON.Banner === "" ? '/img/banner.webp' : '/img/banner.webp'}')";>
        <h2 class="inline-text" style="padding: 10px 0 10px 0;">${coverArticalJSON.Title}</h2>
        <div style="margin-bottom: 45px;">
            <span>${formatTimestamp(coverArticalJSON.CreateDate)}</span>
            <span style="color: rgb(255,193,7);">${coverArticalJSON.Views} Views</span>
        </div>
        <div>
            ${coverArticalJSON.Excerpts}
        </div>
    </div>
  </a>
  </div>
  ` : ""

  normalArticlesJSON.map(normalArticleJSON => ret += `
  <a href="/post/${normalArticleJSON.PostID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <div class="card normal-article" style="background-image: linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('${normalArticleJSON.Banner === "" ? '/img/banner.webp' : '/img/banner.webp'}')";>
        <h2 class="inline-text" style="padding: 10px 0 10px 0;">${normalArticleJSON.Title}</h2>
        <div style="margin-bottom: 45px;">
            <span>${formatTimestamp(normalArticleJSON.CreateDate)}</span>
            <span style="color: rgb(255,193,7);">${normalArticleJSON.Views} Views</span>
        </div>
        <div>
            ${normalArticleJSON.Excerpts}
        </div>
    </div>
  </a>
  `)
  return ret;
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

// END RENDERING HELPERS
fetchDataWithCache('/api/index', "index", true)
  .then(data => {
    const renderPromises = [
      renderWrapper(document.getElementById('cover-article-container'), renderCoverArticle(data['cover_article'])),
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
