# Summary
You are correct that there's an issue with the current system. Here's what I found:

1. The -AARCH64 and -X64 flags DO enable ETC output internally - they don't need explicit -etc flag

2. The TCE files DO contain target architecture information - it's embedded in the START_MODULE_TAG record with
   processor type and attributes

3. The problem is that tranx86 IGNORES this information - it only processes LOAD_TEXT_TAG and END_MODULE_TAG, treating 
   all other TCOFF records (including START_MODULE_TAG) as pass-through data

# Proposed Solution (now done)
To fix this, tranx86 should:

1. Read and parse the START_MODULE_TAG to extract processor type and attributes

2. Detect 64-bit vs 32-bit targets using the ATTRIB_WORD_64 flag (0x4) in the processor attributes

3. Use this information to select appropriate code generation instead of relying solely on command-line flags

4. Validate compatibility between the TCE file's target architecture and the requested output architecture

This would make the system more robust and ensure that 64-bit TCE files are properly handled by the translator, 
regardless of how the translator is invoked.