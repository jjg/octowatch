function fetchPrinterStatus() {
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://10.0.1.14:5000/api/state?apikey=72314DBEB2E0475CA0F98DB25B17FB86", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        //console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var filename, temp, progress;
        filename = response.job.filename;
        temp = response.temperatures.extruder.current.toString();
        var prog_percent = Number(response.progress.progress) * 100;
        progress = prog_percent.toString();
        
        console.log(filename);
        console.log(temp);
        console.log(progress);
        
        Pebble.sendAppMessage({
            "filename":filename,
            "temperature":temp,
            "progress":progress});
      }
    }
  }
  req.send(null);
}

Pebble.addEventListener("ready",
    function(e) {
    	console.log("got ready");
        fetchPrinterStatus();
    }
);

Pebble.addEventListener("appmessage",
                        function(e) {
                          //window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          fetchPrinterStatus();
                          console.log(e.type);
                          console.log(e.payload.filename);
                          console.log("message!");
                        });
                        
Pebble.addEventListener("webviewclosed",
                                     function(e) {
                                     console.log("webview closed");
                                     console.log(e.type);
                                     console.log(e.response);
                                     });
