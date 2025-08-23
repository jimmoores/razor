This project is for 32-bit x86 systems. The eventual aim is to support 64-bit systems, initially x64, but eventually 
aarch64 as well. 
 * The first issue is that the build system hardcodes 32-bit flags for the C compiler, which needs to be
   fixed.  Once the system builds, it still won't create 64-bit code.
 * The compiler, under tools/occ21, takes occam source and produces a variety of legacy targets.  The target used
   for translation to modern targets is called ETC code, which is a pseudo-transputer binary output that is most similar 
   to the T805 target.  There was a historical target of DEC Alpha, which means some 64-bit work was done, but I'm not
   sure how the runtime supported pointers (were they truncated 32-bit pointers or native 64-bit?).
 * The translater from ETC code output by the compiler to target architectures is in the tools/transx86 folder.  This 
   supports a range of targets.  Ideally we would extend this existing codebase to support 64-bit x86.
 * The main x86 runtime is under runtime/ccsp and possibly still uses a custom calling convention that uses macros to
   manipulate arguments to the runtime on and off the stack.
 * There are a range of tests under the tests/ folder and also some complex standard libraries to compile in the
   modules/ folder.  These represent canonical occam and you should never change their contents to make a test pass.
 * There is range of detailed documentation under the external-docs/ folder.  In particular the file sw-0239-2.pdf
   contains a description of the compiler internals.