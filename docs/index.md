# Welcome to KISS Fuzzer! 🔓

Hey there, hardware hacker! Ready to dive into the world of JTAG fuzzing? You've come to the right place. 

KISS Fuzzer is your new best friend for embedded security research. Think of it as a Swiss Army knife for JTAG and SWD interfaces - compact, powerful, and surprisingly easy to use.

```{toctree}
:maxdepth: 2
:caption: Let's explore:

getting-started
hardware
software
api-reference
tutorials
development
troubleshooting
```

## What makes KISS Fuzzer special?

Ever been frustrated with bulky JTAG adapters that cost more than your car? Or tired of command-line tools that make simple tasks feel like rocket science? We felt the same way, so we built something better.

KISS Fuzzer fits in your pocket but packs serious punch. Here's what makes it awesome:

**🖥️ Ultra-wide display** - That 240×64 OLED isn't just for show. It gives you real-time feedback without squinting at tiny screens.

**🕹️ Joystick control** - Navigate menus like you're playing a retro game. No more memorizing cryptic key combinations.

**🔋 All-day battery** - Li-ion battery keeps you going through those long hacking sessions. USB-C charging means you're never stranded.

**🚀 Blazing fast** - JTAG/SWD interface runs up to 10 MHz. Because waiting is for chumps.

**📡 Wi-Fi enabled** - Control it remotely through a slick web interface. Perfect for automated testing or when you need to stay cozy at your desk.

**⚡ Power glitching** - Optional glitch injection for when targets get a little too stubborn.

**💾 Smart logging** - MicroSD card captures everything. Your future self will thank you.

## How it all works together

Picture this: you've got a mysterious embedded device sitting on your bench, and you want to know what makes it tick. Here's how KISS Fuzzer helps you crack the code:

```{mermaid}
graph TB
    A[Your Target Device] --> B[KISS Fuzzer]
    B --> C[Real-time Display]
    B --> D[Wi-Fi Interface]
    B --> E[SD Card Storage]
    
    C --> F[Instant Feedback]
    D --> G[Remote Control]
    E --> H[Permanent Logs]
    
    subgraph "What you see"
        F
        G
        H
    end
    
    style A fill:#ff6b6b
    style B fill:#4ecdc4
    style F fill:#95e1d3
    style G fill:#95e1d3
    style H fill:#95e1d3
```

The magic happens when you connect KISS Fuzzer to your target. The built-in JTAG engine starts probing, the display shows you what's happening in real-time, and everything gets logged for later analysis. If you're feeling fancy, fire up the web interface and control everything from your laptop.

## Ready to get started?

Jump into our [Getting Started Guide](getting-started.md) and you'll be fuzzing like a pro in no time. Don't worry if you're new to JTAG - we've got your back with plenty of examples and explanations.

Got questions? Check out our [Troubleshooting](troubleshooting.md) section or dive into the [Hardware Guide](hardware.md) if you want to understand what's under the hood.
    B --> H
    C --> H
    D --> H
```

## Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Comprehensive guides and API reference
- **Community**: Join our Discord for real-time support

```{admonition} Safety Notice
:class: warning

KISS Fuzzer is designed for authorized security testing and educational purposes only. 
Always ensure you have permission to test target devices and comply with local laws.
```
