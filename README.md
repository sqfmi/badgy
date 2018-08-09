# Badgy

<a href="https://www.tindie.com/products/squarofumi/badgy-iot-badge/"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104"></a>

<img src="/website/img/badgytechnical.jpg" />
<img src="/website/img/badgy_stock_photo.jpg" />

# Getting Started

By default your Badgy displays the welcome message when powered on. To upload new firmware, you'll need to first connect it to your WiFi network.

1. Power your Badgy through USB or a rechargeable battery, or both. Make sure the power switch is in the off position (up)

**WARNING: The battery charging IC does not have reverse polarity protection! Check the polarity before inserting the battery, and make sure you are using a high quality Micro-USB cable to avoid potentially burning the IC.**

2. Hold down the center button and slide the power switch on (down). **Note: On older firmware versions it is not necessary to hold down the center button and OTA is on by default**
3. The screen should now update letting you know to connect to the "Badgy AP" network. You can now let go of the center button
4. Using your mobile phone, connect to the "Badgy AP" WiFi network
5. Follow the captive portal instructions to select your desired WiFi connection
6. Once connected, Badgy is ready to recieve new firmware! Go to http://*YOUR_IP_ADDRESS*:8888/update to upload your new firmware
7. WiFi credentials are automatically saved onboard, your Badgy will automatically connect to your selected network the next time you perform an update

Check out the examples folders for various Arduino code samples, we're constantly adding more!
