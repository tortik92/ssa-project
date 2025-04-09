
// Erlaubte Themen für Fragen
const allowedTopics = ["SoundLeap", "App", "Selbermachen", "Fragen", "Produkt", "Matten"]; // Erlaubte Themen

// Prüft, ob Eingabe einem erlaubten Thema entspricht
function isAllowedQuestion(input) {
    return allowedTopics.some(topic => input.toLowerCase().includes(topic.toLowerCase()));
}

// Begrüßung erkennen
function isGreeting(input) {
    const greetings = ["hallo", "hi", "wie gehts", "wie geht es dir", "guten tag", "hey", "was geht"];
    return greetings.some(greeting => input.toLowerCase().includes(greeting));
}

// Fügt eine Chatbot-Nachricht hinzu und liest sie bei Bedarf vor
function addChatbotMessage(message) {
    const chatContainer = document.getElementById("chatContainer");
    const chatbotMessageElement = document.createElement("div");
    chatbotMessageElement.classList.add("message", "chatbotMessage");
    chatbotMessageElement.textContent = message;
    chatContainer.appendChild(chatbotMessageElement);
    chatContainer.scrollTop = chatContainer.scrollHeight;

    if (document.getElementById("speechSwitch").checked) {
        const speech = new SpeechSynthesisUtterance(message);
        window.speechSynthesis.speak(speech);
    }
}


// Nachricht absenden und verarbeiten
window.sendMessage = async function() {
    const inputField = document.getElementById("userInput");
    const input = inputField.value.trim();
    inputField.value = "";  // Eingabefeld sofort leeren

    if (!input) return;

    const chatContainer = document.getElementById("chatContainer");

    // Benutzer-Nachricht anzeigen
    const userMessageElement = document.createElement("div");
    userMessageElement.classList.add("message", "userMessage");
    userMessageElement.textContent = input;
    chatContainer.appendChild(userMessageElement);
    chatContainer.scrollTop = chatContainer.scrollHeight;

    // Begrüßung
    if (isGreeting(input)) {
        const responseMessage = "Hallo! Wie kann ich dir bei SoundLeap helfen?";
        addChatbotMessage(responseMessage);
        return;
    }

    // Nicht erlaubte Frage
    if (!isAllowedQuestion(input)) {
        const responseMessage = "Dazu kann ich leider nichts sagen.";
        addChatbotMessage(responseMessage);
        return;
    }

    // Ladeanzeige
    const loadingMessageElement = document.createElement("div");
    loadingMessageElement.classList.add("message", "chatbotMessage");
    loadingMessageElement.textContent = "Lädt...";
    chatContainer.appendChild(loadingMessageElement);

    try {
        const response = await fetch("https://openrouter.ai/api/v1/chat/completions", {
            method: "POST",
            headers: {
                "Authorization": "Bearer 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",
                "HTTP-Referer": "http://localhost:63342/product-website/testbase.html?_ijt=r2df9aof374oomt20rurfr6a6j", // Optional. Site URL for rankings on openrouter.ai.
                "X-Title": "soundleap", // Optional. Site title for rankings on openrouter.ai.
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                "model": "deepseek/deepseek-r1:free",
                "messages": [
                    {
                        "role": "system",
                        "content": `
                            Du bist ein KI-Assistent für das Projekt SoundLeap.SoundLeap entwickelt spezielle Matten mit Sensoren und Lautsprechern, die blinden und sehbeeinträchtigten Personen ermöglichen, Sport zu treiben. Diese Matten sind über WLAN vernetzt und erzeugen einfache Biepstöne in hohen und tiefen Frequenzen, um Orientierung zu bieten. Es gibt keine Melodien oder komplexen Sounds. Die Nutzer können über eine barrierefreie App verschiedene Spiele auswählen und anpassen.

                        Wie funktioniert SoundLeap?
                        
                        Die Matten kommunizieren über WLAN und spielen unterschiedliche einfache Biepstöne ab, um den Spielern zu helfen, sich im Raum zu orientieren.
                        
                        Über eine Smartphone-App können Nutzer Spiele auswählen und ihre Einstellungen anpassen.
                        
                        Die Töne helfen den Spielern, die Position der Matten zu erkennen und aktiv am Spiel teilzunehmen.
                        
                        Welche Spiele gibt es?SoundLeap bietet verschiedene Spielmodi, darunter:
                        
                        Memory: Töne merken und die richtigen Matten in der richtigen Reihenfolge betreten.
                        
                        Reaktionsspiel: Schnell auf das richtige Geräusch reagieren und die passende Matte treffen.
                        
                        Himmel & Hölle: Springen und Überspringen bestimmter Matten.
                        
                        Eigene Spiele erstellen: Mit einer einfachen Programmiersprache können Nutzer eigene Spielabläufe gestalten.
                        
                        Wie sieht die App aus?Unsere App ist barrierefrei und einfach zu bedienen:
                        
                        Registrierung mit einem Code – Nutzer geben einen 5-stelligen Code ein, um sich zu verbinden.
                        
                        Spielauswahl – Es gibt verschiedene Spielmodi wie Memory, Reaktionsspiel und Himmel & Hölle.
                        
                        Spiel konfigurieren – Nutzer können die Anzahl der Matten, den Soundtyp und weitere Optionen anpassen.
                        
                        Matten lokalisieren – Eine visuelle Darstellung zeigt die Position der Matten.
                        
                        Spiel starten – Sobald alles eingestellt ist, beginnt das Spiel.
                        
                        Die App ist für Screenreader optimiert, hat klare Beschriftungen und Alternativtexte und ermöglicht eine einfache Navigation.
                        
                        Selbstmachen: Eigene Spiele programmierenMit der SoundLeap-Programmiersprache können Nutzer eigene Spiele erstellen. Dazu:
                        
                        Öffne den Bereich "Neues Spiel erstellen" in der App.
                        
                        Beschreibe dein Spiel mit einfachen Textbefehlen.
                        
                        Definiere Aktionen und Reaktionen der Matten.
                        
                        Teste dein Spiel, bevor du es speicherst und spielst.
                        
                        Wie kann ich dir helfen?Ich beantworte Fragen zu SoundLeap, den Matten, den Spielen und der App. Zum Beispiel:
                        
                        Wie funktionieren die Matten?
                        
                        Welche Spiele gibt es?
                        
                        Wie verbinde ich die Matten mit der App?
                        
                        Wie erstelle ich mein eigenes Spiel?
                        
                        Wie kann ich mein Spiel testen, bevor ich es veröffentliche?
                        
                        Falls du technische Probleme hast, kannst du mich auch zu Themen wie Download, Login und Support fragen.
                        
                        Freundliche InteraktionIch reagiere auch auf allgemeine Fragen wie "Hallo!" oder "Wie geht’s?" mit kurzen, freundlichen Antworten.Falls jemand fragt, ob du mir bitte etwas mehr dazu sagen kannst oder ähnliches, sollst du auf die letzte gesendete Nachricht zugreifen.Falls deine Frage nichts mit SoundLeap zu tun hat, sage ich höflich:"Dazu kann ich leider nichts sagen.""  
                                                `
                    },
                    {
                        "role": "user",
                        "content": input
                    }
                ]
            })
        });

        const data = await response.json();
        const botMessage = data.choices?.[0]?.message?.content || "Keine Antwort erhalten.";

        chatContainer.removeChild(loadingMessageElement);
        addChatbotMessage(botMessage);

        conversationHistory.push({ sender: "bot", message: botMessage });

    } catch (error) {
        chatContainer.removeChild(loadingMessageElement);
        addChatbotMessage("Fehler: API konnte nicht erreicht werden.");
    }
};

// ENTER-Taste zum Absenden
document.addEventListener("DOMContentLoaded", function() {
    document.getElementById("userInput").addEventListener("keydown", function(event) {
        if (event.key === "Enter") {
            event.preventDefault();  // Verhindert das Standardverhalten (z.B. ein Zeilenumbruch)
            sendMessage();  // Sendet die Nachricht
        }
    });
});

// Sprachaufnahme-Funktion (Speech-to-Text)
function setupSpeechRecognition() {
    const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
    const recognition = new SpeechRecognition();
    recognition.continuous = false;
    recognition.interimResults = false;
    let listening = false;

    recognition.onresult = (event) => {
        let transcript = event.results[0][0].transcript;
        document.getElementById("userInput").value = transcript; // Setzt den erkannten Text ins Eingabefeld
    };

    recognition.onerror = (event) => {
        console.log("Speech recognition error", event);
    };

    recognition.onend = () => {
        listening = false;
        document.getElementById("startStopButton").textContent = "Start Listening";
    };

    document.getElementById("startStopButton").addEventListener("click", () => {
        if (listening) {
            recognition.stop();
            listening = false;
            document.getElementById("startStopButton").textContent = "Start Listening";
        } else {
            recognition.start();
            listening = true;
            document.getElementById("startStopButton").textContent = "Stop Listening";
        }
    });
}

