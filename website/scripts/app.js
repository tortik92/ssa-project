function checkLoginCredentials() {
    let idInput = document.getElementById('device-id-input').value;
    if (idInput === '123') {
        window.location.href = './pages/game-selection.html';
    } else {
        alert('Invalid Device-ID. Please try again.');
    }
}

function createButton(text, clickHandler) {
    const button = document.createElement('button');
    button.innerText = text;
    button.addEventListener('click', clickHandler);
    return button;
}

function createInput(preference) {
    const inputElement = document.createElement('input');
    inputElement.type = "text";
    inputElement.value = preference.default_value;
    inputElement.placeholder = 'Type in a value!';
    inputElement.maxLength = preference.max_length;
    inputElement.minLength = preference.min_length;

    // Add an input event listener to enforce max length and check min length
    inputElement.addEventListener('input', function () {
        const inputValue = inputElement.value;
        if (inputValue.length > preference.max_length) {
            inputElement.value = inputValue.slice(0, preference.max_length);
        }
        if (inputValue.length < preference.min_length) {
            inputElement.style.borderColor = 'red';
        } else {
            inputElement.style.borderColor = ''; // Reset to default
        }
    });

    return inputElement;
}

function incrementPreferenceValue(preferenceValue, maxValue) {
    const currentValue = parseInt(preferenceValue.innerText);
    if (currentValue < maxValue) {
        preferenceValue.innerText = currentValue + 1;
    }
}

function decrementPreferenceValue(preferenceValue, minValue) {
    const currentValue = parseInt(preferenceValue.innerText);
    if (currentValue > minValue) {
        preferenceValue.innerText = currentValue - 1;
    }
}

function incrementListPreferenceValue(preferenceValue, list) {
    const currentValue = preferenceValue.innerText;
    const currentIndex = list.indexOf(currentValue);
    const newIndex = (currentIndex + 1) % list.length; // Wrap around to 0 if at the end
    preferenceValue.innerText = list[newIndex];
}

function decrementListPreferenceValue(preferenceValue, list) {
    const currentValue = preferenceValue.innerText;
    const currentIndex = list.indexOf(currentValue);
    const newIndex = (currentIndex - 1 + list.length) % list.length; // Wrap around to the end if at the beginning
    preferenceValue.innerText = list[newIndex];
}

function toggleBooleanPreferenceValue(preferenceValue) {
    const currentValue = preferenceValue.innerText === 'true'; // Convert to boolean
    const newValue = !currentValue; // Toggle between true and false
    preferenceValue.innerText = newValue.toString(); // Convert back to string ('true' or 'false')
}

function createPreferenceElement(preference) {
    const preferenceListElement = document.createElement('div');
    preferenceListElement.classList.add('preference-list-element');
    const preferenceName = document.createElement('h2');
    const preferenceNameValuePair = document.createElement('div');
    preferenceNameValuePair.classList.add('preference-key-value-pair');
    let preferenceValue = null;

    switch (preference.preference_type) {
        case 'number':
            preferenceValue = document.createElement('h2');
            preferenceValue.innerText = preference.default_value;
            const leftButton = createButton('-', () => decrementPreferenceValue(preferenceValue, parseInt(preference.min_value)));
            const rightButton = createButton('+', () => incrementPreferenceValue(preferenceValue, parseInt(preference.max_value)));
            preferenceName.innerText += preference.preference_name;

            preferenceNameValuePair.append(preferenceName);
            preferenceNameValuePair.append(preferenceValue);
            preferenceListElement.append(leftButton);
            preferenceListElement.append(preferenceNameValuePair);
            preferenceListElement.append(rightButton);
            break;

        case 'list':
            preferenceValue = document.createElement('h2');
            preferenceValue.innerText = preference.list[preference.default_value];
            const leftListButton = createButton('<', () => decrementListPreferenceValue(preferenceValue, preference.list));
            const rightListButton = createButton('>', () => incrementListPreferenceValue(preferenceValue, preference.list));
            preferenceName.innerText += preference.preference_name;

            preferenceNameValuePair.append(preferenceName);
            preferenceNameValuePair.append(preferenceValue);
            preferenceListElement.append(leftListButton);
            preferenceListElement.append(preferenceNameValuePair);
            preferenceListElement.append(rightListButton);
            break;

        case 'bool':
            preferenceValue = document.createElement('h2');
            preferenceValue.innerText = preference.default_value === true ? 'true' : 'false';
            const leftToggleButton = createButton('<', () => toggleBooleanPreferenceValue(preferenceValue));
            const rightToggleButton = createButton('>', () => toggleBooleanPreferenceValue(preferenceValue));
            preferenceName.innerText += preference.preference_name;

            preferenceNameValuePair.append(preferenceName);
            preferenceNameValuePair.append(preferenceValue);
            preferenceListElement.append(leftToggleButton);
            preferenceListElement.append(preferenceNameValuePair);
            preferenceListElement.append(rightToggleButton);
            break;

        case 'text':
            preferenceValue = createInput(preference);
            preferenceName.innerText += preference.preference_name;

            preferenceNameValuePair.append(preferenceName);
            preferenceNameValuePair.append(preferenceValue);

            preferenceListElement.append(preferenceNameValuePair);
            break;
    }

    return preferenceListElement;
}

function createGameListElement(game) {
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

    return listElement;
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
                    const listElement = createGameListElement(game);
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
        let preferencesArr = [];

        fetch(`https://cakelab.co.nl/ssa-server/?uid=${parameterValue}`, {method: 'GET'})
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json();
            })
            .then(data => {
                const preferenceList = document.getElementById('preference-list');
                preferencesArr = data.preferences;
                preferencesArr.forEach(preference => {
                    const preferenceElement = createPreferenceElement(preference);
                    preferenceList.append(preferenceElement);
                })
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
            } else if (pathname.includes('/ssa-project/website/pages/game-settings.html')) {
                window.location.href = './game-status.html';
            }
        })
    }
})