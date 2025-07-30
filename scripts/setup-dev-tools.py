#!/usr/bin/env python3
"""
KISS Fuzzer Development Environment Setup Script

This script installs and configures code quality tools for the KISS Fuzzer project.
It supports multiple package managers and platforms.
"""

import os
import sys
import subprocess
import platform
import shutil
from pathlib import Path


def run_command(cmd, shell=False, capture_output=True):
    """Run a shell command and return the result."""
    try:
        result = subprocess.run(
            cmd, shell=shell, capture_output=capture_output, text=True, check=True
        )
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        return False, e.stdout, e.stderr
    except FileNotFoundError:
        return False, "", f"Command not found: {cmd[0] if isinstance(cmd, list) else cmd}"


def check_tool(tool_name, cmd=None):
    """Check if a tool is available."""
    if cmd is None:
        cmd = [tool_name, "--version"]
    
    success, stdout, stderr = run_command(cmd)
    return success, stdout.strip() if success else stderr.strip()


def install_with_package_manager():
    """Install tools using system package manager."""
    system = platform.system().lower()
    
    if system == "darwin":  # macOS
        # Check for Homebrew
        if shutil.which("brew"):
            print("Installing tools with Homebrew...")
            commands = [
                ["brew", "install", "llvm"],  # includes clang-format, clang-tidy
                ["brew", "install", "cppcheck"],
            ]
        else:
            print("Homebrew not found. Please install Homebrew first:")
            print("  /bin/bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"")
            return False
            
    elif system == "linux":
        # Try apt-get (Ubuntu/Debian)
        if shutil.which("apt-get"):
            print("Installing tools with apt-get...")
            commands = [
                ["sudo", "apt-get", "update"],
                ["sudo", "apt-get", "install", "-y", "clang-format", "clang-tidy", "cppcheck"],
            ]
        # Try dnf (Fedora)
        elif shutil.which("dnf"):
            print("Installing tools with dnf...")
            commands = [
                ["sudo", "dnf", "install", "-y", "clang-tools-extra", "cppcheck"],
            ]
        # Try yum (CentOS/RHEL)
        elif shutil.which("yum"):
            print("Installing tools with yum...")
            commands = [
                ["sudo", "yum", "install", "-y", "clang-tools-extra", "cppcheck"],
            ]
        else:
            print("No supported package manager found (apt-get, dnf, yum)")
            return False
    else:
        print(f"Unsupported system: {system}")
        return False
    
    # Execute installation commands
    for cmd in commands:
        print(f"Running: {' '.join(cmd)}")
        success, stdout, stderr = run_command(cmd)
        if not success:
            print(f"Failed to run command: {stderr}")
            return False
    
    return True


def install_python_tools():
    """Install Python code quality tools."""
    print("Installing Python tools...")
    
    # Check if pip is available
    if not shutil.which("pip") and not shutil.which("pip3"):
        print("pip not found. Please install Python pip first.")
        return False
    
    pip_cmd = "pip3" if shutil.which("pip3") else "pip"
    
    # Python tools to install
    python_tools = [
        "black",      # Code formatter
        "isort",      # Import sorter
        "flake8",     # Linter
        "mypy",       # Type checker
        "pre-commit", # Git hooks
    ]
    
    for tool in python_tools:
        print(f"Installing {tool}...")
        success, stdout, stderr = run_command([pip_cmd, "install", tool])
        if not success:
            print(f"Failed to install {tool}: {stderr}")
            return False
    
    return True


def setup_pre_commit_hooks():
    """Setup pre-commit hooks for automated checks."""
    print("Setting up pre-commit hooks...")
    
    # Create .pre-commit-config.yaml if it doesn't exist
    pre_commit_config = Path(".pre-commit-config.yaml")
    if not pre_commit_config.exists():
        config_content = """# KISS Fuzzer pre-commit configuration
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-merge-conflict
      - id: check-yaml
      - id: check-added-large-files
      - id: check-case-conflict

  - repo: https://github.com/psf/black
    rev: 23.1.0
    hooks:
      - id: black
        language_version: python3

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort

  - repo: https://github.com/pycqa/flake8
    rev: 6.0.0
    hooks:
      - id: flake8

  - repo: local
    hooks:
      - id: clang-format
        name: clang-format
        entry: clang-format
        language: system
        files: \\.(c|h|cpp|hpp)$
        args: [--style=file, -i]

      - id: cmake-format
        name: cmake-format
        entry: cmake-format
        language: system
        files: CMakeLists\\.txt$|.*\\.cmake$
        args: [--in-place]
"""
        
        with open(pre_commit_config, "w") as f:
            f.write(config_content)
        print("Created .pre-commit-config.yaml")
    
    # Install pre-commit hooks
    success, stdout, stderr = run_command(["pre-commit", "install"])
    if not success:
        print(f"Failed to install pre-commit hooks: {stderr}")
        return False
    
    print("Pre-commit hooks installed successfully!")
    return True


def create_development_scripts():
    """Create convenience scripts for development."""
    scripts_dir = Path("scripts")
    scripts_dir.mkdir(exist_ok=True)
    
    # Quality check script
    quality_script = scripts_dir / "check-quality.sh"
    quality_content = """#!/bin/bash
# KISS Fuzzer Code Quality Check Script

set -e

echo "🔍 Running code quality checks..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with quality checks enabled
cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..

echo "📋 Running all quality checks..."
make check-all

echo "✅ All quality checks passed!"
"""
    
    with open(quality_script, "w") as f:
        f.write(quality_content)
    quality_script.chmod(0o755)
    
    # Auto-fix script
    fix_script = scripts_dir / "fix-quality.sh"
    fix_content = """#!/bin/bash
# KISS Fuzzer Code Quality Auto-Fix Script

set -e

echo "🔧 Auto-fixing code quality issues..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with quality checks enabled
cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..

echo "🛠️  Running auto-fixers..."
make fix-all

echo "✅ Auto-fix completed!"
echo "ℹ️  Please review the changes and run check-quality.sh to verify."
"""
    
    with open(fix_script, "w") as f:
        f.write(fix_content)
    fix_script.chmod(0o755)
    
    print("Created development scripts in scripts/ directory")


def main():
    """Main setup function."""
    print("🚀 KISS Fuzzer Development Environment Setup")
    print("=" * 50)
    
    # Check current tools
    print("\n📋 Checking existing tools...")
    tools_status = {}
    
    tools_to_check = [
        ("clang-format", ["clang-format", "--version"]),
        ("clang-tidy", ["clang-tidy", "--version"]),
        ("cppcheck", ["cppcheck", "--version"]),
        ("black", ["python3", "-m", "black", "--version"]),
        ("isort", ["python3", "-m", "isort", "--version"]),
        ("flake8", ["python3", "-m", "flake8", "--version"]),
    ]
    
    for tool_name, cmd in tools_to_check:
        available, output = check_tool(tool_name, cmd)
        tools_status[tool_name] = available
        status = "✅" if available else "❌"
        print(f"  {status} {tool_name}: {output if available else 'Not found'}")
    
    # Install missing tools
    missing_c_tools = not all([tools_status.get("clang-format"), tools_status.get("clang-tidy"), tools_status.get("cppcheck")])
    missing_python_tools = not all([tools_status.get("black"), tools_status.get("isort"), tools_status.get("flake8")])
    
    if missing_c_tools:
        print("\n🔧 Installing C/C++ tools...")
        if not install_with_package_manager():
            print("❌ Failed to install C/C++ tools")
            return False
    
    if missing_python_tools:
        print("\n🐍 Installing Python tools...")
        if not install_python_tools():
            print("❌ Failed to install Python tools")
            return False
    
    # Setup additional development tools
    print("\n⚙️  Setting up development environment...")
    
    if not setup_pre_commit_hooks():
        print("⚠️  Pre-commit setup failed, but continuing...")
    
    create_development_scripts()
    
    print("\n✅ Setup completed successfully!")
    print("\n📝 Next steps:")
    print("  1. Run './scripts/check-quality.sh' to verify code quality")
    print("  2. Run './scripts/fix-quality.sh' to auto-fix issues")
    print("  3. Use 'make check-all' and 'make fix-all' in build directory")
    print("  4. Pre-commit hooks will run automatically on git commit")
    
    return True


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
