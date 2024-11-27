# Assembly to Machine Language Translator 

This repository contains a C-based assembler implementation for translating assembly language files into machine code.

## Features
- Assembly File Parsing: Reads and interprets assembly language files.
- Error Detection: Identifies and reports syntax and semantic errors in the input.
- Symbol Table Management: Maps labels to memory addresses and maintains a structured symbol table.
- Binary Code Generation: Converts valid assembly instructions into machine-readable binary code.

## How It Works
1. Input: Provide an assembly file as input.
2. Processing:
  - Parses the assembly file line by line.
  - Validates syntax and semantic correctness.
  - Generates a symbol table and intermediate code.
  - Converts instructions to binary machine code.
3. Output:
A binary output file containing the machine code.
Error logs for invalid instructions (if any).
