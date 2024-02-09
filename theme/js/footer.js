import { getBlogInfo } from './blog.js';
import { generateColor } from './tagColorPicker.js';
import { wrapArticleUrl } from './post.js';


document.addEventListener("DOMContentLoaded", function() {
    // render blog name and title
    document.querySelector('.blog-footer-logo').textContent = getBlogInfo("blog_name");

    // the render ended, set back the scroll bar and hide the loading page
    // min 0.3s delay to make animation smoother 
    setTimeout(function() {
        const loadingOverlay = document.getElementById('loadingOverlay');
        const loadingText = document.querySelector('.loadingText');
        const innerBar = document.querySelector('.innerBar');
        
        loadingOverlay.style.opacity = '0';
        innerBar.style.opacity = '0'
        loadingText.style.opacity = '0'
        document.body.style.overflow = 'auto'

        loadingOverlay.addEventListener('transitionend', function() {
            loadingOverlay.style.display = 'none';
        });
    }, 300);

    // coloring tags
    const tags = document.querySelectorAll('.tag');
    tags.forEach(tag => {
        const color = generateColor(tag.textContent.trim());
        tag.style.backgroundColor = color;
    });
});

// bind links
document.querySelector('.article_box').addEventListener('click', function(event) {
    // Check if the clicked element or its parent is a .blog-post
    let blogPost = event.target.closest('.blog-post');

    if (!blogPost) return;

    const articleId = blogPost.getAttribute('article-id');
    page.redirect('/article/' + articleId);
  });


// search
document.getElementById('tag-input').addEventListener('keyup', function(event) {
    // Check if the Enter key was pressed
    if (event.key === 'Enter' && this.value.trim() !== "" && this.value.trim().startsWith('#')) {
        const tagList = document.querySelector('.tag-list');
        
        // Create the tag element
        const tagElem = document.createElement('span');
        tagElem.className = 'badge bg-secondary me-2';
        tagElem.textContent = this.value.trim().slice(1);
        
        // Add a close button to the tag
        const closeButton = document.createElement('button');
        closeButton.className = 'btn-close ms-2';
        closeButton.type = 'button';
        closeButton.addEventListener('click', function() {
            tagList.removeChild(tagElem);
        });
        tagElem.appendChild(closeButton);
        
        // Append the tag to the tag list
        tagList.appendChild(tagElem);
        
        // Clear the input
        this.value = "";
    } else if (event.key === 'Enter') {
        // Start search
        const tagList = document.querySelector('.tag-list');
        let query = {
            "tags": [],
            "query": this.value
        }
        while (tagList.firstChild) {
            query.tags.push(tagList.firstChild.textContent.trim());
            tagList.removeChild(tagList.firstChild);
        } 

        this.value = "";
        console.log(query);
    }
});