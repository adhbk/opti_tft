#ifndef TFT_DATA_PARSER_H
#define TFT_DATA_PARSER_H

#include "TFT_Model.h"

class TFT_Data_Parser
{
    private:
    static std::optional<std::pair<std::vector<Champion>, std::vector<std::vector<uint32_t>>>> parse_champion_data(std::string championsFilePath);
    static std::optional<std::vector<Trait>> parse_traits_data(std::string traitsFilePath);

    public:

    static std::optional<std::pair<std::vector<Champion>, std::vector<Trait>>> parse_json_data(std::string championsFilePath, std::string traitsFilePath);

    static std::string get_champions_json_file_path(uint32_t set);
    static std::string get_traits_json_file_path(uint32_t set);
};

#endif
