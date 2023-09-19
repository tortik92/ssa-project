function checkLoginCredentials () { // eslint-disable-line
  const idInput = document.getElementById('device-id-input').value;
  if (idInput === '123') {
    window.location.href = './pages/game-selection.html';
  } else {
    alert('Invalid Device-ID. Please try again.');
  }
}

function createButton (text, clickHandler) {
  const button = document.createElement('button');
  button.innerText = text;
  button.addEventListener('click', clickHandler);
  return button;
}

function createInput (preference) {
  const inputElement = document.createElement('input');
  inputElement.type = 'text';
  inputElement.value = preference.default_value;
  inputElement.placeholder = 'Geben Sie einen Wert ein!';
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
      document.getElementById('next-button').disabled = true;
    } else {
      inputElement.style.borderColor = ''; // Reset to default
      document.getElementById('next-button').disabled = false;
    }
  });

  return inputElement;
}

function incrementPreferenceValue(preferenceValue, maxValue, incrementButton, decrementButton) {
  const currentValue = parseInt(preferenceValue.innerText);
  if (currentValue < maxValue) {
    preferenceValue.innerText = currentValue + 1;
    if (currentValue + 1 === maxValue) {
      // If the value reaches the maximum, disable the increment button
      incrementButton.disabled = true;
    } else {
      // Reset the disabled state of the decrement button
      decrementButton.disabled = false;
    }
  }
}

function decrementPreferenceValue(preferenceValue, minValue, incrementButton, decrementButton) {
  const currentValue = parseInt(preferenceValue.innerText);
  if (currentValue > minValue) {
    preferenceValue.innerText = currentValue - 1;
    if (currentValue - 1 === minValue) {
      // If the value reaches the minimum, disable the decrement button
      decrementButton.disabled = true;
    } else {
      // Reset the disabled state of the increment button
      incrementButton.disabled = false;
    }
  }
}

function incrementListPreferenceValue (preferenceValue, list) {
  const currentValue = preferenceValue.innerText;
  const currentIndex = list.indexOf(currentValue);
  const newIndex = (currentIndex + 1) % list.length; // Wrap around to 0 if at the end
  preferenceValue.innerText = list[newIndex];
}

function decrementListPreferenceValue (preferenceValue, list) {
  const currentValue = preferenceValue.innerText;
  const currentIndex = list.indexOf(currentValue);
  const newIndex = (currentIndex - 1 + list.length) % list.length; // Wrap around to the end if at the beginning
  preferenceValue.innerText = list[newIndex];
}

function toggleBooleanPreferenceValue (preferenceValue) {
  const currentValue = preferenceValue.innerText === 'true'; // Convert to boolean
  const newValue = !currentValue; // Toggle between true and false
  preferenceValue.innerText = newValue.toString(); // Convert back to string ('true' or 'false')
}

function createPreferenceElement (preference) {
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
      preferenceName.innerText += preference.preference_name;

      preferenceNameValuePair.append(preferenceName);
      preferenceNameValuePair.append(preferenceValue);

      const incrementButton = createButton('+', () => {
        incrementPreferenceValue(preferenceValue, parseInt(preference.max_value), incrementButton, decrementButton);
      });

      const decrementButton = createButton('-', () => {
        decrementPreferenceValue(preferenceValue, parseInt(preference.min_value), incrementButton, decrementButton);
      });

      preferenceListElement.append(decrementButton);
      preferenceListElement.append(preferenceNameValuePair);
      preferenceListElement.append(incrementButton);

      if (parseInt(preferenceValue.innerText) === parseInt(preference.max_value)) {
        incrementButton.disabled = true;
      }

      if (parseInt(preferenceValue.innerText) === parseInt(preference.min_value)) {
        decrementButton.disabled = true;
      }

      break;

    case 'list':
      preferenceValue = document.createElement('h2');
      preferenceValue.innerText = preference.list[preference.default_value];
      preferenceName.innerText += preference.preference_name;

      preferenceNameValuePair.append(preferenceName);
      preferenceNameValuePair.append(preferenceValue);


      preferenceListElement.append(createButton('<', () => decrementListPreferenceValue(preferenceValue, preference.list)));
      preferenceListElement.append(preferenceNameValuePair);
      preferenceListElement.append(createButton('>', () => incrementListPreferenceValue(preferenceValue, preference.list)));
      break;

    case 'bool':
      preferenceValue = document.createElement('h2');
      preferenceValue.innerText = preference.default_value === true ? 'Ja' : 'Nein';
      preferenceName.innerText += preference.preference_name;

      preferenceNameValuePair.append(preferenceName);
      preferenceNameValuePair.append(preferenceValue);
      preferenceListElement.append(createButton('<', () => toggleBooleanPreferenceValue(preferenceValue)));
      preferenceListElement.append(preferenceNameValuePair);
      preferenceListElement.append(createButton('>', () => toggleBooleanPreferenceValue(preferenceValue)));
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

function createGameListElement (game) {
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
  const pathname = window.location.pathname;
  const url = 'https://cakelab.co.nl/ssa-server/';
  if (pathname.includes('/ssa-project/website/pages/game-selection.html')) {
    fetch(url)
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
        });

        const gameListElements = document.querySelectorAll('.list-element');

        gameListElements.forEach((listElement, index) => {
          listElement.addEventListener('click', () => {
            gameListElements.forEach(listElement => {
              listElement.classList.remove('selected');
            });
            listElement.classList.add('selected');
            selectedGameUID = gameArr[index].uid;
          });
        });
      })
      .catch(err => {
        console.error('Fetch error:', err);
      });
  } else if (pathname.includes('/ssa-project/website/pages/game-settings.html')) {
    const urlParams = new URLSearchParams(window.location.search);
    const parameterValue = urlParams.get('uid');
    let preferencesArr = [];
    fetch(`${url}?uid=${parameterValue}`, { method: 'GET' })
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
        });
      })
      .catch(err => {
        console.error('Fetch error:', err);
      });
  } else if (pathname.includes('/ssa-project/website/pages/game-status.html')) {
    const urlParams = new URLSearchParams(window.location.search);
    const parameterValue = urlParams.get('uid');
    fetch(`${url}?uid=${parameterValue}`, { method: 'GET' })
        .then(response => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then(data => {
          const headerElement = document.getElementById('header');
          headerElement.innerText += ` "${data.name}"!`;
        })
        .catch(err => {
          console.error('Fetch error:', err);
        });
  }

  const nextButton = document.getElementById('next-button');
  const backButton = document.getElementById('back-button');
  const closeGameButton = document.getElementById('close-game-button');

  if (backButton != null && nextButton != null) {
    backButton.addEventListener('click', () => {
      window.history.go(-1);
    });

    nextButton.addEventListener('click', () => {
      if (pathname.includes('/ssa-project/website/pages/game-selection.html')) {
        if (document.querySelectorAll('.selected').length > 0) {
          window.location.href = './game-settings.html?uid=' + encodeURIComponent(selectedGameUID);
        } else {
          alert('Please select a game!');
        }
      } else if (pathname.includes('/ssa-project/website/pages/game-settings.html')) {
        const urlParams = new URLSearchParams(window.location.search);
        const parameterValue = urlParams.get('uid');
        window.location.href = './game-status.html?uid=' + encodeURIComponent(parameterValue);
      }
    });
  }

  if (closeGameButton != null) {
    closeGameButton.addEventListener('click', () => {
      window.location.href = './game-selection.html';
    })
  }
});
