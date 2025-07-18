#include <iostream>
#include <vector>

class Shape
{
public:
    virtual std::ostream& print(std::ostream& out) const = 0;
    
    friend std::ostream& operator<<(std::ostream& out, const Shape& p)
    {
        return p.print(out);
    }

    virtual ~Shape() = default;
};

class Point
{
private:
	int m_x{};
	int m_y{};

public:
	Point(int x, int y)
		: m_x{ x }, m_y{ y }
	{

	}

	friend std::ostream& operator<<(std::ostream& out, const Point& p)
	{
		return out << "Point(" << p.m_x << ", " << p.m_y << ')';
	}
};

class Triangle : public Shape
{
private:
    Point m_p1;
    Point m_p2;
    Point m_p3;

public:
    Triangle(const Point& p1, const Point& p2, const Point& p3)
        : m_p1{ p1 }, m_p2{ p2 }, m_p3{ p3 }
    {
    }

    std::ostream& print(std::ostream& out) const override
    {
        return out << "Triangle(" << m_p1 << ", " << m_p2 << ", " << m_p3 << ')';
    }
};

class Circle : public Shape
{
private:
    Point m_p;
    int m_radius;

public:
    Circle(const Point& p, int radius)
        : m_p{ p }, m_radius{ radius }
    {
    }

    std::ostream& print(std::ostream& out) const override
    {
        return out << "Circle(" << m_p << ", radius " << m_radius << ')';
    }

    int getRadius() const { return m_radius; }
};

int getLargestRadius(const std::vector<Shape*>& v)
{
    int largestRadius{ 0 };

    // Loop through all the shapes in the vector
    for (const auto* element : v)
    {
        // Ensure it's a circle by checking the upcast
        if (auto* c { dynamic_cast<const Circle*>(element)})
        {
            largestRadius = std::max(largestRadius, c->getRadius());
        }
    }
    return largestRadius;
}

int main()
{
	std::vector<Shape*> v{
	  new Circle{Point{ 1, 2 }, 7},
	  new Triangle{Point{ 1, 2 }, Point{ 3, 4 }, Point{ 5, 6 }},
	  new Circle{Point{ 7, 8 }, 3}
	};

	// print each shape in vector v on its own line here
    for (const auto* element : v)
    {
        std::cout << *element << '\n'; // element will be a *shape
    }

	std::cout << "The largest radius is: " << getLargestRadius(v) << '\n';

	for (const auto* element : v)
        delete element;

	return 0;
}