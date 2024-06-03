#include <SFML/Graphics.hpp>
#include <iostream>

#include "parse.h"

// This will draw the cartesian plane
void draw_plane(sf::RenderWindow &window) {
    // Draw the X-axis
    sf::RectangleShape x_axis(sf::Vector2f(window.getSize().x, 1.f));
    x_axis.setPosition(sf::Vector2f(0, window.getSize().y / 2));
    x_axis.setFillColor(sf::Color::Black);
    window.draw(x_axis);

    // Draw the Y-axis
    sf::RectangleShape y_axis(sf::Vector2f(1, window.getSize().y));
    y_axis.setPosition(sf::Vector2f(window.getSize().x / 2, 0));
    y_axis.setFillColor(sf::Color::Black);
    window.draw(y_axis);
}

// This will calculate all the value of the expression and add them to the VertexArray
bool calculate_function(sf::RenderWindow const &window, sf::VertexArray &function, Expr *expr, sf::Vector2f steps) {
    if (expr == nullptr)
        return false;

    double horizontal_step = (double)window.getSize().x / steps.x;
    double vertical_step = (double)window.getSize().y / steps.y;

    bool success = true;

    function.clear();
    for (double x = -steps.x / 2; x <= steps.x / 2; x += 0.15) {
        double y = execute(expr, x);

        if (isnan(y)) {
            success = false;
            break;
        }

        sf::Vector2f pos;
        pos.x = x * horizontal_step + window.getSize().x / 2.f;
        pos.y = window.getSize().y / 2.f - y * vertical_step;

        function.append(sf::Vertex(pos, sf::Color::Black));
    }

    return success;
}

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    sf::RenderWindow window(sf::VideoMode(1000, 500), "Graphing Calculator", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "[ERROR] Couldn't load font" << std::endl;
        abort();
    }

    sf::String input = "y = cos(x)";

    sf::Text textbox;
    textbox.setFont(font);
    textbox.setFillColor(sf::Color::Black);
    textbox.setCharacterSize(14);
    textbox.setPosition(20, 20);

    sf::Text error;
    error.setFont(font);
    error.setFillColor(sf::Color::Red);
    error.setCharacterSize(36);

    sf::VertexArray function(sf::LineStrip);
    Expr* expr = expr_function(ExprKind::Cos, expr_x());
    sf::Vector2f steps(60, 40);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::Closed: {
                window.close();
            } break;
            case sf::Event::Resized: {
                // https://www.sfml-dev.org/tutorials/2.6/graphics-view.php#showing-more-when-the-window-is-resized
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            } break;
            case sf::Event::KeyReleased: {
                if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                    dealloc_expr(expr);
                    Parser parser(input.substring(4).toAnsiString());
                    expr = parser.parse();
                }
            } break;
            case sf::Event::TextEntered: {
                if (event.text.unicode == 8) {
                    size_t n = input.getSize() - 1;
                    if (n < 4) n = 4;
                    input = input.substring(0, n);
                }

                if (event.text.unicode < 128 && isprint(event.text.unicode))
                    input += (char)event.text.unicode;
            } break;
            }
        }

        textbox.setString(input);

        if (!calculate_function(window, function, expr, steps)) {
            error.setString("ERROR!");
            sf::FloatRect textRect = error.getLocalBounds();
            error.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
            error.setPosition(window.getView().getCenter());
        }
        else {
            error.setString("");
        }

        // Draw
        window.clear(sf::Color::White);

        draw_plane(window);
        window.draw(function);

        window.draw(textbox);
        window.draw(error);

        window.display();
    }

    return 0;
}