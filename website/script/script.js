const main = document.querySelector("main");

if (document.getElementById("top-buttons") !== null) {
    main.style.marginTop = "50px";
}

if (document.getElementById("bottom-buttons") !== null) {
    main.style.maxHeight = "calc(100vh - 100px)";
}