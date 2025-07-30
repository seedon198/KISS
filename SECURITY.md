# Security Policy

## Supported Versions

We actively support the following versions of KISS Fuzzer with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

**Please do not report security vulnerabilities through public GitHub issues.**

If you discover a security vulnerability in KISS Fuzzer, please report it responsibly:

### Preferred Reporting Method

**Email:** security@kiss-fuzzer.org

### What to Include

Please include the following information in your report:

- **Description** of the vulnerability
- **Steps to reproduce** the issue
- **Potential impact** and attack scenarios
- **Hardware/firmware versions** affected
- **Any proof-of-concept code** (if applicable)
- **Suggested fixes** (if you have any)

### Response Timeline

- **Acknowledgment**: Within 24 hours
- **Initial Assessment**: Within 72 hours  
- **Status Updates**: Weekly until resolved
- **Fix Timeline**: 30-90 days depending on severity

### Severity Levels

We classify vulnerabilities using the following severity levels:

#### Critical (CVSS 9.0-10.0)
- Remote code execution on target devices
- Complete bypass of hardware security mechanisms
- Permanent device damage or bricking

#### High (CVSS 7.0-8.9)
- Privilege escalation attacks
- Bypass of critical security features
- Data exfiltration from secured devices

#### Medium (CVSS 4.0-6.9)
- Local privilege escalation
- Information disclosure
- Denial of service attacks

#### Low (CVSS 0.1-3.9)
- Information leakage
- Minor configuration issues
- Non-security impacting bugs

## Bug Bounty Program

We operate a **swag-based bug bounty program** for security researchers:

### Rewards

- **Critical**: Custom KISS Fuzzer device + Premium swag pack + Public recognition
- **High**: Premium swag pack + Public recognition  
- **Medium**: Standard swag pack + Recognition
- **Low**: Digital recognition in Hall of Fame

### Eligibility

- Vulnerability must be in the current supported version
- Must follow responsible disclosure practices
- First reporter of a unique vulnerability
- Must not violate any laws or regulations
- Must not access user data or disrupt services

For full details, see our [Bug Bounty Program](https://github.com/seedon198/KISS/wiki/Bug-Bounty-Program).

## Hall of Fame

Security researchers who have contributed to KISS Fuzzer's security are recognized in our [Hall of Fame](https://github.com/seedon198/KISS/wiki/Hall-of-Fame).

## Security Best Practices

### For Users

- Keep firmware updated to the latest version
- Use strong, unique passwords for WiFi credentials
- Regularly review stored scan results
- Follow proper physical security for the device
- Validate target device ownership before testing

### For Developers

- Follow secure coding practices
- Validate all input data
- Use proper authentication and authorization
- Implement secure communication protocols
- Regular security audits and code reviews

## Security Features

KISS Fuzzer includes several built-in security features:

- **Encrypted WiFi communications** using WPA3 when available
- **Hardware-based random number generation** for cryptographic operations  
- **Secure boot verification** of firmware integrity
- **Input validation** on all user-provided data
- **Rate limiting** on network operations
- **Secure storage** of sensitive configuration data

## Compliance

KISS Fuzzer is designed to comply with:

- **Responsible disclosure** practices
- **Ethics guidelines** for security research
- **Local laws and regulations** regarding security testing
- **Industry standards** for embedded device security

## Contact

For security-related questions or concerns:

- **Security Team**: security@kiss-fuzzer.org
- **General Questions**: support@kiss-fuzzer.org
- **Documentation**: https://kiss-fuzz.readthedocs.io/
- **Community**: https://github.com/seedon198/KISS/discussions

---

**Last Updated**: July 30, 2025
