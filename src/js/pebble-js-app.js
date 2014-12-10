var previous_state = null;

function fetchPrinterStatus() {

  var octoprint_host = localStorage.getItem('octoprinthost');
  var octoprint_port = localStorage.getItem('octoprintport');
  var octoprint_api_key = localStorage.getItem('octoprintapikey');

  var octoprint_api_url = 'http://' + octoprint_host + ':' + octoprint_port + '/api/job?apikey=' + octoprint_api_key;

  var response;
  var req = new XMLHttpRequest();
  req.open('GET', octoprint_api_url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        
        // init default display values
        var filename = 'no file selected';
        var remaining_string = '00:00';
        var job_status = 'Connected';

        // parse response
        response = JSON.parse(req.responseText);

        // only update if a job is loaded
        if(response.job.file.name){
          
          // update filename
          filename = response.job.file.name;

          // update time remaining
          var remaining_seconds = 0;
          if(response.progress.printTimeLeft){
            remaining_seconds = response.progress.printTimeLeft;
          } else {
            remaining_seconds = response.job.estimatedPrintTime;
          }
          var remaining_time = secondsToTime(remaining_seconds);
          var formatted_hours = (remaining_time.h < 10) ? ('0' + remaining_time.h) : remaining_time.h;
          var formatted_minutes = (remaining_time.m < 10) ? ('0' + remaining_time.m) : remaining_time.m;
          remaining_string = formatted_hours + ':' + formatted_minutes;
          
          // update job status
          var prog_percent = Math.round(Number(response.progress.completion));
          job_status = response.state;
          if(prog_percent > 0){
            job_status = job_status + ' ' + prog_percent + '%';
          }
          
          // Issue: too long filenames break messaging, so
          // for now, trimming them to 20chrs max
          filename = filename.substring(0,20) + '...';
        }
        
          // debug
          console.log(filename);
          console.log(remaining_string);
          console.log(job_status);
        
          // update status on watch
          Pebble.sendAppMessage({
            "0":filename,
            "1":remaining_string,
            "2":job_status}, appMessageACK, appMessageNACK);
  
          // send notification when done
          if(previous_state === "Printing" && response.state === "Operational" && response.progress.completion === 100.0){
            var d = new Date();
            Pebble.showSimpleNotificationOnPebble('Printing Complete', filename + ' finished printing at ' + d);
            previous_state = response.state;
          } else {
            previous_state = response.state;
          }
      }
    } else {
      console.log('something went wrong, ' + req.status);
    }
  };
  
  req.send(null);
}

function pausePrinter() {

  var octoprint_host = localStorage.getItem('octoprinthost');
  var octoprint_port = localStorage.getItem('octoprintport');
  var octoprint_api_key = localStorage.getItem('octoprintapikey');
  
  var octoprint_api_url = 'http://' + octoprint_host + ':' + octoprint_port + '/api/control/job';
  
  // debug
  console.log('calling ' + octoprint_api_url + ' to pause current print job');
  
  var response;
  var req = new XMLHttpRequest();
  req.open('POST', octoprint_api_url, true);
  req.data = 'x-api-key=' + octoprint_api_key + '&body={"command":"pause"}';
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        
        response = JSON.parse(req.responseText);
        
        console.log(response);
        
        Pebble.sendAppMessage({"3":"paused"}, appMessageACK, appMessageNACK);
        }
      } else {
      
        console.log('something went wrong, ' + req.status);
      }
    };
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
    
    if(e.payload.octoprint_command == "pause"){
      // toggle pause state
      //pausePrinter();
    }
    
    if(e.payload.octoprint_command == "cancel"){
      // cancel the print job
    }
});

Pebble.addEventListener("showConfiguration",
  function(){
    console.log('running configuration');
  
      var octoprint_host = localStorage.getItem('octoprinthost'),
        octoprint_port = localStorage.getItem('octoprintport'),
        octoprint_api_key = localStorage.getItem('octoprintapikey'),
        uri;
      uri = 'https://rawgithub.com/jjg/octowatch/master/configure.html?host=' + encodeURIComponent(octoprint_host) + '&port=' + encodeURIComponent(octoprint_port) + '&key=' + encodeURIComponent(octoprint_api_key);
      Pebble.openURL(uri);
  }
);

Pebble.addEventListener("webviewclosed", function(e) {

    console.log('saving settings');

    try{
      var options = JSON.parse(decodeURIComponent(e.response));
    
      localStorage.setItem('octoprinthost', options.server_host);
      localStorage.setItem('octoprintport', options.server_port);
      localStorage.setItem('octoprintapikey', options.server_api_key);
    } catch(e) {
      console.log('settings not updated');
    }
  }
);



// utilities
function secondsToTime(secs)
{
    secs = Math.round(secs);
    var hours = Math.floor(secs / (60 * 60));

    var divisor_for_minutes = secs % (60 * 60);
    var minutes = Math.floor(divisor_for_minutes / 60);

    var divisor_for_seconds = divisor_for_minutes % 60;
    var seconds = Math.ceil(divisor_for_seconds);

    var obj = {
        "h": hours,
        "m": minutes,
        "s": seconds
    };
    return obj;
}

