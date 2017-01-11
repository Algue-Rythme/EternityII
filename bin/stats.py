import matplotlib.pyplot as plt

populations = []

##
def load():
    eval = open("D:\\Projets\\EternityII\\bin\\eval.txt")
    stats = eval.read().split("\n")[:-1]
    N = []
    SP = []
    for stat in stats:
        n, s, sp = stat.split()
        N.append(int(n))
        SP.append(float(sp[:-1]))
    populations.append((N, SP))
    return len(populations)

def draw():
    for (N, SP) in populations:
        plt.plot(N, SP)
    plt.show()

##
def middle():
    S = [0 for k in range(3000)]
    N = [0 for k in range(3000)]
    for (n, sp) in populations:
        for i in range(min(len(sp), len(S))):
            S[i] += sp[i]
            N[i] += 1
    Y = [S[i] / N[i] for i in range(len(S)) if N[i] != 0]
    X = [k for k in range(len(Y))]
    plt.plot(X, Y, color='red')
    M1 = open("D:\\Projets\\EternityII\\bin\\results\\M1.txt", "a")
    for y in Y:
        M1.write(str(y)+"\n")
    M1.close()
            
##
def combinate():
    base = "D:\\Projets\\EternityII\\bin\\results\\"
    noMutations=["Random_10000_MOYENNE.txt", "Roulette2500_RegionExchange2500_NoMutation_ReplaceWorst_10000_MOYENNE.txt", "Tournament2500_100_RegionExchange2500_NoMutation_ReplaceWorst_10000_MOYENNE.txt","Tournament2500_10_RegionExchange2500_NoMutation_ReplaceWorst_10000_MOYENNE.txt","Random2500_RegionExchange2500_NoMutation_ReplaceWorst_10000_MOYENNE.txt"]
    mutations = ["Roulette2500_RegionExchange2500_Heavy10_ReplaceWorst_10000_MOYENNE.txt","Tournament2500_3_RegionExchange2500_Heavy10_ReplaceWorst_10000_MOYENNE.txt","Tournament2500_100_RegionExchange2500_Heavy10_ReplaceWorst_10000_MOYENNE.txt"]
    performs = ["Tournament2500_3_RegionExchange2500_Heavy10_ReplaceWorst_10000_PERFORM.txt","Roulette2500_RegionExchange2500_Heavy10_ReplaceWorst_10000_PERFORM.txt","Random2500_RegionExchange2500_NoMutation_ReplaceWorst_10000_PERFORM.txt","Roulette2500_RegionExchange2500_Heavy10_ReplaceWorst_10000_PRESSURE.txt"]
    names = performs+mutations+noMutations
    for name in names:
        file = open(base+name)
        values = file.read().split()
        Y = [float(v) for v in values]
        X = [k for k in range(len(Y))]
        plt.plot(X, Y)
        file.close()

            