#include <algorithm>
#include <fstream>
#include <iterator>

#include "SFML/System.hpp"

#include "EternityII.hpp"
#include "GeneticAlgorithm.hpp"

using namespace std;
using namespace sf;

const unsigned int indexMax = 2000;

EternityII::EternityII(const std::string& piecesFile, const std::string& colorsFile): size(0), view(nullptr)
{
    loadPieces(piecesFile);
    vector<Color> colors = loadTexture(colorsFile);
    view = unique_ptr<UserView>(new UserView(size, colors));
    view->updatePuzzle(reference);
}

void EternityII::start(unsigned int sizePopulation)
{
    ofstream out("eval.txt");
    out.precision(2);
	out.setf(ios::fixed, ios::floatfield);
    
    Population population(sizePopulation, reference);
    unsigned int index = 0;
    Puzzle best = population.getIndividual(bestIndividual);
    
    while (view->isOpen() && index <= indexMax)
    {
        view->eventLoop();
        
        population.nextGeneration(
            RouletteWheel(population.size()/4), 
            RegionExchange(population.size()/4),
            AllMutations(10),
            ReplaceWorst());
            
        Puzzle bestCurrent = population.getIndividual(bestIndividual);
        if (bestCurrent > best)
        {
            view->updatePuzzle(bestCurrent);
            best = bestCurrent;
        }
        
        unsigned int score = best.evaluate();
        out << (index++) << " " << score << " ";
        out << (score*100)/double(2*size*(size+1)) << "%\n";
        
        #ifdef DEBUG
        if (best != reference)
        {
            out << "Corruption !\n";
            print(out, best, reference);
            return ;
        }
        #endif
    }
}

void EternityII::loadPieces(const std::string& fileName)
{
    vector<Piece> pieces;
    ifstream in(fileName);
    in >> size;
    pieces.resize(size*size);
    for (auto& piece : pieces)
        in >> piece;
    reference = Puzzle(size, pieces);
}

istream& operator>>(istream& in, Color& color)
{
    unsigned int red, green, blue;
    in >> red >> green >> blue;
    color = Color(red, green, blue);
    return in;
}

vector<Color> loadTexture(const std::string& fileName)
{
    ifstream in(fileName);
    unsigned int nbColors;
    in >> nbColors;
    
    vector<Color> colors(nbColors);
    for (auto& color : colors)
        in >> color;
    
    return colors;
}