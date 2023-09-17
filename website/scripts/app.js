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
            const gameList = document.getElementById('game-list');
            data.forEach(game => {
                const listElement = document.createElement('li');
                listElement.classList.add('list-element');

                const gameNameTitle = document.createElement('h2');
                gameNameTitle.classList.add('game-name');
                gameNameTitle.innerText = game.name;

                const gameInformationContainer = document.createElement('div');
                gameInformationContainer.classList.add('game-information-container');

                const gameDescription = document.createElement('p');
                gameDescription.classList.add('game-description');
                gameDescription.innerText = game.description;

                const gameVersion = document.createElement('p');
                gameVersion.classList.add('game-description');
                gameVersion.innerText = game.version;

                gameInformationContainer.append(gameDescription);
                gameInformationContainer.append(gameVersion);

                listElement.append(gameNameTitle);
                listElement.append(gameInformationContainer);

                gameList.append(listElement);
            })
        })
        .catch(err => {
        console.error('Fetch error:', err);
    });
})