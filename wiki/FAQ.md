# ❓ Frequently Asked Questions (FAQ)

Find answers to the most common questions about KISS Fuzzer. Can't find what you're looking for? Check our [Community Support](Community-Support) page.

---

## 🚀 Getting Started

### **Q: What is KISS Fuzzer?**
**A:** KISS Fuzzer is a compact, handheld JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W. It's designed for hardware security research, embedded system debugging, and security assessments.

### **Q: Do I need special hardware knowledge to use KISS Fuzzer?**
**A:** Basic understanding of digital electronics and JTAG/SWD protocols is helpful, but our documentation includes beginner-friendly tutorials. Start with our [Quick Start Guide](Quick-Start-Guide).

### **Q: Where can I buy a KISS Fuzzer?**
**A:** KISS Fuzzer is open-source hardware! You can:
- Build your own using our [Hardware Assembly](Hardware-Assembly) guide
- Order PCBs using our gerber files in the [hardware directory](https://github.com/seedon198/KISS/tree/main/hardware)
- Check with community members who might sell assembled units

### **Q: What's the difference between KISS Fuzzer and other JTAG tools?**
**A:** KISS Fuzzer focuses on:
- **Portability**: Handheld form factor with battery power
- **Security Research**: Built-in fuzzing and glitching capabilities
- **Simplicity**: Easy-to-use interface for quick assessments
- **Open Source**: Fully open hardware and software
- **Community**: Strong focus on education and knowledge sharing

---

## 🔧 Hardware & Assembly

### **Q: What components do I need to build KISS Fuzzer?**
**A:** Complete BOM (Bill of Materials) is available in our [Hardware Assembly](Hardware-Assembly) guide. Key components include:
- Raspberry Pi Pico W
- 240×64 OLED display
- Li-ion battery (2000mAh recommended)
- Various passive components and ICs

### **Q: Can I use a regular Raspberry Pi Pico instead of Pico W?**
**A:** The Pico W is required for Wi-Fi functionality. Regular Pico will work for basic JTAG operations but you'll lose web interface and remote control features.

### **Q: What 3D printer settings should I use for the case?**
**A:** Recommended settings:
- **Layer Height**: 0.2mm
- **Infill**: 20%
- **Support**: Yes, for overhangs
- **Material**: PLA or PETG
- **Print Speed**: 50mm/s

### **Q: Where can I get the PCB manufactured?**
**A:** Use any PCB manufacturer that accepts gerber files. Popular options:
- JLCPCB
- PCBWay
- OSH Park
- Advanced Circuits

### **Q: What's the estimated cost to build KISS Fuzzer?**
**A:** Typical costs:
- **PCB**: $10-30 (depending on quantity)
- **Components**: $40-60
- **Case**: $5-15 (3D printing)
- **Total**: ~$60-100 per unit

---

## 💻 Software & Firmware

### **Q: How do I flash the firmware?**
**A:** Follow these steps:
1. Download latest firmware from [Releases](https://github.com/seedon198/KISS/releases)
2. Hold BOOTSEL button while connecting USB
3. Copy .uf2 file to mounted drive
4. Device reboots automatically

### **Q: Can I build the firmware myself?**
**A:** Yes! Follow our [Development Setup](Development-Setup) guide:
```bash
git clone https://github.com/seedon198/KISS.git
cd KISS
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make -j4
```

### **Q: What if firmware update fails?**
**A:** Try these steps:
1. Ensure you're using a data-capable USB cable
2. Hold BOOTSEL while plugging in USB
3. Verify the drive mounts properly
4. Try a different USB port/computer
5. Check our [Troubleshooting](Troubleshooting) guide

### **Q: How do I reset to factory settings?**
**A:** 
- **Software Reset**: Settings → Factory Reset
- **Hardware Reset**: Hold joystick center + power button for 10 seconds
- **Emergency Reset**: Flash fresh firmware

---

## 🔍 JTAG/SWD Operations

### **Q: What devices are compatible with KISS Fuzzer?**
**A:** KISS Fuzzer supports:
- **JTAG**: IEEE 1149.1 compliant devices
- **SWD**: ARM Cortex-M microcontrollers
- **Voltage Levels**: 1.8V, 3.3V, 5V targets
- See our [Hardware Compatibility](Hardware-Compatibility) list

### **Q: Why can't KISS Fuzzer detect my target?**
**A:** Common issues:
- Check power supply (ensure target is powered)
- Verify pin connections (use auto-detect first)
- Confirm voltage levels match
- Check if JTAG/SWD is enabled on target
- Try manual pin configuration

### **Q: What's the maximum JTAG clock speed?**
**A:** Up to 10 MHz using RP2040's PIO (Programmable I/O) blocks. Actual speed depends on:
- Target device capabilities
- Cable length and quality
- Environmental interference
- Power supply stability

### **Q: Can I use KISS Fuzzer with my own cables?**
**A:** Yes! The 8-pin connector follows standard JTAG pinout:
1. VCC (Target Power)
2. TCK/SWCLK
3. TDI
4. TDO/SWO
5. TMS/SWDIO
6. TRST
7. NRST
8. GND

---

## 🌐 Web Interface & Connectivity

### **Q: How do I connect to Wi-Fi?**
**A:** Using the device:
1. Navigate to Settings → Network
2. Select your Wi-Fi network
3. Enter password
4. Wait for connection confirmation

### **Q: What's the default web interface address?**
**A:** After Wi-Fi connection:
- **mDNS**: http://kiss-fuzzer.local
- **IP Address**: Displayed on OLED screen
- **Port**: 80 (HTTP)

### **Q: Can I use KISS Fuzzer without Wi-Fi?**
**A:** Absolutely! All core functionality works offline:
- JTAG/SWD operations
- Memory operations
- Power glitching
- Local display and controls
- SD card logging

### **Q: Is the web interface secure?**
**A:** Current security features:
- Local network only (no internet exposure)
- Basic authentication planned for future versions
- HTTPS support in development
- See our [Security Policy](Security-Policy) for details

### **Q: Why can't I access the web interface?**
**A:** Troubleshooting steps:
1. Verify Wi-Fi connection (check OLED display)
2. Try IP address instead of kiss-fuzzer.local
3. Ensure devices are on same network
4. Check firewall settings
5. Try different browser

---

## ⚡ Power & Battery

### **Q: How long does the battery last?**
**A:** Typical battery life:
- **Active Use**: 6-8 hours
- **Standby**: 48+ hours
- **Heavy Wi-Fi Use**: 4-6 hours
- **OLED Off**: 10+ hours

### **Q: What battery should I use?**
**A:** Recommended specifications:
- **Type**: Li-ion single cell
- **Voltage**: 3.7V nominal
- **Capacity**: 2000-3000mAh
- **Connector**: JST-PH 2.0mm
- **Protection**: Built-in PCB protection recommended

### **Q: Can I use it while charging?**
**A:** Yes! KISS Fuzzer supports:
- USB-C charging while operating
- Pass-through power operation
- Battery health monitoring
- Automatic charging management

### **Q: Why isn't my device charging?**
**A:** Check these items:
- USB-C cable supports data (not just power)
- Charging port isn't blocked
- Battery is properly connected
- Temperature is within normal range
- Try different USB charger/port

---

## 🔒 Security & Safety

### **Q: Is KISS Fuzzer legal to use?**
**A:** KISS Fuzzer is legal when used responsibly:
- ✅ **Legal**: Testing your own devices
- ✅ **Legal**: Authorized penetration testing
- ✅ **Legal**: Educational research
- ❌ **Illegal**: Unauthorized access to systems
- Always follow local laws and regulations

### **Q: Can KISS Fuzzer damage my target device?**
**A:** While designed with protection features, hardware testing always carries risks:
- **Power glitching** can potentially damage circuits
- **Overvoltage** protection helps prevent damage
- **Current limiting** protects against shorts
- Always test on non-critical devices first

### **Q: How do I report security vulnerabilities?**
**A:** We welcome security research:
- **Email**: security@kissfuzzer.dev
- **Bug Bounty**: Check our [Bug Bounty Program](Bug-Bounty-Program)
- **Responsible Disclosure**: Follow our [Security Policy](Security-Policy)
- **Rewards**: Exclusive swag and Hall of Fame recognition

---

## 🛠️ Troubleshooting

### **Q: My OLED display is blank**
**A:** Try these solutions:
1. Check power connections
2. Verify display cable connections
3. Reset device (hold power + center button)
4. Check for firmware corruption
5. Test with known good display

### **Q: SD card isn't detected**
**A:** Troubleshooting steps:
1. Format SD card as FAT32
2. Check card is properly inserted
3. Try different SD card
4. Clean card contacts
5. Verify card is supported size (32GB max recommended)

### **Q: Device won't power on**
**A:** Check these items:
1. Battery charge level
2. Battery connection
3. Power button functionality
4. USB-C power supply
5. Internal fuse (if accessible)

### **Q: JTAG scan finds no devices**
**A:** Common solutions:
1. Verify target power
2. Check cable connections
3. Try different voltage level
4. Use manual pin configuration
5. Check target JTAG is enabled

---

## 📚 Documentation & Learning

### **Q: Where can I learn more about JTAG/SWD?**
**A:** Recommended resources:
- Our [JTAG/SWD Operations](JTAG-SWD-Operations) guide
- [JTAG Security Guide](JTAG-Security-Guide)
- ARM's SWD specification
- IEEE 1149.1 standard
- Online hardware security courses

### **Q: Are there video tutorials available?**
**A:** We're developing video content:
- YouTube channel (coming soon)
- Community-contributed tutorials
- Conference presentations
- Live stream demonstrations

### **Q: Can I contribute to documentation?**
**A:** Absolutely! Ways to help:
- Submit improvements via GitHub
- Translate content to other languages
- Create tutorials and examples
- Share your research and findings
- Help answer community questions

---

## 🌍 Multi-Language Support

### **Q: Is KISS Fuzzer available in my language?**
**A:** We support multiple languages:

| Language | Status | Wiki | Documentation |
|:---------|:-------|:-----|:--------------|
| 🇺🇸 English | Native | ✅ | ✅ |
| 🇩🇪 Deutsch | Auto-translated | ✅ | ✅ |
| 🇫🇷 Français | Auto-translated | ✅ | ✅ |
| 🇪🇸 Español | Auto-translated | ✅ | ✅ |
| 🇯🇵 日本語 | Auto-translated | ✅ | ✅ |
| 🇷🇺 Русский | Auto-translated | ✅ | ✅ |
| 🇨🇳 中文 | Auto-translated | ✅ | ✅ |
| 🇮🇩 Bahasa Indonesia | Auto-translated | ✅ | ✅ |
| 🇷🇴 Română | Auto-translated | ✅ | ✅ |

### **Q: How can I help improve translations?**
**A:** Translation contributions welcome:
- Review auto-translations for accuracy
- Submit corrections via GitHub
- Help translate new content
- Provide cultural/technical context
- Join our translation team

---

## 🤝 Community & Support

### **Q: Where can I get help?**
**A:** Multiple support channels:
- **Discord**: [Community chat](https://discord.gg/kiss-fuzzer)
- **GitHub Issues**: [Bug reports and features](https://github.com/seedon198/KISS/issues)
- **GitHub Discussions**: [Q&A and community](https://github.com/seedon198/KISS/discussions)
- **Email**: community@kissfuzzer.dev

### **Q: How can I contribute to the project?**
**A:** Many ways to help:
- **Code**: Submit bug fixes and features
- **Documentation**: Improve guides and tutorials
- **Hardware**: Design improvements and mods
- **Testing**: Help test new features
- **Community**: Answer questions and help others

### **Q: Can I sell KISS Fuzzer devices?**
**A:** Yes, under the MIT license:
- You can manufacture and sell devices
- Include proper attribution
- Provide source code access
- Follow the license terms
- Consider contributing improvements back

---

## ☕ Support the Project

### **Q: How can I support KISS Fuzzer development?**
**A:** Several ways to help:

[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20A%20Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/kissfuzzer)

- **Financial**: Buy us a coffee to fund development
- **Code**: Contribute improvements and fixes
- **Documentation**: Help improve guides and tutorials
- **Testing**: Test new features and report bugs
- **Community**: Help answer questions and support users
- **Promotion**: Share the project with others

### **Q: Where does donation money go?**
**A:** Donations help with:
- 🔧 Development tools and hardware
- 🌐 Documentation hosting and translation
- 🎁 Bug bounty swag and rewards
- 📚 Educational content creation
- 🎪 Community events and meetups

---

## 📞 Still Need Help?

### **Contact Options**
- **General Questions**: [GitHub Discussions](https://github.com/seedon198/KISS/discussions)
- **Bug Reports**: [GitHub Issues](https://github.com/seedon198/KISS/issues)
- **Security Issues**: security@kissfuzzer.dev
- **Community Chat**: [Discord](https://discord.gg/kiss-fuzzer)
- **Business Inquiries**: business@kissfuzzer.dev

### **Response Times**
- **GitHub**: 1-3 business days
- **Discord**: Real-time community support
- **Email**: 2-5 business days
- **Security**: <48 hours

---

<p align="center">
  <strong>Didn't find your answer?</strong><br>
  <em>Ask the community or submit a question!</em>
</p>

<p align="center">
  <a href="https://discord.gg/kiss-fuzzer">Join Discord</a> • 
  <a href="https://github.com/seedon198/KISS/discussions">GitHub Discussions</a> • 
  <a href="mailto:community@kissfuzzer.dev">Email Support</a>
</p>
