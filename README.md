# 🌱 Greenhouse Automated System

Welcome to the Greenhouse Automated System project! This system is designed to automate and optimize the environmental conditions of a greenhouse using real-time sensor data and remote control functionalities.

## 🚀 Features

- **Real-Time Monitoring**  
  Monitor soil moisture, temperature, and humidity through Firebase integration.

- **Automated Control**  
  Automatically control fans and water pumps based on sensor readings to maintain optimal conditions.

- **Remote Management**  
  Manage and control the system remotely using a web-based interface built with React.

## 🛠️ Technologies Used

- **Hardware:**  
  - ESP32 microcontroller  
  - Soil moisture, temperature, and humidity sensors  
  - Water pump and fan

- **Software:**  
  - Arduino IDE for firmware development  
  - Firebase for real-time data storage and synchronization  
  - Next Js for the web-based dashboard  
  - Firebase_ESP_Client library for ESP32-Firebase communication

## 💡 Final Outcome
![](https://github.com/Kavi-ya/Greenhouse-1/blob/be2f35ea0af683b997e2607dd7e72a8d87ef6459/hardware/Final%20Project.jpg)

## 📁 Project Structure
```
Greenhouse_Automated_System/
├── src/                    # Source code for ESP32
├── app/                    # React project for web interface
├── hardware/               # Circuit diagrams and hardware setup
└── README.md               # Project overview
```

## 🚨 How to Use

### Prerequisites

- Arduino IDE with necessary ESP32 board configuration.
- Firebase project set up with the Firebase Realtime Database.
- React installed for the web interface.

### Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Kavi-ya/Greenhouse-Automation.git
   ```
2. **Flash the ESP32:**
   - Open the `src` folder in Arduino IDE.
   - Configure Wi-Fi and Firebase credentials in the code.
   - Upload the code to the ESP32.

3. **Run the Web Dashboard:**
   ```bash
   cd app name
   npm run dev
   npm run build
   firebase  deploy   
   ```

4. **Access the Local Web Dashboard:**  
   Open your browser and navigate to `http://localhost:3000`.

## 📖 Documentation
For detailed setup instructions and system documentation, if you want more details refer to my FireNext tutorial...

## 📄 License
This project is licensed under the MIT License.

## 🤝 Contributing
We welcome contributions!

## 📞 Contact

For questions or suggestions, feel free to reach out:  
- **Email:** k.kavindusahan.s@gmail.com  
- **GitHub:** [Kavi-ya](https://github.com/Kavi-ya)

## 📷Images
![](https://github.com/Kavi-ya/Greenhouse-1/blob/82c86a54f7c9ee80f37cad1398558d1fe3025d9b/hardware/6.jpeg)
![](https://github.com/Kavi-ya/Greenhouse-1/blob/82c86a54f7c9ee80f37cad1398558d1fe3025d9b/hardware/After%20Soldering.jpeg)
![](https://github.com/Kavi-ya/Greenhouse-1/blob/82c86a54f7c9ee80f37cad1398558d1fe3025d9b/hardware/Without%20Sensors.jpg)


