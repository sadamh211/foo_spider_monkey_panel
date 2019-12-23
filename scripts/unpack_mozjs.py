#!/usr/bin/env python3

import zipfile
from pathlib import Path
from shutil import rmtree
from zipfile import ZipFile

import call_wrapper

def unpack():
    cur_dir = Path(__file__).parent.absolute()
    root_dir = cur_dir.parent
    zip_dir = root_dir/"submodules"/"mozjs"
    assert(zip_dir.exists() and zip_dir.is_dir())
    
    output_dir = root_dir/"mozjs"
    if (output_dir.exists()):
        rmtree(output_dir)
        output_dir.mkdir(parents=True)
    
    zips = list(zip_dir.glob("*.zip"))
    assert(len(zips) == 1)
    zip = Path(zips[0])
    assert(zipfile.is_zipfile(zip))
    
    with ZipFile(zip) as z:
        z.extractall(output_dir)

if __name__ == '__main__':
    call_wrapper.final_call_decorator(
        "Unpacking mozjs", 
        "Unpacking mozjs: success", 
        "Unpacking mozjs: failure!"
    )(unpack)()
