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
        await new Promise(resolve => setTimeout(resolve, delay));

        // Only continue if the current operation is still the latest
        if (currentTypingOperation !== operationId) {
            return;
        }

        document.querySelector(eleRef).innerText += letters[i];
        i++;
    }
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

function formatTimestamp(seconds) {
    const days = Math.floor(seconds / (3600 * 24));
    const hours = Math.floor((seconds % (3600 * 24)) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;
    return `${days}天${hours}小时${minutes}分${secs}秒`;
}

function renderWrapper(elementDOM, newHTML) {
    elementDOM.innerHTML = newHTML;
    elementDOM.classList.add('float-up');
}


function renderWrapperT2(elementDOM, newHTML) {
    elementDOM.innerHTML = newHTML;
    // elementDOM.classList.add('fade-in');
}


AbortSignal.timeout ??= function timeout(ms) {
    const ctrl = new AbortController()
    setTimeout(() => ctrl.abort(), ms)
    return ctrl.signal
}

function cacheData(key, data, exp_hour) {
    const now = new Date().getTime();
    const item = {
        data: data,
        expiry: now + exp_hour * 60 * 60 * 1000,
    };
    localStorage.setItem(key, JSON.stringify(item));
}

function getCachedData(key) {
    const itemStr = localStorage.getItem(key);
    if (!itemStr) {
        return null;
    }
    const item = JSON.parse(itemStr);
    const now = new Date().getTime();
    if (now > item.expiry) {
        localStorage.removeItem(key);
        return null;
    }
    return item.data;
}

async function fetchDataWithCache(url, name, cached_flag = false, exp_hour = 1) {
    const cachedData = getCachedData(url);
    if (cachedData) {
        return Promise.resolve(cachedData);
    } else {
        console.log(`load data: ${name}`);
        return fetch(url, { signal: AbortSignal.timeout(5000) })
            .then(async response => {
                return response.json().then(data => ({
                    status: response.status,
                    data: data
                }));
            })
            .then(({ status, data }) => {
                if (data.status === false) {
                    return Promise.reject({ code: status, msg: data.content });
                }
                if (cached_flag)
                    cacheData(url, data.content, exp_hour);
                return data.content;
            })
    }
}

async function updateTOC(container, output) {
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
                toc += '<li><a href="#' + anchor + '" onclick="event.preventDefault(); document.getElementById(\'' + anchor + '\').scrollIntoView({ behavior: \'smooth\' });">' + titleText + '</a>';

                return '<h' + openLevel + ' id="' + anchor + '">' + titleText + '</h' + closeLevel + '>';
            }
        );

    if (level) {
        toc += (new Array(level + 1)).join('</ul>');
    }

    let elementDOM = document.querySelector(output)

    if (toc === "")
        elementDOM.style.display = "none"
    else
        renderWrapperT2(elementDOM, toc)
};

function registerLikeButton() {
    const likeButton = document.getElementById('like');
    const likeCountElement = document.getElementById('likeCount');
    const likeCounterElement = document.getElementById('likeCounter');
    const likeIconInner = document.querySelector('.like-button .like-icon #like-icon-inner');
    const likeText = document.querySelector('.like-button .like-text');

    let likeCount = parseInt(likeCountElement.textContent, 10);
    let clickEnabled = false;
    let timer;
    let start_timer = false;
    let scalar = 2;
    let love_icon = confetti.shapeFromText({ text: '♥', scalar });

    if (typeof countdown !== "undefined")
        clearInterval(countdown)

    likeButton.addEventListener('click', function () {

        likeIconInner.style.fill = '#FF5050';
        likeText.style.color = '#FF5050';

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

async function sha256(password) {
    const encoder = new TextEncoder();
    const data = encoder.encode(password);
    const hashBuffer = await crypto.subtle.digest('SHA-256', data);
    const hashArray = Array.from(new Uint8Array(hashBuffer));
    const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
    return hashHex;
}

function getUrlParams() {
    const params = {};
    const queryString = window.location.search.slice(1);
    const pairs = queryString.split("&");
    pairs.forEach(pair => {
        const [key, value] = pair.split("=");
        params[key] = decodeURIComponent(value);
    });
    return params;
}

function updateErrorMessage() {
    const params = getUrlParams();
    const errorCode = params.code || "503";
    const errorMessage = params.msg || "Service Unavailable(ᗜ˰ᗜ)";

    document.getElementById("error-code").textContent = errorCode;
    document.getElementById("error-message").textContent = errorMessage;
}

function putLoadMoreBtn() {
    document.getElementById('normal-article-container').innerHTML += `
    <div style="text-align: center; margin-top:30px" id="reload-btn">
    <button class="nice-btn" style="font-size: .75em;" onclick="fetchMoreArticles()">Load More</button>
    <div>
    `
}

var isFetching = false;
var postOffset = 0;
var isLoadAll = false

function fetchMoreArticles() {
    document.getElementById("reload-btn").remove();

    if (isFetching || isLoadAll) return;
    isFetching = true;

    const articleContainer = document.createElement('div');
    articleContainer.id = `ext-normal-article-${postOffset}`;
    const loadingBlock = document.createElement('div');
    loadingBlock.classList.add('loading-block');
    loadingBlock.style.height = '230px';
    articleContainer.appendChild(loadingBlock);
    document.getElementById('normal-article-container').appendChild(articleContainer);

    fetchDataWithCache(`/api/postInfos?from=${postOffset}`, `postinfos-${postOffset}`)
        .then(data => {
            if (data.length === 0)
                isLoadAll = true;

            const renderPromises = [
                renderWrapperT2(document.getElementById(`ext-normal-article-${postOffset}`), renderNormalArticle(data)),
            ];
            postOffset += data.length; // Update postOffset with the number of fetched articles
            return Promise.all(renderPromises);
        })
        .then(() => {
            {
                if (!isLoadAll) putLoadMoreBtn()
            }
        })
        .catch(error => {
            if (error.code && error.msg) {
                navigateTo(`/err?code=${error.code}&msg=${encodeURIComponent(error.msg)}`);
            } else {
                navigateTo("/err");
            }
            throw error;
        })
        .finally(() => {
            isFetching = false;
        });
}

function loadJsr(url, callback) {
    if (!loadedJS[url]) {
        var script = document.createElement('script');
        script.type = 'text/javascript';
        script.src = url;
        script.async = true
        script.onload = callback;
        document.head.appendChild(script);
        loadedJS[url] = true;
        console.log(`load jsr: ${url}`)
    } else {
        callback()
    }
}

function updateUptime(startTimestamp) {
    const uptimeElement = document.getElementById('uptime');
    const now = Math.floor(Date.now() / 1000);
    const elapsed = now - startTimestamp;
    uptimeElement.innerText = formatTimestamp(elapsed);
}

function getCookie(name) {
    const value = `; ${document.cookie}`;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
}

function setCookie(name, value, days) {
    let expires = "";
    if (days) {
        const date = new Date();
        date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "") + expires + "; path=/";
}
/*
Start Up Script

*/
toggleCloud("欢迎来到 贰岛博客！")

document.querySelector('.top').addEventListener('click', function (event) {
    event.preventDefault();
    window.scrollTo({ top: 0, behavior: 'smooth' });

    toggleCloud("↑↑↑↑↑")
});

fetch('/plugin/Birthday/get')
    .then(response => response.json())
    .then(data => {
        if (data && data.birthday !== undefined && data.birthday !== -1) {
            const startTimestamp = data.birthday;
            updateUptime(startTimestamp);
            setInterval(() => updateUptime(startTimestamp), 1000);
        } else {
            document.getElementById('uptime').innerText = 'Failed to fetch uptime';
        }
    })
    .catch(error => {
        console.error('Error fetching uptime:', error);
        document.getElementById('uptime').innerText = 'Failed to fetch uptime';
    });