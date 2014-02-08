#Octowatch
Use your Pebble watch to monitor (and eventually, control) 3D printers that use the excellent [Octoprint](https://github.com/foosel/OctoPrint) host software.

![Octowatch screenshot](https://raw.github.com/jjg/octowatch/master/pebble-screenshot_2014-02-08_01-49-18.png "Sample screenshot")

For now you'll need the Pebble developer tools to install Octowatch.  Pebble doesn't allow Javascript Framework apps to be installed manually from the .pbw files, so you either build it yourself or download it from the Pebble App Store.  I'm working on making it avaliable via app store, but you have to supply a lot of graphics and stuff, so that's going to take a bit longer.

As mentioned above, Octowatch uses the new Pebblekit Javascript Framework, so no "companion app" is needed and it is compatible with both Android and iOS platforms.  Configuration (Octoprint server api key, etc.) is done via the Pebble app, but bear in mind it is very rough at the moment (for example, if you provide invalid server information there's no error message, it just doesn't work :).

Currently Octowatch provides basic status info for active print jobs.  Essential printer control is in the works, however it will require you to run a cutting-edge dev build of Octoprint to work, so if you're not feeling adventurous perhaps stick to just monitoring your printer for now.