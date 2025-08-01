#!/bin/bash

# Multi-language documentation setup for Read the Docs
# This script sets up proper internationalization

set -e

echo "🌍 Setting up multi-language documentation..."

# Change to docs directory
cd docs

# Clean up any existing builds
echo "🧹 Cleaning existing builds..."
rm -rf _build/

# Extract translatable strings
echo "📝 Extracting translatable strings..."
make gettext

# Update translation files for all languages
echo "🔄 Updating translation files..."
sphinx-intl update -p _build/gettext -l de -l fr -l es -l ja -l ru -l zh -l id -l ro

# Build the main English documentation
echo "🇺🇸 Building English documentation..."
make html

# Create language-specific configuration files for Read the Docs
echo "⚙️ Creating language-specific configurations..."

# Create a base template for language configs
cat > .readthedocs-template.yaml << 'EOF'
version: 2

build:
  os: ubuntu-22.04
  tools:
    python: "3.11"
  jobs:
    pre_build:
      - apt-get update
      - apt-get install -y gettext

sphinx:
  configuration: docs/conf.py
  fail_on_warning: false

formats:
  - pdf
  - epub

python:
  install:
    - requirements: docs/requirements.txt
EOF

# Languages and their codes
declare -A LANGUAGES
LANGUAGES["de"]="Deutsch"
LANGUAGES["fr"]="Français" 
LANGUAGES["es"]="Español"
LANGUAGES["ja"]="日本語"
LANGUAGES["ru"]="Русский"
LANGUAGES["zh"]="中文"
LANGUAGES["id"]="Bahasa Indonesia"
LANGUAGES["ro"]="Română"

# Create language-specific conf.py files
for lang in "${!LANGUAGES[@]}"; do
    echo "📖 Setting up $lang (${LANGUAGES[$lang]})..."
    
    # Create a language-specific configuration
    cp conf.py "conf-${lang}.py"
    
    # Modify the language setting
    sed -i.bak "s/language = 'en'/language = '${lang}'/" "conf-${lang}.py"
    rm "conf-${lang}.py.bak"
    
    # Create a Read the Docs config for this language
    cat > "../.readthedocs-${lang}.yaml" << EOF
version: 2

build:
  os: ubuntu-22.04
  tools:
    python: "3.11"
  jobs:
    pre_build:
      - apt-get update
      - apt-get install -y gettext

sphinx:
  configuration: docs/conf-${lang}.py
  fail_on_warning: false

formats:
  - pdf
  - epub

python:
  install:
    - requirements: docs/requirements.txt
EOF

done

echo "✅ Multi-language setup complete!"
echo ""
echo "📋 Next steps for Read the Docs:"
echo "1. In your Read the Docs dashboard, go to your project settings"
echo "2. Under 'Advanced Settings', enable 'Build pull requests for this project'"
echo "3. Create translation projects for each language:"
echo "   - Go to 'Translations' in your project dashboard"
echo "   - Add each language as a separate translation project"
echo "   - Or set up separate projects for each language"
echo ""
echo "🔗 Language URLs will be available at:"
for lang in "${!LANGUAGES[@]}"; do
    echo "   https://kiss-fuzz.readthedocs.io/${lang}/latest/ (${LANGUAGES[$lang]})"
done
echo ""
echo "📚 For more info: https://docs.readthedocs.io/en/stable/localization.html"
