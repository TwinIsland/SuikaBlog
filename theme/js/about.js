function TableOfContents(container, output) {
    var toc = "";
    var level = 0;
    var container = document.querySelector(container) || document.querySelector('#contents');
    var output = output || '#toc';

    container.innerHTML =
        container.innerHTML.replace(
            /<h([\d])>([^<]+)<\/h([\d])>/gi,
            function (str, openLevel, titleText, closeLevel) {
                if (openLevel != closeLevel) {
                    return str;
                }

                if (openLevel > level) {
                    toc += (new Array(openLevel - level + 1)).join('<ul>');
                } else if (openLevel < level) {
                    toc += (new Array(level - openLevel + 1)).join('</li></ul>');
                } else {
                    toc += (new Array(level + 1)).join('</li>');
                }

                level = parseInt(openLevel);

                var anchor = titleText.replace(/ /g, "_");
                toc += '<li><a href="#' + anchor + '">' + titleText
                    + '</a>';

                return '<h' + openLevel + ' id="' + anchor + '">'
                    + titleText + '</h' + closeLevel + '>';
            }
        );

    if (level) {
        toc += (new Array(level + 1)).join('</ul>');
    }
    document.querySelector(output).innerHTML += toc;
};

TableOfContents(".article", "#toc")

function handleScroll() {
    var scrolledHeight = window.pageYOffset;
    var parallaxSpeed = 0.5; // Adjust this value for different speeds. Less than 1 will be slower.
    var bannerDecoration = document.querySelector('.banner-decoration');

    // This will move the decoration at half the speed of the default scroll
    var newBottomPosition = -scrolledHeight * parallaxSpeed;
    bannerDecoration.style.bottom = newBottomPosition + 'px';
}

document.addEventListener('scroll', handleScroll);


(function () {
    const likeButton = document.getElementById('like');
    const likeCountElement = document.getElementById('likeCount');
    let likeCount = parseInt(likeCountElement.textContent, 10);
    let counterInterval;
    let incrementAmount = 1;
    let stop_sending = false;

    // Placeholder for function to send likes to the backend
    function sendLikesToBackend(count) {
        console.log('Sending likes to the backend:', count);
    }

    function handleLikeMouseDown() {
        incrementLikes();
        // Continue incrementing the like count every 200ms
        counterInterval = setInterval(function () {
            incrementLikes();
        }, 150);
    }

    function stopLikeIncrement() {
        clearInterval(counterInterval);
        if (!stop_sending)
            sendLikesToBackend(likeCount); // Send the updated count to the backend
    }

    function incrementLikes() {
        if (incrementAmount <= 20) { // Check if maximum increment amount reached
            likeCount++;
            likeCountElement.textContent = likeCount;
            incrementAmount++;
        } else {
            stop_sending = true;
            toggleCloud("感谢你的喜爱！")
            stopLikeIncrement(); // Stop incrementing if maximum reached
        }
    }

    likeButton.addEventListener('mousedown', handleLikeMouseDown);
    likeButton.addEventListener('mouseup', stopLikeIncrement);
    likeButton.addEventListener('mouseleave', stopLikeIncrement);
})();

// Cleanup function
window.currentCleanup = function () {
    document.removeEventListener('scroll', handleScroll);
    return "/about listeners";
}


