# API

### API Route

The API Route is available under `http://<host>/api` as **POST Method** via **JSON Objects**.

### Relais

The Board comes with two Relais (max 10A at 24 VDC) wich can be controlled via the HTTP POST API.

#### Set Relais State.

The Relais on the Board can be toggled via the Relai Type, the Channel is the Relai (eq. Relais 1 or Relais 2).

```json
{
  "type": "relais",
  "channel": 1,
  "state": true
}
```