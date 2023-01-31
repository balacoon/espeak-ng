# Copyright 2023 Balacoon

import os
import logging
import argparse

import msgpack


def parse_args():
    ap = argparse.ArgumentParser(
        description="Packs espeak data into addon compatible with Balacoon frontend"
    )
    ap.add_argument("--install-dir", default="build", help="Location of installed eSpeak")
    ap.add_argument("--packing-info", default="packing_info", help="Directory with packing information")
    ap.add_argument("--out", required=True, help="Path to put packed addon to")
    args = ap.parse_args()
    return args


def main():
    logging.basicConfig(level=logging.INFO)
    args = parse_args()
    # balacoon addon is just a list of dictionaries, where each dictionary has id
    addon_dict = {"id": "espeak"}
    # files that go into addon by default
    for name in ["phontab", "phonindex", "phondata", "intonations"]:
        path = os.path.join(args.install_dir, "share", "espeak-ng-data", name)
        if not os.path.isfile(path):
            raise RuntimeError("There is no [{}]. Is eSpeak built and installed?".format(path))
        with open(path, "rb") as fp:
            addon_dict[name] = fp.read()

    # iterate over packing information directory. it should have following stricture
    # packing_info
    #   - en_us
    #       - code (contains eSpeak language code, for ex. "gmw/en-US")
    #       - lang_config (contains path to lang config, relative to install dir, for ex. "share/espeak-ng-data/lang/gmw/en-US")
    #       - dict (contains path to dictionary, relative to install dir, for ex. "share/espeak-ng-data/en_dict")
    #       - phoneme_mapping (contains mapping between eSpeak phonemes and unified Balacoon phonemeset)
    #       - stress_mapping (contains mapping of stress marks)
    for locale in os.listdir(args.packing_info):
        locale_dir = os.path.join(args.packing_info, locale)
        logging.info("Packing [{}]".format(locale))

        # read eSpeak language code
        with open(os.path.join(locale_dir, "code"), "r") as fp:
            espeak_locale = fp.readline().strip()
            logging.info("[{}] corresponds to [{}] in espeak".format(locale, espeak_locale))
            addon_dict["{}_code".format(locale)] = espeak_locale

        # read language config
        with open(os.path.join(locale_dir, "lang_config"), "r") as fp:
            suffix = fp.readline().strip()
            path = os.path.join(args.install_dir, suffix)
            with open(path, "r") as lang_fp:
                lines = lang_fp.readlines()
                lines = [x.strip() for x in lines]
                addon_dict["{}_lang".format(locale)] = lines

        # read dictionary
        with open(os.path.join(locale_dir, "dict"), "r") as fp:
            suffix = fp.readline().strip()
            path = os.path.join(args.install_dir, suffix)
            with open(path, "rb") as dict_fp:
                addon_dict["{}_dict".format(locale)] = dict_fp.read()

        # read phoneme mapping
        with open(os.path.join(locale_dir, "phoneme_mapping"), "r") as fp:
            mapping = {}
            for line in fp:
                # expected format:
                # <espeak phoneme>[\t<balacoon phoneme1> <balacoon phoneme2>...]
                parts = line.strip().split("\t")
                if len(parts) == 1:
                    # means espeak phoneme simply deleted
                    mapping[parts[0]] = []
                else:
                    assert len(parts) == 2
                    mapping[parts[0]] = parts[1].split()
            addon_dict["{}_phoneme_mapping"] = mapping

        # read stress mapping
        with open(os.path.join(locale_dir, "stress_mapping"), "r") as fp:
            mapping = []
            for line in fp:
                # expected format:
                # <espeak stress>\t<balacoon stress>
                in_stress, out_stress = line.strip().split("\t")
                mapping.append((in_stress, out_stress))
            addon_dict["{}_stress_mapping"] = mapping

    # save created addon
    with open(args.out, "wb") as fp:
        msgpack.dump([addon_dict], fp)


if __name__ == "__main__":
    main()

