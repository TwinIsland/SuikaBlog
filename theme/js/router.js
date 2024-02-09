page('/about', about);
page('/article/:id', loadArticle);
page.start();

function about() {
  console.log("about page")
}

function loadArticle(ctx) {
  const articleId = ctx.params.id;
  console.log("article with id: " + articleId)
}
