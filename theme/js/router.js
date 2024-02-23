const route = (event) => {
    event = event || window.event;
    event.preventDefault();
    window.history.pushState({}, "", event.target.href);
    handleLocation();
};

const routes = {
    404: { html: "/pages/404.html", js: null },
    "/": { html: "/pages/index.html", js: "/js/index.js" },
    "/about": { html: "/pages/about.html", js: "/js/about.js" },
    "/err": { html: "/pages/503.html", js: null},
};

const handleLocation = async () => {
    // cleanup the previous page renderer
    if (window.currentCleanup) {
        const cleaner_name = window.currentCleanup(); 
        window.currentCleanup = null; 
        console.log("clean script: " + cleaner_name)
    }

    const path = window.location.pathname;
    const route = routes[path] || routes[404];
    const html = await fetch(route.html).then((data) => data.text());
    
    document.getElementById("page-content").innerHTML = html;

    const oldScript = document.querySelector('.route-script');
    if (oldScript) {
        oldScript.parentNode.removeChild(oldScript);
    }

    if (route.js) {
        console.log("loading script: " + route.js)
        const script = document.createElement("script");
        script.src = route.js;
        script.className = 'route-script';
        document.body.appendChild(script);
    }
};

window.onpopstate = handleLocation;
window.route = route;

handleLocation();