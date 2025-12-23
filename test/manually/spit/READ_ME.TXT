"Spit READ_ME"
Paul Boersma, 16 September 2025

How to compile spit.c
=====================

On the Mac:
-----------

cc -arch arm64  spit.c -o spit_mac_arm64
cc -arch x86_64 spit.c -o spit_mac_intel64

On Windows:
-----------

We must note that we cannot compile with MinGW,
because we need the standard Windows CRT (C runtime),
which uses GetCommandLine and CommandLineToArgv
(and unfortunately also optional wildcard expansion,
namely if matching files exist) before `main`.

clang spit.c -o spit_win_arm64
clang spit.c -o spit_win_intel64
