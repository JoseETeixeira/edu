# Evolutionary Development Upgrade (EDU)

A high-performance programming language with a clean syntax and modern features.

## Overview

Edu is a statically-typed programming language designed for readability and performance. It features a direct AST interpreter that enables extremely fast execution compared to traditional transpilation approaches.

## Features

- Static type system with type inference
- First-class functions and closures
- Object-oriented programming with classes and inheritance
- Direct AST interpretation for faster execution
- Core language constructs: conditionals, loops, variables, and functions

## Performance

The Edu language offers significant performance improvements with direct AST interpretation:

| Implementation              | Average Time (seconds) |
| --------------------------- | ---------------------- |
| Edu (direct interpretation) | <0.001                 |
| Edu (transpile+compile)     | 0.318                  |
| Python                      | 0.006                  |
| Node.js                     | 0.014                  |

Direct interpretation is at least **300x faster** than the transpilation approach!

## Roadmap

The following features are planned for future releases:

- **For Loop Logic**: Enhanced for loop implementation with iterators
- **Async/Await**: Automatic thread creation and management
- **Module System**: Import functionality for code organization
- **Package Installer**: Dependency management for Edu projects
- **Interpreter Enhancements**:
  - Template literals for string formatting
  - Improved error messages
  - Debugging capabilities
- **Standard Library**: Build out core functionality for common operations

## Getting Started

### Prerequisites

- C++ compiler (GCC or Clang)
- Python 3.x
- SCons build system

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/yourusername/edu.git
cd edu

# Install SCons (if not already installed)
pip install scons

# Build the project
scons
```

### Running Edu Programs

```bash
# Run by interpreting the AST directly (fastest)
./build/edu your_program.edu

# Transpile to C++ without running
./build/edu --transpile your_program.edu

# Transpile to C++, compile and run
./build/edu --compile your_program.edu

# Run with debug output
./build/edu --debug your_program.edu
```

### Performance Testing

```bash
# Run performance comparison
./measure_times.sh
```

## Examples

### Hello World

```
// Hello world in Edu
void function main() {
  print("Hello, World!");
}
```

### Class Example

```
class MathUtils {
  int function add(int x, int y) {
    return x + y;
  }

  int function factorial(int n) {
    if (n <= 1) {
      return 1;
    }
    return n * factorial(n - 1);
  }
}

void function main() {
  MathUtils math = MathUtils();
  print("5! = " + math.factorial(5));
}
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.
