# arduino projects

In the "assignment project" is the following laser sentry project located:
![Alt text](IMG_20180125_153854.jpg?raw=true "Title")
The motion is shown in white on a black background. Basically it captures the first frame which is then stored as the background, and then every following frame is compared with this background frame for every pixel, and if the pixel differentiates more than a specified threshold it is shown as motion (white pixel).
The red vertical line indicates the most motion on teh horizantol axis, and this is the horizontal pixel position(converted to servo degree) send to the arduino over Serial. And this where the laser aims at.
![Alt text](IMG_20180125_153937_001.jpg?raw=true "Title")
The how the person in the upper right corner is being targetted
The servo position can be overwritten by the potentiometer connected to a wemos d1 R2 mini with an esp8266 module (over wifi). So that no longer the laser aims at the most motion, but just at the manual user input.
