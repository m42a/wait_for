This is a small program which waits until all of its decendants die, not just its immediate children.

Right now it only works on Linux>=3.4, but apart from the `prctl` call it should be POSIX compliant, so patches for other systems are welcome.
