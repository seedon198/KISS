# Your First Adventure with KISS Fuzzer 🚀

Congrats on getting your hands on a KISS Fuzzer! Let's get you set up and running your first JTAG scan. Don't worry - we'll take it step by step, and you'll be a JTAG wizard before you know it.

## What you'll need

Let's make sure you've got everything ready for your first hacking session:

**Essential gear:**
- Your shiny new KISS Fuzzer (obviously!)
- A USB-C cable (the good kind, not that flimsy one from your old phone)
- A MicroSD card - 32GB or smaller works best, Class 10 if you want speed
- Something interesting to poke at (any device with JTAG or SWD pins)

**Nice to have:**
- A good cup of coffee ☕
- Some jumper wires or a probe adapter
- Patience (optional but recommended)

## Let's fire this thing up!

### First power-on

Plug in that USB-C cable and watch the magic happen:

1. **The light show**: Your KISS Fuzzer will wake up with a satisfying boot sequence
2. **Status parade**: You'll see each system coming online - display, power, Wi-Fi, the works
3. **Main menu**: After a few seconds, you'll land on the main screen with that classic retro game feel

If something looks weird, don't panic! Check out our [troubleshooting guide](troubleshooting.md) - we've probably seen it before.

### Getting connected (Wi-Fi style)

Time to get your KISS Fuzzer talking to the world. Use that joystick to navigate to **Settings → Wi-Fi**:

**The easy way**: Your KISS Fuzzer creates its own Wi-Fi network called `KISS-Fuzzer`. Connect to it with the password `fuzzing123`, then point your browser to `http://192.168.4.1`. Boom - instant web interface!

**Pro tip**: The web interface is perfect for logging results or when you want to control things remotely. Very handy when your target device is tucked away somewhere awkward.

### SD card magic

Slide that MicroSD card into the slot (it only goes in one way, trust us). The KISS Fuzzer will automatically set up folders for logs, dumps, and configs. It's like having a digital filing cabinet for all your discoveries.

## Your first JTAG adventure

Now for the fun part - let's connect to something and see what secrets it's hiding!

### Making the connection

Time to hook up your target device. Don't worry about breaking anything - JTAG is pretty forgiving, and the KISS Fuzzer has protection built in.

Here's the standard JTAG hookup:

```
KISS Fuzzer    →    Your Target
-----------          -----------
VCC (red)      →    VCC (usually 3.3V or 5V)
TCK (yellow)   →    TCK (clock signal)
TDI (green)    →    TDI (data going to target)
TDO (blue)     →    TDO (data coming from target)
TMS (purple)   →    TMS (mode select)
GND (black)    →    GND (ground - very important!)
```

**Can't find the JTAG pins?** No worries! Most of the time they're labeled, but sometimes you have to play detective. Look for groups of 4-6 pins near the main processor, or check the device manual if you're lucky enough to have one.

### The moment of truth - first scan

Navigate to **JTAG Scan → Auto Scan** and hit that OK button. Your KISS Fuzzer will start probing:

1. **Pin detection**: First, it figures out which pins are which
2. **Device discovery**: Then it tries to talk to whatever's connected
3. **ID code reading**: If it finds something, it'll show you the device ID
4. **Results**: Everything gets displayed and logged automatically

**What you might see:**
- `Found device: ARM Cortex-M4` - Jackpot! You've got a connection
- `No JTAG devices found` - Either nothing's connected or the pins aren't quite right
- `Unknown device ID: 0x12345678` - You found something, but it's not in our database yet

### When things don't work (and that's okay!)

JTAG can be finicky. Here are the usual suspects:

**No response at all?**
- Double-check your connections (seriously, we all forget ground sometimes)
- Make sure your target device has power
- Try a different clock speed in the settings

**Getting weird data?**
- Your target might be using SWD instead of JTAG - try the SWD scan option
- Voltage levels might not match - check if your target uses 1.8V logic

## Level up with the web interface

Once you've got the basics down, the web interface opens up a whole new world of possibilities. It's like having a mission control center for your JTAG adventures.

### Getting online

Remember that Wi-Fi network we set up earlier? Time to put it to work:

1. **Connect**: Join the `KISS-Fuzzer` network from any device
2. **Browse**: Point your browser to `http://192.168.4.1`
3. **Explore**: You'll see a clean, mobile-friendly interface with all the good stuff

### What you can do remotely

**Live monitoring**: Watch your scans happen in real-time, complete with progress bars and status updates. Perfect when you're running long operations.

**Remote control**: Start and stop scans from your couch. Great for automated testing or when your target setup is in an awkward spot.

**Log streaming**: See everything happening live, with filtering and search. No more squinting at the tiny display.

**File management**: Download your scan results, memory dumps, and logs directly to your laptop. Share discoveries with your team instantly.

## What's next?

You've just scratched the surface! Here's where to go from here:

**Want to understand the hardware better?** Check out our [Hardware Guide](hardware.md) - we'll show you what's under the hood and how everything works together.

**Ready for advanced techniques?** Our [Tutorials](tutorials.md) section has real-world examples, from basic memory dumping to advanced exploitation techniques.

**Building your own tools?** The [API Reference](api-reference.md) has everything you need to extend KISS Fuzzer or integrate it into your workflow.

**Hit a roadblock?** Don't worry - our [Troubleshooting Guide](troubleshooting.md) covers the most common issues and how to solve them.

Welcome to the JTAG party! 🎉
