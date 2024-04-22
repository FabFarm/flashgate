document.addEventListener('DOMContentLoaded', function () {
    var radios = document.getElementsByName('flash-type');
    var customPatternDiv = document.querySelector('.custom-pattern');
    var morseCodeDisplay = document.getElementById('morse-code-display');
    var longFlashButton = document.querySelector('.long-flash');
    var shortFlashButton = document.querySelector('.short-flash');
    var clearButton = document.querySelector('.clear');

    // Function to update the morse code display
    function updateMorseCodeDisplay(symbol) {
        morseCodeDisplay.textContent += symbol;
    }

    // Event listeners for the buttons
    longFlashButton.addEventListener('click', function () {
        updateMorseCodeDisplay('-'); // Dash for long flash
    });

    shortFlashButton.addEventListener('click', function () {
        updateMorseCodeDisplay('.'); // Dot for short flash
    });

    clearButton.addEventListener('click', function () {
        morseCodeDisplay.textContent = ''; // Clear the display
    });

    // Event listener for radio buttons to show/hide custom pattern div
    radios.forEach(function (radio) {
        radio.addEventListener('change', function () {
            if (document.getElementById('custom-pattern').checked) {
                customPatternDiv.classList.add('active');
            } else {
                customPatternDiv.classList.remove('active');
            }
        });
    });
});
var uploadPatternButton = document.querySelector('.upload-pattern'); // New button reference

// Function to upload the flash pattern
function uploadFlashPattern() {
    var morseCodeDisplay = document.getElementById('morse-code-display');
    var pattern = morseCodeDisplay.textContent.split('').map(function (symbol) {
        return symbol === '-'; // True for long flash (dash), False for short flash (dot)
    });
    console.log(pattern); // For demonstration, replace with actual upload logic
}

// Event listener for the new button
uploadPatternButton.addEventListener('click', uploadFlashPattern);