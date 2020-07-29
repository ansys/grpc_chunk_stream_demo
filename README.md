### Large Array Serialization using gRPC

#### Prerequisites

Install `gRPC` for C++ using by following the directions at [gRPC Quick Start](https://grpc.io/docs/languages/cpp/quickstart/).  This will install both `gRPC` and `protobuf`, which you will need to compile the server and client.  If you wish you use the Python server as well, install `Python 3.6` or newer.

You will also need `cmake` to build the server and client, and the directions for installing a modern version of `cmake` are also at [gRPC Quick Start](https://grpc.io/docs/languages/cpp/quickstart/).


#### C++ Server

Enter the `cpp` directory and run the following to build the `demo_server`

```
mkdir build
cd build
cmake ..
make -j
```

Next, start the `demo_server` with:


```
./demo_server
```

You should see:

```
Server listening on 0.0.0.0:50000
```

The C++ server is now waiting for connections from port 50000.


#### C++ Client

From a different terminal within the `build` directory, run:

```
./demo_client
```

This will connect to the server at your local host and run a basic
array transfer test using both a byte stream and repeated messages
using the C++ server and client.


```
Connected to server at: localhost:50000
Created an INT32 array on the server size 10000000
Array is 38.15 MB

Testing with byte stream...
Using chunk size 256 kB
Average time: 0.0269195
Aprox speed: 1.38 GBps

Testing with repeated messages...
Average time: 0.101271
Aprox speed: 376.68 MBps
```

These timings were taken using `-O2` with `gcc` with Ubuntu 18.04 on a
Intel i7-7820HQ.  `MEMCPY` speed is:

```
$ mbw 32 | grep AVG | grep MEMCPY

AVG	Method: MEMCPY	Elapsed: 0.00412	MiB: 32.00000	Copy: 7775.861 MiB/s

```

#### Python Client

First, ensure you have the following packages installed.  Ensure that
the version of the packages is greater than or equal to your `gRPC`
C++ version.

- `grpcio`
- `google-api-python-client`

Next, enter the `python` directory and run:

```
make
```

This will generate the python gRPC interface files `chunkdemo_pb2.py`
and `chunkdemo_pb2_grpc.py`.  While you have the C++ server running,
run:

```
python client.py
```

This will connect to the server at your local host and run a basic
array transfer test using both a byte stream and repeated messages.

```
Connected to server at 127.0.0.1:50000
Created an INT32 array on the server size 20000000
Testing with byte stream...
Average time: 0.06707191489986145
Aprox speed: 1.1GiB

Testing with repeated messages...
Average time: 1.647849985335294
Aprox speed: 46.3MiB
```

Note that the performance with repeated messages on Python is
remarkably slow compared with the byte stream.  This is because the
`RepeatedInts` message must be iterated ove within Python.  While this
could be improved with a C extension to Python, it would still be
slower than the byte stream considering that the native C++
performance of the byte stream is still ~3.6x slower than using a byte
stream.
