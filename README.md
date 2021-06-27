# esp8266-keypad-mqtt
Connect a Keypad to an MQTT server with an ESP8266

By default it will send whatever number you punch in to /keypad/KEY on your MQTT broker when you press * on the keypad.

# Setup

I did it in PlatformIO to install the needed libraries use 

```
pio lib install
```

# Compile

```
pio run
```

# Upload to Wemos D1 mini

```
pio run --target upload 
```

# Usage

Restructured the code from the fork and did some clean up. 

Details for your setup now go into ``secrets.h`` You will need to copy the ``secrets.h.dist`` to ``secrets.h`` and fill in your details.

Also added the ability to connect to a FritzBox and ring connected phones.

More documentation to follow.
