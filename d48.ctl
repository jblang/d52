;
; D48.CTL - Control File for D48
;
;   Control codes allowed in the CTL file:
;
;  A - Address          Specifies that the word entry is the address of
;                       something for which a label should be generated.
;
;  B - Byte binary      Eight bit binary data (db).
;
;  C - Code             Executable code that must be analyzed.
;
;  I - Ignore           Treat as uninitialized space. Will not be dis-
;                       assembled as anything unless some other valid
;                       code reaches it.
;
;  L - Label            Generate a label for this address.
;
;  M - Memory select    Select memory bank (DBF flip-flop) for instruction
;                       at address specified. m <mb> <address>, where mb
;                       is 0 or 1.
;
;  R - Register         Specify name for register
;                       (instead of rb2r5 for example).
;
;  S - Symbol           Generate a symbol for this value.
;
;  T - Text             ASCII text (db).
;
;  W - Word binary      Sixteen bit binary data (dw).
;
;  X - Operand name	Specify special name for operand.
;
;  Y - Operand name	Specify special name for operand, suppress EQU generation.
;
;  # - Comment		Add header comment to output file.
;
;  ! - Inline comment	Add comment to end of line.
;
;  The difference between labels and symbols is that a label refers
;  to a referenced address, whereas a symbol may be used for 8 or 16
;  bit immediate data. For some opcodes (eg: mov r2,#xx) only the symbol
;  table will be searched. Other opcodes (eg: mov dptr,#) will search
;  the label table first and then search the symbol table only if the
;  value is not found in the label table.
;
;  Values for some control codes may specify ranges, ie:
;
;       A 100-11f specifies a table of addresses starting at address
;       0x100 and continuing to address 0x11f.
;
;       T 100-120 specifies that addresses 0x100 through (and including)
;       address 0x120 contain ascii text data.
;
;  Range specifications are allowed for codes A, B, C, I, T, and W, but
;  obviously don't apply to codes L, R, S, and X.
;
;---------------------------------------------------------------------------
;
; labels for interrupt vectors
;
L 0   reset
L 3   intvec
L 7   tcvec
;
; tell D48 to disassemble code at interrupt vector locations
;
C 0     ; reset
C 3     ; interrupt
C 7     ; timer/counter interrupt
;
; end of control file
;
