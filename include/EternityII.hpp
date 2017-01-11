#ifndef ETERNITY_II_HPP
#define ETERNITY_II_HPP

#include <istream>
#include <memory>
#include <string>

#include "UserView.hpp"

class EternityII
{
    public:
    
        EternityII(const std::string& pieces, const std::string& colors);
        EternityII(const EternityII&) = delete;
        const EternityII& operator=(const EternityII&) = delete;
        
        void start(unsigned int sizePopulation);
    
    private:
    
        void loadPieces(const std::string&);
    
        unsigned int size;
        Puzzle reference;
        std::unique_ptr<UserView> view;
};

std::istream& operator>>(std::istream&, sf::Color&);
std::vector<sf::Color> loadTexture(const std::string&);

#endif