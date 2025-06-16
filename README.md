# Tandem t:slim X2 Insulin Pump Simulator

A full-stack simulator of the Tandem t:slim X2 insulin pump, built using C++ and the Qt framework. This project replicates core features of a medical insulin pump including CGM graphing, manual bolus logic, PIN security, and cartridge replacement workflows.

## Features

- Real-time CGM graph with prediction overlays and bolus indicators
- Manual bolus delivery with UI-based dosage input
- Profile management with configurable basal settings
- Secure PIN system with set/change/remove functionality
- Cartridge replacement simulation with timed fill and success screens
- Modular MVC architecture with Qt signals and slots
- JSON-based profile persistence

## Installation

### 1. Install Qt and Qt Creator

Download and install Qt from the official site:  
👉 [https://www.qt.io/download](https://www.qt.io/download)

During installation:
- Select the latest **Qt 6.x version** (e.g., Qt 6.5+)
- Select the **Desktop GCC or MSVC Kit** (depending on your OS)
- Make sure to include the **QtCharts** module when choosing packages

### 2. Clone the Repository

```bash
git clone https://github.com/Raj2b/Tandem-t-slim-X2-Insulin-Pump.git
cd Tandem-t-slim-X2-Insulin-Pump
