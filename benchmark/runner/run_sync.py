import pathlib
import subprocess

if __name__ == '__main__':
    bin_dir = pathlib.Path(__file__).parent.parent / "build" / "bin"
    out_dir = pathlib.Path(__file__).parent.parent / "results"
    server_exe = str(bin_dir / "sync_server")
    client_exe = str(bin_dir / "sync_client")
    server_proc = subprocess.Popen([server_exe])
    with open(out_dir / "sync.csv", mode="wb") as out_f:
        client_proc = subprocess.check_call([client_exe], stdout=out_f)
    server_proc.terminate()
