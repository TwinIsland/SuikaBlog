/*
    The module to rendering the index page
    block when loading html

*/

function formatTimestamp(timestamp) {
  // Create a Date object from the timestamp
  const date = new Date(timestamp);

  // Get the month, day, and year from the Date object
  const month = date.toLocaleString('default', { month: 'short' }).toUpperCase();
  const day = ("0" + date.getDate()).slice(-2); // Ensure the day is two digits
  const year = date.getFullYear();

  // Combine the components into the desired format
  return `${month} ${day}, ${year}`;
}

function openUrlInNewTab(url) {
  window.open(url);
}

function renderWrapper(elementDOM, newHTML) {
  setTimeout(() => {
    elementDOM.classList.add('fade-in');
    setTimeout(() => {
      elementDOM.innerHTML = newHTML;
      elementDOM.classList.add('visible');
    }, 230);
  }, 150);
}

function renderCoverArticle(coverArticleJSON) {
  return `
  <a href="/post/${coverArticleJSON.postID}" style="text-decoration: none; color: inherit;">
    <h1 class="inline-text" style="padding-top: 20px; min-height: 154px">${coverArticleJSON.title}</h1>
    <p style="font-size: 20px; height: 92px; overflow: hidden">${coverArticleJSON.excerpt}</p>
  </a>
    `
}

function renderTrendArticles(trendArticlesJSON) {
  return trendArticlesJSON.map(trendArticleJSON => `
    <div>
      <div class="trend-article-post">
          <strong style="color: #bf1932;">Trend</strong>
          <h3 class="inline-text" style="padding-bottom: 0px; padding-top: 7px;">${trendArticleJSON.title}</h3>
          <p style="color: rgba(33, 37, 41, 0.75); margin-top: 0px;">${formatTimestamp(trendArticleJSON.date)}</p>
          <p style="margin-bottom: 35px;">${trendArticleJSON.excerpt}</p>
          <a href="/post/${trendArticleJSON.postID}">Continue Reading...</a>
      </div>
    </div>
  `).join('');
}

function renderNormalArticle(normalArticlesJSON) {
  return normalArticlesJSON.map(normalArticleJSON => `
  <a href="/post/${normalArticleJSON.postID}" style="text-decoration: none; color: inherit;">
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

function renderAbout(aboutJSON) {
  return `
    <header>${aboutJSON.title}</header>
    <p>${aboutJSON.content}</p>
  `
}

function renderTags(tagsJSON) {
  return tagsJSON.map(tagJSON => `
    <button onclick=openUrlInNewTab("/tag/${tagJSON.tagId}")>${tagJSON.tagName}</button>
  `).join('')
}

function renderArchives(archivesJSON) {
  return archivesJSON.map(archiveJSON => `
    <li>
      <a href="/archives/${archiveJSON.archivesId}">${archiveJSON.name}</a>
    </li>  
  `).join('')
}

AbortSignal.timeout ??= function timeout(ms) {
  const ctrl = new AbortController()
  setTimeout(() => ctrl.abort(), ms)
  return ctrl.signal
}

fetch('http://localhost:3000/index', { signal: AbortSignal.timeout(5000) })
  .then(response => {
    if (!response.ok) {
      throw new Error('Network response was not ok');
    }
    return response.json();
  })
  .then(data => {
    let elementDOM = null;

    // rendering cover article
    elementDOM = document.getElementById('cover-article-container');
    renderWrapper(elementDOM, renderCoverArticle(data['cover-article']))

    // rendering trend articles
    elementDOM = document.getElementById('trend-article-container');
    renderWrapper(elementDOM, renderTrendArticles(data['trend-article']));

    // rendering normal articles
    elementDOM = document.getElementById('normal-article-container');
    renderWrapper(elementDOM, renderNormalArticle(data['normal-article']));

    // rendering about sidebar
    elementDOM = document.getElementById('about-container');
    renderWrapper(elementDOM, renderAbout(data['about']));

    // rendering tags sidebar
    elementDOM = document.getElementById('tags-container');
    renderWrapper(elementDOM, renderTags(data['tags']));

    // rendering archives sidebar
    elementDOM = document.getElementById('archives-container');
    renderWrapper(elementDOM, renderArchives(data['archives']));
  })
  .catch(error => {
    console.error('There was a problem with your fetch operation:', error);
    elementDOM = document.getElementById('cover-article-container');
    renderWrapper(elementDOM, "<h1 >⚠エラー発生!⚠</h1><h2>Oppps, Something Bad Happens, Please Contact Admin for help (ᗜ˰ᗜ)</h2>")
  });
