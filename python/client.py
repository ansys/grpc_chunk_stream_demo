import time

import numpy as np
import grpc

from chunkdemo_pb2_grpc import ChunkDemoStub
from chunkdemo_pb2 import StreamRequest


# chunk sizes for streaming and file streaming
DEFAULT_CHUNKSIZE = 256*1024  # 256 kB


class ChunkDemoClient:
    """
    """

    def __init__(self, ip='127.0.0.1', port=50000, timeout=5):
        """Initialize connection to the mapdl server"""
        self._stub = None

        self._channel_str = '%s:%d' % (ip, port)
        self.channel = grpc.insecure_channel(self._channel_str)
        self._state = grpc.channel_ready_future(self.channel)
        self._stub = ChunkDemoStub(self.channel)

        # verify connection
        tstart = time.time()
        while ((time.time() - tstart) < timeout) and not self._state._matured:
            time.sleep(0.01)

        if not self._state._matured:
            raise IOError('Unable to connect to server at %s' % self._channel_str)
        print('Connected to server at %s' % self._channel_str)

    def request_array(self, arr_sz, chunk_size=DEFAULT_CHUNKSIZE):
        """Request an array from the server"""
        request = StreamRequest(chunk_size=chunk_size)
        metadata = [('arr_sz', str(arr_sz))]
        chunks = self._stub.DownloadArray(request, metadata=metadata)
        return self.parse_chunks(chunks, np.int32)

    def parse_chunks(self, chunks, dtype=None):
        """Deserialize chunks into a numpy array

        Parameters
        ----------
        chunks : generator
            generator from grpc.  Each chunk contains a bytes payload

        dtype : np.dtype
            Numpy data type to interpert chunks as.

        Returns
        -------
        array : np.ndarray
            Deserialized numpy array.

        """
        if not chunks.is_active():
            raise RuntimeError('Empty Record')

        try:
            chunk = chunks.next()
        except:
            raise Exception('Failed to read first chunk')

        array = np.frombuffer(chunk.payload, dtype)
        if chunks.done():
            return array

        arrays = [array]
        for chunk in chunks:
            arrays.append(np.frombuffer(chunk.payload, dtype))

        return np.hstack(arrays)


if __name__ == '__main__':
    client = ChunkDemoClient()
    print(client.request_array(10000))
