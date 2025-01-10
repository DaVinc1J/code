import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap, TwoSlopeNorm

def main():
    multipliers = readMultipliersFrom("samples/sample-total.txt")

    bailLower = 1.01
    bailHigher = 200.00
    bailStep = 0.01

    distanceLower = 1 
    distanceHigher = 6
    distanceStep = 1 

    BET_AMOUNT = 1

    useMesh = False

    bails = np.arange(bailLower, bailHigher, bailStep)

    distances = np.arange(distanceLower, distanceHigher, distanceStep)

    profits = calculateAllCombosOf(multipliers, bails, distances, BET_AMOUNT)

    mapData(profits, useMesh)

def readMultipliersFrom(filePath):
    with open(filePath, 'r') as file:

        multipliers = []
    
        for line in file:
            multiplier = float(line.strip())
            multipliers.append(round(float(line.strip()), 2))
    
        return multipliers

def calculateAllCombosOf(multipliers, bails, distances, betAmount):
    profits = []

    for bail in bails:
        for distance in distances:

            profit = 0 
            count = 0 
            counting = False

            for multiplier in multipliers:

                if distance >= count:
                    if multiplier >= bail:
                        profit += (bail - 1) * betAmount
                    else:
                        profit -= betAmount 

                if multiplier == 1.00:
                    if count != 0:
                        count = 0
                    else:
                        counting = True

                if counting == True:
                    count += 1
            
            profits.append((round(bail, 2), distance, int(profit)))
            print(f"{bail:.2f}, {distance}, {int(profit)}")

    return profits

def mapData(profits, useMesh):

    profitArray = np.array(profits)

    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')

    bails = profitArray[:, 0]
    distances = profitArray[:, 1]
    profits = profitArray[:, 2]

    negativeColours = plt.get_cmap('inferno_r', 128)
    positiveColours = plt.get_cmap('summer', 128)

    combinedColours = np.vstack((negativeColours(np.linspace(0, 1, 128)),
                                 positiveColours(np.linspace(0, 1, 128))))
    combinedColourMap = ListedColormap(combinedColours, name='combined')

    norm = TwoSlopeNorm(vmin=min(profits), vcenter=0, vmax=max(profits))

    if useMesh:
        ax.plot_trisurf(bails, distances, profits, cmap=combinedColourMap, norm=norm, alpha=0.8)
    else:
        ax.scatter(bails, distances, profits, c=profits, cmap=combinedColourMap, norm=norm, marker='x')

    ax.set_title('Profit vs Bail vs Distance')
    ax.set_xlabel('Bail')
    ax.set_ylabel('Distance')
    ax.set_zlabel('Profit')

    plt.show()

main()
