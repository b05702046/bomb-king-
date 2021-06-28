#include <iostream>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <SFML\Window.hpp>
#include <SFML\System.hpp>

#include <cstdlib>
#include <Vector>

int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(640, 480), "Simple Square Swag", sf::Style::Default);
    window.setFramerateLimit(60);

    sf::CircleShape player;
    player.setFillColor(sf::Color::White);
    player.setRadius(50.f);
    player.setPosition(window.getSize().x / 2 - player.getRadius(), window.getSize().y - player.getRadius() * 2 - 10.f);
    sf::Vector2f playerCenter;

    sf::CircleShape projectile;
    projectile.setFillColor(sf::Color::Red);
    projectile.setRadius(5.f);
    std::vector<sf::CircleShape> projectiles;
    projectiles.push_back(projectile);
    int shootTimer = 0;

    sf::RectangleShape enemy;
    enemy.setFillColor(sf::Color::Magenta);
    enemy.setSize(sf::Vector2f(50.f, 50.f));
    std::vector<sf::RectangleShape> enemies;
    enemies.push_back(enemy);
    int enemySpawnTimer = 0;
    

    while (window.isOpen()) {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            if (evnt.type == sf::Event::Closed) {
                window.close();
            }
            if (evnt.KeyPressed && evnt.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        player.setPosition(sf::Mouse::getPosition(window).x - player.getRadius(), player.getPosition().y);

        playerCenter = sf::Vector2f(player.getPosition().x + player.getRadius(), player.getPosition().y + player.getRadius());

        window.clear(sf::Color::Black);

        if (shootTimer < 30)
            shootTimer++;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer >= 6) {
            projectile.setPosition(playerCenter);
            projectiles.push_back(projectile);
            shootTimer -= 6;
        }

        for (std::size_t i = 0; i < projectiles.size(); i++) {
            projectiles[i].move(0.f, -10.f);

            if (projectiles[i].getPosition().y <= 0)
                projectiles.erase(projectiles.begin() + i);
        }

        if (enemySpawnTimer < 20)
            enemySpawnTimer++;

        if (enemySpawnTimer == 20) {
            enemy.setPosition(rand() % int(window.getSize().x - enemy.getSize().x), -enemy.getSize().y);
            enemies.push_back(enemy);
            enemySpawnTimer = 0;
        }

        for (std::size_t i = 0; i < enemies.size(); i++) {
            enemies[i].move(0.f, 5.f);

            if(enemies[i].getPosition().y >= window.getSize().y)
                enemies.erase(enemies.begin() + i);
        }

        for (std::size_t i = 0; i < projectiles.size(); i++) {
            for (std::size_t j = 0; j < enemies.size(); j++) {
                if (projectiles[i].getGlobalBounds().intersects(enemies[j].getGlobalBounds())) {
                    projectiles.erase(projectiles.begin() + i);
                    enemies.erase(enemies.begin() + j);
                    break;
                }
            }
        }

        window.draw(player);

        for (std::size_t i = 0; i < enemies.size(); i++) {
            window.draw(enemies[i]);
        }
        
        for (std::size_t i = 0; i < projectiles.size(); i++) {
            window.draw(projectiles[i]);
        }

        window.display();

    }

    return 0;
}
