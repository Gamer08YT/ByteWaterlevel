# API

### API Route

The API Route is available under `http://<host>/api` as **POST Method** via **JSON Objects**.

#### API Response

The API Route provides some Debug Responses

##### Not implemented

This Error occurs if you use an API Type which is unknown or was deprecated.

```{"type":"error","message":"Not implemented"}```

##### Invalid Request

This Error occurs if your requesting JSON Frame is wrong.

```{"type":"error","message":"Invalid request"}```

##### No JSON payload provided

This Error occurs if the Software has problems parsing your JSON Request.

```{"type":"error","message":"No JSON payload provided"}```

### Relais

The Board comes with two Relais (max 10A at 24 VDC) wich can be controlled via the HTTP POST API.

#### Set Relais State

The Relais on the Board can be toggled via the Relai Type, the Channel is the Relai (eq. Relais 1 or Relais 2).

In this Example the Relais on Channel 1 is turned on.

```json
{
  "type": "relais",
  "channel": 1,
  "state": true
}
```

#### Switch Relais on for X Seconds.

The Relais on the Board can be turned on for a specified Time and then automatically turned off.

In this Example the Relais on Channel 1 is turned on for 1 Minute.

```json
{
  "type": "relais",
  "channel": 1,
  "duration": 60,
  "state": true
}
```

### Status

You can retrieve the Device Status by using the Status Type.

#### Sensors

```json
{
  "type": "status"
}
```

This will return the current Status:

```json
{
  "type": "success",
  "channels": [
    1,
    0
  ],
  "adc": "1.10",
  "cpu": "10.5",
  "frequency": 160
}
```

### Info

You can retrieve the Device Info by using the Info Type.

```json
{
  "type": "info"
}
```

This will return the current Status:

```json
{
  "type": "success",
  "wifi": {
    "client": {
      "ssid": "",
      "password": ""
    },
    "ap": {
      "ssid": "BYTELEVEL",
      "password": "BYTESTORE"
    }
  },
  "mqtt": {
    "state": false,
    "host": "",
    "port": 1883,
    "user": "",
    "password": ""
  },
  "admin": {
    "state": false,
    "user": "",
    "password": ""
  },
  "hardware": {
    "led": true
  },
  "firmware": "1.0.1"
}
```