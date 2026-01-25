# Reset

### Manual Reset

To reset your Device please turn power off, then short IO6 with 3V3 and turn Power back on, after that please reboot the
device again.

![img.png](assets/img/reset.png)

### Reset via API

You can reset the Device via API (**If the login details have been forgotten, only a manual reset is possible**).

To call the API Channel you need to send a POST Request to `http://<host>/api`.

For more Details have a look in the API Docs <a href="./API.md">API.md</a>.

```json
{
  "type": "reset"
}
```