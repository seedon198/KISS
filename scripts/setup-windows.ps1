# KISS Fuzzer Development Environment Setup
# Windows 10/11 PowerShell Script
# 
# This script installs all required dependencies for KISS Fuzzer development
# on Windows systems using Chocolatey package manager.

param(
    [switch]$Force = $false,
    [switch]$SkipOptional = $false
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Colors for output (Windows PowerShell compatible)
$ColorInfo = "Cyan"
$ColorSuccess = "Green" 
$ColorWarning = "Yellow"
$ColorError = "Red"

# Logging functions
function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor $ColorInfo
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor $ColorSuccess
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor $ColorWarning
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor $ColorError
}

# Check if running as administrator
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# Check Windows version
function Test-WindowsVersion {
    $version = [System.Environment]::OSVersion.Version
    $isWindows10Plus = ($version.Major -eq 10 -and $version.Build -ge 19041) -or ($version.Major -gt 10)
    
    if ($isWindows10Plus) {
        Write-Success "Windows version $($version.ToString()) is supported"
        return $true
    } else {
        Write-Warning "Windows version $($version.ToString()) detected"
        Write-Warning "Windows 10 version 2004+ or Windows 11 recommended"
        return $false
    }
}

# Install Chocolatey package manager
function Install-Chocolatey {
    if (Get-Command choco -ErrorAction SilentlyContinue) {
        Write-Success "Chocolatey is already installed"
        choco --version
    } else {
        Write-Info "Installing Chocolatey package manager..."
        
        # Set execution policy for current process
        Set-ExecutionPolicy Bypass -Scope Process -Force
        
        # Download and install Chocolatey
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        
        Write-Success "Chocolatey installed successfully"
    }
}

# Install development tools via Chocolatey
function Install-DevelopmentTools {
    Write-Info "Installing development tools..."
    
    # Essential build tools
    $buildTools = @(
        "cmake",
        "ninja", 
        "git",
        "python3",
        "vscode",
        "7zip"
    )
    
    foreach ($tool in $buildTools) {
        Write-Info "Installing $tool..."
        try {
            choco install $tool -y
        } catch {
            Write-Warning "Failed to install $tool, continuing..."
        }
    }
    
    Write-Success "Development tools installed"
}

# Install ARM GCC toolchain
function Install-ArmToolchain {
    Write-Info "Installing ARM GCC toolchain..."
    
    try {
        # Install GNU ARM Embedded Toolchain
        choco install gcc-arm-embedded -y
        
        # Verify installation
        $armGccPath = Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue
        if ($armGccPath) {
            $version = & arm-none-eabi-gcc --version | Select-Object -First 1
            Write-Success "ARM GCC installed: $version"
        } else {
            throw "ARM GCC not found in PATH"
        }
    } catch {
        Write-Warning "Chocolatey ARM GCC installation failed, trying manual installation..."
        
        # Manual installation from ARM website
        $downloadUrl = "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-win32.exe"
        $installerPath = "$env:TEMP\gcc-arm-none-eabi-installer.exe"
        
        Write-Info "Downloading ARM GCC from ARM website..."
        Invoke-WebRequest -Uri $downloadUrl -OutFile $installerPath
        
        Write-Info "Running ARM GCC installer..."
        Start-Process -FilePath $installerPath -ArgumentList "/S" -Wait
        
        # Add to PATH if not already there
        $armPath = "C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10\bin"
        if (Test-Path $armPath) {
            $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
            if ($currentPath -notlike "*$armPath*") {
                [Environment]::SetEnvironmentVariable("PATH", "$currentPath;$armPath", "User")
                Write-Info "Added ARM GCC to user PATH"
            }
        }
        
        Write-Success "ARM GCC toolchain installed manually"
    }
}

# Install LLVM tools
function Install-LLVMTools {
    Write-Info "Installing LLVM tools..."
    
    try {
        choco install llvm -y
        Write-Success "LLVM tools installed via Chocolatey"
    } catch {
        Write-Warning "Chocolatey LLVM installation failed, trying manual installation..."
        
        # Manual LLVM installation
        $llvmUrl = "https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.7/LLVM-15.0.7-win64.exe"
        $installerPath = "$env:TEMP\llvm-installer.exe"
        
        Write-Info "Downloading LLVM..."
        Invoke-WebRequest -Uri $llvmUrl -OutFile $installerPath
        
        Write-Info "Running LLVM installer..."
        Start-Process -FilePath $installerPath -ArgumentList "/S" -Wait
        
        Write-Success "LLVM tools installed manually"
    }
}

# Install static analysis tools
function Install-AnalysisTools {
    Write-Info "Installing static analysis tools..."
    
    # Install cppcheck
    try {
        choco install cppcheck -y
    } catch {
        Write-Warning "Failed to install cppcheck via Chocolatey"
    }
    
    Write-Success "Static analysis tools installed"
}

# Install Python development tools
function Install-PythonTools {
    Write-Info "Installing Python development tools..."
    
    # Upgrade pip
    & python -m pip install --upgrade pip
    
    # Install Python quality tools
    $pythonPackages = @(
        "black",
        "isort", 
        "flake8",
        "mypy",
        "pre-commit",
        "sphinx",
        "sphinx-rtd-theme",
        "myst-parser",
        "sphinx-copybutton",
        "sphinxcontrib-mermaid",
        "breathe"
    )
    
    foreach ($package in $pythonPackages) {
        Write-Info "Installing Python package: $package"
        try {
            & python -m pip install $package
        } catch {
            Write-Warning "Failed to install $package"
        }
    }
    
    Write-Success "Python tools installed"
}

# Install Git LFS
function Install-GitLFS {
    Write-Info "Installing Git LFS..."
    
    try {
        choco install git-lfs -y
        & git lfs install
        Write-Success "Git LFS installed"
    } catch {
        Write-Warning "Failed to install Git LFS"
    }
}

# Install WSL2 (optional)
function Install-WSL2 {
    if ($SkipOptional) {
        Write-Info "Skipping WSL2 installation (optional)"
        return
    }
    
    Write-Info "Checking WSL2 installation..."
    
    $wslStatus = wsl --status 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "WSL2 is already installed"
        return
    }
    
    Write-Info "Installing WSL2 (Windows Subsystem for Linux)..."
    
    try {
        # Enable Windows features
        dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
        dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
        
        # Install WSL2
        wsl --install --no-distribution
        
        Write-Success "WSL2 installed (requires restart)"
        Write-Info "After restart, run: wsl --install -d Ubuntu"
    } catch {
        Write-Warning "WSL2 installation failed or requires manual setup"
    }
}

# Setup Pico SDK
function Install-PicoSDK {
    Write-Info "Installing Raspberry Pi Pico SDK..."
    
    $picoSdkPath = "$env:USERPROFILE\pico-sdk"
    
    if (Test-Path $picoSdkPath) {
        Write-Info "Pico SDK already exists, updating..."
        Set-Location $picoSdkPath
        & git pull
        & git submodule update --init --recursive
    } else {
        Write-Info "Cloning Pico SDK..."
        & git clone https://github.com/raspberrypi/pico-sdk.git $picoSdkPath
        Set-Location $picoSdkPath
        & git submodule update --init --recursive
    }
    
    # Set environment variable
    [Environment]::SetEnvironmentVariable("PICO_SDK_PATH", $picoSdkPath, "User")
    $env:PICO_SDK_PATH = $picoSdkPath
    
    Write-Success "Pico SDK installed at $picoSdkPath"
}

# Install FreeRTOS kernel
function Install-FreeRTOS {
    Write-Info "Installing FreeRTOS kernel..."
    
    $freertosPath = "$env:USERPROFILE\FreeRTOS-Kernel"
    
    if (Test-Path $freertosPath) {
        Write-Info "FreeRTOS kernel already exists, updating..."
        Set-Location $freertosPath
        & git pull
    } else {
        Write-Info "Cloning FreeRTOS kernel..."
        & git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git $freertosPath
    }
    
    Write-Success "FreeRTOS kernel installed"
}

# Setup project environment
function Setup-ProjectEnvironment {
    Write-Info "Setting up project environment..."
    
    # Return to project directory
    Set-Location $ProjectRoot
    
    # Initialize git submodules if any
    if (Test-Path ".gitmodules") {
        & git submodule update --init --recursive
    }
    
    # Create build directory
    New-Item -ItemType Directory -Name "build" -Force | Out-Null
    
    # Run Python setup script if available
    if (Test-Path "scripts\setup-dev-tools.py") {
        Write-Info "Running Python development tools setup..."
        & python scripts\setup-dev-tools.py
    }
    
    Write-Success "Project environment configured"
}

# Test build system
function Test-BuildSystem {
    Write-Info "Testing build system..."
    
    Set-Location "$ProjectRoot\build"
    
    # Test CMake configuration
    Write-Info "Testing CMake configuration..."
    try {
        & cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON -G "Ninja" ..
        Write-Success "CMake configuration successful"
    } catch {
        Write-Error-Custom "CMake configuration failed"
        return $false
    }
    
    # Test build targets
    Write-Info "Testing available build targets..."
    & cmake --build . --target help
    
    Write-Success "Build system test completed"
    return $true
}

# Verify installation
function Test-Installation {
    Write-Info "Verifying installation..."
    
    # Check required tools
    $tools = @{
        "cmake" = "CMake"
        "arm-none-eabi-gcc" = "ARM GCC"
        "clang-format" = "clang-format"
        "python" = "Python"
        "git" = "Git"
    }
    
    $allGood = $true
    
    foreach ($tool in $tools.Keys) {
        $toolName = $tools[$tool]
        try {
            $command = Get-Command $tool -ErrorAction Stop
            $version = & $tool --version 2>$null | Select-Object -First 1
            Write-Success "$toolName`: $version"
        } catch {
            Write-Error-Custom "$toolName not found"
            $allGood = $false
        }
    }
    
    # Check Python packages
    $pythonPackages = @("black", "isort", "flake8", "mypy", "pre-commit")
    
    foreach ($package in $pythonPackages) {
        try {
            $version = & python -c "import $package; print($package.__version__)" 2>$null
            Write-Success "Python $package`: $version"
        } catch {
            Write-Error-Custom "Python package $package not found"
            $allGood = $false
        }
    }
    
    # Check environment variables
    if ($env:PICO_SDK_PATH -and (Test-Path $env:PICO_SDK_PATH)) {
        Write-Success "PICO_SDK_PATH: $env:PICO_SDK_PATH"
    } else {
        Write-Error-Custom "PICO_SDK_PATH not set or invalid"
        $allGood = $false
    }
    
    if ($allGood) {
        Write-Success "All tools and dependencies verified successfully!"
        return $true
    } else {
        Write-Error-Custom "Some tools or dependencies are missing"
        return $false
    }
}

# Print next steps
function Write-NextSteps {
    Write-Host @"

🎉 KISS Fuzzer Development Environment Setup Complete!

Next Steps:
1. Restart PowerShell or reload environment variables
2. Navigate to project directory: cd $ProjectRoot
3. Test the build system:
   mkdir build -Force; cd build
   cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON -G "Ninja" ..
   cmake --build . --target check-all

Available Development Commands:
• cmake --build . --target check-all     - Run all quality checks
• cmake --build . --target fix-all       - Auto-fix code formatting
• cmake --build . --target format-check  - Check C/C++ formatting
• cmake --build . --target cppcheck      - Run static analysis
• cmake --build .                        - Build firmware

Hardware Development:
• Connect Raspberry Pi Pico via USB
• Use Device Manager to verify COM port
• Flash firmware: drag .uf2 file to mounted Pico drive

Tools Installed:
• Visual Studio Code with C/C++ extensions
• ARM GCC cross-compiler
• CMake and Ninja build system
• LLVM tools (clang-format, clang-tidy)
• Python development tools
• Git with LFS support

Documentation:
• Project Docs: https://kiss-fuzz.readthedocs.io/
• Development Guide: docs/development.md
• Quality System: cmake/README.md

Happy coding! 🚀

"@ -ForegroundColor Green
}

# Main execution function
function main {
    # Store project root
    $script:ProjectRoot = Get-Location
    
    Write-Host @"
╔═══════════════════════════════════════════════════════════════╗
║                    KISS Fuzzer Setup                          ║
║                    Windows 10/11                              ║
║                                                               ║
║  This script will install all development dependencies        ║
║  for KISS Fuzzer embedded development on Windows.            ║
╚═══════════════════════════════════════════════════════════════╝

"@ -ForegroundColor Cyan
    
    Write-Info "Starting KISS Fuzzer development environment setup..."
    
    # Check prerequisites
    if (-not (Test-Administrator)) {
        Write-Error-Custom "This script must be run as Administrator"
        Write-Info "Please right-click PowerShell and select 'Run as Administrator'"
        exit 1
    }
    
    if (-not (Test-WindowsVersion)) {
        if (-not $Force) {
            Write-Error-Custom "Unsupported Windows version. Use -Force to continue anyway."
            exit 1
        }
    }
    
    try {
        # Run all setup steps
        Install-Chocolatey
        Install-DevelopmentTools
        Install-ArmToolchain
        Install-LLVMTools
        Install-AnalysisTools
        Install-PythonTools
        Install-GitLFS
        Install-WSL2
        Install-PicoSDK
        Install-FreeRTOS
        Setup-ProjectEnvironment
        
        # Test and verify
        if ((Test-BuildSystem) -and (Test-Installation)) {
            Write-NextSteps
            Write-Success "Setup completed successfully!"
            exit 0
        } else {
            Write-Error-Custom "Setup encountered errors. Please check the output above."
            exit 1
        }
    } catch {
        Write-Error-Custom "Setup failed with error: $($_.Exception.Message)"
        Write-Error-Custom "Please check the output above and try running individual commands manually."
        exit 1
    }
}

# Run main function
main
