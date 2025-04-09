document.addEventListener("DOMContentLoaded", function () {
    const body = document.body;

    // Standardwerte festlegen
    const defaultBgColor = 'white'; // Standard Hintergrundfarbe
    const defaultTextColor = 'black'; // Standard Textfarbe
    const defaultFontSize = '1rem'; // Standard Schriftgröße
    const newHeadingFontSizeH1 = "2rem"; // Standardgröße für h1
    const newHeadingFontSizeH2 = "1.5rem"; // Standardgröße für h2
    const newHeadingFontSizeH3 = "1.17rem"; // Standardgröße für h3";
    const defaultNavColor = 'black'; // Standard Toolbar Textfarbe
    const defaultNavBorder="black";
    const defaultNavTextColor = 'white';
    const defaultHoverColor = 'black';
    const defaultHoverBackgroundColor = 'white';
    const yellowBgColor = 'gold'; // Gold für den Hintergrund jeder Section
    const blackTextColor = 'black'; // Schwarz für den Text jeder Section
    const blackBgColor = 'black'; // Gold für den Hintergrund jeder Section
    const yellowTextColor = 'gold'; // Schwarz für den Text jeder Section
    const whiteBgColor = 'white'; // Gold für den Hintergrund jeder Section
    const blueTextColor = 'blue'; // Schwarz für den Text jeder Section
    const blueBgColor = 'blue'; // Gold für den Hintergrund jeder Section
    const whiteTextColor = 'white'; // Schwarz für den Text jeder Section

    // Überprüfen, ob es gespeicherte Einstellungen gibt (z.B. aus dem lokalen Speicher)
    const savedBgColor = localStorage.getItem('bgColor');
    const savedTextColor = localStorage.getItem('textColor');
    const savedFontSize = localStorage.getItem('fontSize');
    const savedNavBorder = localStorage.getItem('navBorder');
    const savedNavTextColor = localStorage.getItem('navTextColor');
    const savedHoverColor = localStorage.getItem('hoverColor');
    const savedHoverBackgroundColor = localStorage.getItem('hoverBackgroundColor');
    const languageLinks = document.querySelectorAll(".language");

    // Wenn es gespeicherte Einstellungen gibt, wenden wir sie hier an
    if (savedBgColor) {
        body.style.backgroundColor = savedBgColor;
        document.querySelector('.toolbar').style.backgroundColor = savedBgColor;
    }
    if (savedTextColor) {
        body.style.color = savedTextColor;
        document.querySelector('.toolbar').style.color = savedTextColor;
    }
    if (savedFontSize) {
        body.style.fontSize = savedFontSize;
        document.querySelectorAll('section').forEach(section => {
            section.style.fontSize = savedFontSize;
        });
    }
    if(savedNavBorder) {
        body.style.borderColor = savedNavBorder;
        document.querySelectorAll('section').forEach(section => {
            section.style.borderColor = savedNavBorder;
        })
    }
    if(savedNavTextColor) {
        body.style.color = savedNavTextColor;
        document.querySelectorAll('section').forEach(section => {
            section.style.color = savedNavTextColor;
        })
    }
    if(savedHoverColor) {
        body.style.color = savedHoverColor;
        document.querySelector('nav ul li a').style.color = savedHoverColor;
    }
    if(savedHoverBackgroundColor) {
        body.style.backgroundColor = savedHoverBackgroundColor;
        document.querySelector('nav ul li a').style.color = savedHoverBackgroundColor;
    }




    // Speichermechanismus für die Farbauswahl durch Buttons
    const colorButtons = [document.getElementById('btn-yellow-1'), document.getElementById('btn-yellow-2'), document.getElementById('btn-blue'), document.getElementById('btn-white')];
    colorButtons.forEach(icon => {
        icon.addEventListener('click', () => {
            // Zuerst entfernen wir die aktive Klasse und das Häkchen von allen Buttons
            colorButtons.forEach(i => {
                i.classList.remove('active');
                i.classList.remove('color-active');
            });

            icon.classList.add('active');  // Der geklickte Button bekommt die "active" Klasse
            icon.classList.add('color-active'); // Häkchen hinzufügen

            // Hier wird die Hintergrundfarbe und Textfarbe je nach gewähltem Button geändert
            if (icon.classList.contains('yellow')) {
                body.style.backgroundColor = yellowBgColor;
                body.style.color = blackTextColor;

                // Alle relevanten Bereiche bekommen ebenfalls diese Farben
                document.querySelector('.toolbar').style.backgroundColor = yellowBgColor;
                document.querySelector('.toolbar').style.color = blackTextColor;
                document.querySelector('nav').style.backgroundColor=yellowBgColor;
                document.querySelector('.rights').style.backgroundColor = yellowBgColor;
                document.querySelector('.rights').style.color = blackTextColor;
                document.querySelector('.language').style.color = blackTextColor;
                document.querySelector('.support-section').style.backgroundColor = yellowBgColor;
                document.querySelectorAll('.question-list li').forEach(section=>section.style.color=blackTextColor);
                document.querySelectorAll('.question-list li').forEach(section=>section.style.backgroundColor=yellowBgColor);
                document.querySelector('.support-section h2').style.color=blackTextColor;
                document.querySelectorAll('.topic').forEach(link => link.style.color = blackTextColor);
                document.querySelectorAll('.topic').forEach(link => link.style.backgroundColor = yellowBgColor);
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.color = blackTextColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.backgroundColor = yellowBgColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.borderColor = blackTextColor;})
                document.querySelectorAll('.next-language').forEach(button => {  button.style.backgroundColor =yellowBgColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.borderColor = blackTextColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.color = blackTextColor;});
                document.querySelectorAll('.faq-item').forEach(button => {  button.style.borderColor = blackTextColor;});
                document.querySelectorAll('.language').forEach(link => link.style.color = blackTextColor);
                document.querySelectorAll('.faq-button').forEach(button => {  button.style.color = blackTextColor;});
                document.querySelectorAll('.answer').forEach(link=>link.style.color = blackTextColor);
                document.querySelector('#chatContainer').style.color = blackTextColor;
                document.querySelector('#chatContainer').style.backgroundColor = yellowBgColor;
                document.querySelector('#chatContainer').style.borderColor = blackTextColor;
                document.querySelector('#userInput').style.color = blackTextColor;
                document.querySelector('#userInput').style.backgroundColor = yellowBgColor;
                document.querySelector('.btn-success').style.color = blackTextColor;
                document.querySelector('.btn').style.borderColor = blackTextColor;
                document.querySelector('#startStopButton').style.color = blackTextColor;
                document.querySelector('#startStopButton').style.borderColor = blackTextColor;
                document.querySelector('#userInput.form-control').style.color = blackTextColor;
                document.querySelector('.intro').style.color = blackTextColor;
                document.querySelectorAll('.product-section').forEach(section => section.style.color = blackTextColor);
                document.querySelector('.support-box-list').style.color = blackTextColor;
                document.querySelector('.support-box-list').style.borderColor = blackTextColor;
                document.querySelector('.support-box-list').style.backgroundColor = yellowBgColor;
                document.querySelectorAll('.support-box').forEach(box => box.style.borderColor = blackTextColor);
                document.querySelectorAll('.support-box').forEach(link=>link.style.borderColor = blackTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.color = blackTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.backgroundColor = yellowBgColor);
                document.querySelectorAll('.app-section').forEach(link=>link.style.color = blackTextColor);
                document.querySelector('#toggle-button.toggle-button').style.color = blackTextColor;
                document.querySelectorAll('#toggle-button .bar').forEach(bar => {
                    bar.style.backgroundColor = blackTextColor;
                });
                document.querySelector('nav').style.borderColor = blackTextColor;
                document.querySelectorAll('nav a').forEach(link => link.style.color = blackTextColor);
                document.querySelector('.selbstmachen-section').style.color = blackTextColor;
                document.querySelector('.support-section').style.color = blackTextColor;
                document.querySelectorAll('.support-box').forEach(box => {
                    box.addEventListener('click', () => {
                        const modal = document.querySelector('#myModal'); // Das Modal-Element
                        const modalContent = modal.querySelector('#modalText'); // Der innere Content

                        const modalText = box.getAttribute('data-modal-text'); // Den Text holen
                        const bgColor = 'gold'; // Die Farbe für die innere Box

                        // Inhalt setzen
                        modalContent.innerHTML = modalText;

                        // Nur den inneren Bereich einfärben:
                        modalContent.style.backgroundColor = bgColor;

                        // Modal anzeigen
                        modal.style.display = 'flex';

                    });
                });


                document.querySelector('#btn-plus').style.color = blackTextColor;
                document.querySelector('.fragen-section').style.color = blackTextColor;
                document.querySelector('#slash').style.color = blackTextColor;
                document.querySelector('#btn-minus').style.color = blackTextColor;
                document.querySelector('#btn-minus').style.backgroundColor = yellowBgColor;
                document.querySelector('#btn-plus').style.color = blackTextColor;
                document.querySelector('#btn-plus').style.backgroundColor = yellowBgColor;
                document.querySelector('#btn-a').style.color = blackTextColor;
                document.querySelector('#btn-a').style.backgroundColor = yellowBgColor;
                document.querySelector('#btn-a.icon').style.borderColor = blackTextColor;
                document.querySelector('#btn-plus.icon').style.borderColor = blackTextColor;
                document.querySelector('#btn-minus.icon').style.borderColor = blackTextColor;
                document.querySelector('#userInput').style.color=blackTextColor;
                document.querySelector('#userInput.form-control').style.backgroundColor =yellowBgColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.style.color = blackTextColor;
                });
                document.querySelectorAll('.chatContainer').forEach(button => {
                    button.style.backgroundColor = yellowBgColor;
                })
                // Ändere alle Sections, nicht nur die einzelnen
                document.querySelectorAll('section').forEach(section => {
                    section.style.borderColor = blackTextColor; // Oder je nach ausgewählter Farbe
                });

                document.querySelector('nav').style.borderColor = blackTextColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.addEventListener('mouseenter', () => {
                        link.style.color = blackTextColor; // Farbe beim Hover ändern
                        link.style.backgroundColor = yellowBgColor; // Text ändern (optional)
                    });

                    link.addEventListener('mouseleave', () => {
                        link.style.color = blackTextColor; // Setzt die ursprüngliche Farbe zurück
                        link.style.backgroundColor = yellowBgColor; // Zurücksetzen auf den ursprünglichen Text (anpassen)
                    });
                });



                setHoverEffects('gold', 'black', 'black', 'gold');

            } else if (icon.classList.contains('yellow-inverse')) {
                body.style.backgroundColor = blackBgColor;
                body.style.color = yellowTextColor;

                // Alle relevanten Bereiche bekommen ebenfalls diese Farben
                document.querySelector('.toolbar').style.backgroundColor = blackBgColor;
                document.querySelector('.toolbar').style.color = yellowTextColor;
                document.querySelector('nav').style.backgroundColor=blackBgColor;
                document.querySelector('.rights').style.backgroundColor = blackBgColor;
                document.querySelector('.rights').style.color = yellowTextColor;
                document.querySelector('.language').style.color = yellowTextColor;
                document.querySelector('.support-section').style.backgroundColor = blackBgColor;
                document.querySelectorAll('.question-list li').forEach(section=>section.style.color=yellowTextColor);
                document.querySelectorAll('.question-list li').forEach(section=>section.style.backgroundColor=blackBgColor);
                document.querySelector('.support-section h2').style.color=yellowTextColor;
                document.querySelectorAll('.topic').forEach(link => link.style.color = yellowTextColor);
                document.querySelectorAll('.topic').forEach(link => link.style.backgroundColor = blackBgColor);
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.color = yellowTextColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.backgroundColor = blackBgColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.borderColor = yellowTextColor;})
                document.querySelectorAll('.next-language').forEach(button => {  button.style.backgroundColor =blackBgColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.borderColor = yellowTextColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.color = yellowTextColor;});
                document.querySelectorAll('.faq-item').forEach(button => {  button.style.borderColor = yellowTextColor;});
                document.querySelectorAll('.language').forEach(link => link.style.color = yellowTextColor);
                document.querySelectorAll('.faq-button').forEach(button => {  button.style.color = yellowTextColor;});
                document.querySelectorAll('.answer').forEach(link=>link.style.color = yellowTextColor);
                document.querySelector('#chatContainer').style.color = yellowTextColor;
                document.querySelector('#chatContainer').style.backgroundColor = blackBgColor;
                document.querySelector('#chatContainer').style.borderColor = yellowTextColor;
                document.querySelector('#userInput').style.color = yellowTextColor;
                document.querySelector('#userInput').style.backgroundColor = blackBgColor;
                document.querySelector('.btn').style.borderColor = yellowTextColor;
                document.querySelector('#startStopButton').style.borderColor = yellowTextColor;
                document.querySelector('#userInput.form-control').style.color = yellowTextColor;
                document.querySelector('.intro').style.color = yellowTextColor;
                document.querySelectorAll('.product-section').forEach(section => section.style.color = yellowTextColor);
                document.querySelector('.support-box-list').style.color = yellowTextColor;
                document.querySelector('.support-box-list').style.backgroundColor = blackBgColor;
                document.querySelectorAll('.support-box').forEach(link=>link.style.borderColor = yellowTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.color = yellowTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.backgroundColor = blackBgColor);
                document.querySelectorAll('.app-section').forEach(link=>link.style.color = yellowTextColor);
                document.querySelector('#toggle-button.toggle-button').style.color = yellowTextColor;
                document.querySelectorAll('#toggle-button .bar').forEach(bar => {
                    bar.style.backgroundColor = blackBgColor;
                });
                document.querySelector('nav').style.borderColor = yellowTextColor;
                document.querySelectorAll('nav a').forEach(link => link.style.color = yellowTextColor);
                document.querySelector('.selbstmachen-section').style.color = yellowTextColor;
                document.querySelector('.support-section').style.color = yellowTextColor;
                document.querySelectorAll('.support-box').forEach(box => {
                    box.addEventListener('click', () => {
                        const modal = document.querySelector('#myModal'); // Das Modal-Element
                        const modalContent = modal.querySelector('#modalText'); // Der innere Content

                        const modalText = box.getAttribute('data-modal-text'); // Den Text holen
                        const bgColor = 'black'; // Die Farbe für die innere Box

                        // Inhalt setzen
                        modalContent.innerHTML = modalText;

                        // Nur den inneren Bereich einfärben:
                        modalContent.style.backgroundColor = bgColor;

                        // Modal anzeigen
                        modal.style.display = 'flex';

                    });
                });

                document.querySelector('#btn-plus').style.color = yellowTextColor;
                document.querySelector('.fragen-section').style.color = yellowTextColor;
                document.querySelector('#slash').style.color = yellowTextColor;
                document.querySelector('#btn-minus').style.color = yellowTextColor;
                document.querySelector('#btn-minus').style.backgroundColor = blackBgColor;
                document.querySelector('#btn-plus').style.color = yellowTextColor;
                document.querySelector('#btn-plus').style.backgroundColor = blackBgColor;
                document.querySelector('#btn-a').style.color = yellowTextColor;
                document.querySelector('#btn-a').style.backgroundColor = blackBgColor;
                document.querySelector('#btn-a.icon').style.borderColor = yellowTextColor;
                document.querySelector('#btn-plus.icon').style.borderColor = yellowTextColor;
                document.querySelector('#btn-minus.icon').style.borderColor = yellowTextColor;
                document.querySelector('#userInput').style.color=yellowTextColor;
                document.querySelector('#userInput.form-control').style.backgroundColor =blackBgColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.style.color = yellowTextColor;
                });
                document.querySelectorAll('.chatContainer').forEach(button => {
                    button.style.backgroundColor = blackBgColor;
                })
                // Ändere alle Sections, nicht nur die einzelnen
                document.querySelectorAll('section').forEach(section => {
                    section.style.borderColor = yellowTextColor; // Oder je nach ausgewählter Farbe
                });


                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.addEventListener('mouseenter', () => {
                        link.style.color = yellowTextColor; // Farbe beim Hover ändern
                        link.style.backgroundColor = blackBgColor; // Text ändern (optional)
                    });

                    link.addEventListener('mouseleave', () => {
                        link.style.color = yellowTextColor; // Setzt die ursprüngliche Farbe zurück
                        link.style.backgroundColor = blackBgColor; // Zurücksetzen auf den ursprünglichen Text (anpassen)
                    });
                });



                setHoverEffects('black', 'gold', 'gold', 'black');
            } else if (icon.classList.contains('blue')) {
                body.style.backgroundColor = 'blue';
                body.style.color = 'white';

                // Alle relevanten Bereiche bekommen ebenfalls diese Farben
                document.querySelector('.toolbar').style.backgroundColor = blueBgColor;
                document.querySelector('.toolbar').style.color = whiteTextColor;
                document.querySelector('nav').style.backgroundColor=blueBgColor;
                document.querySelector('.rights').style.backgroundColor = blueBgColor;
                document.querySelector('.rights').style.color = whiteTextColor;
                document.querySelector('.language').style.color = whiteTextColor;
                document.querySelector('.support-section').style.backgroundColor = blueBgColor;
                document.querySelectorAll('.question-list li').forEach(section=>section.style.color=whiteTextColor);
                document.querySelectorAll('.question-list li').forEach(section=>section.style.backgroundColor=blueBgColor);
                document.querySelector('.support-section h2').style.color=whiteTextColor;
                document.querySelectorAll('.support-box').forEach(box => {
                    box.addEventListener('click', () => {
                        const modal = document.querySelector('#myModal'); // Das Modal-Element
                        const modalContent = modal.querySelector('#modalText'); // Der innere Content

                        const modalText = box.getAttribute('data-modal-text'); // Den Text holen
                        const bgColor = 'blue'; // Die Farbe für die innere Box

                        // Inhalt setzen
                        modalContent.innerHTML = modalText;

                        // Nur den inneren Bereich einfärben:
                        modalContent.style.backgroundColor = bgColor;

                        // Modal anzeigen
                        modal.style.display = 'flex';

                    });
                });

                document.querySelectorAll('.topic').forEach(link => link.style.color = whiteTextColor);
                document.querySelectorAll('.topic').forEach(link => link.style.backgroundColor = blueBgColor);
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.color = whiteTextColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.backgroundColor = blueBgColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.borderColor = whiteTextColor;})
                document.querySelectorAll('.next-language').forEach(button => {  button.style.backgroundColor =blueBgColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.borderColor = whiteTextColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.color = whiteTextColor;});
                document.querySelectorAll('.faq-item').forEach(button => {  button.style.borderColor = whiteTextColor;});
                document.querySelectorAll('.language').forEach(link => link.style.color = whiteTextColor);
                document.querySelectorAll('.faq-button').forEach(button => {  button.style.color = whiteTextColor;});
                document.querySelectorAll('.answer').forEach(link=>link.style.color = whiteTextColor);
                document.querySelector('#chatContainer').style.color = whiteTextColor;
                document.querySelector('#chatContainer').style.backgroundColor = blueBgColor;
                document.querySelector('#chatContainer').style.borderColor = whiteTextColor;
                document.querySelector('#userInput').style.color = whiteTextColor;
                document.querySelector('#userInput').style.backgroundColor = blueBgColor;
                document.querySelector('.btn').style.borderColor = whiteTextColor;
                document.querySelector('#startStopButton').style.borderColor = whiteTextColor;
                document.querySelector('#userInput.form-control').style.color = whiteTextColor;
                document.querySelector('.intro').style.color = whiteTextColor;
                document.querySelectorAll('.product-section').forEach(section => section.style.color = whiteTextColor);
                document.querySelector('.support-box-list').style.color = whiteTextColor;
                document.querySelector('.support-box-list').style.backgroundColor = blueBgColor;
                document.querySelectorAll('.support-box').forEach(link=>link.style.borderColor = whiteTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.color = whiteTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.backgroundColor = blueBgColor);
                document.querySelectorAll('.app-section').forEach(link=>link.style.color = whiteTextColor);
                document.querySelector('#toggle-button.toggle-button').style.color = whiteTextColor;
                document.querySelectorAll('#toggle-button .bar').forEach(bar => {
                    bar.style.backgroundColor = blueBgColor;
                });
                document.querySelector('nav').style.borderColor = whiteTextColor;
                document.querySelectorAll('nav a').forEach(link => link.style.color = whiteTextColor);
                document.querySelector('.selbstmachen-section').style.color = whiteTextColor;
                document.querySelector('.support-section').style.color = whiteTextColor;
                document.querySelector('#btn-plus').style.color = whiteTextColor;
                document.querySelector('.fragen-section').style.color = whiteTextColor;
                document.querySelector('#slash').style.color = whiteTextColor;
                document.querySelector('#btn-minus').style.color = whiteTextColor;
                document.querySelector('#btn-minus').style.backgroundColor = blueBgColor;
                document.querySelector('#btn-plus').style.color = whiteTextColor;
                document.querySelector('#btn-plus').style.backgroundColor = blueBgColor;
                document.querySelector('#btn-a').style.color = whiteTextColor;
                document.querySelector('#btn-a').style.backgroundColor = blueBgColor;
                document.querySelector('#btn-a.icon').style.borderColor = whiteTextColor;
                document.querySelector('#btn-plus.icon').style.borderColor = whiteTextColor;
                document.querySelector('#btn-minus.icon').style.borderColor = whiteTextColor;
                document.querySelector('#userInput').style.color=whiteTextColor;
                document.querySelector('#userInput.form-control').style.backgroundColor =blueBgColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.style.color = whiteTextColor;
                });
                document.querySelectorAll('.chatContainer').forEach(button => {
                    button.style.backgroundColor = blueBgColor;
                })
                // Ändere alle Sections, nicht nur die einzelnen
                document.querySelectorAll('section').forEach(section => {
                    section.style.borderColor = whiteTextColor; // Oder je nach ausgewählter Farbe
                });

                document.querySelector('nav').style.borderColor = whiteTextColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.addEventListener('mouseenter', () => {
                        link.style.color = whiteTextColor; // Farbe beim Hover ändern
                        link.style.backgroundColor = blueBgColor; // Text ändern (optional)
                    });

                    link.addEventListener('mouseleave', () => {
                        link.style.color = whiteTextColor; // Setzt die ursprüngliche Farbe zurück
                        link.style.backgroundColor = blueBgColor; // Zurücksetzen auf den ursprünglichen Text (anpassen)
                    });
                });



                setHoverEffects('blue', 'white', 'white', 'blue');
            } else if (icon.classList.contains('white')) {
                body.style.backgroundColor = 'white';
                body.style.color = 'blue';

                // Alle relevanten Bereiche bekommen ebenfalls diese Farben
                document.querySelector('.toolbar').style.backgroundColor = whiteBgColor;
                document.querySelector('.toolbar').style.color = blueTextColor;
                document.querySelector('nav').style.backgroundColor=whiteBgColor;
                document.querySelector('.rights').style.backgroundColor = whiteBgColor;
                document.querySelector('.rights').style.color = blueTextColor;
                document.querySelector('.language').style.color = blueTextColor;
                document.querySelector('.support-section').style.backgroundColor = whiteBgColor;
                document.querySelectorAll('.support-box').forEach(box => {
                    box.addEventListener('click', () => {
                        const modal = document.querySelector('#myModal'); // Das Modal-Element
                        const modalContent = modal.querySelector('#modalText'); // Der innere Content

                        const modalText = box.getAttribute('data-modal-text'); // Den Text holen
                        const bgColor = 'white'; // Die Farbe für die innere Box

                        // Inhalt setzen
                        modalContent.innerHTML = modalText;

                        // Nur den inneren Bereich einfärben:
                        modalContent.style.backgroundColor = bgColor;

                        // Modal anzeigen
                        modal.style.display = 'flex';

                    });
                });
                document.querySelectorAll('.question-list li').forEach(section=>section.style.color=blueTextColor);
                document.querySelectorAll('.question-list li').forEach(section=>section.style.backgroundColor=whiteBgColor);
                document.querySelector('.support-section h2').style.color=blueTextColor;
                document.querySelectorAll('.topic').forEach(link => link.style.color = blueTextColor);
                document.querySelectorAll('.topic').forEach(link => link.style.backgroundColor = whiteBgColor);
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.color = blueTextColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.backgroundColor = whiteBgColor;})
                document.querySelectorAll('.navigation-button').forEach(button => {  button.style.borderColor = blueTextColor;})
                document.querySelectorAll('.next-language').forEach(button => {  button.style.backgroundColor =whiteBgColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.borderColor = blueTextColor;});
                document.querySelectorAll('.next-language').forEach(button => {  button.style.color = blueTextColor;});
                document.querySelectorAll('.faq-item').forEach(button => {  button.style.borderColor = blueTextColor;});
                document.querySelectorAll('.language').forEach(link => link.style.color = blueTextColor);
                document.querySelectorAll('.faq-button').forEach(button => {  button.style.color = blueTextColor;});
                document.querySelectorAll('.answer').forEach(link=>link.style.color = blueTextColor);
                document.querySelector('#chatContainer').style.color = blueTextColor;
                document.querySelector('#chatContainer').style.backgroundColor = whiteBgColor;
                document.querySelector('#chatContainer').style.borderColor = blueTextColor;
                document.querySelector('#userInput').style.color = blueTextColor;
                document.querySelector('#userInput').style.backgroundColor = whiteBgColor;
                document.querySelector('.btn').style.borderColor = blueTextColor;
                document.querySelector('#startStopButton').style.borderColor = blueTextColor;
                document.querySelector('#userInput.form-control').style.color = blueTextColor;
                document.querySelector('.intro').style.color = blueTextColor;
                document.querySelectorAll('.product-section').forEach(section => section.style.color = blueTextColor);
                document.querySelector('.support-box-list').style.color = blueTextColor;
                document.querySelector('.support-box-list').style.backgroundColor = whiteBgColor;
                document.querySelectorAll('.support-box').forEach(link=>link.style.borderColor = blueTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.color = blueTextColor);
                document.querySelectorAll('.support-box').forEach(link => link.style.backgroundColor = whiteBgColor);
                document.querySelectorAll('.app-section').forEach(link=>link.style.color = blueTextColor);
                document.querySelector('#toggle-button.toggle-button').style.color = blueTextColor;
                document.querySelectorAll('#toggle-button .bar').forEach(bar => {
                    bar.style.backgroundColor = whiteBgColor;
                });
                document.querySelector('nav').style.borderColor = blueTextColor;
                document.querySelectorAll('nav a').forEach(link => link.style.color = blueTextColor);
                document.querySelector('.selbstmachen-section').style.color = blueTextColor;
                document.querySelector('.support-section').style.color = blueTextColor;
                document.querySelector('#btn-plus').style.color = blueTextColor;
                document.querySelector('.fragen-section').style.color = blueTextColor;
                document.querySelector('#slash').style.color = blueTextColor;
                document.querySelector('#btn-minus').style.color = blueTextColor;
                document.querySelector('#btn-minus').style.backgroundColor = whiteBgColor;
                document.querySelector('#btn-plus').style.color = blueTextColor;
                document.querySelector('#btn-plus').style.backgroundColor = whiteBgColor;
                document.querySelector('#btn-a').style.color = blueTextColor;
                document.querySelector('#btn-a').style.backgroundColor = whiteBgColor;
                document.querySelector('#btn-a.icon').style.borderColor = blueTextColor;
                document.querySelector('#btn-plus.icon').style.borderColor = blueTextColor;
                document.querySelector('#btn-minus.icon').style.borderColor = blueTextColor;
                document.querySelector('#userInput').style.color=blueTextColor;
                document.querySelector('#userInput.form-control').style.backgroundColor =whiteBgColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.style.color = blueTextColor;
                });
                document.querySelectorAll('.chatContainer').forEach(button => {
                    button.style.backgroundColor = whiteBgColor;
                })
                // Ändere alle Sections, nicht nur die einzelnen
                document.querySelectorAll('section').forEach(section => {
                    section.style.borderColor = blueTextColor; // Oder je nach ausgewählter Farbe
                });

                document.querySelector('nav').style.borderColor = blueTextColor;
                document.querySelectorAll('nav ul li a').forEach(link => {
                    link.addEventListener('mouseenter', () => {
                        link.style.color = blueTextColor; // Farbe beim Hover ändern
                        link.style.backgroundColor = whiteBgColor; // Text ändern (optional)
                    });

                    link.addEventListener('mouseleave', () => {
                        link.style.color = blueTextColor; // Setzt die ursprüngliche Farbe zurück
                        link.style.backgroundColor = whiteBgColor; // Zurücksetzen auf den ursprünglichen Text (anpassen)
                    });
                });



                setHoverEffects('white', 'blue', 'blue', 'white');
            }

            // Ändert die Hintergrundfarbe und den Rand der Textgrößen-Buttons basierend auf der gewählten Farbe
            textSizeButtons.forEach(button => {

                // Wenn der gelbe Button (yellow) geklickt wurde, ändere die Farben
                if (icon.classList.contains('yellow')) {
                    button.style.backgroundColor = 'gold';
                    button.style.borderColor = 'black';

                }
                // Wenn der Button mit der Klasse yellow-inverse geklickt wurde
                else if (icon.classList.contains('yellow-inverse')) {
                    button.style.backgroundColor = 'black';
                    button.style.borderColor = 'gold';
                }
                // Wenn der blaue Button geklickt wurde
                else if (icon.classList.contains('blue')) {
                    button.style.backgroundColor = 'blue';
                    button.style.borderColor = 'white';
                }
                // Wenn der weiße Button geklickt wurde
                else if (icon.classList.contains('white')) {
                    button.style.backgroundColor = 'white';
                    button.style.borderColor = 'blue';
                }
            });
        });
    });

    // Funktion, um Hover-Effekte für die Links im Navigation-Menü zu setzen
    function setHoverEffects(hoverColor, hoverBgColor, defaultColor, defaultBgColor) {

        // Wir gehen alle Links im Navigation-Menü durch
        document.querySelectorAll('nav ul li a').forEach(link => {

            // Zuerst setzen wir die Standardfarben zurück (damit sie immer gleich sind)
            link.style.color = defaultColor;
            link.style.backgroundColor = defaultBgColor;

            // Wenn die Maus über den Link fährt (mouseenter), ändern wir die Farben
            link.onmouseenter = () => {
                link.style.color = hoverColor;
                link.style.backgroundColor = hoverBgColor;
            };

            // Wenn die Maus den Link verlässt (mouseleave), setzen wir die Farben zurück
            link.onmouseleave = () => {
                link.style.color = defaultColor;
                link.style.backgroundColor = defaultBgColor;
            };

            // Damit der Farbwechsel beim Hover schön fließend ist, fügen wir einen Übergangseffekt hinzu
            link.style.transition = "background-color 0.3s ease, color 0.3s ease";
        });
    }


    // Funktion zum Anpassen der Schriftgröße
    function adjustFontSize(buttonId) {

        // Holen alle relevanten Elemente, die ihre Schriftgröße ändern sollen
        const sections = document.querySelectorAll('section');
        const headings = document.querySelectorAll('h1, h2, h3, h4, h5, h6');
        const buttons = document.querySelectorAll('button, .faq-button');
        const links = document.querySelectorAll('a');
        const plusIcons = document.querySelectorAll('.plus');
        const textSizeButtons = [
            document.getElementById('btn-minus'),
            document.getElementById('btn-a'),
            document.getElementById('btn-plus')
        ];

        // Lade den aktuellen Schriftgrößenzähler aus dem lokalen Speicher oder setze ihn auf 0, wenn noch nicht gespeichert
        let counter = parseInt(localStorage.getItem('fontSizeCounter')) || 0;

        // Wenn der Button für "Standard" (btn-a) geklickt wurde
        if (buttonId === 'btn-a') {
            // Setze den Zähler auf 0 zurück
            counter = 0;

            // Lösche alle gespeicherten Werte aus dem lokalen Speicher, um die Standardwerte zu verwenden
            localStorage.removeItem('fontSizeCounter');
            localStorage.removeItem('fontSize');
            localStorage.removeItem('headingFontSize');
            localStorage.removeItem('buttonFontSize');
            localStorage.removeItem('linkFontSize');
            localStorage.removeItem('plusFontSize');

            // Setze die Schriftgrößen auf Standardwerte (größere Schrift als Ausgangspunkt)
            document.body.style.fontSize = "2rem";
            sections.forEach(section => section.style.fontSize = "2rem");
            headings.forEach(heading => heading.style.fontSize = "2.7rem");
            buttons.forEach(button => button.style.fontSize = "2.4rem");
            links.forEach(link => link.style.fontSize = "2.4rem");
            plusIcons.forEach(plus => plus.style.fontSize = "2.6rem");

        }
        // Wenn der Minus-Button (btn-minus) geklickt wurde, verringere den Zähler
        else if (buttonId === 'btn-minus') {
            counter = Math.max(counter - 1, -2); // Begrenzen nach unten
        }
        // Wenn der Plus-Button (btn-plus) geklickt wurde, erhöhe den Zähler
        else if (buttonId === 'btn-plus') {
            counter = Math.min(counter + 1, 2); // Begrenzen nach oben
        }

        // Berechne die neuen Schriftgrößen basierend auf dem Zähler
        const baseFontSize = 2; // Basisgröße in "rem"
        const newFontSize = (baseFontSize + counter * 0.5) + "rem";
        const newHeadingFontSize = (baseFontSize + counter * 0.7) + "rem";
        const newButtonFontSize = (baseFontSize + counter * 0.4) + "rem";
        const newLinkFontSize = (baseFontSize + counter * 0.4) + "rem";
        const newPlusFontSize = (baseFontSize + counter * 0.6) + "rem";

        // Wenn der "Standard" Button nicht geklickt wurde, wende die neuen Schriftgrößen an
        if (buttonId !== 'btn-a') {
            document.body.style.fontSize = newFontSize;
            sections.forEach(section => section.style.fontSize = newFontSize);
            headings.forEach(heading => heading.style.fontSize = newHeadingFontSize);
            buttons.forEach(button => button.style.fontSize = newButtonFontSize);
            links.forEach(link => link.style.fontSize = newLinkFontSize);
            plusIcons.forEach(plus => plus.style.fontSize = newPlusFontSize);
        }

        // Speichern der aktuellen Schriftgröße und Zähler im lokalen Speicher, damit die Einstellungen beim nächsten Laden bestehen bleiben
        if (buttonId !== 'btn-a') {
            localStorage.setItem('fontSizeCounter', counter);
            localStorage.setItem('fontSize', newFontSize);
            localStorage.setItem('headingFontSize', newHeadingFontSize);
            localStorage.setItem('buttonFontSize', newButtonFontSize);
            localStorage.setItem('linkFontSize', newLinkFontSize);
            localStorage.setItem('plusFontSize', newPlusFontSize);
        }

        // Aktualisieren der Button-Anzeige (fügt die "active"-Klasse zum geklickten Button hinzu)
        textSizeButtons.forEach(button => button.classList.remove('active'));
        document.getElementById(buttonId).classList.add('active');
    }

    // Event Listener für Textgrößen-Buttons
    const textSizeButtons = [document.getElementById('btn-minus'), document.getElementById('btn-a'), document.getElementById('btn-plus')];
    textSizeButtons.forEach(button => {

        // Wenn auf einen der Buttons geklickt wird, rufe die Funktion "adjustFontSize" auf
        button.addEventListener('click', () => {
            adjustFontSize(button.id);
        });
    });

    // Reset button für das Standarddesign
    document.getElementById('btn-normal').addEventListener('click', () => {
        body.style.backgroundColor = defaultBgColor;
        body.style.color = defaultTextColor;

        // Alle relevanten Bereiche bekommen ebenfalls diese Farben
        document.querySelector('.toolbar').style.backgroundColor = defaultBgColor;
        document.querySelector('.toolbar').style.color = defaultTextColor;
        document.querySelector('nav').style.backgroundColor=defaultBgColor;
        document.querySelector('.rights').style.backgroundColor = defaultTextColor;
        document.querySelector('.rights').style.color = defaultBgColor;
        document.querySelector('.language').style.color = defaultTextColor;
        document.querySelector('.support-section').style.backgroundColor = defaultBgColor;
        document.querySelectorAll('.support-box').forEach(box => {
            box.addEventListener('click', () => {
                const modalContent = box.getAttribute('data-modal-text'); // HTML-Inhalt aus dem data-modal-text
                const bgColor = defaultBgColor; // Dynamische Hintergrundfarbe


                // Modal-Inhalt setzen
                modalElement.innerHTML = modalContent;

                // Hintergrundfarbe dynamisch ändern
                modalElement.style.backgroundColor = bgColor;

                // Zeige das Modal (sollte eine Funktion wie `showModal()` geben)
                showModal();
            });
        });

        document.querySelectorAll('.question-list li').forEach(section=>section.style.color=defaultTextColor);
        document.querySelectorAll('.question-list li').forEach(section=>section.style.backgroundColor=defaultBgColor);
        document.querySelector('.support-section h2').style.color=defaultTextColor;
        document.querySelectorAll('.topic').forEach(link => link.style.color = defaultTextColor);
        document.querySelectorAll('.topic').forEach(link => link.style.backgroundColor = defaultBgColor);
        document.querySelectorAll('.navigation-button').forEach(button => {  button.style.color = defaultTextColor;})
        document.querySelectorAll('.navigation-button').forEach(button => {  button.style.backgroundColor = defaultBgColor;})
        document.querySelectorAll('.navigation-button').forEach(button => {  button.style.borderColor = defaultTextColor;})
        document.querySelectorAll('.next-language').forEach(button => {  button.style.backgroundColor =defaultBgColor;});
        document.querySelectorAll('.next-language').forEach(button => {  button.style.borderColor = defaultTextColor;});
        document.querySelectorAll('.next-language').forEach(button => {  button.style.color = defaultTextColor;});
        document.querySelectorAll('.faq-item').forEach(button => {  button.style.borderColor = defaultTextColor;});
        document.querySelectorAll('.language').forEach(link => link.style.color = defaultTextColor);
        document.querySelectorAll('.faq-button').forEach(button => {  button.style.color = defaultTextColor;});
        document.querySelectorAll('.answer').forEach(link=>link.style.color = defaultTextColor);
        document.querySelector('#chatContainer').style.color = defaultTextColor;
        document.querySelector('#chatContainer').style.backgroundColor = defaultBgColor;
        document.querySelector('#chatContainer').style.borderColor = defaultTextColor;
        document.querySelector('#userInput').style.color = defaultTextColor;
        document.querySelector('#userInput').style.backgroundColor = defaultBgColor;
        document.querySelector('.btn-success').style.color = defaultTextColor;
        document.querySelector('.btn-success').style.backgroundColor = defaultBgColor;
        document.querySelector('.btn').style.borderColor = defaultTextColor;
        document.querySelector('#startStopButton').style.color = defaultTextColor;
        document.querySelector('#startStopButton').style.backgroundColor = defaultBgColor;
        document.querySelector('#startStopButton').style.borderColor = defaultTextColor;
        document.querySelector('#userInput.form-control').style.color = defaultTextColor;
        document.querySelector('.intro').style.color = defaultTextColor;
        document.querySelectorAll('.product-section').forEach(section => section.style.color = defaultTextColor);
        document.querySelector('.support-box-list').style.color = defaultTextColor;
        document.querySelector('.support-box-list').style.backgroundColor = defaultBgColor;
        document.querySelectorAll('.support-box').forEach(link=>link.style.borderColor = defaultTextColor);
        document.querySelectorAll('.support-box').forEach(link => link.style.color = defaultTextColor);
        document.querySelectorAll('.support-box').forEach(link => link.style.backgroundColor = defaultBgColor);
        document.querySelectorAll('.app-section').forEach(link=>link.style.color = defaultTextColor);
        document.querySelector('#toggle-button.toggle-button').style.color = defaultTextColor;
        document.querySelectorAll('#toggle-button .bar').forEach(bar => {
            bar.style.backgroundColor = defaultBgColor;
        });
        document.querySelector('nav').style.borderColor = defaultTextColor;
        document.querySelector('nav').style.backgroundColor = defaultTextColor;
        document.querySelectorAll('nav a').forEach(link => link.style.color = defaultTextColor);
        document.querySelector('.selbstmachen-section').style.color = defaultTextColor;
        document.querySelector('.support-section').style.color = defaultTextColor;
        document.querySelector('#btn-plus').style.color = defaultTextColor;
        document.querySelector('.fragen-section').style.color = defaultTextColor;
        document.querySelector('#slash').style.color = defaultTextColor;
        document.querySelector('#btn-minus').style.color = defaultTextColor;
        document.querySelector('#btn-minus').style.backgroundColor = defaultBgColor;
        document.querySelector('#btn-plus').style.color = defaultTextColor;
        document.querySelector('#btn-plus').style.backgroundColor = defaultBgColor;
        document.querySelector('#btn-a').style.color = defaultTextColor;
        document.querySelector('#btn-a').style.backgroundColor = defaultBgColor;
        document.querySelector('#btn-a.icon').style.borderColor = defaultTextColor;
        document.querySelector('#btn-plus.icon').style.borderColor = defaultTextColor;
        document.querySelector('#btn-minus.icon').style.borderColor = defaultTextColor;
        document.querySelector('#userInput').style.color=defaultTextColor;
        document.querySelector('#userInput.form-control').style.backgroundColor =defaultBgColor;
        document.querySelectorAll('nav ul li a').forEach(link => {
            link.style.color = defaultTextColor;
        });
        document.querySelectorAll('.chatContainer').forEach(button => {
            button.style.backgroundColor = defaultBgColor;
        })
        // Ändere alle Sections, nicht nur die einzelnen
        document.querySelectorAll('section').forEach(section => {
            section.style.borderColor = defaultTextColor; // Oder je nach ausgewählter Farbe
        });

        document.querySelector('nav').style.borderColor = blackBgColor;
        document.querySelectorAll('nav ul li a').forEach(link => {
            link.addEventListener('mouseenter', () => {
                link.style.color = defaultTextColor; // Farbe beim Hover ändern
                link.style.backgroundColor = defaultBgColor; // Text ändern (optional)
            });

            link.addEventListener('mouseleave', () => {
                link.style.color = defaultTextColor; // Setzt die ursprüngliche Farbe zurück
                link.style.backgroundColor = defaultBgColor // Zurücksetzen auf den ursprünglichen Text (anpassen)
            });
        });



        setHoverEffects('black', 'white', 'white', 'black');

    // Entferne aktive Klassen von Farb- und Schriftgrößen-Buttons
        colorButtons.forEach(icon => icon.classList.remove('active', 'color-active'));
        textSizeButtons.forEach(button => button.classList.remove('active'));

        // Setze Styles zurück
        textSizeButtons.forEach(button => button.removeAttribute('style'));

        // Entferne aktive Klasse vom Reset-Button selbst
        document.getElementById('btn-normal').classList.remove('active', 'color-active');

        colorButtons.forEach(icon => icon.classList.remove('active', 'color-active'));


    });

});
const toggleButton = document.getElementById('toggle-button');
const listContainer = document.getElementById('list-container');

// Event Listener hinzufügen, um das Menü umzuschalten
toggleButton.addEventListener('click', function() {
    listContainer.classList.toggle('active'); // Fügt die 'active' Klasse hinzu oder entfernt sie
});

// Funktion, um das Popup zu schließen
function closePopup() {
    // Macht das Popup und das Overlay unsichtbar
    document.getElementById('popup').style.display = 'none';
    document.getElementById('popup-overlay').style.display = 'none';

// Entfernt die Event Listener, die hinzugefügt wurden, um Memory-Leaks zu vermeiden
    document.removeEventListener('keydown', handleEscapeKey);
    document.getElementById('popup-overlay').removeEventListener('click', closePopup);
}

// Funktion, die das Popup schließt, wenn die Escape-Taste gedrückt wird
function handleEscapeKey(event) {
    // Wenn die Escape-Taste gedrückt wird
    if (event.key === 'Escape') {
        closePopup();
    }
}

// Funktion, um die Bild-Galerie und das Modal einzurichten
function setupImageGallery() {

    // Modal und Bild-Elemente holen
    var modal = document.getElementById("myModalImage");
    var modalImg = document.getElementById("modalImg");
    var closeBtn = document.getElementById("closeBtnImage");

    // Alle Bilder der Galerie holen
    var galleryImages = document.querySelectorAll(".gallery-img");

    // Wenn eines der Bilder angeklickt wird
    galleryImages.forEach(function(image) {
        image.addEventListener("click", function() {
            modal.style.display = "flex"; // Modal sichtbar machen und als Flexbox anzeigen
            modalImg.src = this.src; // Setze das Bild im Modal auf das angeklickte Bild
        });
    });

    // Schließen des Modals, wenn der Schließknopf geklickt wird
    closeBtn.addEventListener("click", function() {
        modal.style.display = "none";
    });

    // Schließen des Modals, wenn außerhalb des Bildes geklickt wird
    modal.addEventListener("click", function(event) {
        if (event.target === modal) {
            modal.style.display = "none";
        }
    });

    // Schließen des Modals, wenn außerhalb des Bildes geklickt wird
    window.addEventListener("keydown", function(event) {
        if (event.key === "Escape") {
            modal.style.display = "none";
        }
    });
}

// Funktion, um das Text-Modal einzurichten
function setupTextModal() {

    // Modal und Text-Elemente holen
    var modal = document.getElementById("myModal");
    var modalText = document.getElementById("modalText");
    var closeBtn = document.getElementById("closeBtn");

    // Alle Support-Boxen (Text-Container) holen
    var supportBoxes = document.querySelectorAll(".support-box");

    // Wenn eine Support-Box angeklickt wird
    supportBoxes.forEach(function(box) {
        box.onclick = function() {

            // Hole den Text, der im data-modal-text Attribut gespeichert ist
            var textContent = box.getAttribute("data-modal-text");

            // Setze den Text im Modal
            modalText.innerHTML = textContent;

            // Zeige das Modal
            modal.style.display = "block";
        };
    });

    // Wenn der Schließknopf geklickt wird
    closeBtn.onclick = function() {
        modal.style.display = "none";
    };

    // Wenn außerhalb des Modals geklickt wird, schließt sich das Modal
    window.onclick = function(event) {
        if (event.target == modal) {
            modal.style.display = "none";
        }
    };

    // Wenn die Escape-Taste gedrückt wird, schließt sich das Pop-up
    window.addEventListener("keydown", function(event) {
        if (event.key === "Escape") {
            modal.style.display = "none";
        }
    });
}

// Ruft die Funktion auf, um das Modal zu aktivieren
setupTextModal();


// Funktion, um ein Kapitel anzuzeigen und den Screenreader zu aktivieren
function zeigeKapitel(kapitelId) {

    // Zuerst alle Kapitel ausblenden, damit nur das gewählte sichtbar ist
    document.querySelectorAll('.product-section, .selbstmachen-section, .app-section, .support-section, .fragen-section')
        .forEach(kapitel => {
            kapitel.classList.remove('aktiv'); // Entfernt die 'aktiv' Klasse (Kapitel wird ausgeblendet)
            kapitel.setAttribute("aria-hidden", "true"); // Setzt das Attribut aria-hidden auf "true" (Kapitel wird für den Screenreader unsichtbar)
            kapitel.setAttribute("tabindex", "-1"); // Entfernt den Tabindex, damit das Kapitel nicht fokussiert werden kann
        });

    // Das aktuelle Kapitel auswählen und aktiv machen
    let aktuellesKapitel = document.getElementById(kapitelId);// Holt das Kapitel mit der ID, die übergeben wurde

    if (!aktuellesKapitel) {
        console.warn("Kapitel-ID nicht gefunden:", kapitelId); // Gibt eine Warnung aus, wenn die ID nicht gefunden wird
        return; // Beendet die Funktion, wenn das Kapitel nicht existiert
    }

    // Alle Kapitel jetzt aktivieren (sichtbar machen und fokussierbar)
    document.querySelectorAll('.product-section, .selbstmachen-section, .app-section, .support-section, .fragen-section')
        .forEach(kapitel => {

            kapitel.classList.add('aktiv'); // Fügt die 'aktiv' Klasse hinzu (Kapitel wird sichtbar)
            kapitel.setAttribute("aria-hidden", "false"); // Macht das Kapitel für den Screenreader sichtbar
            kapitel.setAttribute("tabindex", "0"); // Macht das Kapitel fokussierbar

        });

    // Fokus nach einer kleinen Verzögerung setzen, damit der Screenreader den Abschnitt vorliest
    setTimeout(() => aktuellesKapitel.focus(), 100); // Verzögerung, um sicherzustellen, dass der Screenreader den Fokus bekommt
}

// Funktion, um zum nächsten Kapitel zu wechseln und es dem Screenreader vorzulesen
function vorleseNaechstesKapitel(kapitelId) {
    // Mapping von Kapiteln, welches Kapitel das nächste ist
    const naechstesKapitelMapping = {
        'dasprodukt': 'dieapp',
        'dieapp': 'selbstmachen',
        'selbstmachen': 'support',
        'support': 'fragen',
        'fragen': 'dasprodukt'  // Zurück zum ersten Abschnitt
    };

    // Das nächste Kapitel basierend auf der aktuellen Kapitel-ID holen
    const naechstesKapitel = naechstesKapitelMapping[kapitelId];
    const naechstesKapitelElement = document.getElementById(naechstesKapitel);

    // Wenn das nächste Kapitel nicht existiert, gibt es eine Warnung aus
    if (!naechstesKapitelElement) {
        console.warn("Kapitel-ID nicht gefunden:", naechstesKapitel);
        return; // Beendet die Funktion, wenn das nächste Kapitel nicht gefunden wurde
    }


    // Alle Kapitel ausblenden, bevor das nächste Kapitel sichtbar gemacht wird
    document.querySelectorAll('.product-section, .selbstmachen-section, .app-section, .support-section, .fragen-section')
        .forEach(kapitel => {
            kapitel.classList.remove('aktiv'); // Entfernt die 'aktiv' Klasse (Kapitel wird ausgeblendet)
            kapitel.setAttribute("aria-hidden", "true"); // Macht das Kapitel für den Screenreader unsichtbar
            kapitel.setAttribute("tabindex", "-1"); // Entfernt den Tabindex, damit das Kapitel nicht fokussiert werden kann
        });

    // Das nächste Kapitel sichtbar und fokussierbar machen
    naechstesKapitelElement.classList.add('aktiv'); // Fügt die 'aktiv' Klasse hinzu (Kapitel wird sichtbar)
    naechstesKapitelElement.setAttribute('aria-hidden', 'false'); // Macht das Kapitel für den Screenreader sichtbar
    naechstesKapitelElement.setAttribute("tabindex", "0"); // Macht das Kapitel fokussierbar

    // Fokus nach einer Verzögerung setzen, damit der Screenreader das nächste Kapitel liest
    setTimeout(() => naechstesKapitelElement.focus(), 100); // Verzögerung, um sicherzustellen, dass der Screenreader den Fokus bekommt
}

// Funktion, um die Antwort in einem FAQ anzuzeigen oder zu verstecken
function toggleAnswer(button) {

    // Sucht das "faq-item" (die ganze Frage mit Antwort) in der Nähe des Buttons
    const faqItem = button.closest(".faq-item");

    // Holt die Antwort, die sich innerhalb des "faq-item" befindet
    const answer = faqItem.querySelector(".answer");

    // Holt das Icon (Pluszeichen) aus dem Button
    const icon = button.querySelector(".plus");

    // Überprüft, ob die Antwort gerade sichtbar ist
    const isOpen = answer.style.display === "block"; // Wenn die Antwort sichtbar ist, ist "isOpen" true

    // Schließt alle offenen Antworten, indem wir die Anzeige auf "none" setzen
    document.querySelectorAll(".answer").forEach(ans => ans.style.display = "none");

    // Setzt das Icon in allen FAQ-Einträgen zurück auf "+"
    document.querySelectorAll(".plus").forEach(icon => icon.textContent = "+");

    // Wenn die Antwort aktuell nicht sichtbar ist (also "isOpen" false), dann zeige sie an
    if (!isOpen) {

        answer.style.display = "block"; // Antwort sichtbar machen
        icon.textContent = "-"; // Icon auf "-" ändern

    }
    // Wenn die Antwort schon sichtbar war, passiert nichts, weil sie vorher geschlossen wurde
}

// Wenn die Seite vollständig geladen ist, wird diese Funktion aufgerufen
document.addEventListener("DOMContentLoaded", function() {
    setupImageGallery(); // Ruft die Funktion "setupImageGallery" auf, um die Bildergalerie einzurichten
});


