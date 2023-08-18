import hashlib
import os


def combine_sha256():
    files = [
        "build-x64-linux-release/ScreenPlay-" +
        os.environ['CI_COMMIT_TAG'] + "-x64-linux-release.zip",
        "build-x64-windows-release/ScreenPlay-" +
        os.environ['CI_COMMIT_TAG'] + "-x64-windows-release.zip",
        "build-64-osx-universal-release/ScreenPlay-" +
        os.environ['CI_COMMIT_TAG'] + "-x64-osx-universal-release.zip"
    ]

    with open('SHA512-SUMS.txt', 'w') as f_out:
        for file in files:
            with open(f"{file}.sha256.txt", 'r') as f_in:
                sha256_hash = f_in.read().strip()
                sha512_hash = hashlib.sha512(sha256_hash.encode()).hexdigest()
                f_out.write(f"{sha512_hash}  {file}\n")


if __name__ == "__main__":
    combine_sha256()
