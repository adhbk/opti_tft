# importing the module
import json
import random


class TFTModel:
    def __init__(self, taille_composition=5):
        with open('Traits.json') as traits_file, open('Champions.json') as champions_file:
            # Listes des traits et des champions
            self.traits = json.load(traits_file)
            self.champions = json.load(champions_file)
            self.id_champions = list(range(len(self.champions)))
            self.taille_composition = taille_composition
            self.solution = None
            self.init_solution()

    def init_solution(self):
        self.solution = random.sample(list(range(len(self.champions))), self.taille_composition)

    def eval_solution(self, solution=None):
        if solution is None:
            solution = self.solution
        count_each_traits = {}
        for id_champion in solution:
            champion = self.champions[id_champion]
            for trait in champion["traits"]:
                count_each_traits[trait] = count_each_traits.get(trait, 0) + 1
        # print(count_each_traits)
        if count_each_traits.get(10, 0) in (2, 3):
            count_each_traits.pop(10)

        objectif = 0
        for id_trait, count in count_each_traits.items():
            trait = self.traits[id_trait]
            for stages in trait["stages"]:
                if stages <= count:
                    objectif += 1
        return objectif

    # BI c'est Best improvement : on teste toutes les possibilités et qu'on garde que la meilleure
    # on pourrait tester First Improvement mais souvent ça marche moins bien
    def swap_champion_BI(self, id_champion):
        sol_modif = self.solution.copy()
        best_sol = self.solution.copy()
        best_value = self.eval_solution()

        if id_champion not in self.solution:
            for idx, id_champion_sol in enumerate(self.solution):
                sol_modif[idx] = id_champion
                new_value = self.eval_solution(sol_modif)
                if new_value > best_value:
                    # print(f"improved from {best_value} to {new_value}")
                    best_value = new_value
                    best_sol = sol_modif.copy()
                sol_modif[idx] = id_champion_sol
        # else:
        #     print(f"{id_champion} already in solution {self.solution}")

        return best_sol, best_value

    def swap_BI(self):
        improved = False
        sol_init = self.solution.copy()
        best_sol = self.solution.copy()
        best_value = self.eval_solution()
        for id_champion in self.id_champions:
            new_sol, new_value = self.swap_champion_BI(id_champion)
            if new_value > best_value:
                # print(f"improved from {best_value} to {new_value}")
                best_value = new_value
                best_sol = new_sol.copy()
                improved = True
        return best_sol, best_value, improved

    # TODO ajouter un swap2_BI au lieu de swap les champions 1 par 1 on les swap 2 par deux

    def recherche_local(self):
        improved = True
        while improved:
            best_sol, best_value, improved = self.swap_BI()
            self.solution = best_sol
            print("new best value", best_value)


model = TFTModel(8)
print("solution", model.solution)
print("value", model.eval_solution())

model.recherche_local()
print("solution", model.solution)
print("value", model.eval_solution())
# actuellement c'est un peu naze mais on voit qu'en le lançnant en boucle des fois ça amrche mieux que d'autres
# on tombe trop rapidement dans des extremums locaux, faut trouver un opératuer plus violent que le swap (ex swap2)
