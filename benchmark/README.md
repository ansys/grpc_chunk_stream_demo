# gRPC array benchmark

**Note:** This is a work in progress, there aren't any actual benchmarks yet.

## Design goal

The goal of this benchmark is to test the performance of sending an array via gRPC, in various different ways and for different data types.

As such, architecturally sound interfaces were sacrificed where that would make it harder to measure performance. Do **not** use this code as an array sending library without at least _some_ modification.

## Building

On Linux, the whole project can be built with

```bash
make
```

However, it's quite likely that you will encounter edge cases that the ``Makefile`` doesn't cover.

On Windows, it should still be possible to build and run the benchmarks. Follow the ``Makefile`` from top to bottom for instructions.

Benchmarks which include bandwidth throttling and artificial latency are implemented specifically with Linux tools.