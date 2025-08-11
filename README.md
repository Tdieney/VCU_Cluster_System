# Vehicle Control Unit (VCU) Digital Instrument Cluster and for Electric Vehicles

A simulated automotive system featuring a **Digital Instrument Cluster (IPC)** and **Vehicle Control Unit (VCU)** for electric vehicles, built on Raspberry Pi and STM32 with CAN bus communication. The VCU centrally manages vehicle states and I/O mapping, while ECU nodes (STM32-based simulator) run identical firmware with self-assigned IDs for scalability and flexibility. This project delivers a realistic automotive E/E architecture, HMI design, and CAN networking.

## Key Features

- **Digital HMI**: Qt/QML-based interface displaying speed, battery SoC, odometer, and tell-tales (ISO 2575-compliant).
- **Vehicle State Management**: VCU handles states (OFF, ACCESSORY, READY_TO_DRIVE, CHARGING, FAULT).
- **CAN Bus Communication**: Reliable data exchange between VCU (Raspberry Pi) and ECU nodes (STM32).
- **I/O Mapping**: VCU controls ECU outputs based on input signals (e.g., ECU2.input1 → ECU4.output3).
- **Diagnostics**: Two-trip DTC logic with MIL activation for fault detection.
- **Adaptive UI**: Driving modes (Normal, Eco, Sport) and charging screen with animations.
- **Performance**: Boot time < 2s, data latency < 100ms, HMI refresh rate ≥ 30 FPS.

## System Requirements

### Hardware

- Raspberry Pi 5 (4GB RAM recommended)
- STM32F1 for ECU nodes
- CAN transceivers (SN65HVD230)
- USB to CAN for Raspberry Pi (use the same CAN transceiver)
- Sensors: rotary encoder (speed), potentiometer (SoC), push buttons
- Waveshare 7 inch HDMI display (1024×600) for HMI

### Software

- Linux OS
- Qt5 with Qt Creator
- STM32CubeIDE
- SocketCAN in Linux
- GCC cross-compiler for ARM

## Installation

1. **Clone the repository**:

   ```bash
   git clone https://github.com/Tdieney/VCU_Cluster_System.git
   cd VCU_Cluster_System
   ```
   
2. **Set up Raspberry Pi 5**:

3. **Set up STM32**:

4. **Build VCU software**:

5. **Connect hardware**:

## Usage

## Project Structure

```
├── firmware/         # STM32 ECU firmware
├── vcu/              # VCU software (C++, Qt/QML)
├── docs/             # Documentation
├── tests/            # Unit and integration tests
└── README.md
```

## Documentation

- [ECU Nodes](ecu_nodes/README.md)
- [3D case](hardware/README.md)
- [VCU software overview](software/README.md)
- [Build image for Raspberry Pi 5.md](docs/yocto/1.%20Build%20image%20for%20Raspberry%20Pi%205.md)
- [CAN Communication.md](docs/yocto/2.%20CAN%20Communication.md)
- [Systemd startup script.md](docs/yocto/3.%20Systemd%20startup%20script.md)
- [Bring up Qt UI.md](docs/yocto/4.%20Bring%20up%20Qt%20UI.md)


## Contributing

1. Fork the repository.
2. Create a feature branch.
3. Commit changes.
4. Push and open a pull request.

## References

- ISO 26262: Functional Safety for Road Vehicles
- ISO 2575: Symbols for Controls and Tell-tales
- Qt Documentation: [qt.io](https://doc.qt.io)
- USB to CAN: [CANable-MKS](https://github.com/makerbase-mks/CANable-MKS/blob/main/User%20Manual/CANable%20V2.0/Makerbase%20CANable%20V2.0%20Use%20Manual.pdf)
- SocketCAN: [SocketCAN - Controller Area Network — The Linux Kernel documentation](https://docs.kernel.org/networking/can.html)

## Contact

For issues or inquiries, open a GitHub issue or contact [vanthinh.nguyentran.4@gmail.com].

---

*Demo video and system block diagram available in `docs/`.*