#!/usr/bin/env python

import os
import sys
import pathlib
import contextlib
import subprocess

@contextlib.contextmanager
def emulate_network_properties(netem_options):
    qdisc_cmds = ["sudo", "tc", "qdisc"]
    dev_cmds = ["dev", "lo", "root"]
    try:
        subprocess.check_call(qdisc_cmds + ["add"] + dev_cmds + ["netem"] + netem_options)
        yield
    finally:
        try:
            subprocess.check_call(qdisc_cmds + ["del"] + dev_cmds)
        except Exception as exc:
            raise RuntimeError("Could not reset 'tc' properties! You may have to do so manually with 'sudo tc qdisc del dev lo root'.") from exc

if __name__ == '__main__':
    bin_dir = pathlib.Path(__file__).parent.parent / "build" / "bin"
    # out_dir = pathlib.Path(__file__).parent.parent / "results"
    server_exe = str(bin_dir / "sync_server")
    client_exe = str(bin_dir / "sync_client_fixed_chunksize")
    server_proc = subprocess.Popen([server_exe])

    out_filename = sys.argv[1]
    netem_options = out_filename.split('/')[-1].split(".")[0].split("_")
    try:
        env_no_threading = os.environ.copy()
        env_no_threading["OMP_NUM_THREADS"] = "1"
        print(f"Running constrained measurement with options '{' '.join(netem_options)}'.")
        with open(out_filename, mode="wb") as out_f:
            with emulate_network_properties(netem_options):
                subprocess.check_call([client_exe], stdout=out_f, env=env_no_threading)
    finally:
        server_proc.terminate()
