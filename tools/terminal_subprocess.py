
# Helper module for launching terminal processes and forwarding the output
# to the Python program.

# Does not support input. Only intended for automated processes.

import subprocess

def run(subprocess_args):
    # Start subprocess
    # bufsize = 1 means output is line buffered
    # universal_newlines = True is required for line buffering
    process = subprocess.Popen(subprocess_args,
                               bufsize=1,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               universal_newlines=True)


    while process.poll() is None:
        print(
            # process.stdout.readline().decode("utf-8"),
            process.stdout.readline(),
            end='', flush=True
        )
    
    return process.wait()