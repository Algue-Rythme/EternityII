#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>

#include "Puzzle.hpp"

using namespace std;

static const unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine randomEngine(seed);

const Puzzle& randomIndividual(const vector<Puzzle>& population)
{
    return population[randomEngine()%population.size()];
}

const Puzzle& bestIndividual(const vector<Puzzle>& population)
{
    return *max_element(begin(population), end(population));
}

enum Direction
{
    Top = 2,
    Left = 1,
    Bottom = 0,
    Right = 3
};

enum PieceColor
{
    Side = 0,
};

Piece::Piece(): ID(undefined_ID)
{ }

Piece::Piece(const array<unsigned int, nbEdges>& _edges): 
edges(_edges), ID(hash(edges))
{ }

unsigned int Piece::hash(const array<unsigned int, nbEdges>& edges)
{
    vector<unsigned int> puissances = {100*100*100, 100*100, 100, 1};
    return inner_product(begin(edges), end(edges), begin(puissances), 0);
}

void Piece::rotate(unsigned int shift)
{
    std::rotate(begin(edges), begin(edges)+shift%nbEdges, end(edges));
}

unsigned int Piece::operator[](unsigned int edge) const
{
    return edges[edge];
}

istream& operator>>(istream& in, Piece& piece)
{
    static unsigned int ID = 1;
    for (auto& edge : piece.edges)
        in >> edge;
    piece.ID = ID++;
    return in;
}

ostream& operator<<(ostream& out, const Piece& piece)
{
    out << "[" << piece.ID << "]: ";
    for (auto& edge : piece.edges)
        out << edge << " ";
    return out;
}

bool operator==(const Piece& left, const Piece& right)
{
    return left.ID == right.ID;
}

bool operator!=(const Piece& left, const Piece& right)
{
    return !(left == right);
}

bool operator<(const Piece& left, const Piece& right)
{
    return left.ID < right.ID;
}

Puzzle::Puzzle(): size(0), evaluation(0)
{ }

Puzzle::Puzzle(unsigned int _size, 
               const vector<Piece>& _pieces = vector<Piece>()): size(_size), pieces(_pieces)
{
    computeEvaluation();
}

unsigned int Puzzle::boardSize() const
{
    return size;
}

unsigned int Puzzle::evaluate() const
{
    return evaluation;
}

void Puzzle::computeEvaluation()
{
    unsigned int nbCorrectEdges = 0;
    for (unsigned int index = 0; index < size; ++index)
    {
        if (get(0, index)[Top] == PieceColor::Side)
            ++nbCorrectEdges;
        if (get(index, 0)[Left] == PieceColor::Side)
            ++nbCorrectEdges;
        if (get(index, size-1)[Right] == PieceColor::Side)
            ++nbCorrectEdges;
        if (get(size-1, index)[Bottom] == PieceColor::Side)
            ++nbCorrectEdges;
    }
    for (unsigned int line = 0; line < size; ++line)
    {
        for (unsigned int column = 0; column < size; ++column)
        {
            if (column+1 < size && get(line, column)[Right] != PieceColor::Side &&
                get(line, column)[Right] == get(line, column+1)[Left])
                ++nbCorrectEdges;
            if (line+1 < size && get(line, column)[Bottom] != PieceColor::Side &&
                get(line, column)[Bottom] == get(line+1, column)[Top])
                ++nbCorrectEdges;
        }
    }
    evaluation = nbCorrectEdges;
}

void Puzzle::shuffle()
{
    for (auto& piece : pieces)
        piece.rotate(randomEngine());
    std::shuffle(begin(pieces), end(pieces), randomEngine);
}

Piece Puzzle::get(unsigned int line, unsigned int column) const
{
    return pieces[line*size + column];
}

Piece Puzzle::operator()(unsigned int line, unsigned int column) const
{
    return pieces[line*size + column];
}

Piece& Puzzle::operator()(unsigned int line, unsigned int column)
{
    return pieces[line*size + column];
}

bool operator==(Puzzle left, Puzzle right)
{
    sort(begin(left.pieces), end(left.pieces));
    sort(begin(right.pieces), end(right.pieces));
    return equal(begin(left.pieces), end(left.pieces), begin(right.pieces));
}

#ifdef DEBUG
void print(ostream& out, Puzzle left, Puzzle right)
{
    sort(begin(left.pieces), end(left.pieces));
    sort(begin(right.pieces), end(right.pieces));
    for (unsigned int line = 0; line < left.boardSize(); ++line)
    {
        for (unsigned int column = 0; column < left.boardSize(); ++column)
        {
            out << left(line, column) << "\t" << right(line, column) << "\n";
        }
    }
}
#endif

bool operator!=(const Puzzle& left, const Puzzle& right)
{
    return !(left == right);
}

bool operator<(const Puzzle& left, const Puzzle& right)
{
    return left.evaluate() < right.evaluate();
}

bool operator>(const Puzzle& left, const Puzzle& right)
{
    return right < left;
}
