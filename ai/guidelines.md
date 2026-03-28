Coding Guidelines
=================
# Style
* Conform to the coding style used in the codebase generally.
  * Match indentation depth
  * Naming convention of variables and functions (e.g. CamelCase vs underscores, constants in CAPS, etc.)

# Platform specific target code
* Make sure platform specific code is protected with `#ifdef`s and `#define`s or autotools macros.
* Do not change code such that existing platforms would no longer work, even if obsolete.
* Use implementations for existing architectures as a template for aarch64 and x64.
* Look for code that makes assumptions about word length, alignment etc. and make sure 
  behaviour will be as desired on a 64-bit architecture.

# Things to avoid
* Do NOT create stubs for functions that are not working to get past underlying issues.
* NEVER create a code path just to work around a single test passing.
* NEVER change the behavior of the system that was intended just to make a test pass.
* Skipping problems prevents project progress and masks underlying issues.
* Creating temporary workaround when you have no long term memory leads to them becoming permanent.
