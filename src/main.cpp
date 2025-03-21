#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>  // Install via PlatformIO: "WebSockets" by Markus Sattler

const char *ssid = "ANON";

ESP8266WebServer server(80);
DNSServer dnsServer;
WebSocketsServer webSocketServer(81); // Separate port for WebSocket

// Function to generate a random username
String generateUsername() {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    String username = "User";
    for (int i = 0; i < 6; ++i) { // 6-digit random part
        username += alphanum[random(sizeof(alphanum) - 1)];
    }
    return username;
}

// Structure to hold client information
struct ClientInfo {
    uint8_t id;
    String username;
};
std::vector<ClientInfo> clients; // Keep track of connected clients


void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED: {
            Serial.printf("[%u] Disconnected!\n", num);
            // Remove client from the list
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].id == num) {
                    clients.erase(clients.begin() + i);
                    break;
                }
            }
             // Notify other clients about the disconnection
            for(auto& client : clients) {
                if (client.id != num) { // Don't send to the disconnected client
                  String message = "{\"type\": \"userLeft\", \"userId\": " + String(num) + "}";
                  webSocketServer.sendTXT(client.id, message);

                }
            }
            break;
        }
        case WStype_CONNECTED: {
            IPAddress ip = webSocketServer.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // Generate and assign a username
            String username = generateUsername();
            clients.push_back({num, username}); // Store client info

            // Send the assigned username to the client
             String usernameMessage = "{\"type\": \"username\", \"username\": \"" + username + "\"}";
             webSocketServer.sendTXT(num, usernameMessage);

               // Send the list of all connected users to newly connected client.
            String userList = "{\"type\": \"userList\", \"users\": [";
             for (size_t i = 0; i < clients.size(); ++i) {
                userList += "{\"id\": " + String(clients[i].id) + ", \"username\": \"" + clients[i].username + "\"}";
                if (i < clients.size() - 1) {
                    userList += ",";
                }
            }
            userList += "]}";
              webSocketServer.sendTXT(num, userList);

               // Notify other clients about the new connection
            for(auto& client : clients) {
                if (client.id != num) {
                     String newUserMessage = "{\"type\": \"newUser\", \"id\": " + String(num) + ", \"username\": \"" + username + "\"}";
                     webSocketServer.sendTXT(client.id, newUserMessage);
                }
            }


            break;
        }
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            // Broadcast the message to all other clients
            for (auto& client : clients) {
                if (client.id != num) { // Don't send back to the sender
                    webSocketServer.sendTXT(client.id, payload, length);
                }
            }
            break;

        case WStype_BIN: // Not used in this example, but good practice to handle
            Serial.printf("[%u] get binary length: %u\n", num, length);
            break;
    
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
        case WStype_PING:   // add these so the warnings go away.  
        case WStype_PONG:
            break;
    }
}

void handleRoot() {
    String page = R"rawliteral(
<!DOCTYPE html>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Chat</title>
    <style>
        /* (Your CSS styles - no changes needed) */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f4f4f4;
            color: #333;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            padding: 20px;
        }

        h1 {
            color: #007bff;
            margin-bottom: 10px;
            text-align: center;
        }

        h2 {
          margin-top: 20px;
          margin-bottom: 10px;
          color: #007bff;
        }

        #container {
            width: 100%;
            max-width: 600px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
            overflow: hidden;
            display: flex;
            flex-direction: column;
        }

        #chat {
            height: 300px;
            overflow-y: scroll;
            padding: 15px;
            border-bottom: 1px solid #eee;
        }

        #chat div {
            margin-bottom: 10px;
            padding: 8px 12px;
            border-radius: 5px;
            background-color: #f0f0f0;
            word-wrap: break-word;
        }
      #chat div.my-message {
        background-color: #DCF8C6;
        text-align: right;
      }

        #input-area {
            display: flex;
            padding: 10px;
        }

        #message {
            flex-grow: 1;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
            margin-right: 10px;
            font-size: 16px;
        }

        button {
            padding: 10px 15px;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            transition: background-color 0.2s ease;
        }

        button:hover {
            background-color: #0056b3;
        }

        #users {
            list-style: none;
            padding: 15px;
        }

        #users li {
            padding: 8px 12px;
            border-bottom: 1px solid #eee;
        }
        #users li:last-child{
          border-bottom: none;
        }

        @media (max-width: 480px) {
            #container{
              border-radius: 0;
            }
            #chat {
                height: 200px;
            }
            #message {
                font-size: 14px;
            }
            button {
                font-size: 14px;
            }
             body{
              padding: 0;
             }
        }
    </style>
</head>
<body>
    <div id="container">
    <h1>ESP8266 WebRTC Chat</h1>
    <p>Your username: <span id="username"></span></p>
    <div id="chat"></div>
    <div id="input-area">
      <input type="text" id="message" placeholder="Type your message...">
      <button id="sendButton" onclick="sendMessage()">Send</button> </div>

    <h2>Connected Users</h2>
    <ul id="users"></ul>
  </div>

    <script>
        let ws;
        let myUsername;
        let myId;
        const chatDiv = document.getElementById('chat');
        const messageInput = document.getElementById('message');
        const usernameSpan = document.getElementById('username');
        const userListUl = document.getElementById('users');
        const sendButton = document.getElementById('sendButton'); // Get the button element

        // Function to create and play a beep sound
        function playBeep(frequency = 520, duration = 200, volume = 0.1) {
          try {
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const oscillator = audioContext.createOscillator();
            const gainNode = audioContext.createGain();

            oscillator.type = 'sine'; // 'sine', 'square', 'sawtooth', 'triangle'
            oscillator.frequency.setValueAtTime(frequency, audioContext.currentTime);
            gainNode.gain.setValueAtTime(volume, audioContext.currentTime);
            gainNode.gain.exponentialRampToValueAtTime(0.0001, audioContext.currentTime + duration / 1000);

            oscillator.connect(gainNode);
            gainNode.connect(audioContext.destination);

            oscillator.start();
            oscillator.stop(audioContext.currentTime + duration / 1000);
          } catch (error) {
                console.error("Error playing sound:", error);
          }
        }

        function connectWebSocket() {
            ws = new WebSocket('ws://' + window.location.hostname + ':81/');
            ws.onopen = () => {
                console.log('WebSocket connected');
            };

            ws.onmessage = (event) => {
                const data = JSON.parse(event.data);

                if (data.type === 'username') {
                    myUsername = data.username;
                    usernameSpan.textContent = myUsername;
                } else if (data.type === 'userList') {
                    data.users.forEach(user => {
                        if (!document.getElementById('user-' + user.id)) {
                            addUserToList(user.id, user.username);
                        }
                    });
                } else if (data.type === 'newUser') {
                    if (!document.getElementById('user-' + data.id)) {
                        addUserToList(data.id, data.username);
                    }
                } else if (data.type === 'userLeft') {
                    removeUserFromList(data.userId);
                } else if (data.type === 'message') {
                    displayMessage(data.username, data.text);
                    // Play beep *only* if it's not our own message
                    if (data.username !== myUsername) {
                        playBeep(); // Use the beep function
                    }
                }
            };

            ws.onclose = () => {
                console.log('WebSocket disconnected');
                setTimeout(connectWebSocket, 5000);
            };

            ws.onerror = (error) => {
                console.error('WebSocket error:', error);
            };
        }

        function sendMessage() {
            const text = messageInput.value;
            if (text && ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ type: 'message', username: myUsername, text: text }));
                messageInput.value = '';
                displayMessage(myUsername, text, true);
            }
        }

        function addUserToList(id, username) {
            const li = document.createElement('li');
            li.id = 'user-' + id;
            li.textContent = username;
            userListUl.appendChild(li);
        }

        function removeUserFromList(id) {
            const userElement = document.getElementById('user-' + id);
            if (userElement) {
                userListUl.removeChild(userElement);
            }
        }

        function displayMessage(username, text, isMyMessage = false) {
            const messageElement = document.createElement('div');
            messageElement.textContent = `${username}: ${text}`;
            if (isMyMessage) {
                messageElement.classList.add('my-message');
            }
            chatDiv.appendChild(messageElement);
            chatDiv.scrollTop = chatDiv.scrollHeight;
        }

        connectWebSocket();

        // Listen for keydown events on the message input
        messageInput.addEventListener('keydown', (event) => {
            if (event.key === 'Enter') {
                event.preventDefault(); // Prevent default form submission behavior
                sendButton.click();     // Trigger the button's click event
            }
        });

    </script>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", page);
}




void setup() {
    Serial.begin(115200);
    randomSeed(micros()); // Initialize random number generator

    // Set up Access Point
    WiFi.softAP(ssid);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(myIP);

    // Setup DNS Server
    dnsServer.start(53, "*", myIP);


    // Set up WebSocket Server
    webSocketServer.begin();
    webSocketServer.onEvent(webSocketEvent);

    // Serve Webpage + Captive Portal Redirect
    server.on("/", handleRoot);
    server.onNotFound([]() {
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    webSocketServer.loop();
}