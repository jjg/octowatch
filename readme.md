#Octowatch
Use your Pebble watch to monitor (and eventually, control) 3D printers that use the excellent [Octoprint](https://github.com/foosel/OctoPrint) host software.

![Octowatch screenshot](https://raw.github.com/jjg/octowatch/master/pebble-screenshot_2014-02-08_01-49-18.png "Sample screenshot")

The good news is that Octowatch uses the new Pebblekit Javascript Framework, so no "companion app" is needed and it is compatible with both Android and iOS platforms.  The bad news is that currently configuration requires modification of the source code (unless your printer happens to have the exact same settings as mine :).  Pebble has provided a clever way to allow the Javascript Frameworks apps to be configured, but I haven't implemented that yet (it's on the list).

Currently Octowatch provides basic status info for active print jobs.  Essential printer control is in the works, however it will require you to run a cutting-edge dev build of Octoprint to work, so if you're not feeling adventurous perhaps stick to just monitoring your printer for now.

Once configuration is added and some other odds-and-ends have been polished I'll be releasing the app on the Pebble app store, but it will always be avaliable here in source code form as well; I just want to make it easy to use for folks who maybe don't want to compile it themselves.

