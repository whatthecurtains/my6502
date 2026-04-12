opcodes_verify/ README — inst_action conventions

Purpose
- Describe the machine-readable syntax used in the JSON files' inst_action field so future tools and contributors interpret them consistently.

Core syntax
- Assignment: TARGET <- EXPRESSION
  - TARGET is a register (A, X, Y, SP) or memory: M(<effective address>)
- Memory: M(expr) denotes the byte read from or written to the address computed by expr.
- Registers: A, X, Y, SP, PC. Status bits use SR.<bit> (SR.C, SR.Z, SR.N, SR.V, SR.I, SR.D).
- Operators: +, -, <<, >>, |, &, ^.
- Conditional flags: SR.Z <- (result==0) ? 1 : 0; SR.N <- (result & 0x80) ? 1 : 0.
- Arithmetic: show the destination update, then explicit SR updates when applicable, e.g.: A <- A + M(PC+1) + SR.C; SR.C <- (carry expression); SR.Z <- (...); SR.N <- (...); SR.V <- (...)

Effective-address examples
- Immediate: M(PC+1)
- Zero page: M(M(PC+1))
- X-indexed zero page: M((M(PC+1)+X)&0xFF)  # wrap to page zero
- Zero page indirect,Y: M(M(M(PC+1)) + Y)
- Absolute: M(M(PC+2)<<8 | M(PC+1))
- X/Y indexed absolute: M((M(PC+2)<<8|M(PC+1))+X) etc.
- Absolute indirect: M(M(M(PC+2)<<8|M(PC+1)) | M(M(PC+2)<<8|M(PC+1)+1)<<8)

Stack and PC
- Stack push/pull use M(SP) and M(SP+N) as shown in the JSON. Example push: M(SP) <- SR
- PC updates use PC <- <expr> or PC <- PC + M(PC+1) for relative branches.
- When an instruction restores the entire status register, use: SR <- <expr>

Temporaries
- Temporary names such as orig, res, oldC are permitted in inst_action when needed to express multi-step updates.

Other notes
- Illegal opcodes are placed at the end of op2.json. Many complex illegal/undocumented opcodes remain with empty inst_action; fill only when semantics are certain.
- Keep expressions short, deterministic, and unambiguous. If a desired inst_action cannot be determined from the "description", ask for clarification before guessing.

Contact
- If you want the README to show more examples or stricter grammar (BNF), request an update.
