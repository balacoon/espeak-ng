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

typedef struct LangDataMemStruct {
    std::vector<char> phontab;
    std::vector<char> phonindex;
    std::vector<char> phondata;
    std::vector<char> intonations;
    std::vector<char> dict_buf;
    std::vector<char> secondary_dict_buf;
    std::vector<std::string> lang_conf_lines;
} LangDataMem;

LangDataMem init_lang_data(espeak_LOADED_DATA *lang_data, std::string lang_name, std::string dict_name) {
    LangDataMem lang_data_mem;
    lang_data_mem.phontab = ReadBinaryFile("build/share/espeak-ng-data/phontab");
    lang_data->phontab = lang_data_mem.phontab.data();
    lang_data->phontab_size = lang_data_mem.phontab.size();
    lang_data_mem.phonindex = ReadBinaryFile("build/share/espeak-ng-data/phonindex");
    lang_data->phonindex = lang_data_mem.phonindex.data();
    lang_data->phonindex_size = lang_data_mem.phonindex.size();
    lang_data_mem.phondata = ReadBinaryFile("build/share/espeak-ng-data/phondata");
    lang_data->phondata = lang_data_mem.phondata.data();
    lang_data->phondata_size = lang_data_mem.phondata.size();
    lang_data_mem.intonations = ReadBinaryFile("build/share/espeak-ng-data/intonations");
    lang_data->intonations = lang_data_mem.intonations.data();
    lang_data->intonations_size = lang_data_mem.intonations.size();
    // locale specific files, change below for a different locale
    // read lang config
    std::ifstream lang_conf(std::string("build/share/espeak-ng-data/lang/") + lang_name);
    std::string line;
    while (std::getline(lang_conf, line)) {
    	lang_data_mem.lang_conf_lines.push_back(line);
    }
    lang_data->lang_conf_lines = (const char **)malloc(lang_data_mem.lang_conf_lines.size() * sizeof(char*));
    for (size_t i = 0; i < lang_data_mem.lang_conf_lines.size(); i++) {
	// copy pointer
    	lang_data->lang_conf_lines[i] = lang_data_mem.lang_conf_lines[i].c_str();
    }
    lang_data->lang_conf_lines_num = lang_data_mem.lang_conf_lines.size();
    // read dict
    lang_data_mem.dict_buf = ReadBinaryFile(std::string("build/share/espeak-ng-data/") + dict_name);
    lang_data->dict = lang_data_mem.dict_buf.data();
    lang_data->dict_size = lang_data_mem.dict_buf.size();

    // read secondary dict
    if (dict_name == "en_dict") {
       lang_data->secondary_dict = NULL;
    } else {
        lang_data_mem.secondary_dict_buf = ReadBinaryFile("build/share/espeak-ng-data/en_dict");
        lang_data->secondary_dict = lang_data_mem.secondary_dict_buf.data();
        lang_data->secondary_dict_size = lang_data_mem.secondary_dict_buf.size();
    }
    return lang_data_mem;
}	

int main(int argc, char *argv[]) {

    // lang data - structure with data needed for initialization in memory
    espeak_LOADED_DATA uk_lang_data;
    LangDataMem uk_lang_data_mem = init_lang_data(&uk_lang_data, "zle/uk", "uk_dict");

    // init
    int result = espeak_InitializeMem(AUDIO_OUTPUT_SYNCHRONOUS, 0 /*buflen*/, &uk_lang_data, 0/*options*/);

    // set voice, change voice code if needed
    result = espeak_SetVoiceByBinaryData("zle/uk", &uk_lang_data);
    free(uk_lang_data.lang_conf_lines);

    if (argc == 2) {
        std::ifstream in_txt(argv[1]);
	std::string line;
	while(std::getline(in_txt, line)) {
	    for (auto res : getPhonemes(line, false)) {
		// print without any extra info
	        std::cout << res << std::endl;
	    }
	}
    } else if (argc == 1) {
        while (true) {
	    std::cout << "Write line to phonemize: ";
	    std::string line;
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
