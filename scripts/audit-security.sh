#!/bin/bash
# KISS Fuzzer Security Audit Script
# Scans the repository for potential security issues

set -e

echo "KISS Fuzzer Security Audit"
echo "================================"

# Colors for output
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Security issue counter
ISSUES=0

echo ""
echo "Scanning for sensitive files..."

# Check for system artifacts
echo -n "   Checking for .DS_Store files... "
if find . -name ".DS_Store" -type f | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    find . -name ".DS_Store" -type f
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for Thumbs.db files... "
if find . -name "Thumbs.db" -type f | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    find . -name "Thumbs.db" -type f
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for backup files... "
if find . -name "*.bak" -o -name "*~" -o -name "*.tmp" | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    find . -name "*.bak" -o -name "*~" -o -name "*.tmp"
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo ""
echo "Scanning for hardcoded credentials..."

# Check for hardcoded passwords
echo -n "   Checking for hardcoded passwords... "
if grep -r -i --exclude-dir=.git --exclude="*.sh" --exclude="audit-security.sh" \
   "password.*[=:].*[\"'][^\"']{3,}[\"']" . | grep -v "CHANGE_ME" | grep -v "placeholder" | grep -v "Password" | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    grep -r -i --exclude-dir=.git --exclude="*.sh" --exclude="audit-security.sh" \
        "password.*[=:].*[\"'][^\"']{3,}[\"']" . | grep -v "CHANGE_ME" | grep -v "placeholder" | grep -v "Password"
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for hardcoded API keys... "
if grep -r -i --exclude-dir=.git --exclude="*.sh" \
   "api[_-]\?key.*[=:].*[\"'][^\"']{10,}[\"']" . | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    grep -r -i --exclude-dir=.git --exclude="*.sh" \
        "api[_-]\?key.*[=:].*[\"'][^\"']{10,}[\"']" .
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for hardcoded secrets... "
if grep -r -i --exclude-dir=.git --exclude="*.sh" \
   "secret.*[=:].*[\"'][^\"']{5,}[\"']" . | grep -v "CHANGE_ME" | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    grep -r -i --exclude-dir=.git --exclude="*.sh" \
        "secret.*[=:].*[\"'][^\"']{5,}[\"']" . | grep -v "CHANGE_ME"
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo ""
echo "Scanning for sensitive files..."

echo -n "   Checking for private keys... "
if find . -name "*.key" -o -name "*.pem" -o -name "id_rsa*" -o -name "id_dsa*" | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    find . -name "*.key" -o -name "*.pem" -o -name "id_rsa*" -o -name "id_dsa*"
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for config files with credentials... "
if find . -name "config.env" -o -name "secrets.env" -o -name "credentials.*" | grep -q .; then
    echo -e "${YELLOW}FOUND${NC}"
    find . -name "config.env" -o -name "secrets.env" -o -name "credentials.*"
    echo "   ⚠️  Verify these files are not committed and contain no real credentials"
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo ""
echo "Scanning for network security issues..."

echo -n "   Checking for hardcoded IP addresses... "
# Allow documentation IPs but flag any others
HARDCODED_IPS=$(grep -r -o '\b\([0-9]\{1,3\}\.\)\{3\}[0-9]\{1,3\}\b' src/ 2>/dev/null | grep -v "192.168.4.1" | grep -v "127.0.0.1" || true)
if [ -n "$HARDCODED_IPS" ]; then
    echo -e "${YELLOW}FOUND${NC}"
    echo "$HARDCODED_IPS"
    echo "   ⚠️  Review hardcoded IP addresses in source code"
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo ""
echo "Checking .gitignore coverage..."

# Check if .gitignore covers important patterns
echo -n "   Checking .gitignore for security patterns... "
MISSING_PATTERNS=""

if ! grep -q "\.env" .gitignore; then
    MISSING_PATTERNS="$MISSING_PATTERNS .env"
fi

if ! grep -q "\.key" .gitignore; then
    MISSING_PATTERNS="$MISSING_PATTERNS *.key"
fi

if ! grep -q "\.DS_Store" .gitignore; then
    MISSING_PATTERNS="$MISSING_PATTERNS .DS_Store"
fi

if [ -n "$MISSING_PATTERNS" ]; then
    echo -e "${YELLOW}INCOMPLETE${NC}"
    echo "   Missing patterns:$MISSING_PATTERNS"
else
    echo -e "${GREEN}COMPLETE${NC}"
fi

echo ""
echo "Checking file permissions..."

echo -n "   Checking for world-writable files... "
if find . -perm -002 -type f | grep -q .; then
    echo -e "${RED}FOUND${NC}"
    find . -perm -002 -type f
    ISSUES=$((ISSUES + 1))
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo -n "   Checking for executable config files... "
if find . -name "*.env" -o -name "*.conf" -o -name "*.config" | xargs ls -la 2>/dev/null | grep -E '^-.*x' | grep -q .; then
    echo -e "${YELLOW}FOUND${NC}"
    find . -name "*.env" -o -name "*.conf" -o -name "*.config" | xargs ls -la 2>/dev/null | grep -E '^-.*x'
    echo "   ⚠️  Configuration files should not be executable"
else
    echo -e "${GREEN}CLEAN${NC}"
fi

echo ""
echo "Security Audit Summary"
echo "========================="

if [ $ISSUES -eq 0 ]; then
    echo -e "${GREEN}✅ No security issues found!${NC}"
    echo "   Repository appears secure."
elif [ $ISSUES -eq 1 ]; then
    echo -e "${YELLOW}⚠️  1 security issue found${NC}"
    echo "   Please review and fix the issue above."
else
    echo -e "${RED}🚨 $ISSUES security issues found${NC}"
    echo "   Please review and fix the issues above."
fi

echo ""
echo ""
echo "For more information, see:"
echo "   - docs/development.md (Security section)"
echo "   - .gitignore (Security patterns)"
echo "   - config.env.template (Secure configuration)"

exit $ISSUES
