export function getBlogInfo(key = null) {
    // Sample data for demonstration purposes
    let blogInfo = JSON.parse(sessionStorage.getItem('blogInfo'));

    if (!blogInfo) {
        blogInfo = {
            blog_name: "贰岛",
            blog_title: "贰岛博客",
            about: "Lorem ipsum dolor sit amet consectetur adipisicing elit. Impedit dolorem, reprehenderit quisquam esse nam facere sunt ipsam soluta accusantium incidunt, neque molestias necessitatibus non laborum unde rem ea voluptates minima!",
            tags: ["Technology", "AI", "Programming", "Web Development", "Philosophy", "Education", "Research", "Innovation", "Design", "Tutorials"],

            featured_post: {
                id: 22,
                title: "CS361: 期中 Review",
                leadContent: "Lorem ipsum dolor sit amet consectetur adipisicing elit. Doloribus sunt qui reprehenderit nihil possimus molestias iusto illo fugiat, excepturi aspernatur quo unde nulla libero maxime soluta eaque debitis voluptatem itaque.",
                cover_img: ""
            },

            trend_post: [
                {
                    id: 88,
                    title: "云计算的未来趋势",
                    meta: {
                        date: "APR 11, 2023",
                        word_count: "690 Words",
                        cover_img: "https://api.cirno.me/anipic?v=6"
                    },
                    description: "Lorem ipsum adipiscing elit, sed diam",
                },
                {
                    id: 91,
                    title: "深度学习在医疗领域的应用",
                    meta: {
                        date: "APR 12, 2023",
                        word_count: "712 Words",
                        cover_img: ""
                    },
                    description: "Lorem ipsum tincidunt ut laoreet",
                }            
            ],

            archives: [
                { k: "January 2021", link: "./jan2021" },
                { k: "February 2021", link: "./feb2021" },
                { k: "March 2021", link: "./mar2021" },
                { k: "April 2021", link: "./apr2021" },
                { k: "May 2021", link: "./may2021" },

            ],

            navLinks: [
                { text: "首页", link: "#" },
                { text: "笔记", link: "#" },
                { text: "追番", link: "#" },
                { text: "关于", link: "/about" },
                { text: "相册", link: "#" },
                { text: "存档", link: "#" },
                { text: "友链", link: "#" },
                { text: "实验室", link: "#" },
            ],

            about: "贰岛主题 V1.0 正式发布啦 Erdao Theme V1.0 officially released!!!"
        };

        // set local storage is first initialized
        sessionStorage.setItem('blogInfo', JSON.stringify(blogInfo));
    }
    if (key == null) return blogInfo;
    return blogInfo[key];
}

