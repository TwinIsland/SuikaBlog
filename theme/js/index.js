/*
    The module to rendering the index page
    block when loading html

*/

function renderCoverArticle(coverArticleJSON) {
  return `
  <a href="/post/${coverArticleJSON.postID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <h1 class="inline-text" style="padding-top: 20px; min-height: 154px">${coverArticleJSON.title}</h1>
    <p style="font-size: 20px; height: 92px; overflow: hidden">${coverArticleJSON.excerpt}</p>
  </a>
    `
}

function renderNormalArticle(normalArticlesJSON) {
  return normalArticlesJSON.map(normalArticleJSON => `
  <a href="/post/${normalArticleJSON.postID}" onclick="route()" style="text-decoration: none; color: inherit;">
    <div class="card normal-article" style="background-image: linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('${normalArticleJSON.coverIMG}')";>
        <h2 class="inline-text" style="padding: 10px 0 10px 0;">${normalArticleJSON.title}</h2>
        <div style="margin-bottom: 45px;">
            <span>${formatTimestamp(normalArticleJSON.date)}</span>
            <span style="color: rgb(255,193,7);">${normalArticleJSON.wordCount} Words</span>
        </div>
        <div>
            ${normalArticleJSON.excerpt}
        </div>
    </div>
  </a>
  `).join('');
}

function renderNotice(noticeJSON) {
  return `
    <header>${noticeJSON.title}</header>
    <p>${noticeJSON.content}</p>
  `
}

function renderTags(tagsJSON) {
  return tagsJSON.map(tagJSON => `
    <button href="/tag/${tagJSON.uri}" onclick="route()">${tagJSON.tagName}</button>
  `).join('')
}

function renderArchives(archivesJSON) {
  return archivesJSON.map(archiveJSON => `
    <li>
      <a href="/archives/${archiveJSON.uri}" onclick="route()">${archiveJSON.name}</a>
    </li>  
  `).join('')
}

// END RENDERING HELPERS
fetchDataWithCache('http://localhost:3000/index', "index")
  .then(data => {
    const renderPromises = [
      renderWrapper(document.getElementById('cover-article-container'), renderCoverArticle(data['cover-article'])),
      renderWrapper(document.getElementById('normal-article-container'), renderNormalArticle(data['normal-article'])),
      renderWrapper(document.getElementById('notice-container'), renderNotice(data['notice'])),
      renderWrapper(document.getElementById('tags-container'), renderTags(data['tags'])),
      renderWrapper(document.getElementById('archives-container'), renderArchives(data['archives']))
    ];

    return Promise.all(renderPromises)
  })
  .catch(error => {
    console.error('There was a problem with your fetch operation:', error);
    navigateTo("/err");
  });
