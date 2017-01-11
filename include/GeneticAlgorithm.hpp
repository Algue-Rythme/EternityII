#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP

#include <functional>
#include <tuple>

#include "SFML/Graphics.hpp"

#include "Puzzle.hpp"

class Selection; // DONE
class Crossover; // DONE
class Mutation; // IN PROGRESS
class Insertion; // IN PROGRESS

class Population : public std::vector<Puzzle>
{
    public:
    
        Population();
        Population(unsigned int, const Puzzle& = Puzzle());
        
        void makeRandomPopulation();
        void nextGeneration(const Selection&, const Crossover&, const Mutation&, const Insertion&);
        const Puzzle& getIndividual(std::function<const Puzzle& (const Population&)>) const;
        
    private:
    
        void evaluate();
};

class Selection
{
    public:
        Selection(unsigned int);
        virtual Population operator()(const Population&) const = 0;
    protected:
        unsigned int nbParents;
};

class Tournament : public Selection
{
    public:
        Tournament(unsigned int, unsigned int);
        Population operator()(const Population&) const;
    private:
        unsigned int tournamentSize;
};

class Truncation : public Selection
{
    public:
        Truncation(unsigned int, unsigned int);
        Population operator()(const Population&) const;
    private:
        unsigned int nbTaken;
};

class RouletteWheel : public Selection
{
    public:
        RouletteWheel(unsigned int);
        Population operator()(const Population&) const;
};

class Crossover
{
    public:
        Crossover(unsigned int);
        virtual Population operator()(const Population&) const = 0;
    protected:
        unsigned int nbChilds;
};

class RegionExchange : public Crossover
{
    public:
        RegionExchange(unsigned int);
        Population operator()(const Population&) const;
    private:
        std::tuple<Puzzle, Puzzle> crossover(const Puzzle&, const Puzzle&) const;
        std::vector<Piece> piecesRemovedFrom(sf::Rect<unsigned int>, const Puzzle&) const;
        void addRemovedPieces(sf::Rect<unsigned int>, Puzzle&, std::vector<Piece>, std::vector<Piece>) const;
        void copyRegion(sf::Rect<unsigned int> region, Puzzle& parent, const Puzzle&) const;
};

class Mutation
{
    public:
        Mutation();
        virtual void operator()(Population&) const = 0;
};

class NoMutation : public Mutation
{
    public:
        NoMutation();
        void operator()(Population&) const;
};

class AllMutations : public Mutation
{
    typedef std::function<void (Puzzle&)> MutationOperator;
    
    public:
        AllMutations(unsigned int);
        void operator()(Population&) const;
    private:
        static void swapLine(Puzzle&);
        static void swapColumn(Puzzle&);
        static void rotateRegion(Puzzle&);
        static void transpose(sf::Rect<unsigned int>, Puzzle&);
        static void rotateRegionPlus(sf::Rect<unsigned int>, Puzzle& puzzle);
        static void rotateRegionMinus(sf::Rect<unsigned int>, Puzzle& puzzle);
        static void regionExchange(Puzzle&);
        static void lineInversion(Puzzle&, unsigned int, unsigned int, unsigned int);
        static void columnInversion(Puzzle&, unsigned  int, unsigned int, unsigned int);
        static void rotateRange(Puzzle& puzzle, unsigned int);
        static sf::Rect<unsigned int> randomRegion(const Puzzle&);
    
        unsigned int rate;
        static const std::vector<MutationOperator> mutations;
};

class Insertion
{
    public:
        virtual void operator()(Population&, const Population&) const = 0;
};

class ReplaceWorst : public Insertion
{
    public:
        void operator()(Population&, const Population&) const;
};

#endif


