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
Testing with byte stream...
Average time: 0.3553605652999977
Aprox speed: 214.7MiB

Testing with repeated messages...
Average time: 0.5059016572000019
Aprox speed: 22.6MiB
```