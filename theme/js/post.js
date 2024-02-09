function request_article(from, to) {
    // Sample data for demonstration purposes

    const PostMetas = [
        {
            id: 1,
            title: "CS412: Note for Midterm1",
            meta: {
                date: "APR 05, 2023",
                word_count: "662 Words",
                cover_img: "https://api.cirno.me/anipic?v=1"
            },
            description: "Lorem ipsum aasdad",
        },
        {
            id: 4,
            title: "CS412: Lecture 2",
            meta: {
                date: "APR 05, 2023",
                word_count: "662 Words",
                cover_img: "https://api.cirno.me/anipic?v=2"
            },
            description: "Lorem ipsum aasdad",
        },
        {
            id: 8,
            title: "Finger - 贝斯点弦练习",
            meta: {
                date: "APR 05, 2023",
                word_count: "662 Words",
                cover_img: "https://api.cirno.me/anipic?v=3"
            },
            description: "Lorem ipsum aasdad",
        },
        {
            id: 41,
            title: "一种简易的API加密方法",
            meta: {
                date: "APR 05, 2023",
                word_count: "662 Words",
                cover_img: "https://api.cirno.me/anipic?v=4"
            },
            description: "Lorem ipsum aasdad",
        },
        {
            id: 42,
            title: "Typecho Mysql转Sqlite",
            meta: {
                date: "APR 05, 2023",
                word_count: "662 Words",
                cover_img: "https://api.cirno.me/anipic?v=5"
            },
            description: "Lorem ipsum aasdad",
        },
        {
            id: 43,
            title: "深入理解计算机网络",
            meta: {
                date: "APR 06, 2023",
                word_count: "715 Words",
                cover_img: "https://api.cirno.me/anipic?v=6"
            },
            description: "Lorem ipsum dolor sit amet",
        },
        {
            id: 18,
            title: "前端开发的最佳实践",
            meta: {
                date: "APR 07, 2023",
                word_count: "689 Words",
                cover_img: ""
            },
            description: "Lorem ipsum consectetuer adipiscing",
        },
        {
            id: 19,
            title: "后端框架对比与分析",
            meta: {
                date: "APR 08, 2023",
                word_count: "653 Words",
                cover_img: "https://api.cirno.me/anipic?v=6"
            },
            description: "Lorem ipsum elit sed diam",
        },
        {
            id: 21,
            title: "最后的生还者",
            meta: {
                date: "APR 09, 2023",
                word_count: "678 Words",
                cover_img: ""
            },
            description: "Lorem ipsum nonummy nibh euismod",
        }
    ]

    return PostMetas.slice(from, to);
}


export function fetchArticles(start, end) {
    return new Promise((resolve, reject) => {
        // Simulate fetching the post metadata, for example
        setTimeout(() => {
            resolve(request_article(start, end));
        }, 500); // for example, simulating variable fetch times
    });
}


export function getArticle(article_id) {
    const content = "<p>后知后觉为什么是叫“第0章”。 本以为是用来介绍角色的，谁知只是用来介绍大标题上的概念，并引出卡纳依区这座“雾雨谜宫”的。 合着请来那么豪华的声优阵容真就只是走个过场而已。毕竟动用如此多老牌声优来配ADV还是太贵了（笑）</p> <p>This is some additional paragraph placeholder content. It has been written to fill the available space and show how a longer snippet of text affects the surrounding content. We'll repeat it often to keep the demonstration flowing, so be on the lookout for this exact same string of text.</p> <blockquote> <p>Longer quote goes here, maybe with some <strong>emphasized text</strong> in the middle of it.</p> </blockquote> <p>This is some additional paragraph placeholder content. It has been written to fill the available space and show how a longer snippet of text affects the surrounding content. We'll repeat it often to keep the demonstration flowing, so be on the lookout for this exact same string of text.</p> <img src=\"assets/imgs/test.webp\" alt=\"\"> <h3>Example table</h3> <p>And don't forget about tables in these posts:</p> <table class=\"table\"> <thead> <tr> <th>Name</th> <th>Upvotes</th> <th>Downvotes</th> </tr> </thead> <tbody> <tr> <td>Alice</td> <td>10</td> <td>11</td> </tr> <tr> <td>Bob</td> <td>4</td> <td>3</td> </tr> <tr> <td>Charlie</td> <td>7</td> <td>9</td> </tr> </tbody> <tfoot> <tr> <td>Totals</td> <td>21</td> <td>23</td> </tr> </tfoot> </table> <p>This is some additional paragraph placeholder content. It's a slightly shorter version of the other highly repetitive body text used throughout.</p> <p>Lorem ipsum dolor, sit amet consectetur adipisicing elit. Natus, quaerat? Ab officiis sequi qui vel mollitia repudiandae dolorum unde nemo explicabo, esse perferendis eius a dignissimos? Harum veritatis quisquam quaerat. Quasi at incidunt quisquam porro eum sunt, saepe soluta, dignissimos ut fugiat nemo distinctio assumenda! Ipsum quos enim aliquid debitis. Perferendis corrupti officiis minima soluta quisquam fugiat atque, dolor qui? Omnis obcaecati adipisci corrupti voluptates similique illo itaque excepturi velit dolores nostrum recusandae numquam odio, sequi laborum iste. Cupiditate ipsum rem quas omnis ullam minima atque consequuntur iure exercitationem perspiciatis!</p>"

    return {
        id: article_id,
        title: `文章 ${article_id} 是也`,
        meta: {
            date: "APR 09, 2023",
            modified: "APR 10, 2023",
            word_count: "678 Words",
            cover_img: "",
            comment_count: 13,
        },
        categories: ["Technology", "Health", "Travel"],
        tags: ["AI", "Blockchain", "Diet"],
        content: content
    }
}

export function wrapArticleUrl(article_id) {
    return "/article/" + article_id;
}