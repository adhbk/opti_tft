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
    
    #Implémentation alternative en minimisation
    #Minimiser le nombre de traits morts au lieu de maximiser les traits utilisés
    #Certains champions ont 3 traits et sont priorisés par la première méthode (Alistar/Vi/Samira qui sont dans toutes les meilleurs solutions quasiment)
    def eval_solution_minimize(self,solution=None):
        if solution is None:
            solution = self.solution

        count_each_traits = {}
        for id_champion in solution:
            champion = self.champions[id_champion]
            for trait in champion["traits"]:
                count_each_traits[trait] = count_each_traits.get(trait, 0) + 1

        # Cette fois Ace (10) doit bien être pris en compte pour le compte de traits morts
        ace_canceled = count_each_traits.get(10, 0) if count_each_traits.get(10, 0) in(2, 3) else 0

        objectif = ace_canceled
        for id_trait, count in count_each_traits.items():
            trait = self.traits[id_trait]
            if trait["stages"][0] > count:
                objectif += count
        
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
            #print("new best value", best_value)

    
    def get_readable_solution(self):
        liste_champions_name = sorted(list(map(lambda x: self.champions[x]['name'],self.solution)))

        count_each_traits = {}
        for id_champion in self.solution:
            champion = self.champions[id_champion]
            for trait in champion["traits"]:
                count_each_traits[trait] = count_each_traits.get(trait, 0) + 1


        count_each_traits = list(map(lambda x: (self.traits[x[0]]['name'],x[1]),count_each_traits.items()))

        count_each_traits = sorted(count_each_traits, key=lambda d: (d[1],d[0]),reverse=True) 
        
        return (tuple(liste_champions_name),tuple(count_each_traits),self.eval_solution())

    def display_readable_solution(self):
        a = self.get_readable_solution()
        print(f"Champions: {a[0]}")
        print(f"Traits: {a[1]}")
        print(f"Value: {a[2]}")


#model = TFTModel(8)
#print("solution", model.solution)
#print("value", model.eval_solution())
#model.display_readable_solution()
#
#model.recherche_local()
#print("solution", model.solution)
#model.display_readable_solution()


model = TFTModel(8)
solutions = []
sample_size = 1000
for i in range(sample_size):
    if i % (sample_size/10) == 0:
        print(f"{(i/sample_size)*100}%")
    model.init_solution()
    model.recherche_local()

    solutions.append(model.get_readable_solution())

solutions = list(set(solutions))
solutions = sorted(solutions,key=lambda x: x[2],reverse=True)

i=0
while solutions[i][2] in (solutions[0][2], solutions[0][2] - 1):
    print(f"Champions: {solutions[i][0]}")
    print(f"Traits: {solutions[i][1]}")
    print(f"Value: {solutions[i][2]}")
    i += 1
    

# actuellement c'est un peu naze mais on voit qu'en le lançnant en boucle des fois ça amrche mieux que d'autres
# on tombe trop rapidement dans des extremums locaux, faut trouver un opératuer plus violent que le swap (ex swap2)

