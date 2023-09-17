function checkLoginCredentials() {
    let idInput = document.getElementById('device-id-input').value;
    if (idInput === '123') {
        window.location.href = './pages/game-selection.html';
    } else {
        alert('Invalid Device-ID. Please try again.');
    }
}

const gameArr = [];
let selectedGameUID = 0;

addEventListener('DOMContentLoaded', () => {
    let pathname = window.location.pathname;
    if (pathname.includes('/ssa-project/website/pages/game-selection.html')) {
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
                    gameArr.push(game);
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

                let gameListElements = document.querySelectorAll('.list-element');

                gameListElements.forEach((listElement, index) => {
                    listElement.addEventListener('click', () => {
                        gameListElements.forEach(listElement => {
                            listElement.classList.remove('selected');
                        })
                        listElement.classList.add('selected');
                        selectedGameUID = gameArr[index].uid;
                    })
                })
            })
            .catch(err => {
                console.error('Fetch error:', err);
            })
    } else if(pathname.includes('/ssa-project/website/pages/game-settings.html')) {
        let urlParams = new URLSearchParams(window.location.search);
        let parameterValue = urlParams.get('uid')

        fetch(`https://cakelab.co.nl/ssa-server?uid=${parameterValue}`, {mode: 'no-cors'})
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
            })
    }


    let nextButton = document.getElementById('next-button');
    let backButton = document.getElementById('back-button');

    if (backButton != null && nextButton != null) {
        backButton.addEventListener('click', () => {
            window.history.go(-1);
        })

        nextButton.addEventListener('click', () => {
            let pathname = window.location.pathname;
            if (pathname.includes('/ssa-project/website/pages/game-selection.html')){
                if (document.querySelectorAll('.selected').length > 0) {
                    window.location.href = './game-settings.html?uid=' + encodeURIComponent(selectedGameUID);
                } else {
                    alert('Please select a game!');
                }
            }
        })
    }
})