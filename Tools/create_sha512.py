import hashlib
import os
from pathlib import Path
import util


def combine_sha256():
    tag = os.environ['CI_COMMIT_TAG']

    # Get the repo root path as a Path object
    root_path = Path(util.repo_root_path())

    files = [
        Path(
            f"{root_path}/build-x64-windows-release/ScreenPlay-{tag}-x64-windows-release.zip"),
        Path(
            f"{root_path}/build-x64-linux-release/ScreenPlay-{tag}-x64-linux-release.zip"),
        Path(f"{root_path}/build-64-osx-universal-release/ScreenPlay-{tag}-64-osx-universal-release.zip")
    ]

    with open('SHA512-SUMS.txt', 'w') as f_out:
        for file in files:
            with open(file.with_name(f"{file.name}.sha256.txt"), 'r') as f_in:
                sha256_hash = f_in.read().strip()
                sha512_hash = hashlib.sha512(sha256_hash.encode()).hexdigest()
                f_out.write(f"{sha512_hash}  {file}\n")


if __name__ == "__main__":
    combine_sha256()
