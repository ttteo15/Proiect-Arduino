# ESP-Sensor-Controller  
**ESP32/ESP8266 Project with Sensors and Web Interface**  

This project leverages the ESP32 microcontroller to create a web-based server for monitoring and controlling a sensor and actuator system. It integrates a temperature and humidity sensor (DHT11), a light sensor, an IR sensor, and an LED, providing real-time data visualization and control through a web interface accessible from any device connected to the same network.

## **Hardware Components**  
- **Microcontroller**: ESP32  
- **Sensors**:  
  - DHT11 (Temperature and Humidity)  
  - Analog Light Sensor  
  - Digital IR Sensor  
- **Actuators**:  
  - LED  
  - Button for local LED control  
- **Display**: 16x2 LCD with I2C for local data output  

## **Key Features**  
- **Real-Time Monitoring**:  
  - Temperature and humidity data from the DHT11 sensor are displayed on both the LCD and the web interface.  
  - Ambient light levels are measured using the analog light sensor and shown on the web interface.  
  - IR sensor detects object presence and updates its status on the web interface.  
- **LED Control**:  
  - The LED can be toggled on or off via the web interface or a physical button.  
- **Web Interface**:  
  - Built using HTML and JavaScript, the interface updates sensor data every 2 seconds without requiring a page refresh.  
  - Includes a toggle switch for remote LED control.  
- **Debounce Implementation**:  
  - Ensures reliable button press detection by filtering out noise and unintended multiple triggers.  

## **Technical Details**  
- **WiFi Connectivity**:  
  - The ESP device connects to a WiFi network, enabling remote access to the web server.  
  - The IP address is displayed in the serial monitor for easy access.  
- **Asynchronous Web Server**:  
  - Utilizes the ESPAsyncWebServer and AsyncTCP libraries for efficient handling of web requests.  
- **Sensor Integration**:  
  - DHT11 sensor data is read using the DHT library.  
  - Light and IR sensor values are read via analog and digital pins, respectively.  
- **LCD Display**:  
  - The 16x2 LCD, connected via I2C, shows temperature and humidity readings locally.  

## **Libraries Used**  
- DHT for reading temperature and humidity from the DHT11 sensor.  
- LiquidCrystal_PCF8574 for controlling the I2C LCD.  
- ESPAsyncWebServer and AsyncTCP for managing the web server.  

## **Setup Instructions**  
1. **WiFi Configuration**:  
   - Update the 'ssid' and 'password' variables in the code with your WiFi credentials.  
2. **Upload the Code**:  
   - Use Arduino IDE or PlatformIO to compile and upload the code to your ESP32/ESP8266.  
   - Ensure the correct board and port are selected.  
3. **Access the Web Interface**:  
   - After the ESP connects to WiFi, the IP address will be displayed in the serial monitor.  
   - Enter this IP address in a web browser to access the interface.  

## **How It Works**  
- The web interface automatically updates sensor data (temperature, humidity, light level, and IR status) every 2 seconds.  
- The LED can be controlled via the toggle switch on the web interface or the physical button.  
- Changes to the LED state are reflected immediately on both the hardware and the web interface.  
## **Code Highlights**  
- **HTML/JavaScript Interface**:  
  - The web page is designed to be responsive and user-friendly, with real-time updates using AJAX.  
  - The LED toggle switch sends an HTTP request to update the LED state.  
- **Debounce Logic**:  
  - Ensures accurate detection of button presses by filtering out noise and bounce effects.  
- **JSON Data Updates**:  
  - Sensor data is sent to the web interface in JSON format, allowing seamless updates without page reloads.
  - 
## **Usage**  
- **Local Control**:  
  - Use the physical button to toggle the LED state without accessing the web interface.  
- **Remote Monitoring**:  
  - Access the web interface from any device on the same network to monitor sensor data and control the LED.  
