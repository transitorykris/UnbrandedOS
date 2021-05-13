# UnbrandedOS

UnbrandedOS is the beginnings of a little pre-emptive multitasking kernel and OS for the MC68010 on a [Rosco-m68k](https://rosco-m68k.com/).
Unbranded because it doesn't have a real name yet ;)

![Screenshot](https://raw.githubusercontent.com/transitorykris/UnbrandedOS/master/assets/screenshot.png)

## Rosco code

Note: for convenience this currently contains a pretty sloppy fork of [Rosco standard libraries](https://github.com/rosco-m68k/rosco_m68k/tree/develop/code/software/libs).

# Notes

* There's some bits hardcoded such as using the DUART board and the 4MB extension boards. If you have different hardware you'll need to tweak things (or better, submit a PR and make this configurable!)