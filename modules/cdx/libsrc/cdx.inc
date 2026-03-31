-- CDX.HANDLE is an opaque C pointer type.  On 64-bit targets it needs
-- INT64 to hold a native pointer; on 32-bit, INT suffices.
#IF DEFINED (TARGET.WORD.BITS)
DATA TYPE CDX.HANDLE IS INT64:
#ELSE
DATA TYPE CDX.HANDLE IS INT:
#ENDIF
