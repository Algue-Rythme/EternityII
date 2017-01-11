#ifndef USER_VIEW_HPP
#define USER_VIEW_HPP

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"

#include "Puzzle.hpp"

class EternityII;

class UserView : public sf::Drawable, public sf::Transformable
{
	public: 
	
		UserView(unsigned int, const std::vector<sf::Color>&);
		UserView(const UserView&) = delete;
		UserView& operator=(const UserView&) = delete;

        bool isOpen() const;
        void close();
        void eventLoop();
        void updatePuzzle(const Puzzle&);
		virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
        
		virtual ~UserView();
	
	private:
    
        void drawBoard();
    
        unsigned int size;
        unsigned int width;
        unsigned int height;
        std::unique_ptr<sf::RenderWindow> window;
        std::unique_ptr<sf::Thread> rendering;
        std::vector<sf::Color> colors;
        sf::VertexArray vertices;
        unsigned int cellSize;
};

#endif
