# RingClock
Automatic FastLED Ring clock using NTP and parsing APIs for GeoIP location and Timezone offset

A simple internet-enabled clock driven by NTP and GeoIP to give an automatic offset from UTP for your location.

I used a WeMos D1 Mini, but you could use any ESP8266 module or any ethernet shield or wireless shield, with suitable modifications.

I used a WS2812B 24-LED ring and awesome FastLED LED library for controlling the WS LEDs. https://github.com/FastLED/FastLED/releases

It uses JSON to parse the API data from http://ip-api.com and https://timezonedb.com, but in theory you could use any GEO-IP database and timezone reconciliation service. These are both free for hobby use, don't sell your details to the Mob and seem to not have sent me one single piece of spam in the last year.

You will need to sign up for an API with https://timezonedb.com but it's more to track you don't abuse the system than to assist any Nigerian princes looking to get a partner or to help them repatriate the millions of dollars they have stuck in the bank.
I'm sure my clock drawing code is overly complicated and I need to check my maths. But, for all that it's been running over a week and the arms (pixels) move as they should and the time is correct. IF you power it off and back on, or reset it, the LEDs turn on at the correct time. 

Improvements: cut down on the unnecessary code bloat; work out the clock arm movement maths properly; build in an alarm function using a webpage or MQTT; Flash the LEDs when time sync occurs.
