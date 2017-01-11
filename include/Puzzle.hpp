#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include <array>
#include <iostream>
#include <vector>

const std::size_t nbEdges = 4;
const unsigned int undefined_ID = 0;
extern std::default_random_engine randomEngine;

class Piece
{
    public:
    
        Piece();
        Piece(const std::array<unsigned int, nbEdges>&);
    
        void rotate(unsigned int);
        
        unsigned int operator[](unsigned int) const;
        friend bool operator==(const Piece&, const Piece&);
        friend std::istream& operator>>(std::istream&, Piece&);
        friend std::ostream& operator<<(std::ostream&, const Piece&);
        friend bool operator<(const Piece&, const Piece&);
        
    private:
    
        static unsigned int hash(const std::array<unsigned int, nbEdges>&);
    
        std::array<unsigned int, nbEdges> edges;
        unsigned int ID;
};

bool operator!=(const Piece&, const Piece&);

class Puzzle
{
    public:
    
        Puzzle();
        Puzzle(unsigned int, const std::vector<Piece>&);
        
        void shuffle();
        unsigned int boardSize() const;
        unsigned int evaluate() const;
        void computeEvaluation();
        
        Piece get(unsigned int, unsigned int) const;
        Piece operator()(unsigned int, unsigned int) const;
        Piece& operator()(unsigned int, unsigned int);
        
        friend bool operator==(Puzzle, Puzzle);
        #ifdef DEBUG
        friend void print(std::ostream&, Puzzle, Puzzle);
        #endif
    
    private:
    
        unsigned int size;
        unsigned int evaluation;
        std::vector<Piece> pieces;
};

bool operator!=(const Puzzle&, const Puzzle&);
bool operator<(const Puzzle&, const Puzzle&);
bool operator>(const Puzzle&, const Puzzle&);

const Puzzle& randomIndividual(const std::vector<Puzzle>&);
const Puzzle& bestIndividual(const std::vector<Puzzle>&);

#endif 