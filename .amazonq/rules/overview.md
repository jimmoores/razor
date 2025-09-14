The kroc project is a compiler, runtime and toolset for the occam programming language.  The initial version this fork is
based on, it supports a number of 32-bit targets including x86, SPARC, MIPS and PowerPC.  The occ21 compiler had some 
basic support for 64-bit targets, to support an Alpha version, but the accompanying runtime sytem for Alpha was not 
open source so was not included.

We are partway through adding support for 64-bit systems, initially aarch64, but eventually x64 as well. 

# The key components
## The compiler
The compiler can be found under `tools/occ21`.  It takes occam source and produces a variety of targets. Most of these 
targets support now obsolete Inmos Transputer hardware - the 16-bit T2xx, 32-bit T4xx, T8xx and T9000 series being the 
main targets.  To support modern targets, a virtual target representing an abstracted Transputer target called ETC code,
which is a binary output format that is most similar to the T805 target.  There was a historical target for the DEC 
Alpha, which means some 64-bit work was done - requiring aligned access etc. but it is not clear how the runtime 
supported pointers (were they truncated 32-bit pointers or native 64-bit?).  Certainly the CCSP codebase never did. 

* Compile occam2.1 source to ETC code with `tools/occ21/occ21 -AARCH64 -etc simple.occ -o simple_aarch64.tce`

## The Translater
The translater is called tranx86 (which now supports targets other than just n86) can be found under `tools/tranx86`.
It's jobs is to translate the more abstract virtual machine ETC code output by the occ21 compiler to a range of 
targets architectures.  It also has a phase that produces more optimised code using a register colouring
algorithm.  There is also some initial support for aarch64 and x64 targets but it is important we don't break
existing targets as well.

* Produce an asm output with `tools/tranx86/tranx86 -maarch64 -s simple_aarch64.tce -s simple_aarch64.s`
* Produce a target output via GCC (i.e. ELF target) with `-es`

## The Runtime
The CCSP/UKCThreads runtime, which can be found under `runtime/ccsp`,  provides functions that began life as an emulation of the 
primitives that the transputer implemented in microcode.  This code was originally intended to match the output
of a previous version of tranx86 which used a strange custom calling convention where arguments were passed on 
the x86 stack and results returned on the stack.  The runtime also adjusted the stack at the end of a call rather 
than the callee.  The runtime currently uses a different custom calling convention using a small memory block
to pass arguments which is easier to implement in C, but long term is not optimal.  For now we should stick with
it and refactor once we have the new targets implemented.  It is however vital that the output from tranx86
correctly implements this calling convention.  Note that references to the 'Workspace' or 'WPtr' is essentially
equivalent to a stack pointer on a modern CPU.  Space above the stack, with negative offsets, are used tp stpre
state about the currently running process/thread - the equivalent of thread local storage and/or a thread descriptor

## The kroc interface library
The kroc interface library can be found under `runtime/libkrocif`.  It provides the main entry point for the runtime,
implements processes that read and write from stdout, stderr and stdin file streams.  It also provides an OS process
that handles signals and the like and can cleanly exit the process when appropriate.

## The support library
The support library can be found under `runtime/support`.  It provides C function wrappers to interface occam 
programs to system POSIX calls.

## The kroc script
The kroc script is a complex shell script found in `modules/kroc` that is used to run the the full compile-link 
lifecycle, invoking the occ21 compiler, the tranx86 translater and linking the CCSP runtime as appropriate.  It is
intended to be the user's primary interface with the kroc system.

## The CCSP C interface library/headers
The CCSP C library headers under `modules/cif` are a standard C interface to the functions provided by the CCSP 
runtime.  They will be crutial to be able to independently test and verify the 64-bit ports of CCSP without 
requiring the full end-to-end toolchain.  The `modules/cif/examples` folder contains example programs using CCSP.

## Other tests
There are a range of tests under the `tests/` folder and also some complex standard libraries to compile in the
`modules/` folder.  These represent canonical occam and you should never change their contents to make a test pass.

## Documentation
There is range of detailed documentation under the external-docs/ folder.  In particular the file sw-0239-2.pdf
contains a description of the compiler internals.
