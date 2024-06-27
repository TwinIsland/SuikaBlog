
document.addEventListener('scroll', textScrollHandler);

updateTOC("#article-body", "#toc-body")
registerLikeButton();

new QRCode(document.getElementById("page-qr"), {
	text: window.location.href,
	width: 128,
	height: 128,
	colorDark : "#B98A82",
	correctLevel : QRCode.CorrectLevel.H
});

// Cleanup function
window.currentCleanup = function () {
    document.removeEventListener('scroll', textScrollHandler);
    return "/about listeners";
}
