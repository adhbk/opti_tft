#include "../include/TFT_Data_Parser.h"

#include "../import/rapidjson/document.h"
#include <iostream>
#include "../import/rapidjson/filereadstream.h"

std::optional<std::pair<std::vector<Champion>, std::vector<Trait>>> TFT_Data_Parser::parse_json_data(std::string championsFilePath, std::string traitsFilePath)
{
    auto champions_data = parse_champion_data(championsFilePath);
    if (!champions_data) {
        return std::nullopt;
    }

    auto traits_data = parse_traits_data(traitsFilePath);
    if (!traits_data) {
        return std::nullopt;
    }


    std::vector<Champion> champions = champions_data.value().first;
    std::vector<std::vector<uint32_t>> traits_id = champions_data.value().second;
    std::vector<Trait> traits = traits_data.value();

    // Find the trait of each trait id and add it the corresponding champion
    for (uint32_t championId = 0; championId < traits_id.size(); championId++) {
        for (uint32_t traitId : traits_id[championId]) {
            champions[championId].traits.insert(traits[traitId]);
        }
    }

    return std::make_pair(champions, traits);
}

std::optional<std::pair<std::vector<Champion>, std::vector<std::vector<uint32_t>>>> TFT_Data_Parser::parse_champion_data(std::string championsFilePath)
{
    FILE* fp = fopen(championsFilePath.c_str(), "rb");
    if (!fp) {
        std::cerr << "Error: unable to open file " << championsFilePath << std::endl;
        return std::nullopt;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document" << championsFilePath << std::endl;
        fclose(fp);
        return std::nullopt;
    }

    std::vector<Champion> champions;
    std::vector<std::vector<uint32_t>> traits_id;

    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            Champion champion;
            std::vector<uint32_t> traits_vec;
            if (item.HasMember("name") && item["name"].IsString()) {
                std::string name = item["name"].GetString();
                std::cout << "Name: " << name << std::endl;
                champion.name = name;
            }
            if (item.HasMember("traits") && item["traits"].IsArray()) {
                const auto& traits = item["traits"].GetArray();
                for (const auto& trait : traits) {
                    traits_vec.push_back(trait.GetInt());
                    std::cout << "Trait: " << trait.GetInt() << std::endl;
                }
            }
            if (item.HasMember("scale") && item["scale"].IsInt()) {
                int scale = item["scale"].GetInt();
                std::cout << "Scale: " << scale << std::endl;
                champion.scale = scale;
            }
            champions.push_back(champion);
            traits_id.push_back(traits_vec);
        }
    }

    fclose(fp);

    return std::make_pair(champions, traits_id);

}

std::optional<std::vector<Trait>> TFT_Data_Parser::parse_traits_data(std::string traitsFilePath)
{
    FILE* fp = fopen(traitsFilePath.c_str(), "rb");
    if (!fp) {
        std::cerr << "Error: unable to open file" << traitsFilePath << std::endl;
        return std::nullopt;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document" << traitsFilePath << std::endl;
        fclose(fp);
        return std::nullopt;
    }

    std::vector<Trait> traits;

    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            Trait trait;
            if (item.HasMember("name") && item["name"].IsString()) {
                std::string name = item["name"].GetString();
                std::cout << "Name: " << name << std::endl;
                trait.name = name;
            }
            if (item.HasMember("stages") && item["stages"].IsArray()) {
                const auto& stages = item["stages"].GetArray();
                for (const auto& stage : stages) {
                    trait.stages.insert(stage.GetInt());
                    std::cout << "Trait: " << stage.GetInt() << std::endl;
                }
            }
            traits.push_back(trait);
        }
    }

    fclose(fp);

    return traits;
}

std::string TFT_Data_Parser::get_traits_json_file_path(uint32_t set)
{
    return "TraitsSet" + std::to_string(set) + ".json";
}

std::string TFT_Data_Parser::get_champions_json_file_path(uint32_t set)
{
    return "ChampionsSet" + std::to_string(set) + ".json";
}
