# Weather Station

This code sample demonstrates a simple weather station that displays the current & forecasted weather using the [openweathermap](https://openweathermap.org) APIs.

## Operation
1. Connect to WiFi on boot
2. If the **center** button is pressed, go into OTA mode (hold button till screen updates)
3. Otherwise, perform an `HTTP GET` request to the OpenWeatherMap API
4. Parse the returned JSON and display the weather
5. Enter deep sleep for approxmiately one hour

## Instructions
- Download code sample and required libraries, and compile/upload in the Arduino IDE (1.8.5)
- Sometimes the API will return an error if the service is unavailable, simply restart Badgy to try again
