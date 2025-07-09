#include <iostream>
#include <string>
#include <string_view>

#include "../Random.h"

class Creature
{
protected:
    std::string m_name;
    char m_symbol {};
    int m_health {};
    int m_damage {};
    int m_gold {};

public:
    Creature(std::string_view name, char symbol, int health, int damage, int gold)
        : m_name { name }
        , m_symbol { symbol }
        , m_health { health }
        , m_damage { damage }
        , m_gold { gold }
    {
    }

    const std::string& getName() const { return m_name; }
    char getSymbol() const { return m_symbol; }
    int getHealth() const { return m_health; }
    int getDamage() const { return m_damage; }
    int getGold() const { return m_gold; }

    void reduceHealth(int health) { m_health -= health; }
    bool isDead() const { return m_health <= 0; }
    void addGold(int gold) { m_gold += gold; }

};

class Player : public Creature
{
    int m_level {1};

public:
    Player(std::string_view name)
        : Creature {name, '@', 10, 1, 0}
    {
    }

    void levelUp() {
        ++m_level;
        ++m_damage;
    }

    int getLevel() const { return m_level; }
    bool hasWon() { return m_level >= 20; }
};

class Monster : public Creature
{
public:
    enum Type
    {
        dragon,
        orc,
        slime,
        max_types
    };

private:
    static inline Creature monsterData[] { 
        Creature {"dragon", 'd', 20, 4, 100},
        Creature {"orc", 'o', 4, 2, 25},
        Creature {"slime", 's', 1, 1, 10}
        };

    static_assert(std::size(monsterData) == max_types);

public:
    Monster(Type type)
        : Creature { monsterData[type] }
    {
    }

    static Monster getRandomMonster() 
    {
        int num { Random::get(0, max_types - 1) };
        return Monster { static_cast<Type>(num) };
    }
};

void fightMonster(Player& p, Monster& m)
{
    char q;

    while( true )
    {
        std::cout << "(R)un or (F)ight: ";
        std::cin >> q;

        if( q == 'f' || q == 'F' )
        {
            m.reduceHealth( p.getDamage() );
            if( m.isDead() )
            {
                std::cout << "You killed the " << m.getName() << std::endl;
                p.levelUp();
                std::cout << "You are now level " << p.getLevel() << '.' << std::endl;
                return;
            }
        }
        else if ( q == 'r' || q == 'R' )
        {
            int num { Random::get(1, 100) };
            if(num > 50) // Successfully flee
            {
                std::cout << "You successfully fled." << std::endl;
                return;
            }
            else
            {
                std::cout << "The " << m.getName() << " hit you for " << m.getDamage() << " damage." << std::endl;
                p.reduceHealth(m.getDamage());
                if( p.isDead() )
            }
        }
    }
}

int main()
{
	std::cout << "Enter your name: ";
    std::string name;
    std::cin >> name;

    Player p { name };
    std::cout << "Welcome, " << p.getName() << std::endl;

    while(!p.isDead() && !p.hasWon())
    {
        Monster m{ Monster::getRandomMonster() };
        std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ").";

        fightMonster(p, m);
    }

	return 0;
}