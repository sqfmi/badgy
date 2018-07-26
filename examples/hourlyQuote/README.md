# Hourly Random Quote

This code sample demonstrates parsing JSON data from an API endpoint, as well as **deep sleep mode** for conserving power. To enter OTA mode, hold down **center** when powering on Badgy until the IP address is displayed. The code performs the following steps:

## Operation
1. Connect to WiFi on boot
2. If the **center** button is pressed, go into OTA mode (hold button till screen updates)
3. Otherwise, perform an `HTTP GET` request to the random quote API endpoint
4. Parse the returned JSON and display the quote
5. Enter deep sleep for approxmiately one hour
6. The sleep time can be modified at **line 59** in microseconds e.g. 60e6 = 60 seconds

## Instructions
- Download code sample and required libraries, and compile/upload in the Arduino IDE (1.8.5)
- Download the pre-compiled binary [hourlyQuote.bin](https://github.com/sqfmi/badgy/raw/master/examples/hourlyQuote/hourlyQuote.bin) and upload using OTA
