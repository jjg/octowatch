#Octowatch
Use your Pebble watch to monitor (and eventually, control) 3D printers that use the excellent [Octoprint](https://github.com/foosel/OctoPrint) host software.

![Octowatch screenshot](https://raw.github.com/jjg/octowatch/master/pebble-screenshot_2014-02-08_13-39-13.png "Sample screenshot")


##Setup

For now you'll need the Pebble developer tools to install Octowatch.  Once it's a little more foregiving (and I have time to gather the necissary visual assets) it will be avaliable in the Pebble App Store.  In the meantime, here's what you need to do to set things up:

###Install the Pebble Dev Tools
Go to https://developer.getpebble.com/2/getting-started/ and follow the necissary steps for your platform.  What you want to do is download the SDK and be capable of running the "pebble" tool to compile and install applications.

###Download the Octowatch source
If you haven't already, install Git (Github has some great instructions for your platform here: https://help.github.com/articles/set-up-git).  Once Git is setup, open up a console and clone the Octowatch repository as such:

     git clone https://github.com/jjg/octowatch.git
     
###Build & Install Octowatch
Change to the octowatch directory created in the previous step and type:

    pebble build
    
This will compile Octowatch and a lot of stuff will fly by the screen.  It should end with something like "'build' finished successfully (0.510s)".  If not, go back to step 1 and double-check your Pebble SDK setup.

If Octowatch compiled correctly you're ready to install it.  In order to install Apps you need to enable developer mode in the Pebble app on the device you have your Pebble Watch connected to.  This page has a nice summary of how to get this setup: https://developer.getpebble.com/2/getting-started/hello-world/

Once you have developer mode enabled, note the IP address of your device and use the command below to install Octowatch on your Pebble:

    pebble install --phone 10.0.1.56
    
(replace 10.0.1.56 with the IP address of your device)

###Configure the Octoprint settings
In order to monitor your printer you have to tell Octowatch a little about it.  You'll need the following:

*  Name or IP address of the device running Octoprint
*  Port that Octoprint is listening on (usually 80 or 5000)
*  The API key for your Octoprint server

The API key take a little fiddling with Octoprint to get setup, but the process is documented here: https://github.com/foosel/OctoPrint/wiki/REST-API

Once you have the above collected, open the Pebble App on your phone, select Octowatch and then tap the "SETTINGS" button with the little gear next to it.

![Pebble App screenshot](https://raw.github.com/jjg/octowatch/master/pebbleapp.png "Pebble App Config")

This will bring up the (yes, ugly) configuration screen for Octowatch.  Enter the information you gathered above into these fields and tap "save".  

![Octowatch config screenshot](https://raw.github.com/jjg/octowatch/master/octowatchconfig.png "Octowatch config")

In a few seconds your Pebble should be displaying the status of your printer!

##Troubleshooting
Octowatch is considered Alpha software at the moment.  Things are likely to change and bugs can be expected.  If you run into trouble, double-check the settings as right now Octowatch won't tell you if you entred something wrong, it simply won't work correctly (this will be improved in future versions :)

If you have questions, feedback or bug reports, visit the Issues section of the repository (https://github.com/jjg/octowatch/issues) to see what is on the roadmap and to provide feedback.  If you'd like to work on improving Octowatch pull requests are always welcome!