const token = getCookie('Suika-Token');
if (token) {
    navigateTo('/admin');
}

document.getElementById('login-form').addEventListener('submit', async function(event) {
    event.preventDefault();

    const password = document.getElementById('password').value;
    const rememberMe = document.getElementById('remember-me').checked;
    const errorMessage = document.getElementById('error-message');
    errorMessage.style.display = 'none'; // Hide the error message initially

    try {
        const token = await sha256(password);
        const response = await fetch('/api/auth', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Suika-Token': token
            },
            body: JSON.stringify({ password })
        });

        const data = await response.json();

        if (data.status === true) {
            const days = rememberMe ? 2 : 0; // Set cookie to expire in 2 days if remember me is checked
            setCookie('Suika-Token', token, days);
            navigateTo("/admin");
        } else {
            errorMessage.style.display = 'block';
        }
    } catch (error) {
        console.error('Error during login:', error);
        errorMessage.style.display = 'block';
    }
});
