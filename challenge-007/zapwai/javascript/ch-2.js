let dict, word1, word2, ladder;
const ourParagraph = document.getElementById('ourParagraph');
const ourParagraph2 = document.getElementById('ourParagraph2');
const fileInput = document.getElementById('fileInput');
fileInput.addEventListener('change', handleFileUpload);
const ourWord = document.getElementById('ourWord');
ourWord.addEventListener('change', onWordChange);
const ourWord2 = document.getElementById('ourWord2');
ourWord2.addEventListener('change', onWord2Change);

function onWordChange(event) {
    word1 = event.target.value;
    display();
}

function onWord2Change(event) {
    word2 = event.target.value;
    display();
}

function handleFileUpload(event) {
    const file = event.target.files[0];
    if (!file) {
	console.error('No file selected');
	return;
    }
    const reader = new FileReader();
    reader.onload = function(e) {
	const content = e.target.result;
	const words = content.split(/\s+/);
	dict = words;
	getLadder();
	display2();
    };
    reader.readAsText(file);
}

function getLadder() {
    console.log("dictionary loaded");
}

function display() {
    ourParagraph.innerHTML = `word1: ${word1}<br>word2: ${word2}<br>`;
}

function display2() {
    ourParagraph2.innerHTML = `${dict}`;
}

