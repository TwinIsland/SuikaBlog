import { getBlogInfo } from './blog.js';
import { OVERLAY_LOADING_TEXT } from './config.js'


document.addEventListener("DOMContentLoaded", function() {

    // before rendering header, hide the scrool bar to support loading page
    document.querySelector(".loadingText").textContent = OVERLAY_LOADING_TEXT
    document.body.style.overflow = 'hidden'

    // render blog name and title
    document.title = getBlogInfo("blog_title");


    document.querySelector('.blog-header-logo').textContent = getBlogInfo("blog_name");

    // render navLinks
    const navLinks = getBlogInfo("navLinks");
    const navElement = document.getElementById('navContainer');

    navLinks.forEach(link => {
        const aTag = document.createElement('a');
        aTag.className = 'p-2 link-secondary';
        aTag.href = link.link;
        aTag.textContent = link.text;
        navElement.appendChild(aTag);
    });
});
