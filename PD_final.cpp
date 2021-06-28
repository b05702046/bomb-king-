#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
const int PLAYER_CNT = 2;  // 玩家數量
// a character moves MOVE_DISTANCE * MOVE_PER_TIME in distance per move
const int MOVE_DISTANCE = 5;      // 初始一幀中位移的距離
const int MOVE_PER_TIME = 10;     // 初始按一次按鍵共位移幾幀
const int SQUARES_PER_SIDE = 15;  // 地圖每邊的格子數
const int SQUARE_SIDE = 50;       // 背景中格子的邊長
const int INITIAL_BOMB_CNT = 1;   // 玩家的初始「可放置炸彈數」
const int INITIAL_LIVE = 1;       // 玩家的初始血量
const int INITIAL_BOMB_RANGE = 1; // 玩家的初始炸彈爆炸半徑
const int Propscnt = 5;           // 道具種類數

// 遊戲背景的格子
class Square
{
private:

    int x;          // 格子顯示在介面上的x值
    int y;          // 格子顯示在介面上的y值
    int situation;  // 格子的狀態，1為走道；2為可被破壞的格子；3為不可被破壞的格子

public:
    sf::VertexArray buildSquare();  // 在介面上顯示格子
    int getSituation();
    void setSituation(int situation);
    void set_x(int x);
    void set_y(int y);

};


sf::VertexArray Square::buildSquare()
{
    int upper_x = x * SQUARE_SIDE - 3;
    int lower_x = (x - 1) * SQUARE_SIDE + 3;
    int upper_y = y * SQUARE_SIDE - 3;
    int lower_y = (y - 1) * SQUARE_SIDE + 3;
    // create a quad
    sf::VertexArray quad(sf::Quads, 4);


    quad[0].position = sf::Vector2f(lower_x, lower_y);
    quad[1].position = sf::Vector2f(upper_x, lower_y);
    quad[2].position = sf::Vector2f(upper_x, upper_y);
    quad[3].position = sf::Vector2f(lower_x, upper_y);

    // define its texture area to be a 25x50 rectangle starting at (0, 0)
    if (situation == 3)
    {
        quad[0].color = sf::Color::Green;
        quad[1].color = sf::Color::Green;
        quad[2].color = sf::Color::Green;
        quad[3].color = sf::Color::Green;
    }
    else if (situation == 2)
    {
        quad[0].texCoords = sf::Vector2f(lower_x, lower_y);
        quad[0].texCoords = sf::Vector2f(upper_x, lower_y);
        quad[0].texCoords = sf::Vector2f(upper_x, upper_y);
        quad[0].texCoords = sf::Vector2f(lower_x, upper_y);
    }

    else
    {
        quad[0].color = sf::Color::Transparent;
        quad[1].color = sf::Color::Transparent;
        quad[2].color = sf::Color::Transparent;
        quad[3].color = sf::Color::Transparent;
    }

    return quad;


}

int Square::getSituation() {
    return situation;
}

void Square::setSituation(int situation) {
    this->situation = situation;
}

void Square::set_x(int x) {
    this->x = x;
}
void Square::set_y(int y) {
    this->y = y;
}

// 炸彈爆炸後產生的火焰
class Fire {
private:
    int x;  // 火焰在介面上的x值
    int y;  // 火焰在介面上的y值
public:
    Fire(int x, int y);
    sf::Sprite fire;
};

Fire::Fire(int x, int y) {
    fire.setScale(sf::Vector2f(0.1f, 0.1f));
    fire.setPosition(x, y);  // 產生火焰時即給定(x, y)值
    this->x = x;
    this->y = y;
}

// 玩家可放置的炸彈
class Bomb {
private:
    int x;               // 炸彈在介面上的x值
    int y;               // 炸彈在介面上的y值
    int timer;           // 炸彈的計時器，根據剩餘時間會有不同狀態
    int explosionRange;  // 炸彈爆炸的半徑
    bool* hit = new bool[PLAYER_CNT] {0};  // 紀錄一顆炸彈已對哪些玩家造成傷害 (避免玩家重複被同炸彈炸到)
public:
    std::vector<Fire> fires;  // 儲存炸彈產生的火焰
    Bomb(int explosionRange, int x, int y, Square squares[][SQUARES_PER_SIDE]);
    bool countdown(sf::Texture bombTex[]);  // 根據計時器產生不同狀態，並回傳是否爆炸
    int getTimer();
    sf::Sprite bomb;
    bool getHit(int player);
    void setHit(int player);
    int getPosition_x();
    int getPosition_y();
};

Bomb::Bomb(int explosionRange, int x, int y, Square squares[][SQUARES_PER_SIDE]) {
    timer = 120;

    bomb.setScale(sf::Vector2f(0.3f, 0.3f));
    bomb.setPosition(x, y);

    // 宣告一陣列，儲存炸彈的上下左右共四個方位上是否有不可破壞的障礙物
    bool* block = new bool[4];
    for (int i = 0; i < 4; i++)
        block[i] = false;
    Fire fire(bomb.getPosition().x + 1, bomb.getPosition().y + 1);  // 先在bomb本身的位置上生成第一個火焰
    fires.push_back(fire);
    // 在炸彈的四個方位，根據爆炸半徑逐層向外拓展，產生火焰
    // 一遇到不可破壞的障礙物就不會再朝該方向產生火焰
    for (int i = 1; i <= explosionRange; i++) {
        if (squares[x / 50][y / 50 - i].getSituation() != 3 && block[0] == false && y - 50 * i >= 0) {
            Fire upFire(x + 1, y - 50 * i + 1);
            fires.push_back(upFire);
        }
        else
            block[0] = true;
        if (squares[x / 50][y / 50 + i].getSituation() != 3 && block[1] == false && y + 50 * i < SQUARES_PER_SIDE * SQUARE_SIDE) {
            Fire downFire(x + 1, y + 50 * i + 1);
            fires.push_back(downFire);
        }
        else
            block[1] = true;
        if (squares[x / 50 - i][y / 50].getSituation() != 3 && block[2] == false && (x - 50 * i) >= 0) {
            Fire leftFire(x - 50 * i + 1, y + 1);
            fires.push_back(leftFire);
        }
        else
            block[2] = true;
        if (squares[x / 50 + i][y / 50].getSituation() != 3 && block[3] == false && (x + 50 * i) < SQUARES_PER_SIDE * SQUARE_SIDE) {
            Fire rightFire(x + 50 * i + 1, y + 1);
            fires.push_back(rightFire);
        }
        else
            block[3] = true;
    }
    this->explosionRange = explosionRange;
    this->x = x;
    this->y = y;
}

// 炸彈的計時
bool Bomb::countdown(sf::Texture bombTex[]) {
    // timer在每一幀會 -1
    // 根據剩餘時間改變外觀，並訂 <=20 為爆炸
    timer -= 1;
    if (timer == 0)
        return true;
    else if (timer <= 10)
        bomb.setTexture(bombTex[4]);
    else if (timer <= 20)
        bomb.setTexture(bombTex[3]);
    else if (timer <= 30)
        bomb.setTexture(bombTex[2]);
    else
        if ((timer / 15) % 2 == 0)
            bomb.setTexture(bombTex[1]);
        else
            bomb.setTexture(bombTex[0]);
    return false;
}

int Bomb::getTimer() {
    return timer;
}

bool Bomb::getHit(int player) {
    return hit[player];
}

void Bomb::setHit(int player) {
    hit[player] = true;
}

int Bomb::getPosition_x() {
    return x;
}

int Bomb::getPosition_y() {
    return y;
}

// 玩家
class Character
{
private:
    int x;                      // 玩家顯示在介面上的y值
    int y;                      // 玩家顯示在介面上的y值
    int playerOrder;            // 玩家編號
    int explosionRange;         // 放置之炸彈的爆炸半徑
    int maxBomb;                // 同時間最大可放置炸彈數
    int lives;                  // 玩家血量
    int movePerTime;            // 按一次按鍵共位移幾幀
    int moveDistance;           // 一幀中位移的距離
    sf::Keyboard::Key up;       // 玩家向上移的按鍵
    sf::Keyboard::Key down;     // 玩家向下移的按鍵
    sf::Keyboard::Key left;     // 玩家向左移的按鍵
    sf::Keyboard::Key right;    // 玩家向右移的按鍵
    sf::Keyboard::Key setBomb;  // 玩家放置炸彈的按鍵
    sf::Sprite apperence;       // 外觀
    sf::Texture charTex;        // 外觀的圖片
    sf::Sprite endScreen;       // 玩家死亡時產生的結束畫面
    sf::Texture endTex;         // 結束畫面的圖片
public:
    Character(int a, int b, int lives, int playerOrder, sf::Texture charTex, sf::Texture endTex, sf::Keyboard::Key up,
        sf::Keyboard::Key down, sf::Keyboard::Key left, sf::Keyboard::Key right, sf::Keyboard::Key setBomb);
    sf::Sprite show();               // 在介面上畫的樣子
    sf::Sprite getEnd();             // 回傳結束畫面
    int getPosition_x();
    int getPosition_y();
    int getExplosionRange();
    int getLiv();
    void minusLiv(float livWntMin);  // 被炸到時扣血
    void moveUp();                   // 向上移，改變y值
    void moveDown();                 // 向下移，改變y值
    void moveLeft();                 // 向左移，改變x值
    void moveRight();                // 向右移，改變x值
    void setTexture();               // 傳入外觀的圖片
    void setEnd();                   // 傳入結束畫面的圖片
    void addMaxBomb(int addCnt);     // 增加同時間最大可放置炸彈數
    int getMaxBomb();
    int get_movePerTime();
    int get_moveDistance();
    sf::Keyboard::Key get_up();
    sf::Keyboard::Key get_down();
    sf::Keyboard::Key get_left();
    sf::Keyboard::Key get_right();
    sf::Keyboard::Key get_setBomb();
    int direction[4];                // 儲存玩家的移動狀態
    std::vector<Bomb> bombs;         // 玩家已放置的炸彈
    friend class Props;              // 道具會改變玩家數值，故設為friend
};

void Character::moveUp()
{
    y = y - moveDistance;
}

void Character::moveDown() {
    y = y + moveDistance;
}

void Character::moveLeft() {
    x = x - moveDistance;
}

void Character::moveRight() {
    x = x + moveDistance;
}

void Character::setTexture() {
    apperence.setTexture(charTex);
}

void Character::setEnd() {
    endScreen.setTexture(endTex);
}

Character::Character(int a, int b, int lives, int playerOrder, sf::Texture charTex, sf::Texture endTex, sf::Keyboard::Key up,
    sf::Keyboard::Key down, sf::Keyboard::Key left, sf::Keyboard::Key right, sf::Keyboard::Key setBomb)
    :up(up), down(down), left(left), right(right), setBomb(setBomb) {
    x = a;
    y = b;
    this->lives = lives;
    apperence.setTexture(charTex);
    apperence.setScale(sf::Vector2f(0.2f, 0.2f));
    explosionRange = INITIAL_BOMB_RANGE;
    this->charTex = charTex;
    // 初始玩家沒在移動，將四個方向的移動都設為0
    for (int i = 0; i < 4; i++)
        direction[i] = 0;
    maxBomb = INITIAL_BOMB_CNT;
    this->playerOrder = playerOrder;
    endScreen.setTexture(endTex);
    endScreen.setPosition(0, 0);
    this->endTex = endTex;
    movePerTime = MOVE_PER_TIME;
    moveDistance = MOVE_DISTANCE;
}

int Character::getPosition_x() {
    return x;
}

int Character::getPosition_y() {
    return y;
}

int Character::getExplosionRange() {
    return explosionRange;
}

int Character::getLiv() {
    return lives;
}

void Character::minusLiv(float livWntMin) {
    lives = lives - livWntMin;
}

void Character::addMaxBomb(int addCnt) {
    maxBomb += addCnt;
}

int Character::getMaxBomb() {
    return maxBomb;
}

int Character::get_movePerTime() {
    return movePerTime;
}

int Character::get_moveDistance() {
    return moveDistance;
}

sf::Sprite Character::show()
{
    apperence.setPosition(x, y);
    return apperence;
}

sf::Sprite Character::getEnd() {
    return endScreen;
}

sf::Keyboard::Key Character::get_up() {
    return up;
}
sf::Keyboard::Key Character::get_down() {
    return down;
}
sf::Keyboard::Key Character::get_left() {
    return left;
}
sf::Keyboard::Key Character::get_right() {
    return right;
}
sf::Keyboard::Key Character::get_setBomb() {
    return setBomb;
}

// 判斷玩家要移動過去的位置是否有炸彈，若有則會被阻擋
bool bombExist(int x, int y, std::vector<Character> players) {
    for (int player = 0; player < players.size(); player++) {
        for (int i = 0; i < players[player].bombs.size(); i++) {
            if (players[player].bombs[i].getPosition_x() == x && players[player].bombs[i].getPosition_y() == y)
                return true;
        }
    }
    return false;
}

// 道具
class Props
{
private:
    int x;
    int y;
    int type;  // 道具種類，1為爆炸半徑+1；2為最大炸彈數+1；3為爆炸半徑+2；4為移動速度變為初始值的2倍
    sf::Texture propTex;  // 道具的圖片
public:
    Props(int position_x, int position_y, sf::Texture propTex, int num);
    sf::Sprite props;
    sf::Sprite show();  // 在介面上顯示道具
    void getbyplayer(std::vector<Character>& players, int player);  // 到具被玩家碰到時，改變玩家數值

};

Props::Props(int position_x, int position_y, sf::Texture propTex, int num)
{
    x = position_x;
    y = position_y;
    type = num;
    this->propTex = propTex;
    props.setTexture(propTex);
    props.setPosition(position_x + 1, position_y + 1);
    props.setScale(sf::Vector2f(0.2f, 0.2f));
}

void Props::getbyplayer(std::vector<Character>& players, int player)
{
    if (type == 1)
    {
        players[player].explosionRange++;
    }
    else if (type == 2)
    {
        players[player].maxBomb++;
    }
    else if (type == 3)
    {
        players[player].explosionRange += 2;
    }
    else if (type == 4)
    {
        players[player].moveDistance = 10;
        players[player].movePerTime = 5;
    }
}

sf::Sprite Props::show()
{
    props.setTexture(propTex);
    return props;
}

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(750, 750), "map");
    window.setFramerateLimit(60);  // 將遊戲畫面的幀數訂為60幀

    // 儲存玩家的vector
    std::vector<Character> players;
    // 從電腦讀取玩家外觀的圖片
    sf::Texture* CharacterTex = new sf::Texture[PLAYER_CNT];
    CharacterTex[0].loadFromFile("textures\\player1.png");
    CharacterTex[1].loadFromFile("textures\\player2.png");
    // 從電腦讀取結束畫面的圖片
    sf::Texture* endTex = new sf::Texture[PLAYER_CNT];
    endTex[0].loadFromFile("textures\\win_player1.jpg");
    endTex[1].loadFromFile("textures\\win_player2.jpg");
    // 宣告2名玩家
    players.push_back(Character(650, 400, INITIAL_LIVE, 0, CharacterTex[0], endTex[0],
                      sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::Space));
    players.push_back(Character(50, 200, INITIAL_LIVE, 1, CharacterTex[1], endTex[1],
                      sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Enter));
    
    // 從電腦讀取炸彈的圖片
    sf::Texture* bombTex = new sf::Texture[5];
    bombTex[0].loadFromFile("textures\\bomb1.png");
    bombTex[1].loadFromFile("textures\\bomb2.png");
    bombTex[2].loadFromFile("textures\\bomb3.png");
    bombTex[3].loadFromFile("textures\\bomb4.png");
    bombTex[4].loadFromFile("textures\\bomb5.png");
    // 從電腦讀取火焰的圖片
    sf::Texture fireTex;
    fireTex.loadFromFile("textures\\fire.png");

    // 儲存介面上所有道具的vector
    std::vector<Props> props;
    // 從電腦讀取道具的圖片
    sf::Texture* PropsTex = new sf::Texture[Propscnt];
    PropsTex[0].loadFromFile("textures\\bombrangeplus.png");
    PropsTex[1].loadFromFile("textures\\bombplus1.png");
    PropsTex[2].loadFromFile("textures\\bombplus2.png");
    PropsTex[3].loadFromFile("textures\\speedplus.png");
    PropsTex[4].loadFromFile("textures\\none.png");

    // 宣告並儲存介面上所有格子
    Square squares[15][15];
    int situation = 0;
    for (int i = 1; i <= 15; i++)
    {
        for (int j = 1; j <= 15; j++)
        {
            squares[i - 1][j - 1].set_x(i);
            squares[i - 1][j - 1].set_y(j);
            //situation3
            if (((i == 1 || i == 2 || i == 3 || i == 13 || i == 14 || i == 15) &&
                (j == 1 || j == 2 || j == 3 || j == 13 || j == 14 || j == 15)) ||
                (i == 8 && j >= 4 && j <= 12) || (j == 8 && i >= 4 && i <= 12))
                squares[i - 1][j - 1].setSituation(3);
            //situation2
            else if (i >= 4 && i <= 12 && j >= 4 && j <= 12 && i != 8 && j != 8)
                squares[i - 1][j - 1].setSituation(2);
            else
                squares[i - 1][j - 1].setSituation(1);
        }
    }

    // 遊戲是否結束的bool，未結束則繼續執行
    bool gameEnd = false;

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 可以按視窗右上角的叉叉關閉視窗
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 若遊戲未結束則持續執行
        if (!gameEnd) {
            for (int player = 0; player < players.size(); player++) {
                // 判斷玩家是否有按按鍵移動或放置炸彈
                // 玩家只能往上下左右共四個方位移動而不可斜著走，故先判斷是否有正在移動的狀態
                if (players[player].direction[0] == 0 && players[player].direction[1] == 0 && players[player].direction[2] == 0 && players[player].direction[3] == 0) {
                    // 依序判斷玩家按的按鍵、移動是否會超出邊界、移動是否被障礙物阻擋、移動是否被炸彈阻擋，若接符合則可執行移動 (四個方位的判斷架構皆同)
                    if (sf::Keyboard::isKeyPressed(players[player].get_up()) && players[player].getPosition_y() > 0 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE][players[player].getPosition_y() / SQUARE_SIDE - 1].getSituation() != 3 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE][players[player].getPosition_y() / SQUARE_SIDE - 1].getSituation() != 2 &&
                        !bombExist(players[player].getPosition_x(), players[player].getPosition_y() - SQUARE_SIDE, players)) {
                        players[player].direction[0] = players[player].get_movePerTime();
                    }
                    else if (sf::Keyboard::isKeyPressed(players[player].get_down()) && players[player].getPosition_y() < (window.getSize().y - SQUARE_SIDE) &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE][players[player].getPosition_y() / SQUARE_SIDE + 1].getSituation() != 3 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE][players[player].getPosition_y() / SQUARE_SIDE + 1].getSituation() != 2 &&
                        !bombExist(players[player].getPosition_x(), players[player].getPosition_y() + SQUARE_SIDE, players)) {
                        players[player].direction[1] = players[player].get_movePerTime();
                    }
                    else if (sf::Keyboard::isKeyPressed(players[player].get_left()) && players[player].getPosition_x() > 0 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE - 1][players[player].getPosition_y() / SQUARE_SIDE].getSituation() != 3 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE - 1][players[player].getPosition_y() / SQUARE_SIDE].getSituation() != 2 &&
                        !bombExist(players[player].getPosition_x() - SQUARE_SIDE, players[player].getPosition_y(), players)) {
                        players[player].direction[2] = players[player].get_movePerTime();
                    }
                    else if (sf::Keyboard::isKeyPressed(players[player].get_right()) && players[player].getPosition_x() < (window.getSize().x - SQUARE_SIDE) &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE + 1][players[player].getPosition_y() / SQUARE_SIDE].getSituation() != 3 &&
                        squares[players[player].getPosition_x() / SQUARE_SIDE + 1][players[player].getPosition_y() / SQUARE_SIDE].getSituation() != 2 &&
                        !bombExist(players[player].getPosition_x() + SQUARE_SIDE, players[player].getPosition_y(), players)) {
                        players[player].direction[3] = players[player].get_movePerTime();
                    }
                    // 依序判斷玩家按的按鍵、是否超出最大炸彈放置數、該位置是否已有炸彈 (炸彈不可重疊)
                    else if (sf::Keyboard::isKeyPressed(players[player].get_setBomb()) && players[player].bombs.size() < players[player].getMaxBomb() &&
                        !bombExist(players[player].getPosition_x(), players[player].getPosition_y(), players)) {
                        // 宣告炸彈並存入vector中
                        Bomb bomb(players[player].getExplosionRange(), players[player].getPosition_x(), players[player].getPosition_y(), squares);
                        players[player].bombs.push_back(bomb);
                    }
                }

                // 判斷並更新玩家的移動狀態
                if (players[player].direction[0] > 0) {
                    players[player].moveUp();
                    players[player].direction[0]--;
                }
                else if (players[player].direction[1] > 0) {
                    players[player].moveDown();
                    players[player].direction[1]--;
                }
                else if (players[player].direction[2] > 0) {
                    players[player].moveLeft();
                    players[player].direction[2]--;
                }
                else if (players[player].direction[3] > 0) {
                    players[player].moveRight();
                    players[player].direction[3]--;
                }
            }

            for (int player = 0; player < players.size(); player++) {
                // 若有玩家的血量為0則從vector中移除
                // 移除後若剩餘玩家為1，則立刻結束遊戲
                if (players[player].getLiv() == 0) {
                    players.erase(players.begin() + player);
                    if (players.size() == 1) {
                        gameEnd = true;
                        break;
                    }
                    continue;
                }
                // 玩家所放置的所有炸彈狀態進行更新
                for (int i = 0; i < players[player].bombs.size(); i++) {
                    // 若炸彈回傳的bool值為true代表計時器歸0，此時移除炸彈
                    if (players[player].bombs[i].countdown(bombTex)) {
                        players[player].bombs.erase(players[player].bombs.begin() + i);
                        break;
                    }
                    // 若炸彈計時器 <=20 ，令其狀態為爆炸並產生火焰
                    if (players[player].bombs[i].getTimer() <= 20) {
                        for (std::size_t j = 0; j < players[player].bombs[i].fires.size(); j++) {
                            players[player].bombs[i].fires[j].fire.setTexture(fireTex);
                            for (int playerConcerned = 0; playerConcerned < players.size(); playerConcerned++)
                                // 若介面上有玩家的圖示與火焰圖示重疊 (即被炸彈炸到) ，則該玩家扣血，並更新該炸彈已炸過該玩家
                                if (players[player].bombs[i].fires[j].fire.getGlobalBounds().intersects(players[playerConcerned].show().getGlobalBounds())
                                    && players[player].bombs[i].getHit(playerConcerned) == false) {
                                    players[playerConcerned].minusLiv(1);
                                    players[player].bombs[i].setHit(playerConcerned);
                                    break;
                                }
                            // 若火焰出現的格子狀態為可被移除的障礙物，則該格子狀態變為走道
                            // 可被移除的障礙物被移除時，機率性掉落道具
                            if (squares[(int(players[player].bombs[i].fires[j].fire.getPosition().x) - 1) / SQUARE_SIDE]
                                [(int(players[player].bombs[i].fires[j].fire.getPosition().y) - 1) / SQUARE_SIDE].getSituation() == 2)
                            {
                                squares[(int(players[player].bombs[i].fires[j].fire.getPosition().x) - 1) / SQUARE_SIDE]
                                [(int(players[player].bombs[i].fires[j].fire.getPosition().y) - 1) / SQUARE_SIDE].setSituation(1);

                                int num;
                                srand(time(NULL));
                                int x = rand() * j;
                                x = x % 100 + 1;

                                // 計算是否掉落道具，若掉落，為哪種道具
                                if (x >= 1 && x <= 5)
                                    num = 4;
                                else if (x >= 6 && x <= 30)
                                    num = 1;
                                else if (x >= 31 && x <= 40)
                                    num = 3;
                                else if (x >= 41 && x <= 70)
                                    num = 2;
                                else
                                    num = 5;  // 不掉落道具

                                if (num != 5) {
                                    // 宣告道具並存入陣列
                                    Props prop((players[player].bombs[i].fires[j].fire.getPosition().x) - 1,
                                               (players[player].bombs[i].fires[j].fire.getPosition().y) - 1, PropsTex[num - 1], num);
                                    props.push_back(prop);
                                }
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < props.size(); i++)
            {
                for (int j = 0; j < players.size(); j++)
                {
                    // 判斷介面上道具的圖示是否與玩家圖示重疊 (即玩家吃到道具)
                    if (players[j].show().getGlobalBounds().intersects(props[i].show().getGlobalBounds()) &&
                        (players[j].getPosition_x() % 50) == 0 && (players[j].getPosition_y() % 50) == 0)
                    {
                        // 若道具被玩家吃到，則更新玩家的狀態並移除介面上的道具
                        props[i].getbyplayer(players, j);
                        props.erase(props.begin() + i);
                        break;
                    }
                }
            }

            // 清除介面上的所有內容
            window.clear();

            // 若遊戲結束則直接跳到結束畫面
            if (!gameEnd) {
                // 畫出背景的格子
                for (int i = 0; i < 15; i++)
                {
                    for (int j = 0; j < 15; j++)
                    {
                        window.draw(squares[i][j].buildSquare());
                    }
                }

                // 畫出玩家、炸彈、火焰
                for (int player = 0; player < players.size(); player++) {
                    players[player].setTexture();
                    window.draw(players[player].show());
                    for (std::size_t i = 0; i < players[player].bombs.size(); i++) {
                        window.draw(players[player].bombs[i].bomb);
                        if (players[player].bombs[i].getTimer() <= 20) {
                            for (std::size_t j = 0; j < players[player].bombs[i].fires.size(); j++) {
                                window.draw(players[player].bombs[i].fires[j].fire);
                            }
                        }
                    }
                }

                // 畫出道具
                for (int i = 0; i < props.size(); i++) {
                    window.draw(props[i].show());
                }
            }
            else
            {
                // 介面上顯示結束畫面
                players[0].setEnd();
                window.draw(players[0].getEnd());
            }

            // 顯示介面
            window.display();
        }
    }

    return 0;
}
