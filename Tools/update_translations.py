import os
import util
import sys
from pathlib import Path
import defines
from execute_util import execute

sys.stdout.reconfigure(encoding='utf-8')


def search_translations(root_dir):
    translations_files = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        if "translations" in dirnames:
            translations_dir = os.path.join(dirpath, "translations")
            for file in os.listdir(translations_dir):
                if file.endswith(".ts"):
                    translations_files.append(
                        os.path.join(translations_dir, file))
    return translations_files


def main():

    translations = search_translations(os.path.join(util.repo_root_path(), ""))

    if translations:
        print("Found translations:")
        for translation in translations:
            print(translation)
    else:
        print("No translations found.")

    executable = "lupdate"
    if os.name == 'nt':
        executable = "lupdate.exe"
    qt_bin_path = defines.QT_BIN_PATH
    executable = os.path.join(qt_bin_path, executable)

    for translation in translations:
        ts = Path(translation)
        source_path = ts.parent.parent.resolve()
        print(f"LUpdate: {translation}. Using source: {source_path}")
        execute( command=f"{executable} -noobsolete -locations none -recursive  {source_path} -ts {translation}",
            workingDir=ts.parent.resolve(),)


if __name__ == '__main__':
    main()
