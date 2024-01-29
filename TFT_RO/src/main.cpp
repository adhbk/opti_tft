#include "../include/TFT_Data_Parser.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

#define SAMPLE_SIZE 10000

main()
{
    std::string champions_json_path = "C:\\Users\\Adrien\\Desktop\\RO_TFT\\ChampionsSet10.json";
    std::string traits_json_path = "C:\\Users\\Adrien\\Desktop\\RO_TFT\\TraitsSet10.json";

    // Parse the data
    auto data_opt = TFT_Data_Parser::parse_json_data(champions_json_path, traits_json_path);

    if (not data_opt.has_value())
    {
        std::cout << "Failed to parse data" << std::endl;
        return 1;
    }

    auto champions = data_opt.value().first;
    auto traits = data_opt.value().second;

    for (uint32_t tailleComposition : { 3, 4, 5, 6, 7, 8, 9, 10})
    {   
        for (Trait bonus_trait : traits)
        {
            TFT_Model model = TFT_Model(
                tailleComposition,
                champions,
                Mode::MINIMISATION,
                bonus_trait
            );

            std::set solutions = model.recherche_locale_multiple(SAMPLE_SIZE);

            // Transform the set into an std::vector, then sort it by score and keep only the best scores
            std::vector<Solution> solutions_vector(solutions.begin(), solutions.end());
            
            uint32_t best_score = solutions_vector[0].score;

            for (Solution& solution : solutions_vector)
            {
                if (model.is_improved(solution.score, best_score))
                {
                    best_score = solution.score;
                }
            }

            // Filter by score, keep all the solutions which have the same score as the best
            std::vector<Solution> best_solutions = solutions_vector;
            //std::copy_if(solutions_vector.begin(), solutions_vector.end(), std::back_inserter(best_solutions), [&](const Solution& solution) { return solution.score == solutions_vector[0].score; });

            // TODO :Sort by balance

            // Output the results in the file

            std::string directory = bonus_trait.name;
            std::replace(directory.begin(), directory.end(), ' ', '_');
            std::replace(directory.begin(), directory.end(), '/', '_');

            // Use std::filesystem to create the directory if it doesn't exist
            std::filesystem::create_directory(directory);

            std::ofstream file(directory + "/" + std::to_string(tailleComposition) + ".txt");

            // Check for errors
            if (not file.is_open())
            {
                std::cout << "Failed to open file" << directory + "/" + std::to_string(tailleComposition) + ".txt"  << std::endl;
                return 1;
            }

            // Write the solutions to the file

            for (const Solution& solution : best_solutions)
            {
                file << model.get_readable_solution(solution) << '\n';
            }

            file.close();
        }
    }

    return 0;
}