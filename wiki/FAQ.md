# Frequently Asked Questions (FAQ)

This project is still really small — basically just me hacking on it.  
If you have questions and don’t see them answered here, feel free to open an issue or ping me on Discord. I’ll try to help when I can, but there’s no big community or team behind this yet.

---

## What is KISS Fuzzer?

KISS Fuzzer is a small JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W. It’s meant for hardware security research, learning about embedded systems, and general tinkering.

---

## Do I need to be a hardware expert?

Not really. Some basic understanding of electronics and JTAG/SWD helps, but I’ve tried to make things beginner-friendly. The docs have guides to get you started.

---

## Can I buy one?

Not at the moment. It’s open source — you can build one yourself using the files and guides in this repo. Sometimes people make extras, but since there’s no community yet, don’t count on it.

---

## How do I flash the firmware?

1. Download the `.uf2` file from [Releases](https://github.com/seedon198/KISS/releases)  
2. Hold BOOTSEL while plugging the device into USB  
3. Copy the file over to the new drive that shows up  
4. It reboots automatically  

---

## What if something doesn’t work?

- Double-check your connections and cables  
- Reflash the firmware  
- Search the issues here to see if anyone else hit it (probably not many yet)  
- If you’re stuck, open an issue or message me — just know I might take a bit to respond

---

## Can I use it without Wi-Fi?

Yep. Everything important works offline: JTAG/SWD stuff, glitching, memory reads, etc. Wi-Fi is mainly for the web interface.

---

## Is this legal?

It’s legal to use on your own hardware or with permission. Don’t use it to mess with things you don’t own — that’s on you.

---

## What if I find a security bug?

Report it to me (check the [Bug Bounty Program](Bug-Bounty-Program) page). If it’s legit, I’ll send you some swag as thanks. No tiers, no scoring — just “found something cool, here’s something for you.”

---

## How do I get help?

There’s no big support network right now. It’s mostly me:
- Open a GitHub issue  
- Drop a note in [Discord](https://discord.gg/kiss-fuzzer)  
- Worst case, DM me on LinkedIn (linked on the main page)

---

## Can I contribute?

Yes, please. This is open source — feel free to fix bugs, add features, improve docs, or even fork it and do your own thing. If you make something cool, let me know.

---

## Why is everything so rough?

Because it’s early days. This is still being built and tested as I go. If you find rough edges or missing docs, that’s normal — just let me know or help improve it.

---

## Support the project

If you like this and want to help:
- Contribute code or docs
- Spread the word
- Or toss a coffee my way: [Buy Me A Coffee](https://buymeacoffee.com/kissfuzzer)

---

## Response times

I try to reply within a couple days, but sometimes life happens.  
- Issues: usually within 1–3 days  
- Discord: same day if I’m around  
- Email: 2–5 days  
- Security bugs: under 48 hours

---

That’s pretty much it. If you’re stuck, just ask — worst case, I’ll admit I don’t know yet.
