# Weather Station

This code sample demonstrates a simple weather station that displays the current & forecasted weather using the [openweathermap](https://openweathermap.org) APIs.

Note that because this example puts the ESP into a deep sleep if you want to reprogram it via the USB then you have to manually stop and restart it using the slider switch at the moment just prior to the upload starting.

## Required Libraries
- See the base required libraries in the root Readme for this repository
- GxEPD 2.x
- ArduinJSON 5.x
- Time 1.5.0 - You can find the repo [here] (https://github.com/PaulStoffregen/Time)).  

## Instructions
- Download code sample and required libraries, and compile/upload in the Arduino IDE (1.8.5)
- Sometimes the API will return an error if the service is unavailable, simply restart Badgy to try again
