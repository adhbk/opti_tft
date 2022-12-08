# Programme python présentant la fonction objectif et les données de l'optimisation TFT
# 
# La fonction objectif prend en entrée une composition de champions dont on veut
# chercher le maximum de la fonction objectif
# Cette composition est une liste d'entiers uniques et non ordonné (set) compris entre 0 et 50 inclus
# c'est à dire:
# Une composition doit être composée de nombres distincts
# 2 compositions contenant les mêmes objets dans un ordre différent sont similaires
# La fonction objectif renvoit des valeurs entre 0 et +infinity

# But 1:
#   Chercher une composition optimisée de peu de champions (4-5)
# But 2:
#   Chercher une composition optimisée pour chaque taille de composition jusqu'à 9
# But 3:
#   Chercher plusieurs (toutes?) compositions optimisées pour chaque taille de composition jusqu'à 9
#   (Je suis persuadé qu'il y a plusieurs solutions, la fonction objectif étant discrète)

import cplex 

Champion = object

class optimisation(object):
    traits = []
    champions = []

    def __init__(self,traits,champions) -> None:
        self.traits = traits
        self.champions = champions
    
    def fonction_objectif(self,composition: set[Champion]) -> int:
        """
            Renvoie le nombre de stages de traits activés

            Prend en entrée un set d'identifiants de champions [0,50]
            ex: [12,15,1,43]

            Renvoie un entier positif
        """
        champions_by_trait = dict()
        #Construction d'un dictionnaire contenant le nombre de champions par trait
        #Pour chaque champion on rajoute 1 au compteur de chacun de ses traits
        for champion in composition:
            for trait in self.champions[champion]['traits']:
                champions_by_trait[trait] = champions_by_trait.get(trait,0) + 1


        #Le trait ACE ne peut avoir que 1 ou 4 persos
        #S'il contient entre 2 ou 3 persos il s'annule
        #Ca pourrait presque être une contrainte, ça m'étonnerait qu'une solution optimale utilise un trait mort 
        if champions_by_trait.get(10,0) > 1 and champions_by_trait.get(10,0) < 4:
            champions_by_trait.pop(10)
        

        nombre_traits_valides = 0
        #Compter le nombre d'étapes traits valides
        #Pour chaque trait, si un stage du trait est passé on ajoute 1
        for trait,quantite in champions_by_trait.items():
            for stage in self.traits[trait]['stages']:
                if quantite >= stage:
                    nombre_traits_valides += 1
                else:
                    continue
        return nombre_traits_valides

    def fonction_objectif_alt(self,composition: set[Champion])->int:
        """
            Implémentation alternative de la fonction objectif
            Laquelle est la plus rapide me direz-vous?
            J'en sais rien

            Renvoie le nombre de stages de traits activés

            Prend en entrée un set d'identifiants de champions [0,50]
            ex: [12,15,1,43]

            Renvoie un entier positif
        """
        champions_by_trait = dict()
        #Construction d'un dictionnaire contenant le nombre de champions par trait
        #Pour chaque champion on rajoute 1 au compteur de chacun de ses traits
        for index_trait,trait in enumerate(traits):
            print(trait)
            t:list = self.intersection(trait['champions'],composition)
            nb_champion_trait = len(t)
            if(nb_champion_trait > 0):
                champions_by_trait[index_trait] = nb_champion_trait


        #Le trait ACE ne peut avoir que 1 ou 4 persos
        #S'il contient entre 2 ou 3 persos il s'annule
        if champions_by_trait.get(10,0) > 1 and champions_by_trait.get(10,0) < 4:
            champions_by_trait.pop(10)
        

        nombre_traits_valides = 0
        #Compter le nombre d'étapes traits valides
        #Pour chaque trait, si un stage du trait est passé on ajoute 1
        for trait,quantite in champions_by_trait.items():
            for stage in self.traits[trait]['stages']:
                if quantite >= stage:
                    nombre_traits_valides += 1
                else:
                    continue
        return nombre_traits_valides

    def intersection(self,lst1, lst2)->list:
 
        # Use of hybrid method
        temp = set(lst2)
        lst3 = [value for value in lst1 if value in temp]
        return lst3




# importing the module
import json
 
# Opening JSON file
with open('Traits.json') as traits_file, open('Champions.json') as champions_file:


    #Listes des traits et des champions
    traits = json.load(traits_file)
    champions = json.load(champions_file)

    opti = optimisation(traits,champions)

    #Test de certaines compositions pour valider le fonctionnement de la fonction objectif
    composition7 = [38,4,17,5,7,44,41,48]
    composition8 = [38,4,17,5,7,44,41,48,32]
    composition7_2 = [38,4,17,5,7,44,41,48,32,6]

    assert(opti.fonction_objectif(composition7) == 7)
    assert(opti.fonction_objectif(composition8) == 8)
    assert(opti.fonction_objectif(composition7_2) == 7)

    assert(opti.fonction_objectif_alt(composition7) == 7)
    assert(opti.fonction_objectif_alt(composition8) == 8)
    assert(opti.fonction_objectif_alt(composition7_2) == 7)

    #Print the data of dictionary
    #print(traits)
    #print('')
    #print(champions)

    taille_composition = 5

    from docplex.mp.model import Model
    m = Model(name='list_integers')

    #Liste d'integers uniques
    #Est-ce possible d'optimiser pour pas qu'il génère une anagramme d'une composition déjà générée au préalable ?
    #si [1,2] a été fait, ça sert à rien de faire [2,1] ce sera le même résultat de la fonction objectif, du moins ça doit l'être
    composition = m.integer_var_dict(name="composition", lb=0,ub=50,keys=taille_composition)
    
    #On a pas le droit de mettre une fonction python, juste des combinaisons linéaires, ce serait trop beau si ça marchait
    #Si cette ligne marchait ce serait fini ptdr
    #Peut-être que ce serait plus simple avec un réseau de neurones mdr
    m.set_objective("max", opti.fonction_objectif(composition))

    #Affiche les informations de la recherche
    m.print_information()
    #Lance la recherche
    m.solve()
    #Affiche le résultat trouvé
    m.print_solution()

    

