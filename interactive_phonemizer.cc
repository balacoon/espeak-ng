// Copyright 2023 Balacoon

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <espeak-ng/speak_lib.h>


std::vector<char> ReadBinaryFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Unable to open file: ") + filepath);
    }
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    return buffer;
}

std::vector<std::string> getPhonemes(const std::string &line, bool to_print) {
    const char *cline = line.c_str();
    std::vector<std::string> res;
    while (cline != NULL) {
	int terminator;
        std::string phonemes {
            espeak_TextToPhonemesTerm((const void **)&cline, espeakCHARS_AUTO /*textmode*/,
            1 | (32 << 8) /*phonememode=ASCII, insert space between phonemes*/,
            &terminator)
        };
	res.push_back(phonemes);
	if (to_print) {
            std::cout << "Phonemes: [" << phonemes << "], terminated with: " << terminator << std::endl;
	}
    }
    return res;
}

int main(int argc, char *argv[]) {
    // lang data - structure with data needed for initialization in memory
    espeak_LOADED_DATA lang_data;
    std::vector<char> phontab = ReadBinaryFile("build/share/espeak-ng-data/phontab");
    lang_data.phontab = phontab.data();
    lang_data.phontab_size = phontab.size();
    std::vector<char> phonindex = ReadBinaryFile("build/share/espeak-ng-data/phonindex");
    lang_data.phonindex = phonindex.data();
    lang_data.phonindex_size = phonindex.size();
    std::vector<char> phondata = ReadBinaryFile("build/share/espeak-ng-data/phondata");
    lang_data.phondata = phondata.data();
    lang_data.phondata_size = phondata.size();
    std::vector<char> intonations = ReadBinaryFile("build/share/espeak-ng-data/intonations");
    lang_data.intonations = intonations.data();
    lang_data.intonations_size = intonations.size();
    // locale specific files, change below for a different locale
    // read lang config
    std::vector<std::string> lang_conf_lines;
    std::ifstream lang_conf("build/share/espeak-ng-data/lang/gmw/en-US");
    std::string line;
    while (std::getline(lang_conf, line)) {
    	lang_conf_lines.push_back(line);
    }
    lang_data.lang_conf_lines = (const char **)malloc(lang_conf_lines.size() * sizeof(char*));
    for (size_t i = 0; i < lang_conf_lines.size(); i++) {
	// copy pointer
    	lang_data.lang_conf_lines[i] = lang_conf_lines[i].c_str();
    }
    lang_data.lang_conf_lines_num = lang_conf_lines.size();
    // read dict
    std::vector<char> dict_buf = ReadBinaryFile("build/share/espeak-ng-data/en_dict");
    lang_data.dict = dict_buf.data();
    lang_data.dict_size = dict_buf.size();

    // init
    int result = espeak_InitializeMem(AUDIO_OUTPUT_SYNCHRONOUS, 0 /*buflen*/, &lang_data, 0/*options*/);

    // set voice, change voice code if needed
    result = espeak_SetVoiceByBinaryData("gmw/en-US", &lang_data);
    free(lang_data.lang_conf_lines);

    if (argc == 2) {
        std::ifstream in_txt(argv[1]);
	while(std::getline(in_txt, line)) {
	    for (auto res : getPhonemes(line, false)) {
		// print without any extra info
	        std::cout << res << std::endl;
	    }
	}
    } else if (argc == 1) {
        while (true) {
	    std::cout << "Write line to phonemize: ";
	    std::getline(std::cin, line);
	    if (!std::cin) {
                break;
	    }
            (void)getPhonemes(line, true);
        }
    } else {
    	std::cout << "Run without arguments or provide text file: " << std::endl;
    }

    // remove allocated mem
    espeak_Terminate();
    return 0;
}
