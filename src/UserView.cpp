#include<cmath>
#include <complex>

#include "UserView.hpp"
#include "Utils.hpp"

using namespace std;
using namespace sf;

static const float pi = atan(1) * 4.0;
static const float sqrt2 = sqrt(2.);
static const complex<float> threeHours = polar<float>(1.0, pi / 2.0);

UserView::UserView(unsigned int sizeSide, 
                   const vector<Color>& colorsReference) : 
size(sizeSide), width(512), height(512), colors(colorsReference)
{
    unsigned int widthSize = width / size;
    unsigned int heightSize = height / size;
    cellSize = min(widthSize, heightSize);
    
    drawBoard();
    
    sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
    window = unique_ptr<RenderWindow>(new RenderWindow(VideoMode(width, height), 
                                                       "EternityII puzzle", 
                                                       Style::Default, 
                                                       settings));
    window->setActive(false);
    window->setFramerateLimit(30);
    auto RenderingThread =
    [&]()
    {
        while (window->isOpen())
        {
            window->clear();
            window->draw(*this);
            window->display();
        }
    };
    rendering = unique_ptr<Thread>(new Thread(RenderingThread));
    rendering->launch();
}

bool UserView::isOpen() const
{
    return window->isOpen();
}

void UserView::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(vertices, states);
}

void UserView::drawBoard()
{
    vertices.setPrimitiveType(Triangles);
    vertices.resize(size*size*nbEdges*3);
    for (unsigned int line = 0; line < size; ++line)
    {
        for (unsigned int column = 0; column < size; ++column)
        {
            complex<float> center(column*cellSize + cellSize/2., line*cellSize + cellSize/2.);
            complex<float> needle = polar<float>(cellSize/sqrt2, pi/4);
            for (unsigned int edge = 0; edge < nbEdges; ++edge)
            {
                Vertex * side = &vertices[((line*size + column)*nbEdges + edge)*3];
                
                side[0].position = makeVector2<float>(center);
                side[1].position = makeVector2<float>(center + needle);
                side[2].position = makeVector2<float>(center + needle * threeHours);
                
                needle *= threeHours;
            }
        }
    }
}

void UserView::updatePuzzle(const Puzzle& puzzle)
{
    for (unsigned int line = 0; line < size; ++line)
    {
        for (unsigned int column = 0; column < size; ++column)
        {
            for (unsigned int edge = 0; edge < nbEdges; ++edge)
            {
                Vertex * side = &vertices[((line*size + column)*nbEdges + edge)*3];
                for (unsigned int point = 0; point < 3; ++point)
                    side[point].color = colors[puzzle.get(line, column)[edge]];
            }
        }
    }
}

void UserView::close()
{
    if (window != nullptr)
        window->close();
    if (rendering != nullptr)
        rendering->wait();
}

void UserView::eventLoop()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
             window->close();
    }
}

UserView::~UserView()
{
    close();
}

