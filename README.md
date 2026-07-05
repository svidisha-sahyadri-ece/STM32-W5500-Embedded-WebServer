# Embedded Web Server using STM32F446RCTx & W5500 Ethernet

## About Me

**Name:** S VIDISHA

**College:** Sahyadri College of Engineering and Management

**Email:** vidisha.ec23@sahyadri.edu.in

### LinkedIn

[![linkedin](https://img.shields.io/badge/LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](YOUR_LINKEDIN_LINK)

---

# Project Overview

This project implements an embedded HTTP web server using a **custom STM32F446RCTx development board** integrated with the **WIZnet W5500 Ethernet controller**.

The W5500 communicates with the STM32 through the **SPI2 peripheral**, providing Ethernet connectivity and TCP/IP communication. A lightweight web server was developed to host custom HTML pages directly from the microcontroller's Flash memory.

The HTML pages were converted into C source files using a Perl-based conversion utility and compiled into the firmware. When accessed through a web browser, the STM32 serves the embedded webpage over Ethernet, demonstrating browser-based interaction with an embedded system.

---

## Features

- Embedded HTTP Web Server
- W5500 Ethernet Controller
- SPI2 Communication
- Static IP Configuration
- Browser-based Dashboard
- HTML to C Conversion
- Flash Memory Webpage Storage
- UART Debugging
- Developed using STM32CubeIDE

---

## Project Demonstration

![Industrial Dashboard](Images/04_dashboard.png)

---

## Project Workflow

```text
                    User
                     │
                     ▼
             Web Browser
                     │
              HTTP Request
                     │
             Ethernet Cable
                     │
                     ▼
            W5500 Ethernet IC
                     │
             SPI2 Communication
                     │
                     ▼
            STM32F446RCTx MCU
                     │
        WIZnet ioLibrary Driver
                     │
             Embedded HTTP Server
                     │
             Embedded HTML Pages
                     │
                     ▼
          Browser Dashboard Response
```

---

<details>

<summary><b>1. Hardware Setup</b></summary>

The project was implemented on a custom STM32F446RCTx development board featuring an onboard W5500 Ethernet controller.

The board includes:

- STM32F446RCTx Microcontroller
- W5500 Ethernet Controller
- RJ45 Ethernet Port
- UART Debug Interface
- Power Supply Section

### Hardware

![Hardware](Images/01_hardware.jpg)

</details>

---

<details>

<summary><b>2. SPI2 Configuration</b></summary>

The W5500 communicates with the STM32 through the SPI2 peripheral.

SPI Configuration

- Master Mode
- Full Duplex
- 8-bit Data
- Software NSS
- MSB First
- CPOL Low
- CPHA First Edge

### SPI Configuration

![SPI Configuration](Images/02_spi_configuration.png)

</details>

---

<details>

<summary><b>3. W5500 Network Configuration</b></summary>

The W5500 Ethernet controller was initialized using the WIZnet ioLibrary.

Static network parameters were configured.

- MAC Address
- Static IP Address
- Gateway
- Subnet Mask
- DNS Server

The firmware verifies the W5500 version and applies the network settings before starting the web server.

### Network Configuration

![Network Configuration](Images/03_network_configuration.png)

</details>

---

<details>

<summary><b>4. Integrating the Embedded Web Pages</b></summary>

The HTML webpages were converted into C source files and included in the STM32 project.

Generated files include

- webpage.c
- webpage.h

These files are stored in Flash memory and served directly by the embedded web server.

### HTML Integration

![HTML Integration](Images/05_webpage_integration.png)

</details>

---

<details>

<summary><b>5. Login Page</b></summary>

A custom login page was developed as the entry point for the embedded web application.

The page authenticates the user before allowing access to the Industrial IoT dashboard.

### Login Page

![Login](Images/05_login_page.png)

</details>

---

<details>

<summary><b>6. Industrial IoT Dashboard</b></summary>

After successful authentication, the browser loads the embedded dashboard hosted by the STM32.

The dashboard demonstrates how industrial monitoring data can be presented using an embedded HTTP server.

### Dashboard

![Dashboard](Images/04_dashboard.png)

</details>

---

<details>

<summary><b>7. Firmware Execution & Testing</b></summary>

The firmware initializes the W5500 controller, configures the Ethernet network, and starts the embedded web server.

The UART terminal confirms:

- W5500 Detection
- Network Configuration
- Assigned IP Address
- Ethernet Connection Status

### UART Output

![UART Output](Images/06_uart_output.png)

</details>

---

## Repository Structure

```text
STM32-W5500-Embedded-WebServer
│
├── Core
├── Drivers
├── Inc
├── Src
├── W5500
├── Images
├── webpage.c
├── webpage.h
├── README.md
└── STM32F446RCTx.ioc
```
