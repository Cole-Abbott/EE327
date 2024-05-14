var websocket = null;
var localhost = "";
var buttonClicked = false;
var webcamState = false;

// Initialize the websocket
function init() {
	if(window.location.hostname != "") {
		localhost = window.location.hostname;
	}

	doConnect();
}

function doConnect() { // makes a connection and defines callbacks
	if (webcamState == false) {
		webcamState = true;
		writeToScreen("Connecting to ws://" + localhost + "/websocket");
        
        const ipAddress = '10.105.1.116'; // Replace with your desired IP address
        websocket = new WebSocket(`ws://${ipAddress}:8888/websocket`);

		websocket.onopen = function(evt) {
			onOpen(evt)
		};
		websocket.onclose = function(evt) {
			onClose(evt)
		};
		websocket.onmessage = function(evt) {
			onMessage(evt)
		};
		websocket.onerror = function(evt) {
			onError(evt)
		};
	} else {
		writeToScreen("Disconnecting ...");
		websocket.close();
		webcamState = false;
	}
}

function onOpen(evt) { // when handshake is complete:
	writeToScreen("Connected.");

}

function onClose(evt) { // when socket is closed:
	writeToScreen("Disconnected. Error: " + evt);

}

function onMessage(msg) {
	//*** Display a new timestamp ***//
	
	// Get the image just taken from WiFi chip's RAM.
	var image = document.getElementById('image');
	var reader = new FileReader();
	reader.onload = function(e) {
		var img_test = new Image();
		img_test.onload = function(){image.src = e.target.result;};
		img_test.onerror = function(){;};
		img_test.src = e.target.result;
	};
	reader.readAsDataURL(msg.data);
	console.log("Image received");
}

function onError(evt) { // when an error occurs
	websocket.close();
	writeToScreen("Websocket error");

}


// Function to display to the message box
 function writeToScreen(message)
  {
	console.log(message);
  }

// Open Websocket as soon as page loads
window.addEventListener("load", init, false);