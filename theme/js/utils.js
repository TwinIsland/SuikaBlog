const Color = {
    RED: 'red',
    BLACK: 'black',
    YELLOW: 'yellow'
};

/* message box */
function displayMsgBox(content, duration = 2500, color = Color.BLACK) {
    // Create the message box div
    const msgBox = document.createElement('div');
    msgBox.innerText = content;
    msgBox.className = 'msg-box';
    msgBox.style.backgroundColor = color;

    // Append to body
    document.body.appendChild(msgBox);

    // Add a show class to initiate any animations
    setTimeout(() => {
        msgBox.classList.add('show');
    }, 10);

    // Remove the message box after the specified duration
    setTimeout(() => {
        msgBox.classList.remove('show');
        setTimeout(() => {
            msgBox.remove();
        }, 300);  // Assuming a 300ms fade-out animation
    }, duration);
}

export { displayMsgBox, Color };
