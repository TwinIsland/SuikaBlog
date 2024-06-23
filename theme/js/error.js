function updateErrorMessage() {
    const params = getUrlParams();
    const errorCode = params.code || "503";
    const errorMessage = params.msg || "Service Unavailable(ᗜ˰ᗜ)";

    document.getElementById("error-code").textContent = errorCode;
    document.getElementById("error-message").textContent = errorMessage;
}

updateErrorMessage()