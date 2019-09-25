## Hack Virtual Machine Compiler

## Introduction

This is a compiler for HACK computers that converts Virtual Machine byte code into ASM code.

I made this program with C language, as part of the [Nand2Tetris](https://www.nand2tetris.org/copy-of-about) course.

![HECK compiler in action](demo.gif)

## Usage

The compiler can process single `.vm` files or entire directories containing `.vm` files.

Use as follows:

`VMTranslator <dirname> -c`

`VMTranslator <filename> -c`

When `-c` is  provided the compiler will include comments in the generated ASM file.

The compiler will generate one single `.asm` file, named after the provided file or directory, containing the generated ASM code.

## Requirements

This program is written for Linux machines with `libc`, however its portability to other operating systems should be fairly trivial.

## Build Process

The project comes with a `makefile` that you can run to build the binary.
