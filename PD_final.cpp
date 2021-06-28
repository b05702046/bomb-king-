#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
const int PLAYER_CNT = 2;  // ���a�ƶq
// a character moves MOVE_DISTANCE * MOVE_PER_TIME in distance per move
const int MOVE_DISTANCE = 5;      // ��l�@�V���첾���Z��
const int MOVE_PER_TIME = 10;     // ��l���@������@�첾�X�V
const int SQUARES_PER_SIDE = 15;  // �a�ϨC�䪺��l��
const int SQUARE_SIDE = 50;       // �I������l�����
const int INITIAL_BOMB_CNT = 1;   // ���a����l�u�i��m���u�ơv
const int INITIAL_LIVE = 1;       // ���a����l��q
const int INITIAL_BOMB_RANGE = 1; // ���a����l���u�z���b�|
const int Propscnt = 5;           // �D�������

// �C���I������l
class Square
{
private:

    int x;          // ��l��ܦb�����W��x��
    int y;          // ��l��ܦb�����W��y��
    int situation;  // ��l�����A�A1�����D�F2���i�Q�}�a����l�F3�����i�Q�}�a����l

public:
    sf::VertexArray buildSquare();  // �b�����W��ܮ�l
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

// ���u�z���Უ�ͪ����K
class Fire {
private:
    int x;  // ���K�b�����W��x��
    int y;  // ���K�b�����W��y��
public:
    Fire(int x, int y);
    sf::Sprite fire;
};

Fire::Fire(int x, int y) {
    fire.setScale(sf::Vector2f(0.1f, 0.1f));
    fire.setPosition(x, y);  // ���ͤ��K�ɧY���w(x, y)��
    this->x = x;
    this->y = y;
}

// ���a�i��m�����u
class Bomb {
private:
    int x;               // ���u�b�����W��x��
    int y;               // ���u�b�����W��y��
    int timer;           // ���u���p�ɾ��A�ھڳѾl�ɶ��|�����P���A
    int explosionRange;  // ���u�z�����b�|
    bool* hit = new bool[PLAYER_CNT] {0};  // �����@�����u�w����Ǫ��a�y���ˮ` (�קK���a���ƳQ�P���u����)
public:
    std::vector<Fire> fires;  // �x�s���u���ͪ����K
    Bomb(int explosionRange, int x, int y, Square squares[][SQUARES_PER_SIDE]);
    bool countdown(sf::Texture bombTex[]);  // �ھڭp�ɾ����ͤ��P���A�A�æ^�ǬO�_�z��
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

    // �ŧi�@�}�C�A�x�s���u���W�U���k�@�|�Ӥ��W�O�_�����i�}�a����ê��
    bool* block = new bool[4];
    for (int i = 0; i < 4; i++)
        block[i] = false;
    Fire fire(bomb.getPosition().x + 1, bomb.getPosition().y + 1);  // ���bbomb��������m�W�ͦ��Ĥ@�Ӥ��K
    fires.push_back(fire);
    // �b���u���|�Ӥ��A�ھ��z���b�|�v�h�V�~�ݮi�A���ͤ��K
    // �@�J�줣�i�}�a����ê���N���|�A�¸Ӥ�V���ͤ��K
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

// ���u���p��
bool Bomb::countdown(sf::Texture bombTex[]) {
    // timer�b�C�@�V�| -1
    // �ھڳѾl�ɶ����ܥ~�[�A�íq <=20 ���z��
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

// ���a
class Character
{
private:
    int x;                      // ���a��ܦb�����W��y��
    int y;                      // ���a��ܦb�����W��y��
    int playerOrder;            // ���a�s��
    int explosionRange;         // ��m�����u���z���b�|
    int maxBomb;                // �P�ɶ��̤j�i��m���u��
    int lives;                  // ���a��q
    int movePerTime;            // ���@������@�첾�X�V
    int moveDistance;           // �@�V���첾���Z��
    sf::Keyboard::Key up;       // ���a�V�W��������
    sf::Keyboard::Key down;     // ���a�V�U��������
    sf::Keyboard::Key left;     // ���a�V����������
    sf::Keyboard::Key right;    // ���a�V�k��������
    sf::Keyboard::Key setBomb;  // ���a��m���u������
    sf::Sprite apperence;       // �~�[
    sf::Texture charTex;        // �~�[���Ϥ�
    sf::Sprite endScreen;       // ���a���`�ɲ��ͪ������e��
    sf::Texture endTex;         // �����e�����Ϥ�
public:
    Character(int a, int b, int lives, int playerOrder, sf::Texture charTex, sf::Texture endTex, sf::Keyboard::Key up,
        sf::Keyboard::Key down, sf::Keyboard::Key left, sf::Keyboard::Key right, sf::Keyboard::Key setBomb);
    sf::Sprite show();               // �b�����W�e���ˤl
    sf::Sprite getEnd();             // �^�ǵ����e��
    int getPosition_x();
    int getPosition_y();
    int getExplosionRange();
    int getLiv();
    void minusLiv(float livWntMin);  // �Q����ɦ���
    void moveUp();                   // �V�W���A����y��
    void moveDown();                 // �V�U���A����y��
    void moveLeft();                 // �V�����A����x��
    void moveRight();                // �V�k���A����x��
    void setTexture();               // �ǤJ�~�[���Ϥ�
    void setEnd();                   // �ǤJ�����e�����Ϥ�
    void addMaxBomb(int addCnt);     // �W�[�P�ɶ��̤j�i��m���u��
    int getMaxBomb();
    int get_movePerTime();
    int get_moveDistance();
    sf::Keyboard::Key get_up();
    sf::Keyboard::Key get_down();
    sf::Keyboard::Key get_left();
    sf::Keyboard::Key get_right();
    sf::Keyboard::Key get_setBomb();
    int direction[4];                // �x�s���a�����ʪ��A
    std::vector<Bomb> bombs;         // ���a�w��m�����u
    friend class Props;              // �D��|���ܪ��a�ƭȡA�G�]��friend
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
    // ��l���a�S�b���ʡA�N�|�Ӥ�V�����ʳ��]��0
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

// �P�_���a�n���ʹL�h����m�O�_�����u�A�Y���h�|�Q����
bool bombExist(int x, int y, std::vector<Character> players) {
    for (int player = 0; player < players.size(); player++) {
        for (int i = 0; i < players[player].bombs.size(); i++) {
            if (players[player].bombs[i].getPosition_x() == x && players[player].bombs[i].getPosition_y() == y)
                return true;
        }
    }
    return false;
}

// �D��
class Props
{
private:
    int x;
    int y;
    int type;  // �D������A1���z���b�|+1�F2���̤j���u��+1�F3���z���b�|+2�F4�����ʳt���ܬ���l�Ȫ�2��
    sf::Texture propTex;  // �D�㪺�Ϥ�
public:
    Props(int position_x, int position_y, sf::Texture propTex, int num);
    sf::Sprite props;
    sf::Sprite show();  // �b�����W��ܹD��
    void getbyplayer(std::vector<Character>& players, int player);  // ���Q���a�I��ɡA���ܪ��a�ƭ�

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
    window.setFramerateLimit(60);  // �N�C���e�����V�ƭq��60�V

    // �x�s���a��vector
    std::vector<Character> players;
    // �q�q��Ū�����a�~�[���Ϥ�
    sf::Texture* CharacterTex = new sf::Texture[PLAYER_CNT];
    CharacterTex[0].loadFromFile("textures\\player1.png");
    CharacterTex[1].loadFromFile("textures\\player2.png");
    // �q�q��Ū�������e�����Ϥ�
    sf::Texture* endTex = new sf::Texture[PLAYER_CNT];
    endTex[0].loadFromFile("textures\\win_player1.jpg");
    endTex[1].loadFromFile("textures\\win_player2.jpg");
    // �ŧi2�W���a
    players.push_back(Character(650, 400, INITIAL_LIVE, 0, CharacterTex[0], endTex[0],
                      sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::Space));
    players.push_back(Character(50, 200, INITIAL_LIVE, 1, CharacterTex[1], endTex[1],
                      sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Enter));
    
    // �q�q��Ū�����u���Ϥ�
    sf::Texture* bombTex = new sf::Texture[5];
    bombTex[0].loadFromFile("textures\\bomb1.png");
    bombTex[1].loadFromFile("textures\\bomb2.png");
    bombTex[2].loadFromFile("textures\\bomb3.png");
    bombTex[3].loadFromFile("textures\\bomb4.png");
    bombTex[4].loadFromFile("textures\\bomb5.png");
    // �q�q��Ū�����K���Ϥ�
    sf::Texture fireTex;
    fireTex.loadFromFile("textures\\fire.png");

    // �x�s�����W�Ҧ��D�㪺vector
    std::vector<Props> props;
    // �q�q��Ū���D�㪺�Ϥ�
    sf::Texture* PropsTex = new sf::Texture[Propscnt];
    PropsTex[0].loadFromFile("textures\\bombrangeplus.png");
    PropsTex[1].loadFromFile("textures\\bombplus1.png");
    PropsTex[2].loadFromFile("textures\\bombplus2.png");
    PropsTex[3].loadFromFile("textures\\speedplus.png");
    PropsTex[4].loadFromFile("textures\\none.png");

    // �ŧi���x�s�����W�Ҧ���l
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

    // �C���O�_������bool�A�������h�~�����
    bool gameEnd = false;

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // �i�H�������k�W�����e�e��������
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // �Y�C���������h�������
        if (!gameEnd) {
            for (int player = 0; player < players.size(); player++) {
                // �P�_���a�O�_�������䲾�ʩΩ�m���u
                // ���a�u�੹�W�U���k�@�|�Ӥ�첾�ʦӤ��i�׵ۨ��A�G���P�_�O�_�����b���ʪ����A
                if (players[player].direction[0] == 0 && players[player].direction[1] == 0 && players[player].direction[2] == 0 && players[player].direction[3] == 0) {
                    // �̧ǧP�_���a��������B���ʬO�_�|�W�X��ɡB���ʬO�_�Q��ê�����סB���ʬO�_�Q���u���סA�Y���ŦX�h�i���沾�� (�|�Ӥ�쪺�P�_�[�c�ҦP)
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
                    // �̧ǧP�_���a��������B�O�_�W�X�̤j���u��m�ơB�Ӧ�m�O�_�w�����u (���u���i���|)
                    else if (sf::Keyboard::isKeyPressed(players[player].get_setBomb()) && players[player].bombs.size() < players[player].getMaxBomb() &&
                        !bombExist(players[player].getPosition_x(), players[player].getPosition_y(), players)) {
                        // �ŧi���u�æs�Jvector��
                        Bomb bomb(players[player].getExplosionRange(), players[player].getPosition_x(), players[player].getPosition_y(), squares);
                        players[player].bombs.push_back(bomb);
                    }
                }

                // �P�_�ç�s���a�����ʪ��A
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
                // �Y�����a����q��0�h�qvector������
                // ������Y�Ѿl���a��1�A�h�ߨ赲���C��
                if (players[player].getLiv() == 0) {
                    players.erase(players.begin() + player);
                    if (players.size() == 1) {
                        gameEnd = true;
                        break;
                    }
                    continue;
                }
                // ���a�ҩ�m���Ҧ����u���A�i���s
                for (int i = 0; i < players[player].bombs.size(); i++) {
                    // �Y���u�^�Ǫ�bool�Ȭ�true�N��p�ɾ��k0�A���ɲ������u
                    if (players[player].bombs[i].countdown(bombTex)) {
                        players[player].bombs.erase(players[player].bombs.begin() + i);
                        break;
                    }
                    // �Y���u�p�ɾ� <=20 �A�O�䪬�A���z���ò��ͤ��K
                    if (players[player].bombs[i].getTimer() <= 20) {
                        for (std::size_t j = 0; j < players[player].bombs[i].fires.size(); j++) {
                            players[player].bombs[i].fires[j].fire.setTexture(fireTex);
                            for (int playerConcerned = 0; playerConcerned < players.size(); playerConcerned++)
                                // �Y�����W�����a���ϥܻP���K�ϥܭ��| (�Y�Q���u����) �A�h�Ӫ��a����A�ç�s�Ӭ��u�w���L�Ӫ��a
                                if (players[player].bombs[i].fires[j].fire.getGlobalBounds().intersects(players[playerConcerned].show().getGlobalBounds())
                                    && players[player].bombs[i].getHit(playerConcerned) == false) {
                                    players[playerConcerned].minusLiv(1);
                                    players[player].bombs[i].setHit(playerConcerned);
                                    break;
                                }
                            // �Y���K�X�{����l���A���i�Q��������ê���A�h�Ӯ�l���A�ܬ����D
                            // �i�Q��������ê���Q�����ɡA���v�ʱ����D��
                            if (squares[(int(players[player].bombs[i].fires[j].fire.getPosition().x) - 1) / SQUARE_SIDE]
                                [(int(players[player].bombs[i].fires[j].fire.getPosition().y) - 1) / SQUARE_SIDE].getSituation() == 2)
                            {
                                squares[(int(players[player].bombs[i].fires[j].fire.getPosition().x) - 1) / SQUARE_SIDE]
                                [(int(players[player].bombs[i].fires[j].fire.getPosition().y) - 1) / SQUARE_SIDE].setSituation(1);

                                int num;
                                srand(time(NULL));
                                int x = rand() * j;
                                x = x % 100 + 1;

                                // �p��O�_�����D��A�Y�����A�����عD��
                                if (x >= 1 && x <= 5)
                                    num = 4;
                                else if (x >= 6 && x <= 30)
                                    num = 1;
                                else if (x >= 31 && x <= 40)
                                    num = 3;
                                else if (x >= 41 && x <= 70)
                                    num = 2;
                                else
                                    num = 5;  // �������D��

                                if (num != 5) {
                                    // �ŧi�D��æs�J�}�C
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
                    // �P�_�����W�D�㪺�ϥܬO�_�P���a�ϥܭ��| (�Y���a�Y��D��)
                    if (players[j].show().getGlobalBounds().intersects(props[i].show().getGlobalBounds()) &&
                        (players[j].getPosition_x() % 50) == 0 && (players[j].getPosition_y() % 50) == 0)
                    {
                        // �Y�D��Q���a�Y��A�h��s���a�����A�ò��������W���D��
                        props[i].getbyplayer(players, j);
                        props.erase(props.begin() + i);
                        break;
                    }
                }
            }

            // �M�������W���Ҧ����e
            window.clear();

            // �Y�C�������h�������쵲���e��
            if (!gameEnd) {
                // �e�X�I������l
                for (int i = 0; i < 15; i++)
                {
                    for (int j = 0; j < 15; j++)
                    {
                        window.draw(squares[i][j].buildSquare());
                    }
                }

                // �e�X���a�B���u�B���K
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

                // �e�X�D��
                for (int i = 0; i < props.size(); i++) {
                    window.draw(props[i].show());
                }
            }
            else
            {
                // �����W��ܵ����e��
                players[0].setEnd();
                window.draw(players[0].getEnd());
            }

            // ��ܤ���
            window.display();
        }
    }

    return 0;
}
