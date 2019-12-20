#!/usr/bin/env python3

import argparse
import os
import subprocess
from pathlib import Path
from typing import Union

import call_wrapper

PathLike = Union[str, Path]

def generate_header_custom( output_dir: PathLike,
                            commit_hash: str = ""):
    if (not len(commit_hash)):
        commit_hash = subprocess.check_output("git rev-parse --short HEAD", shell=True).decode('ascii')
    
    output_dir = Path(output_dir).resolve()
    if (not output_dir.exists()):
        os.makedirs(output_dir)
    
    output_file = output_dir/'commit_hash.h'
    if (output_file.exists()):
        os.remove(output_file)
    
    with open(output_file, 'w') as output:
        output.write("#pragma once\n\n")
        output.write(f"#define SMP_COMMIT_HASH {commit_hash}\n")
    
    print(f"Generated file: {output_file}")

def generate_header():
    cur_dir = Path(__file__).parent.absolute()
    root_dir = cur_dir.parent
    output_dir = root_dir/"_result"/"AllPlatforms"/"generated"
    
    generate_header_custom(output_dir)

if __name__ == '__main__':
    cur_dir = Path(__file__).parent.absolute()
    root_dir = cur_dir.parent
    output_dir = root_dir/"_result"/"AllPlatforms"/"generated"

    parser = argparse.ArgumentParser(description='Generate header with commit hash definition')
    parser.add_argument('--output_dir', default=output_dir)
    parser.add_argument('--commit_hash', default="")

    args = parser.parse_args()

    call_wrapper.final_call_decorator(
        "Generating header with commit hash", 
        "Header was successfully generated!", 
        "Header generation failed!"
    )(
    generate_header_custom
    )(
        args.output_dir,
        args.commit_hash
    )
    
