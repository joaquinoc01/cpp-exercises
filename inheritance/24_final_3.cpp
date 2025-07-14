#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

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

class Potion
{
public:
    enum Type
    {
        health,
        strength,
        poison,

        max_types
    };

    enum Size
    {
        small,
        medium,
        large,

        max_sizes
    };

private:
    Type m_type{};
    Size m_size{};

public:
    Potion(Type type, Size size)
        : m_type{ type }
        , m_size{ size }
    {
    }

    Type getType() const { return m_type; }
    Size getSize() const { return m_size; }

    static std::string_view getPotionSizeName(Size size)
    {
        static constexpr std::string_view names[] {
            "Small",
            "Medium",
            "Large"
        };
        return names[size];
    }

    static std::string_view getPotionTypeName(Type type)
    {
        static constexpr std::string_view types[] {
            "Health",
            "Strength",
            "Poison"
        };
        return types[type];
    }

    std::string getName() const
    {
        std::stringstream result{};

        result << getPotionSizeName(getSize()) << " potion of " << getPotionTypeName(getType());

        return result.str();
    }

    static Potion getRandomPotion()
    {
        return Potion{
            static_cast<Type>(Random::get(0, max_types - 1)),
            static_cast<Size>(Random::get(0, max_sizes - 1))
        };
    }
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

    void drinkPotion(const Potion& potion)
    {
        switch( potion.getType() )
        {
            case Potion::health:
                // Only health potions are dependent on size
                m_health += ((potion.getSize() == Potion::large) ? 5 : 2); // Large potions restore 5 health points
                break;
            case Potion::strength:
                ++m_damage;
                break;
            case Potion::poison:
                reduceHealth(1);
                break;
            case Potion::max_types: // To silence compiler warnings
                break;
            // We don't use default because we want the compiler to complain
            // if we add a new potion but forget to fully implement it
        }

        std::cout << "You drank a " << potion.getName() << '\n';
    }
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

void attackMonster(Player& p, Monster& m)
{
    // If player is dead it does not attack
    if( p.isDead() )
        return;

    std::cout << "You hit the " << m.getName() << " for " << p.getDamage() << " damage.\n";
    m.reduceHealth( p.getDamage() );
    if( m.isDead() )
    {
        std::cout << "You killed the " << m.getName() << '\n';
        p.levelUp();
        std::cout << "You are now level " << p.getLevel() << ".\n";
        p.addGold(m.getGold());
        std::cout << "You get " << m.getGold() << " gold.\n";
        if( Random::get(1, 10) <= 3 ) // 30% chance of finding a potion
        {
            Potion potion{ Potion::getRandomPotion() };
            std::cout << "You found a mythical potion! Do you want to drink it? [y/n]: ";
            char input{};
            std::cin >> input;
            if( input == 'y' )
                p.drinkPotion(potion);
            else if ( input == 'n' )
                std::cout << "You left the potion...\n";
        }
    }
}

void attackPlayer(Player& p, Monster& m)
{
    // If monster is dead it does not attack
    if( m.isDead() )
        return;

    std::cout << "The " << m.getName() << " hits you for " << m.getDamage() << " damage.\n";
    p.reduceHealth(m.getDamage());
}

void fightMonster(Player& p, Monster& m)
{
    char q{};

    while( !p.isDead() && !m.isDead() )
    {
        std::cout << "Your health is " << p.getHealth() << ".\n";
        std::cout << "(R)un or (F)ight: ";
        std::cin >> q;

        if( q == 'f' || q == 'F' )
        {
            attackMonster(p, m);
            attackPlayer(p, m);
        }
        else if ( q == 'r' || q == 'R' )
        {
            if(Random::get(1, 2) == 1) // Successfully flee
            {
                std::cout << "You successfully fled.\n";
                return;
            }
            else
            {
                std::cout << "You failed to flee.\n";
                attackPlayer(p, m);
                continue;
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
    std::cout << "Welcome, " << p.getName() << '\n';

    while(!p.isDead() && !p.hasWon())
    {
        Monster m{ Monster::getRandomMonster() };
        std::cout << "You have encountered a(n) " << m.getName() << " (" << m.getSymbol() << ").\n";

        fightMonster(p, m);
    }

    if( p.isDead() )
    {
        std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
        std::cout << "Too bad you can't take it with you!\n";
    }
    else
    {
        std::cout << "You won the game with " << p.getGold() << " gold!\n";
    }


	return 0;
}