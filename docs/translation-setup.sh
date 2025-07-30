#!/bin/bash
# Translation setup and management script for KISS Fuzzer documentation
# This script sets up Sphinx i18n with gettext for multi-language support

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Supported languages
LANGUAGES=("de" "fr" "es" "ja" "ru" "zh" "id" "ro")

echo -e "${BLUE}KISS Fuzzer Documentation Translation Setup${NC}"
echo "============================================="

# Check if we're in the docs directory
if [[ ! -f "conf.py" ]]; then
    echo -e "${RED}Error: This script must be run from the docs/ directory${NC}"
    exit 1
fi

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required tools
check_dependencies() {
    print_status "Checking dependencies..."
    
    local missing_deps=()
    
    if ! command -v sphinx-build &> /dev/null; then
        missing_deps+=("sphinx")
    fi
    
    if ! command -v sphinx-intl &> /dev/null; then
        missing_deps+=("sphinx-intl")
    fi
    
    if ! command -v msgfmt &> /dev/null; then
        missing_deps+=("gettext")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_status "Install with: pip install sphinx sphinx-intl && apt-get install gettext"
        exit 1
    fi
    
    print_status "All dependencies found!"
}

# Create locale directories
setup_locale_dirs() {
    print_status "Setting up locale directories..."
    
    for lang in "${LANGUAGES[@]}"; do
        mkdir -p "locales/${lang}/LC_MESSAGES"
        print_status "Created directory for language: ${lang}"
    done
}

# Generate .pot files from source
generate_pot_files() {
    print_status "Generating .pot files from documentation source..."
    
    # Clean previous build
    rm -rf _build/gettext
    
    # Generate gettext files
    sphinx-build -b gettext . _build/gettext
    
    if [ $? -eq 0 ]; then
        print_status "Successfully generated .pot files"
    else
        print_error "Failed to generate .pot files"
        exit 1
    fi
}

# Update .po files for all languages
update_po_files() {
    print_status "Updating .po files for all languages..."
    
    # Update for all languages at once
    lang_args=""
    for lang in "${LANGUAGES[@]}"; do
        lang_args="${lang_args} -l ${lang}"
    done
    
    sphinx-intl update -p _build/gettext ${lang_args}
    
    if [ $? -eq 0 ]; then
        print_status "Successfully updated .po files for all languages"
    else
        print_error "Failed to update .po files"
        exit 1
    fi
}

# Build documentation for specific language
build_language() {
    local lang=$1
    print_status "Building documentation for language: ${lang}"
    
    sphinx-build -b html -D language=${lang} . _build/html/${lang}
    
    if [ $? -eq 0 ]; then
        print_status "Successfully built documentation for ${lang}"
    else
        print_error "Failed to build documentation for ${lang}"
        return 1
    fi
}

# Build documentation for all languages
build_all_languages() {
    print_status "Building documentation for all languages..."
    
    # Build English (default)
    sphinx-build -b html . _build/html/en
    
    # Build all translated languages
    for lang in "${LANGUAGES[@]}"; do
        build_language ${lang}
    done
    
    print_status "Documentation built for all languages!"
    print_status "Output directory: _build/html/"
}

# Create language index page
create_language_index() {
    print_status "Creating language selection index..."
    
    cat > _build/html/index.html << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>KISS Fuzzer Documentation - Language Selection</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
            margin: 0;
            padding: 2rem;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .container {
            background: white;
            border-radius: 1rem;
            padding: 3rem;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            max-width: 600px;
            width: 100%;
        }
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 2rem;
            font-size: 2.5rem;
        }
        .language-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 1rem;
            margin-top: 2rem;
        }
        .language-link {
            display: block;
            padding: 1rem;
            text-decoration: none;
            background: #f8f9fa;
            border-radius: 0.5rem;
            transition: all 0.3s ease;
            text-align: center;
            border: 2px solid transparent;
        }
        .language-link:hover {
            background: #e9ecef;
            border-color: #667eea;
            transform: translateY(-2px);
        }
        .flag {
            font-size: 2rem;
            display: block;
            margin-bottom: 0.5rem;
        }
        .lang-name {
            font-weight: 600;
            color: #333;
            margin-bottom: 0.25rem;
        }
        .lang-status {
            font-size: 0.875rem;
            color: #666;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔍 KISS Fuzzer Documentation</h1>
        <p style="text-align: center; color: #666; margin-bottom: 2rem;">
            Choose your preferred language / Choisissez votre langue / Wählen Sie Ihre Sprache
        </p>
        
        <div class="language-grid">
            <a href="en/" class="language-link">
                <span class="flag">🇺🇸</span>
                <div class="lang-name">English</div>
                <div class="lang-status">Native</div>
            </a>
            <a href="de/" class="language-link">
                <span class="flag">🇩🇪</span>
                <div class="lang-name">Deutsch</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="fr/" class="language-link">
                <span class="flag">🇫🇷</span>
                <div class="lang-name">Français</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="es/" class="language-link">
                <span class="flag">🇪🇸</span>
                <div class="lang-name">Español</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="ja/" class="language-link">
                <span class="flag">🇯🇵</span>
                <div class="lang-name">日本語</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="ru/" class="language-link">
                <span class="flag">🇷🇺</span>
                <div class="lang-name">Русский</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="zh/" class="language-link">
                <span class="flag">🇨🇳</span>
                <div class="lang-name">中文</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="id/" class="language-link">
                <span class="flag">🇮🇩</span>
                <div class="lang-name">Bahasa Indonesia</div>
                <div class="lang-status">Auto-translated</div>
            </a>
            <a href="ro/" class="language-link">
                <span class="flag">🇷🇴</span>
                <div class="lang-name">Română</div>
                <div class="lang-status">Auto-translated</div>
            </a>
        </div>
        
        <p style="text-align: center; margin-top: 2rem; color: #666; font-size: 0.875rem;">
            Auto-translated documentation may contain inaccuracies. For technical precision, refer to the English version.
        </p>
    </div>
</body>
</html>
EOF
    
    print_status "Language selection index created"
}

# Show usage information
show_usage() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  setup     - Initial setup of translation environment"
    echo "  update    - Update .po files from source changes"
    echo "  build     - Build documentation for all languages"
    echo "  clean     - Clean build directories"
    echo "  stats     - Show translation statistics"
    echo ""
    echo "Language-specific commands:"
    echo "  build-lang LANG  - Build documentation for specific language"
    echo "  stats-lang LANG  - Show translation stats for specific language"
    echo ""
    echo "Supported languages: ${LANGUAGES[*]}"
}

# Show translation statistics
show_stats() {
    print_status "Translation Statistics"
    print_status "======================"
    
    for lang in "${LANGUAGES[@]}"; do
        if [ -d "locales/${lang}/LC_MESSAGES" ]; then
            echo -e "\n${BLUE}Language: ${lang}${NC}"
            
            po_files=$(find "locales/${lang}/LC_MESSAGES" -name "*.po" 2>/dev/null | wc -l)
            echo "  .po files: ${po_files}"
            
            if [ ${po_files} -gt 0 ]; then
                total_strings=0
                translated_strings=0
                
                for po_file in locales/${lang}/LC_MESSAGES/*.po; do
                    if [ -f "${po_file}" ]; then
                        total=$(msgfmt --statistics "${po_file}" 2>&1 | grep -o '[0-9]\+ translated' | grep -o '[0-9]\+' || echo "0")
                        translated_strings=$((translated_strings + total))
                        
                        untranslated=$(msgfmt --statistics "${po_file}" 2>&1 | grep -o '[0-9]\+ untranslated' | grep -o '[0-9]\+' || echo "0")
                        fuzzy=$(msgfmt --statistics "${po_file}" 2>&1 | grep -o '[0-9]\+ fuzzy' | grep -o '[0-9]\+' || echo "0")
                        
                        total_strings=$((total_strings + total + untranslated + fuzzy))
                    fi
                done
                
                if [ ${total_strings} -gt 0 ]; then
                    percentage=$((translated_strings * 100 / total_strings))
                    echo "  Translated: ${translated_strings}/${total_strings} (${percentage}%)"
                else
                    echo "  Translated: 0/0 (0%)"
                fi
            fi
        else
            echo -e "\n${YELLOW}Language: ${lang} - Not initialized${NC}"
        fi
    done
}

# Clean build directories
clean_build() {
    print_status "Cleaning build directories..."
    rm -rf _build/
    print_status "Build directories cleaned"
}

# Main script logic
case "${1:-}" in
    "setup")
        check_dependencies
        setup_locale_dirs
        generate_pot_files
        update_po_files
        print_status "Translation environment setup complete!"
        print_warning "Next steps:"
        print_warning "1. Translate the .po files in locales/<lang>/LC_MESSAGES/"
        print_warning "2. Run './translation-setup.sh build' to build documentation"
        ;;
    "update")
        check_dependencies
        generate_pot_files
        update_po_files
        print_status "Translation files updated!"
        ;;
    "build")
        check_dependencies
        build_all_languages
        create_language_index
        print_status "All documentation built successfully!"
        ;;
    "build-lang")
        if [ -z "${2:-}" ]; then
            print_error "Please specify a language code"
            print_status "Available languages: ${LANGUAGES[*]}"
            exit 1
        fi
        check_dependencies
        build_language "$2"
        ;;
    "clean")
        clean_build
        ;;
    "stats")
        show_stats
        ;;
    "stats-lang")
        if [ -z "${2:-}" ]; then
            print_error "Please specify a language code"
            exit 1
        fi
        # Show stats for specific language (simplified version)
        lang="$2"
        if [ -d "locales/${lang}/LC_MESSAGES" ]; then
            echo -e "${BLUE}Translation statistics for: ${lang}${NC}"
            for po_file in locales/${lang}/LC_MESSAGES/*.po; do
                if [ -f "${po_file}" ]; then
                    echo "$(basename ${po_file}):"
                    msgfmt --statistics "${po_file}" 2>&1 | sed 's/^/  /'
                fi
            done
        else
            print_error "Language ${lang} not found or not initialized"
        fi
        ;;
    *)
        show_usage
        ;;
esac
