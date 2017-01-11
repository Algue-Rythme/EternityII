#include <algorithm>
#include <cassert>
#include <chrono>
#include <iterator>
#include <numeric>
#include <random>

#include "GeneticAlgorithm.hpp"

using namespace std;

Selection::Selection(unsigned int _nbParents) : nbParents(_nbParents)
{ }

Tournament::Tournament(unsigned int _nbParents, unsigned int _tournamentSize) : 
Selection(_nbParents), tournamentSize(_tournamentSize)
{ }

Population Tournament::operator()(const Population& population) const
{
    Population parents; parents.reserve(nbParents);
    for (unsigned int parent = 0; parent < nbParents; ++parent)
    {
        Population candidates(tournamentSize);
        generate_n(begin(candidates), tournamentSize, [&](){return population[randomEngine()%population.size()];});
        parents.push_back(bestIndividual(candidates));
    }
    return parents;
}

Truncation::Truncation(unsigned int _nbParents, unsigned int _nbTaken) : 
Selection(_nbParents), nbTaken(_nbTaken)
{ }

Population Truncation::operator()(const Population& population) const
{
    Population candidates = population;
    sort(begin(candidates), end(candidates));
    Population parents; parents.reserve(nbParents);
    unsigned int limit = min(nbTaken, candidates.size());
    generate_n(back_inserter(parents), nbParents, [&](){return candidates[randomEngine()%limit];});
    return parents;
}

RouletteWheel::RouletteWheel(unsigned int _nbParents) : 
Selection(_nbParents)
{ }

Population RouletteWheel::operator()(const Population& population) const
{
    vector<unsigned int> selections(nbParents);
    unsigned int wheel = accumulate(begin(population), end(population), 0,
                        [](unsigned int init, const Puzzle& elem) {
                            return init + elem.evaluate();
                        });
    generate_n(begin(selections), nbParents, [&](){return randomEngine()%(wheel+1);});
    sort(begin(selections), end(selections));
    Population parents; parents.reserve(nbParents);
    auto current = begin(population);
    unsigned int partialSum = current->evaluate();
    for (unsigned int value : selections)
    {
        while (partialSum < value)
        {
            ++current;
            partialSum += current->evaluate();
        }
        parents.push_back(*current);
    }
    return parents;
}

Crossover::Crossover(unsigned int _nbChilds): nbChilds(_nbChilds)
{ }

RegionExchange::RegionExchange(unsigned int nbChilds): Crossover(nbChilds)
{ }

Population RegionExchange::operator()(const Population& population) const
{
    Population parents(population);
    shuffle(begin(parents), end(parents), randomEngine);
    unsigned int limit = parents.size();
    Population childs;
    for (unsigned int child = 0; child*2 < nbChilds; ++child)
    {
        tuple<Puzzle, Puzzle> brotherAndSister = 
            crossover(parents[(2*child)%limit], parents[(2*child+1)%limit]);
        childs.push_back(get<0>(brotherAndSister));
        childs.push_back(get<1>(brotherAndSister));
    }
    return childs;
}

tuple<Puzzle, Puzzle> RegionExchange::crossover(const Puzzle& father, const Puzzle& mother) const
{
    unsigned int line = randomEngine()%father.boardSize();
    unsigned int column = randomEngine()%father.boardSize();
    unsigned int height = randomEngine()%(father.boardSize()-line) + 1;
    unsigned int width = randomEngine()%(father.boardSize()-column) + 1;
    sf::Rect<unsigned int> region(column, line, width, height);
    const vector<Piece> removedFromFather = piecesRemovedFrom(region, father);
    const vector<Piece> removedFromMother = piecesRemovedFrom(region, mother);
    Puzzle fatherChild = father;
    Puzzle motherChild = mother;
    addRemovedPieces(region, fatherChild, removedFromMother, removedFromFather);
    addRemovedPieces(region, motherChild, removedFromFather, removedFromMother);
    copyRegion(region, fatherChild, mother);
    copyRegion(region, motherChild, father);
    return make_tuple(fatherChild, motherChild);
}

vector<Piece> RegionExchange::piecesRemovedFrom(sf::Rect<unsigned int> region, const Puzzle& origin) const
{
    vector<Piece> piecesOrigin; piecesOrigin.reserve(region.height*region.width);
    for (unsigned int line = region.top; line < region.top+region.height; ++line)
        for (unsigned int column = region.left; column < region.left+region.width; ++column)
            piecesOrigin.push_back(origin(line, column));
    return piecesOrigin;
}

void RegionExchange::addRemovedPieces(sf::Rect<unsigned int> region, Puzzle& parent, vector<Piece> toRemove, vector<Piece> unfilteredFill) const
{
    sort(begin(toRemove), end(toRemove));
    sort(begin(unfilteredFill), end(unfilteredFill));
    vector<Piece> toFill;
    set_difference(begin(unfilteredFill), end(unfilteredFill), 
                   begin(toRemove), end(toRemove),
                   back_inserter(toFill));
    shuffle(begin(toFill), end(toFill), randomEngine);
    auto fill = begin(toFill);
    for (unsigned int line = 0; line < parent.boardSize(); ++line)
    {
        for (unsigned int column = 0; column < parent.boardSize(); ++column)
        {
            if ((line < region.top || line >= region.top+region.height 
             || column < region.left || column >= region.left+region.width)
             && binary_search(begin(toRemove), end(toRemove), parent(line, column)))
            {
                parent(line, column) = *fill;
                parent(line, column).rotate(randomEngine());
                ++fill;
            }
        }
    }
}

void RegionExchange::copyRegion(sf::Rect<unsigned int> region, Puzzle& parent, const Puzzle& other) const
{
    for (unsigned int line = region.top; line < region.top+region.height; ++line)
        for (unsigned int column = region.left; column < region.left+region.width; ++column)
            parent(line, column) = other(line, column);
}

Mutation::Mutation()
{ }

NoMutation::NoMutation()
{ }

void NoMutation::operator()(Population&) const
{ }

AllMutations::AllMutations(unsigned int _rate) : rate(_rate)
{ }

const std::vector<AllMutations::MutationOperator> 
AllMutations::mutations{AllMutations::rotateRegion, AllMutations::regionExchange};

void AllMutations::operator()(Population& childs) const
{
    for (auto& child : childs)
    {
        if (randomEngine()%101 + 1 <= rate)
        {
            mutations[randomEngine()%mutations.size()](child);
        }
    }
}

sf::Rect<unsigned int> AllMutations::randomRegion(const Puzzle& puzzle)
{
    unsigned int line = randomEngine()%puzzle.boardSize();
    unsigned int column = randomEngine()%puzzle.boardSize();
    unsigned int height = randomEngine()%min(8u, puzzle.boardSize()-line) + 1;
    unsigned int width = randomEngine()%min(8u, puzzle.boardSize()-column) + 1;
    return sf::Rect<unsigned int>(column, line, width, height);
}

void AllMutations::rotateRegion(Puzzle& puzzle)
{
    sf::Rect<unsigned int> region = randomRegion(puzzle);
    if (randomEngine() & 1)
        rotateRegionMinus(region, puzzle);
    else
        rotateRegionPlus(region, puzzle);
}

void AllMutations::transpose(sf::Rect<unsigned int> region, Puzzle& puzzle)
{
    for (unsigned int line = region.top; line < region.top+region.height; ++line)
    {
        for (unsigned int column = region.left; column < region.left+region.width; ++column)
        {
            swap(puzzle(line, column), puzzle(column, line));
            puzzle(line, column).rotate(1);
            puzzle(column, line).rotate(3);
        }
    }
}

void AllMutations::rotateRegionPlus(sf::Rect<unsigned int> region, Puzzle& puzzle)
{
    transpose(region, puzzle);
    for (unsigned int line = 0; line < puzzle.boardSize(); ++line)
        lineInversion(puzzle, line, region.top, region.top+region.height);
}

void AllMutations::rotateRegionMinus(sf::Rect<unsigned int> region, Puzzle& puzzle)
{
    transpose(region, puzzle);
    for (unsigned int column = 0; column < puzzle.boardSize(); ++column)
        columnInversion(puzzle, column, region.left, region.left+region.width);
}

void AllMutations::regionExchange(Puzzle& puzzle)
{
    sf::Rect<unsigned int> region = randomRegion(puzzle);
    unsigned int otherLine = randomEngine() % (puzzle.boardSize() - region.height + 1);
    unsigned int otherColumn = randomEngine() % (puzzle.boardSize() - region.width + 1);
    for (unsigned int line = 0; line < region.height; ++line)
        for (unsigned int column = 0; column < region.width; ++column)
            swap(puzzle(region.top+line, region.left+column), puzzle(otherLine+line, otherColumn+column));
}

void AllMutations::rotateRange(Puzzle& puzzle, unsigned int pivot)
{
    for (unsigned int axe = 0; axe < puzzle.boardSize(); ++axe)
    {
        swap(puzzle(pivot, axe), puzzle(axe, pivot));
        puzzle(pivot, axe).rotate(1);
        puzzle(axe, pivot).rotate(3);
    }
}

void AllMutations::swapLine(Puzzle& puzzle)
{
    unsigned int first = randomEngine() % puzzle.boardSize();
    unsigned int second = randomEngine() % puzzle.boardSize();
    for (unsigned int axe = 0; axe < puzzle.boardSize(); ++axe)
        swap(puzzle(first, axe), puzzle(second, axe));
}

void AllMutations::swapColumn(Puzzle& puzzle)
{
    unsigned int first = randomEngine() % puzzle.boardSize();
    unsigned int second = randomEngine() % puzzle.boardSize();
    for (unsigned int axe = 0; axe < puzzle.boardSize(); ++axe)
        swap(puzzle(axe, first), puzzle(axe, second));
}

void AllMutations::lineInversion(Puzzle& puzzle, unsigned int pivot, unsigned int d, unsigned int e)
{
    for (unsigned int axe = d; axe < e; ++axe)
    {
        swap(puzzle(pivot, axe), puzzle(pivot, e - axe - 1 + d));
        puzzle(pivot, axe).rotate(2);
        puzzle(pivot, e - axe - 1 + d).rotate(2);
    }
}

void AllMutations::columnInversion(Puzzle& puzzle, unsigned int pivot, unsigned int d, unsigned int e)
{
    for (unsigned int axe = d; axe < e; ++axe)
    {
        swap(puzzle(axe, pivot), puzzle(e - axe - 1 + d, pivot));
        puzzle(axe, pivot).rotate(2);
        puzzle(e - axe - 1 + d, pivot).rotate(2);
    }
}

void ReplaceWorst::operator()(Population& origin, const Population& childs) const
{
    sort(begin(origin), end(origin));
    copy(begin(childs), end(childs), begin(origin));
}

Population::Population()
{ }

Population::Population(unsigned int populationSize, const Puzzle& genes):
vector<Puzzle>(populationSize, genes)
{
    makeRandomPopulation();
}

void Population::nextGeneration(
const Selection& selection, 
const Crossover& crossover, 
const Mutation& mutation, 
const Insertion& insertion)
{
    Population parents = selection(*this);
    Population childs = crossover(parents);
    mutation(childs);
    childs.evaluate();
    insertion(*this, childs);
}

const Puzzle& Population::getIndividual(function<const Puzzle& (const Population&)> view) const
{
    return view(*this);
}

void Population::evaluate()
{
    for (auto& individual : (*this))
        individual.computeEvaluation();
}

void Population::makeRandomPopulation()
{
    for (auto& puzzle : (*this))
        puzzle.shuffle();
    evaluate();
}
