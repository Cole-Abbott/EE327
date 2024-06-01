var websocket = null;
var localhost = "";
var buttonClicked = false;
var webcamState = false;

var imageTimes = [];
var fpsLength = 10;

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
	// calculate the FPS
	var currentTime = new Date().getTime();
	imageTimes.push(currentTime);
	if (imageTimes.length > fpsLength) {
		imageTimes.shift();
	}
	var fps = imageTimes.length / ((currentTime - imageTimes[0]) / 1000);
	document.getElementById("fps").innerHTML = "FPS: " + fps.toFixed(2);
	
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

  // forward button stop when released
document.getElementById("forward").addEventListener("mousedown", function() {
	websocket.send("m: 0, 250");
});
document.getElementById("forward").addEventListener("mouseup", function() {
	websocket.send("m: 0, 0");
});

// backward button stop when released
document.getElementById("back").addEventListener("mousedown", function() {
	websocket.send("m: 0, -200");
});
document.getElementById("back").addEventListener("mouseup", function() {
	websocket.send("m: 0, 0");
});

// left button stop when released
document.getElementById("left").addEventListener("mousedown", function() {
	websocket.send("m: 2, 0");
});
document.getElementById("left").addEventListener("mouseup", function() {
	websocket.send("m: 0, 0");
});


// right button stop when released
document.getElementById("right").addEventListener("mousedown", function() {
	websocket.send("m: -2, 0");
});
document.getElementById("right").addEventListener("mouseup", function() {
	websocket.send("m: 0, 0");
});


document.getElementById("stop").addEventListener("mousedown", function() {
	websocket.send("m: 0, 0");
});

document.getElementById("mode").addEventListener("click", function() {
	websocket.send("t");
	websocket.send("m: 0, 0");
});

// Open Websocket as soon as page loads
window.addEventListener("load", init, false);