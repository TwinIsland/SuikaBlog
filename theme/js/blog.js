/*
    universal js functionality cross the whole blog,
    will defer loading

*/

let currentTypingOperation = null;

const phrases = [
    "嘿，别小看鬼哦！",
    "不错嘛，挺有意思的。",
    "哈哈，就这样吧！",
    "喝了这杯，再战一场！",
    "你真是好奇心旺盛呢。",
    "来一场公平的对决吧！",
    "哼，我才不怕呢。",
    "嗨，今天风真不错。",
    "别急，慢慢来更好。",
    "咦，你是新面孔呀？",
    "今天的月色真美。",
    "这可是我的秘密武器！",
    "跟我赌一把如何？",
    "看我的力量！",
    "这就是鬼的力量！",
    "想不想听个故事？",
    "我可是不会轻易认输的！",
    "来，跟我干杯吧！",
    "你准备好了吗？",
    "我永远都不会迷路的！",
    "我们再来比赛一次吧！",
    "一起去喝一杯怎么样？",
    "我会帮你看好捐款箱的！",
    "这个妖怪，我来处理吧！",
    "别太累了。",
    "我们的下一个冒险去哪里？",
    "看我的，保护你不受伤！",
    "一起去看樱花吧！",
    "有我在不用害怕。",
    "我最喜欢你的那个表情了！",
    "下次的祭典，我要帮忙！",
    "嘿嘿，我们的组合无敌哦！",
    "你相信命运吗？",
    "一起去幻想乡的边界看看吧！"
];

async function typeSentence(sentence, eleRef, delay = 100) {
    const operationId = Math.random();
    currentTypingOperation = operationId;

    const letters = sentence.split("");
    let i = 0;
    while (i < letters.length) {
        await waitForMs(delay);

        // Only continue if the current operation is still the latest
        if (currentTypingOperation !== operationId) {
            return;
        }

        document.querySelector(eleRef).innerText += letters[i];
        i++;
    }
}

function waitForMs(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function randomInRange(min, max) {
    return Math.random() * (max - min) + min;
}

let hideTimeout;

function toggleCloud(sayWhat = null) {
    const cloud = document.getElementById('conversationCloud');
    const colors = ['#ffa53a', '#8d0e6b', '#ffffd6'];

    clearTimeout(hideTimeout);

    if (sayWhat == null) {
        sayWhat = phrases[Math.floor(Math.random() * phrases.length)];
    }
    typeSentence(sayWhat, '#conversationCloud', 70);
    cloud.innerHTML = ''

    cloud.style.opacity = '1';

    hideTimeout = setTimeout(() => {
        cloud.style.opacity = '0';
    }, 2000);

    confetti({
        particleCount: 100,
        spread: 70,
        colors: colors,
        angle: randomInRange(30, 60),
        // shapes: ['star'],
        particleCount: randomInRange(20, 30),
        origin: { y: 1, x: 0 }
    });
}

toggleCloud("欢迎来到 贰岛博客！")

document.addEventListener('scroll', function () {
    var top = window.scrollY || document.documentElement.scrollTop;
    var backToTopButton = document.querySelector('.top');

    if (top > 800) {
        backToTopButton.style.display = 'block';
    } else {
        backToTopButton.style.display = 'none';
    }
});

document.querySelector('.top').addEventListener('click', function (event) {
    event.preventDefault();
    window.scrollTo({ top: 0, behavior: 'smooth' });

    toggleCloud("↑↑↑↑↑")
});

function updateTOC(container, output) {
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

    let elementDOM = document.querySelector(output)

    setTimeout(() => {
        elementDOM.classList.add('fade-in');
        setTimeout(() => {
          elementDOM.innerHTML = toc;
          elementDOM.classList.add('visible');
        }, 230);
      }, 150);

    // document.querySelector(output).innerHTML += toc;
};

function registerLikeButton() {
    const likeButton = document.getElementById('like');
    const likeCountElement = document.getElementById('likeCount');
    const likeCounterElement = document.getElementById('likeCounter');
    const likeIcon = document.querySelector('.like-button .like-icon');
    const likeText = document.querySelector('.like-button .like-text');

    let likeCount = parseInt(likeCountElement.textContent, 10);
    let clickEnabled = false;
    let timer;
    let start_timer = false;
    let scalar = 2;
    let love_icon = confetti.shapeFromText({ text: '♥', scalar});

    if (typeof countdown !== "undefined")
        clearInterval(countdown)

    likeButton.addEventListener('click', function () {
    
        likeIcon.style.fill = '#e63946'; 
        likeText.style.color = '#e63946'; 

        if (!start_timer) {
            likeCounterElement.parentNode.style.display = ''; // Show the counter
            let counter = 5;

            countdown = setInterval(() => {
                counter -= 1;
                likeCounterElement.textContent = counter;
                if (counter <= 0) {
                    clearInterval(countdown);
                    likeCounterElement.parentNode.style.display = 'none';
                }
            }, 1000);

            timer = setTimeout(() => {
                clickEnabled = false;
                toggleCloud("感谢你的喜爱！");
            }, 5000);
            clickEnabled = true
            start_timer = true
        }

        if (clickEnabled) {
            likeCount += 1;
            likeCountElement.textContent = likeCount;
            confetti({
                particleCount: 15,
                scalar: 1,
                startVelocity: -35,
                spread: 180,
                origin: { y: 0, },
                shapes: [love_icon]
              });
        } else {
            toggleCloud("感谢你的喜爱！")
        }
    });
};

function textScrollHandler() {
    var scrolledHeight = window.pageYOffset;
    var parallaxSpeed = 0.5; // Adjust this value for different speeds. Less than 1 will be slower.
    var bannerDecoration = document.querySelector(".scroll-text-decoration");

    // This will move the decoration at half the speed of the default scroll
    var newBottomPosition = -scrolledHeight * parallaxSpeed;
    bannerDecoration.style.bottom = newBottomPosition + 'px';
}