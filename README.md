# Linux Shell

## Overview
This project is my attempt to clone the Linux shell in C. It provides basic functionality for navigating and interacting with the Linux filesystem, along with executing commands available in the system's `PATH` environment variable. While it supports several features common to modern shells, it currently does not handle piping between commands.

## Features

- **Built-in Commands**:
  - `cd`: Change the current working directory.
  - `pwd`: Print the current working directory.
  - `echo`: Print arguments to the standard output.
  - `exit`: Exit the shell.
  - `type`: Display information about command types.

- **Execution of Commands**:
  - Supports running any command available in the system's `PATH`.

- **Input Parsing**:
  - Handles single quotes (`'`), double quotes (`"`), and backslashes (`\`).

- **Output Redirection**:
  - Redirects `stdout` and `stderr` using `>` and `>>`.

## Limitations

- **No Piping**: The shell currently does not support chaining commands using pipes.
- **Limited Command Execution**: Commands that are neither built-in nor in the `PATH` cannot be executed (e.g. executables in the current working directory without).
- **No Input Redirection**: The shell does not currently support redirecting `stdin`.
- **Missing Advanced Features**: Does not support features like command history, arrow key navigation for previously executed commands, or tab autocompletion.

## Roadmap

- Add support for command piping.
- Implement input redirection.
- Introduce command history and navigation using arrow keys.
- Add tab autocompletion for commands and paths.
- Enhance input parsing for additional special characters (like `$`).
