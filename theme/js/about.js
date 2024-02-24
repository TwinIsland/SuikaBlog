
document.addEventListener('scroll', textScrollHandler);

updateTOC(".article", "#toc")
registerLikeButton();

// Cleanup function
window.currentCleanup = function () {
    document.removeEventListener('scroll', textScrollHandler);
    return "/about listeners";
}
