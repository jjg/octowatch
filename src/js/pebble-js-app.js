// configuration
var octoprint_host = '10.0.1.14';
var octoprint_port = '5000';
var octoprint_api_key = '72314DBEB2E0475CA0F98DB25B17FB86';

function fetchPrinterStatus() {

	var octoprint_api_url = 'http://' + octoprint_host + ':' + octoprint_port + '/api/state?apikey=' + octoprint_api_key;
	
	var response;
	var req = new XMLHttpRequest();
	req.open('GET', octoprint_api_url, true);
	req.onload = function(e) {
	if (req.readyState == 4) {
		if(req.status == 200) {
			response = JSON.parse(req.responseText);
			
			var filename = response.job.filename;
			
			var remaining = response.progress.printTimeLeft;
			var remaining_string = '00:00';
			
			if(remaining){
				remaining_string = remaining.toString().substring(0,5);
			}
			
			var remaining = remaining_string;
			
			var prog_percent = Math.round(Number(response.progress.progress) * 100);
			prog_percent = prog_percent + '% complete';
			var progress = prog_percent.toString();
			
			console.log(filename);
			console.log(remaining);
			console.log(progress);
			
			Pebble.sendAppMessage({
			    "0":filename,
			    "1":remaining,
			    "2":progress}, appMessageACK, appMessageNACK);
			}
		}
	}
	req.send(null);
}

function appMessageACK(e){
	console.log('message delivered!');
}

function appMessageNACK(e){
	console.log('message failed!');
	console.log(e.error);
}

Pebble.addEventListener("ready",
    function(e) {
    	console.log("got ready event");
    }
);

Pebble.addEventListener("appmessage",
	function(e) {
		console.log('received appMessage:');                        
		console.log(e.type);
		console.log(e.payload.octoprint_command);
		
		if(e.payload.octoprint_command == "update"){
			fetchPrinterStatus();
		}
});

Pebble.addEventListener("showConfiguration",
	function(){
		console.log('running configuration');
	}
);
                        
