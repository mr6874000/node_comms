# 📡 node_comms

> **Offline Chat System Using ESP8266 + WebSockets**  
> 🆘 Perfect for emergencies, disasters, or just cool offline tinkering!

---

## 🌍 What is this?

`node_comms` is a peer-to-peer-style **offline chat system** that runs entirely on an ESP8266 microcontroller. It's designed for situations where **cell towers are down**, **Wi-Fi is unavailable**, or you're just **off-grid** 🏕️. Your ESP8266 becomes a Wi-Fi hotspot that allows nearby devices to connect and chat via a sleek web interface using WebSockets.

💡 **No internet needed. No server required. Just power up and chat!**

---

## 🚀 Features

- 📡 Creates its own Wi-Fi network (AP mode)
- 💬 Real-time chat using WebSockets
- 👥 Random usernames with user join/leave notifications
- 📜 User list display
- 📲 Mobile-friendly responsive UI
- 🔊 Audio beep when you get a new message
- 🔁 Captive portal redirection (just open browser after connecting)
- 🛠️ No external server or internet needed

---

## 🧠 Use Cases

- 🚨 **Disaster scenarios** (earthquakes, hurricanes, power outages)
- 🏕️ Off-grid camping or hiking trips
- 📵 Zero-bar zones or remote work areas
- 🧪 Hobby projects or local mesh chat experiments

---

## 📦 Requirements

- 🔌 [ESP8266 NodeMCU](https://github.com/mr6874000/node_comms.git)
- ⚡ Arduino IDE or PlatformIO
- 📚 Libraries:
  - `ESP8266WiFi`
  - `ESP8266WebServer`
  - `DNSServer`
  - `WebSocketsServer` *(Install via Library Manager or PlatformIO: "WebSockets" by Markus Sattler)*

---

## 🛠️ How to Set It Up

### 👉 Using PlatformIO (recommended)
```bash
git clone https://github.com/mr6874000/node_comms.git
cd node_comms
pio run -t upload
```

### 👉 Using Arduino IDE

1. Open the `.ino` or `.cpp` file.
2. Install the required libraries.
3. Select your board (e.g., NodeMCU 1.0).
4. Click ✅ Upload.

---

## 📱 How to Use

1. 🔌 Power up the ESP8266.
2. 📶 Connect your phone/laptop to the `ANON` Wi-Fi network.
3. 🌐 Open any browser – you'll be redirected to the chat interface.
4. 💬 Start chatting with others connected to the same ESP!

---

## 🧑‍💻 Screenshots

> _(Coming soon — you can help by adding some!)_

---

## 📜 License

MIT — Free to use, hack, and improve!  
Just don't use it for evil ☠️.

---

## 💡 Ideas for the Future

- 🔐 Add optional password protection
- 🌐 Mesh communication between multiple ESPs
- 🧭 GPS integration for rescue coordination
- 🔋 Low-power sleep mode


## 🌟 Star this repo if you find it cool!

```
git clone https://github.com/mr6874000/node_comms.git
```

🛰️ Stay connected — even when the world isn’t.


