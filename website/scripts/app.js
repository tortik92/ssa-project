function checkLoginCredentials() {
    let idInput = document.getElementById('device-id-input').value;
    if (idInput === '123') {
        window.location.href = '../pages/game-selection.html'; //Jumping on other page doesn't work.
    } else {
        alert('Invalid Device-ID. Please try again.');
    }
}

document.addEventListener("DOMContentLoaded", () => {
    fetch('https://cakelab.co.nl/ssa-server/')
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            console.log(data);
        })
        .catch(err => {
        console.error('Fetch error:', err);
    });
})