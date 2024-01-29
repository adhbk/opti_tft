#ifndef TFT_MODEL_H
#define TFT_MODEL_H

#include <string>
#include <set>
#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>

struct Trait{
    std::string name;
    std::set<uint16_t> stages;

    Trait() = default;

    Trait(std::string name, std::set<uint16_t> const &stages) : 
        name(name), 
        stages(stages){};

    bool operator<(Trait const &other) const {
        return name < other.name;
    }

    bool operator==(Trait const &other) const {
        return name == other.name;
    }
};

struct Champion{
    std::string name;
    std::set<Trait> traits;
    int32_t scale = 0;

    Champion() = default;

    Champion(std::string name, std::set<Trait> const &traits, int32_t scale) : 
        name(name), 
        traits(traits),
        scale(scale){};

    bool operator<(Champion const &other) const {
        return name < other.name;
    }

    bool operator==(Champion const &other) const {
        return name == other.name;
    }
};

/** Différents modes de recherche */
enum class Mode{
    MINIMISATION, ///< Réduire le nombre de traits morts
    MAXIMISATION, ///< Maximiser le nombre de traits actif
    MAX_AND_MIN ///< Maximiser le nombre de traits actif ET minimiser traits morts
};

struct Solution {
    std::vector<Champion> champions;
    int32_t score = 0;
    int32_t balance = 0;

    Solution() = default;

    Solution(std::vector<Champion> const &champions, int32_t score, int32_t balance) :
        champions(champions),
        score(score),
        balance(balance){}
    
    bool operator<(Solution const &other) const {
        return champions < other.champions;
    }

    bool operator==(Solution const &other) const {
        return champions == other.champions;
    }

    void clear() {
        champions.clear();
        score = 0;
        balance = 0;
    }
};


class TFT_Model{
    private:
        // Input variables
        uint32_t m_taille_composition;
        std::vector<Champion> m_champions;
        Mode m_mode;
        std::optional<Trait> m_bonus_trait;
        
        // Output variables
        Solution m_solution;

        Solution initialise_solution();

        /** Lance une recherche en minimisation du nombre de traits morts */
        Solution recherche_locale_minimisation();
        /** Lance une recherche en maximisation du nombre de traits actifs */
        Solution recherche_locale_maximisation();
        /** 
         *  Lance une recherche en maximisation du nombre de traits actifs 
         *  et minimisation du nombre de traits morts 
         */
        Solution recherche_locale_max_and_min();

        /** 
         * Essaye d'échanger un champion donné avec tous ceux de la solution actuelle
         * Retourne la meilleure des nouvelles solutions
         */
        Solution swap_champion_BI(Champion const & champion);

        std::pair<bool, Solution> swap_BI();

        std::map<Trait, uint32_t> count_each_trait(Solution const &solution, std::optional<Trait> const &bonus_trait);
        
        uint32_t count_active_traits(Solution const &solution, std::optional<Trait> const &bonus_trait);

        uint32_t count_dead_traits(Solution const &solution, std::optional<Trait> const &bonus_trait);

  
    public:
        TFT_Model(
            uint32_t tailleComposition,
            const std::vector<Champion> &champions,
            Mode mode = Mode::MAX_AND_MIN,
            std::optional<Trait> bonusTrait = std::nullopt) :
                m_taille_composition(tailleComposition),
                m_champions(champions),
                m_mode(mode),
                m_bonus_trait(bonusTrait){}
    
        /** Lance une recherche locale dépendante du mode */
        Solution recherche_locale();

        /** Lance n recherches et renvoie un set de toutes les solutions trouvées */
        std::set<Solution> recherche_locale_multiple(uint32_t nb_solutions);

        /** Returns the current calculated solution */
        const Solution &get_solution() const { return m_solution; }

        /** Returns true if the new score is better than the old one */
        bool is_improved(int32_t new_score, int32_t old_score);

        /** Returns the score of a given solution */
        int32_t eval_solution (Solution const &solution); 

        std::string get_readable_solution(Solution const &solution);
};

#endif