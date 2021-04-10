#include <SFML/Graphics.hpp>
#include <cmath>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 1000;

std::vector<sf::Vector2f> getCircleSegmentPoints(float angle, float percentage, float rad)
{
    std::vector<sf::Vector2f> points;
    int maxPoints = (percentage < 1) ? 2 : percentage;
    float spread = percentage * M_PI * 3.6 / 180; // percentage to radians
    float radius = rad;

    for (int i = 0; i < maxPoints; i++)
    {
        float a = (angle - spread) + (i * spread) / (maxPoints - 1);
        points.push_back(radius * sf::Vector2f(std::cos(a), std::sin(a)));
    }
    return points;
}

sf::VertexArray createCircleSegment(float angle, float percantage, sf::Color color, float radius)
{
    const sf::Vector2f mid = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT) / 2.f;
    sf::VertexArray arr(sf::TriangleFan);

    arr.append(sf::Vertex(mid));
    for (sf::Vector2f v : getCircleSegmentPoints(angle, percantage, radius))
        arr.append(sf::Vertex(mid + v));
    arr.append(sf::Vertex(mid));

    for (int i = 0; i <= arr.getVertexCount() - 1; i++)
        arr[i].color = color;

    return arr;
}

std::vector<sf::VertexArray> createPiechart(std::vector<int> grades, int selected)
{
    std::vector<sf::Color> colors{
        sf::Color::Red,
        sf::Color::Magenta,
        sf::Color::Yellow,
        sf::Color::Green
    };

    std::vector<sf::VertexArray> piechart;

    int sum{}, i{};
    float perSum{}, radius = 200.f;
    for (auto &grade : grades)
    {
        if (grade < 0)
            grade = 0;
        sum += grade;
    }

    for (i = 0; i < grades.size() - 1; i++)
    {
        float per = grades[i] * 100 / sum;
        if (i == selected)
            radius = 250.f;
        else 
            radius = 200.f;
        sf::VertexArray shape = createCircleSegment(perSum, per, colors[i % colors.size()], radius);
        perSum -= per * M_PI * 3.6 / 180;
        piechart.push_back(shape);
    }
    if (i == selected)
        radius = 250.f;
    else 
        radius = 200.f;
    sf::VertexArray shape = createCircleSegment(perSum, 100 + perSum / M_PI / 3.6 * 180, colors[i % colors.size()], radius);
    piechart.push_back(shape);

    return piechart;
}

void render(sf::RenderWindow &win, std::vector<sf::VertexArray> &complexShape)
{
    win.clear();
    for (auto &shape : complexShape)
        win.draw(shape);
    win.display();
}

std::vector<int> createGradeStats(std::vector<int> grades)
{
    std::vector<int> arr{
        0, 0, 0, 0
    };

    for (auto gr : grades)
    {
        if (gr < 2 || gr > 5)
            throw "Error";
        arr[gr - 2]++;
    }

    return arr;
}

void manipulateGrades(std::vector<int>& grades, int change, int selectorPos)
{
    grades[selectorPos] += change;
    if (grades[selectorPos] < 0)
        grades[selectorPos] = 0;
}

int main()
{
    sf::RenderWindow win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Window");
    win.setFramerateLimit(60);
    int selector = 0;
    std::vector<int> grad{
        2, 3, 4, 5
    };
    std::vector<int> grades = createGradeStats(grad);

    std::vector<sf::VertexArray> piechart = createPiechart(grades, selector);

    render(win, piechart);

    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                    win.close();
            
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Down:
                        manipulateGrades(grades, -1, selector);
                        break;

                    case sf::Keyboard::Up:
                        manipulateGrades(grades, 1, selector);
                        break;

                    case sf::Keyboard::Left:
                        selector++;
                        if (selector > 3)
                            selector -= 4;
                        break;

                    case sf::Keyboard::Right:
                        selector--;
                        if (selector < 0)
                            selector += 4;
                        break;
                    
                    case sf::Keyboard::Escape:
                        win.close();
                        break;

                    default:
                        break;
                }
                piechart = createPiechart(grades, selector);
            }
        }
        render(win, piechart);
    }
    return 0;
}