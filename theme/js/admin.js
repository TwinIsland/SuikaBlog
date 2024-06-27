// Function to handle form submission
async function submitPost(event) {
    event.preventDefault();

    const password = document.getElementById('password').value;
    const suikaToken = await sha256(password);

    const postData = {
        title: document.getElementById('title').value,
        excerpts: document.getElementById('excerpts').value,
        banner: document.getElementById('banner').value,
        content: document.getElementById('content').value,
        is_page: document.getElementById('is_page').checked ? "1" : "0"
    };

    fetch('/api/write', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Suika-Token': suikaToken
        },
        body: JSON.stringify(postData)
    })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'OK') {
                alert('Post created successfully');
            } else {
                alert('Failed to create post: ' + data.msg);
            }
        })
        .catch(error => {
            console.error('Error:', error);
            alert('An error occurred while creating the post.');
        });
}
