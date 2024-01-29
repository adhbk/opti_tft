#include "../include/TFT_Model.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>


Solution TFT_Model::initialise_solution()
{
    srand(time(0));

    m_solution.clear();

    for (uint32_t i = 0; i < m_taille_composition; i++)
    {
        uint32_t randomIndex = rand() % m_champions.size();
        m_solution.champions.push_back(m_champions[randomIndex]);
    }

    return m_solution;
}

std::set<Solution> TFT_Model::recherche_locale_multiple(uint32_t nb_solutions)
{
    std::set<Solution> solutions;

    if (m_bonus_trait.has_value())
        std::cout << "Trait : " << m_bonus_trait.value().name << '\n'; 

    for (uint32_t i = 0; i < nb_solutions; i++)
    {
        if (i % (nb_solutions/10) == 0)
            std::cout << (double(i) * 100) / nb_solutions << '%' << '\n';
        solutions.insert(recherche_locale());
    }

    return solutions;
}


Solution TFT_Model::recherche_locale()
{
    initialise_solution();

    switch(m_mode)
    {
        case Mode::MINIMISATION:
            return recherche_locale_minimisation();
            break;
        case Mode::MAXIMISATION:
            return recherche_locale_maximisation();
            break;
        case Mode::MAX_AND_MIN:
            return recherche_locale_max_and_min();
            break;
        default:
            return m_solution;
            break;
    }
}

std::map<Trait, uint32_t> TFT_Model::count_each_trait(Solution const &solution, std::optional<Trait> const &bonus_trait)
{
    std::map<Trait, uint32_t> result;

    if (bonus_trait.has_value())
    {
        result[bonus_trait.value()] = 1;
    }

    for (Champion const &champion : solution.champions)
    {
        for (Trait const &trait : champion.traits)
        {
            result[trait]++;
        }
    }

    return result;
}


uint32_t TFT_Model::count_active_traits(Solution const &solution, std::optional<Trait> const &bonus_trait)
{
    std::map<Trait, uint32_t> traits = count_each_trait(solution, bonus_trait);

    int32_t objectif = 0;

    for (auto const &trait : traits)
    {
        for (uint32_t stage : trait.first.stages)
        {
            if (stage <= trait.second)
            {
                objectif++;
            }
        }
    }

    return objectif;
}

uint32_t TFT_Model::count_dead_traits(Solution const &solution, std::optional<Trait> const &bonus_trait)
{
    std::map<Trait, uint32_t> traits = count_each_trait(solution, bonus_trait);

    int32_t objectif = 0;

    for (auto const &trait : traits)
    {
        uint32_t lastStage = 0;
        for (uint32_t stage : trait.first.stages)
        {
            if (trait.second < stage)
            {
                objectif += trait.second - lastStage;
                lastStage = 0;
                break;
            }

            lastStage = stage;
        }
    }

    return objectif;
}

bool TFT_Model::is_improved(int32_t new_score, int32_t old_score)
{
    switch (m_mode)
    {
        case Mode::MINIMISATION:
            return new_score < old_score;
        case Mode::MAXIMISATION:
        case Mode::MAX_AND_MIN:
            return new_score > old_score;
        default:
            return false;
    }
}

int32_t TFT_Model::eval_solution (Solution const &solution) 
{ 
    switch (m_mode)
    {
        case Mode::MINIMISATION:
            return count_dead_traits(solution, m_bonus_trait);
        case Mode::MAXIMISATION:
            return count_active_traits(solution, m_bonus_trait);
        case Mode::MAX_AND_MIN:
            return count_active_traits(solution, m_bonus_trait) - count_dead_traits(solution, m_bonus_trait);
        default:
            return 0;
    }
}

std::string TFT_Model::get_readable_solution(Solution const &solution)
{

    std::string result = "[";
    for (const Champion& champion : solution.champions) {
        result += champion.name + ", ";
    }
    result.pop_back();
    result.pop_back();
    result += "]\n";

    auto traits_count = count_each_trait(solution, m_bonus_trait);

    result += "Traits : [";

    for (auto const &trait_count : traits_count)
    {
        result += trait_count.first.name + " : " + std::to_string(trait_count.second) + " , ";
    }

    result.pop_back();
    result.pop_back();
    result.pop_back();

    result += "]\n";

    result += "Score : " + std::to_string(solution.score) + '\n';
    result += "Balance : " +std::to_string(solution.balance) + '\n';

    return result;
}

Solution TFT_Model::recherche_locale_minimisation()
{
    bool improved = true;

    m_solution.score = eval_solution(m_solution);

    while (improved)
    {
        std::pair<bool, Solution> result = swap_BI();
        improved = result.first;
        m_solution = result.second;
    }

    return m_solution;
}

Solution TFT_Model::recherche_locale_maximisation()
{
    bool improved = true;
    m_solution.score = eval_solution(m_solution);

    while (improved)
    {
        std::pair<bool, Solution> result = swap_BI();
        improved = result.first;
        m_solution = result.second;
    }
    
    return m_solution;
}

Solution TFT_Model::recherche_locale_max_and_min()
{
    bool improved = true;

    m_solution.score = eval_solution(m_solution);

    while (improved)
    {
        std::pair<bool, Solution> result = swap_BI();
        improved = result.first;
        m_solution = result.second;
    }

    return m_solution;
}

/** 
 * Essaye d'échanger un champion donné avec tous ceux de la solution actuelle
 * Retourne la meilleure des nouvelles solutions
 * 
 * @param[in] eval_solution Fonction permettant d'évaluer le score d'une solution donnée
 */
Solution TFT_Model::swap_champion_BI(Champion const & champion)
{
    Solution solution_modified = m_solution;
    Solution best_solution = m_solution;

    // Si le champion est déjà dans la solution, ne rien faire
    if (std::find(solution_modified.champions.begin(), solution_modified.champions.end(), champion) != solution_modified.champions.end())
    {
        return best_solution;
    }

    // Pour chaque champion dans la solution, l'échanger avec le champion proposé, et évaluer la nouvelle solution
    for (uint32_t idx = 0; const Champion &champion_of_solution : m_solution.champions)
    {
        solution_modified.champions[idx] = champion;
        solution_modified.score = eval_solution(solution_modified);

        if (is_improved(solution_modified.score, best_solution.score))
        {
            // Conserver la meilleure solution
            best_solution = solution_modified;
        }

        solution_modified.champions[idx] = champion_of_solution;
        
        idx++;
    }

    return best_solution;
}

/**
 * Essaye d'améliorer la solution avec chaque champion
 * Renvoie la meilleure des solutions
*/
std::pair<bool, Solution> TFT_Model::swap_BI()
{
    bool improved = false;
    Solution best_solution = m_solution;

    for (Champion const &champion : m_champions)
    {
        Solution new_solution = swap_champion_BI(champion);

        if (is_improved(new_solution.score, best_solution.score))
        {
            best_solution = new_solution;
            improved = true;
        }
    }

    return std::make_pair(improved, best_solution);
}
