# Introduction
Our goal is to create and deploy a GPS-guided navigation headband with LIDAR object avoidance for those with visual impairments to reach new and unlearned destinations. By giving users real-time input on their surroundings and directions to their destinations, this headband aims to improve learning new routes. Instead of having support from someone or taking extra time to learn the route, this headband aims to have a streamlined route learning experience and checker. 
Our approach integrates embedded system features such as I2C/TWI and UART communication with haptic motors, GPS, and LIDAR hardware. GPS provides the current location and distance to the current location. LIDAR is used to detect potential obstacles the user may run into. Vibration motors, haptic feedback motors, are used to communicate this information from GPS and LIDAR in an discrete way. Furthermore, to ensure no obstacles are ran into, LIDAR interrupts take priority over GPS information.

# Design and Implementation 
## Circuit Diagram 
![circuit diagram](https://github.com/user-attachments/assets/60b543ad-4660-483b-8620-ff218c673c3a)

## Hardware Setup
Peripherals consist of 3 vibration motors, one LIDAR module (TFMini-S - Micro LiDAR Module), and one GPS module (GPS Breakout - XA1110). All connect to the AVR BLE. Since there are not enough ground pins, we use a small breadboard to handle the ground connection. All of this is put onto two headbands put together. The inner one holds the motors and wires, and the outer run hides the cables and holds the LDIAR. We went with a headband instead of a watch or helmet, to balance between the difficulty of having smaller electronics and the discreteness with a watch and the easily adjustableness of hardware to user’s needs with the helmet.
The three vibration motors consist of left, middle, and right designated motors. The left motor goes to  PINA4, the middle to PINA5, and the right to PINA6. The GND cables go to the breadboard. From the breadboard, one GND cable goes to GND next to the 3.3V pin. The vibration motors follow simple on/off signals like LEDs.
For the LIDAR module, we use UART communication. Although the LIDAR module can also use I2C, we used UART, so it can maintain constant independent communication with AVR-BLE. Obstacle avoidance is the number one priority. It has a QWICC cable which splits into a white RX cable, green TX cable, red 5V wire, and black GND cable. Since in terms of the LIDAR module the green cable is transmitting data (TX), this means it goes to the AVR-BLE RX, receiving pin. The same logic is for the white RX cable from the LIDAR module; it goes to the TX pin on the board. The GND cable goes to the GND pin next to the 5V pin. 
The GPS module also uses a QWICC connection. However, we are using the I2C TWI protocol for communication. The blue wire goes to SDA, the yellow wire to SDL, the red to the 3.3V pin, and the black GND cable to the breadboard to join with the other GND connections.

## Software Goals
Our headband’s software aims to communicate with GPS and LIDAR sensors, manage real-time tasks (directional and obstacle avoidance information), and control different vibration motor outputs in response to sensor inputs. The software's main objectives are:
1. Real-Time Data Collection and Processing:
To determine how far away obstacles are, the system continuously reads LIDAR data and adjusts its state accordingly. The system determines its location by parsing GPS data, which enables it to compute the distances to a given destination.
2. LIDAR Sensor Data Integration with GPS:
GPS data facilitates the user’s current position, and LIDAR data distinguishes between being closer to and farther from an object. By successfully integrating both peripherals, the system would precisely manage different feedback mechanisms by modifying motor states, such as turning on or off movement pulses, based on real-time data.
3. Haptic Feedback Control:
To prevent collisions and guide movement in the direction of the intended destination, the activation of various motor states is regulated by the assessed LIDAR sensor data.

## Software Design 
The software is structured around a main loop and several interrupt service routines (ISRs) that handle real-time tasks.

### Main Loop
The main loop continuously attempts to read LIDAR data. This ensures that obstacles are always attempted to be detected. The only exception to this is if the user has arrived at their destination. After the arrival event, PULSE_ARRIVED, the system continues reading LIDAR data.
Additionally, main also handles the state changing of PULSE_FURTHER and PULSE_CLOSER based on the changes in distances under the DISTANCE_THRESHOLD if LIDAR data is read correctly.  Following this, parse_gps_data() is called to read GPS data. This is called after reading LIDAR data to not interrupt valid LIDAR UART communication.

### Pulses
These pulse states are used to communicate the following information from LIDAR and GPS:
- PULSE_LEFT: To communicate you are moving further from the destination. There are three repetitions of a single middle motor vibration then a single left motor vibration.
- PULSE_MIDDLE: To communicate you are moving in the right direction to the destination. There are three repetitions of a single middle motor vibration. This was for magnetometer implementation. 
- PULSE_RIGHT: To communicate you are moving closer to the destination. There are three repetitions of a single middle motor vibration then a single right motor vibration.
- PULSE_CLOSER:  To communicate you are moving closer to an object. The three motors move in an inward pattern. The outer motors (left and right) vibrate for 1500ms then the inner motor (middle) for 500ms. 
- PULSE_FURTHER: To communicate you are moving further from an object. The three motors move in an outward pattern. The inner motor (middle) vibrates for 1500ms then the outer motors (left and right) for 500ms. 
- PULSE_ARRIVED:  To communicate you have arrived at your destination. There are three pulses of all three motors vibrating.

### Interrupt Service Routine (ISR)
Real-time clock (RTC) ISR is used to run pulse states for the vibration motors and calibrate timing between the LIDAR and GPS system. THE ISR is triggered every half a second; the variable secondCounter keeps track of this and resets every second. It also helps keep track of pulses, pulseCounter, to ensure pulses happen three times per state (if no new data comes in and changes the state). Furthermore, every three seconds, GPS data is read to save system resources and ensure LIDAR readings are being read more continuously. 

### GPS and LIDAR Integration
GPS data is requested in 32-byte chunks from the GPS module, and stored in a circular buffer (rxBuffer). The circular buffer would then be read one byte at a time and stored in a global buffer (gpsData), a flag, gps_data_ready would then be set to indicate that there is new GPS data available. After three seconds, the GPS-parsing function, parse_gps_data() would be called to extract the time, latitude, and longitude, from the “GNGGA” type NMEA sentences. The extracted latitude and longitude would then be converted to decimals to ease the calculation of distances. Every three seconds, the check_arrival function would be called to check if the user has reached their destination, and offer arrival status, such as whether they should keep going or if they are getting closer/further away from their destination. In such cases, the states would be updated to offer real-time haptic feedback on their arrival status. 
The LIDAR data is processed in readLidarData() function which reads, validates, and sets the distance variable.  Since UART communication is constant, this means we need to read in line 9 bytes at a time. To do this, we have a block of if statements if each bit is where it is supposed to be in the following order: header byte 1, header byte 2, remaining 7 data byes.  After this, we calculate the checksum (sum of first 8 bytes) and verify it with 0xFF that the data is correct. If it is incorrect, we return 0. Otherwise, we extract the distance bytes by combining bytes 2 and 3 and multiplying it by 256.

# Final Product
We have successfully created a wearable GPS guiding and obstacle avoidance device for those with vision impairments. Vibration motors are used in the devices’ headband design to provide feedback regarding obstacles and their current position from their destination. The headband alerts users of impending obstacles and directs users toward their destination by combining GPS, LIDAR, and haptic feedback.

![img1](https://github.com/user-attachments/assets/be31f488-b289-4f30-97b1-7caa315c3aa9)

The lidar sensor is positioned at the front of the headband to allow for obstacle detection and avoidance. 

![img2](https://github.com/user-attachments/assets/078bf323-177e-4071-a097-4a01fc5109b8)

The GPS module is located outside of the headband, while the haptic motors and LIDAR sensor are on the headband, and  are connected to the AVR board. 

![img3](https://github.com/user-attachments/assets/72c933a2-68e0-4c67-9247-1c6b4967e53f)

Vibration motors are attached to the headband, which is hidden between the fabric of the headband.

# Reflection

## Changes/Improvements to Hardware Design
### Redesigning for Comfort and Ergonomics
To enhance usability, we would redesign the headband to be more ergonomic. The LIDAR stayed on fine because it was stitched. The motors and cables were taped. The tape would sometimes come off. Also since there were so many wires, it was bulky and the wires sometimes got in the way.  We would find some way to use stretchable wires that can be stitched to the headband.
The AVR-BLE, GPS Module, and breadboard were all taped together. It would be better to have a better casing such as using 3D printing or resin casing to make it more portable. 
### Implementing Wireless Capabilities 
A laptop in our use case serves as the device's consistent power source in the current configuration. We propose integrating a rechargeable battery, such as a lithium-polymer (LiPo) or lithium-ion battery, to power the device to overcome this limitation. 
To guarantee the safe, effective charging and operation of a lithium-ion or lithium-polymer battery, we might also incorporate a charging circuit (such as the TP4056 module) and a battery management system (BMS), guarding against electrical dangers, overcharging, and over-discharging. When combined, they increase battery life, increase portability, and give stand-alone devices a dependable power supply. 
Incorporating a Magnetometer
Even though we modified the project to only use GPS for directional guidance, the magnetometer's absence resulted in decreased accuracy in situations where heading changes are crucial. Using a magnetometer to increase orientation and navigation would be an additional project improvement. A magnetometer gives accurate heading information and measures the Earth's magnetic field. By including this sensor, the GPS module would be enhanced by the directional information it provides, guaranteeing precise navigation even in places with weak satellite signals, like dense woodlands or urban canyons. Users would be able to stay on track more consistently as a result. In addition, the device as a whole might give users immediate feedback on heading changes, enabling them to make accurate turns without depending entirely on GPS recalibration, which can be slower. Furthermore, integrating magnetometer data with LIDAR outputs would allow for more contextually aware decision-making, such as figuring out whether an obstruction calls for a straightforward sidestep or a major heading adjustment.
## Changes/Improvements to Software Design
### Mobile App Integration
A mobile app will allow ease of use for adjusting settings such as pulse strengths, distance threshold, and destination without having to manually change the code on a laptop. An app would also allow integration with Google Maps to have more specific route data and more choices in route choice. Voice commands can also be implemented. 

