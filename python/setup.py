from setuptools import setup, Extension



setup(
    name='demo',
    ext_modules=[Extension('pbdemo',
                           sources=['pbdemo.c',
                                    'chunkdemo.grpc.pb.cc',
                                    'chunkdemo.pb.cc'],
                           libraries=['protobuf'],
                           include_dirs=['/home/alex/.local/include'],
                           language='c++',
                           )],
)
# /home/alex/.local/include/
