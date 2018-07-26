# Hourly Random Quote

This code sample demonstrates parsing JSON data from an API endpoint, as well as **deep sleep mode** for conserving power. To enter OTA mode, hold down **center** when powering on Badgy until the IP address is displayed. The code performs the following steps:

## Operation
1. Connect to WiFi on boot
2. If the **center** button is pressed, go into OTA mode.
3. Otherwise, perform an `HTTP GET` request to the random quote API endpoint
4. Parse the returned JSON and display the quote.
5. Enter deep sleep for approxmiately one hour.
6. The sleep time can be modified at **line 59** in microseconds e.g. 10e6 = 10 seconds

## Instructions
