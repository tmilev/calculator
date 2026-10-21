# General Project Guidelines & Architecture

## 1. Build System & Compilation
- **Preferred Build Command**: `make -j16` (or `make -j$(nproc)`).
- **Optional Build Configurations**:
  - `make -j16 optimize=1`: Builds with `-O3` optimization.
  - `make -j16 llvm=1`: Builds using `clang++`.
  - `make -j16 nossl=1`: Builds without OpenSSL.
  - `make -j16 wasm=1`: WebAssembly build (`calculator.wasm`).
- **Dependencies**: Keep external dependencies minimal. Mathematics, parser, user authentication, and data structures are natively implemented in C++.

- **Running the Server**:
  - Direct execution: `./calculator server`
  - Restart-on-rebuild daemon mode: `./calculator daemon 1000000` (where `1000000` sets the computation timeout in seconds). The daemon process monitors for executable changes and automatically restarts the web server.

## 2. Code Style & Integrity
- Follow [`codestyle.md`](codestyle.md).
- Preserve existing comments, formatting style, and docstrings.
- Ensure all source modifications are accompanied by relevant unit tests in `src/*_test.cpp`.

## 3. Configuration Management
- Local runtime configuration is stored in `configuration/configuration.json`.
- **Gitignored**: The entire `configuration/` folder is gitignored (line 57 of `.gitignore`), ensuring that sensitive keys, passwords, and machine-specific settings are never committed.
- This is where local port overrides (such as `portHTTPSBuiltIn` set to `"8177"`), database selection, and custom directory roots are configured.

## 4. Web & Network Architecture
- The server manages multiple ports (configured in `configuration/configuration.json`):
  - `portHTTP` (default 8155): Plain HTTP traffic; redirects secure operations (e.g. logins) to HTTPS.
  - `portHTTPSOpenSSL` (default 8166): HTTPS served through OpenSSL.
  - `portHTTPSBuiltIn` (e.g. 8177): In-house C++ TLS server implementation (`TransportLayerSecurityServer`).

