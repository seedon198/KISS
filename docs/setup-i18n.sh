#!/bin/bash
# setup-i18n.sh - Initialize internationalization for KISS Fuzzer documentation

set -e

echo "🌍 Setting up internationalization for KISS Fuzzer documentation..."

# Check if we're in the docs directory
if [ ! -f "conf.py" ]; then
    echo "❌ Error: This script must be run from the docs/ directory"
    exit 1
fi

# Check required tools
echo "🔍 Checking required tools..."
for tool in sphinx-build sphinx-intl msgfmt; do
    if ! command -v $tool &> /dev/null; then
        echo "❌ Error: $tool is not installed. Please install sphinx-intl and gettext."
        exit 1
    fi
done

echo "✅ All required tools found."

# Create gettext files
echo "📝 Extracting translatable strings..."
sphinx-build -b gettext . _build/gettext

# Update translation files for all languages
echo "🔄 Updating translation files..."
languages=("de" "fr" "ja" "ru" "es" "zh" "id" "ro")

for lang in "${languages[@]}"; do
    echo "  📄 Updating $lang..."
    sphinx-intl update -p _build/gettext -l $lang
done

# Create language-specific configuration files
echo "⚙️ Creating language-specific configurations..."

for lang in "${languages[@]}"; do
    lang_dir="locales/$lang"
    if [ ! -d "$lang_dir" ]; then
        mkdir -p "$lang_dir/LC_MESSAGES"
    fi
done

# Create a sample translation for testing
echo "📋 Creating sample translations..."

# Create a README for translators
cat > locales/README.md << 'EOF'
# Translation Files

This directory contains translation files for the KISS Fuzzer documentation.

## Supported Languages

- **de** - Deutsch (German)
- **fr** - Français (French) 
- **ja** - 日本語 (Japanese)
- **ru** - Русский (Russian)
- **es** - Español (Spanish)
- **zh** - 中文 (Chinese)
- **id** - Bahasa Indonesia (Indonesian)
- **ro** - Română (Romanian)

## For Translators

1. Navigate to your language directory: `locales/<language_code>/LC_MESSAGES/`
2. Edit the `.po` files with your translations
3. Use tools like Poedit, Lokalize, or any text editor
4. Test your translations with: `make build-lang LANG=<language_code>`

## Translation Status

To check translation completion: `make translation-stats`

## Building Specific Languages

```bash
# Build English (default)
make html

# Build German
make build-lang LANG=de

# Build all languages
make build-all-langs
```

## File Structure

```
locales/
├── de/LC_MESSAGES/           # German translations
├── fr/LC_MESSAGES/           # French translations  
├── ja/LC_MESSAGES/           # Japanese translations
├── ru/LC_MESSAGES/           # Russian translations
├── es/LC_MESSAGES/           # Spanish translations
├── zh/LC_MESSAGES/           # Chinese translations
├── id/LC_MESSAGES/           # Indonesian translations
└── ro/LC_MESSAGES/           # Romanian translations
```

Each language directory contains `.po` files that correspond to the source documentation files.
EOF

echo "✅ Internationalization setup complete!"
echo ""
echo "📚 Next steps:"
echo "1. Translators can now edit .po files in locales/<lang>/LC_MESSAGES/"
echo "2. Use 'make build-lang LANG=<code>' to build specific languages"  
echo "3. Use 'make build-all-langs' to build all languages"
echo "4. Use 'make translation-stats' to check translation progress"
echo ""
echo "🔗 Configure Read the Docs project settings:"
echo "1. Go to your Read the Docs project dashboard"
echo "2. Navigate to Admin → Advanced Settings"
echo "3. Enable 'Single version' if you want language selector"
echo "4. Set up subprojects for each language if needed"
echo ""
echo "🌟 Happy translating!"
