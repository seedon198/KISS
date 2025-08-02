# Security Policy

KISS Fuzzer is still small and experimental, but we do care about security. If you find something that looks bad (or even mildly suspicious), please let us know. This page explains how to report stuff and what to expect in return.

---

## What’s Covered

Pretty much anything in this repo or hardware files:
- Firmware and embedded code
- Web interface and API (if you’re using the Wi-Fi version)
- Hardware design (debug interface, side-channels, etc.)
- Build process and supply chain quirks

Stuff outside our control (like GitHub or Discord itself) isn’t in scope.

---

## How to Report a Vulnerability

The easiest way: **email `security@kissfuzzer.dev`**  
If it’s sensitive, use the [PGP key](Security-PGP-Key).  
You can also use GitHub’s private vulnerability reporting if you prefer.

Please include:
- What’s broken (plain description)
- How to reproduce (steps, PoC, or screenshots)
- Why it’s bad (impact)
- Your handle (so we can credit you)

---

## What Happens Next

We’re a small project, so timelines are best-effort. Rough idea:

| Stage          | Timeframe      |
|----------------|----------------|
| First reply    | Within ~2 days |
| Check + confirm| About a week   |
| Figure out fix | Few weeks      |
| Public writeup | Around 90 days |

If the bug isn’t urgent or I feel like it doesn’t need fixing right now, I might leave it open-ended. It’s open source, so if it bugs you, feel free to fix it and send a PR.

---

## Rewards

No cash bounties here — but if you find something real:
- You’ll get **exclusive project swag** (stickers, PCB, maybe a shirt)
- Permanent mention in the [Hall of Fame](Hall-of-Fame)
- Public thanks in release notes (if you’re okay with it)

---

## Out of Scope

- Social engineering (phishing me isn’t part of the challenge)
- DDoS or spam tests
- Physical theft
- Anything already known or obvious

---

## Security Tips for Users

- Keep your firmware updated
- Don’t test stuff you don’t own or don’t have permission for
- Assume anything experimental can break things — t
