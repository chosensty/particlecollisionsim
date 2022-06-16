// initialising all the libraries of the program.
#include <SFML/Graphics.hpp>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cmath>
#include <random>
// initialising global constants of the program.
const int circle_amount = 80;
int circle_radius = 5; 
const int window_height = 800;
const int window_width = 800;
const int velocity_constant = 30;
const int fps_constant = 144;
int total_collisions = 0;
struct Circle
{
    sf::CircleShape circle;
    int radius;
    float x;
    float y;
    float dx;
    float dy;
    int mass;
    int collisions;
};
Circle circles[circle_amount];

void sort_circles()
{
    //quick sort/ insertion sort algorithm
    Circle insert_val;
    int free_pos;
    for (int i = 1; i < circle_amount; i++)
    {
        insert_val = circles[i];
        free_pos = i;

        while (free_pos > 0 && circles[free_pos - 1].x > insert_val.x)
        {
            circles[free_pos] = circles[free_pos - 1];
            free_pos -= 1;
        }
        circles[free_pos] = insert_val;
    }
}


float magnitude_squared(float vector[2])
{
    float magnitude = pow(vector[0], 2) + pow(vector[1], 2);
    return magnitude;
}


void calculate_velocities(Circle &c1, Circle &c2, float t)
{
    
    // equation (taken from wikipedia) for 2d elastic collision of particles.
    float mp1[2] = {c1.x + c1.radius, c1.y + c1.radius};
    float mp2[2] = {c2.x + c2.radius, c2.y + c2.radius};

    float n[2] = {mp2[0] - mp1[0], mp2[1] - mp1[1]};

    float magn_sqrd = magnitude_squared(n);

    float diff_v1[2] = {c1.dx - c2.dx, c1.dy - c2.dy};
    float diff_mp1[2] = {mp1[0] - mp2[0], mp1[1] - mp2[1]}; 
    
    float diff_v2[2] = {c2.dx - c1.dx, c2.dy - c1.dy};
    float diff_mp2[2] = {mp2[0] - mp1[0], mp2[1] - mp1[1]}; 


    float dot_prod1 = diff_v1[0] * diff_mp1[0] + diff_v1[1] * diff_mp1[1];
    float dot_prod2 = diff_v2[0] * diff_mp2[0] + diff_v2[1] * diff_mp2[1];

    c1.dx -= (((2 * c2.mass) * dot_prod1 * diff_mp1[0] )/((c1.mass + c2.mass) * magn_sqrd));
    c1.dy -= (((2 * c2.mass) * dot_prod1 * diff_mp1[1] )  / ((c1.mass + c2.mass) * magn_sqrd));
    
    
    c2.dx -= (((2 * c1.mass) * dot_prod2 * diff_mp2[0] )/((c1.mass + c2.mass) * magn_sqrd));
    c2.dy -= (((2 * c1.mass) * dot_prod2 * diff_mp2[1] )  / ((c1.mass + c2.mass) * magn_sqrd));

    c1.x += t * c1.dx;
    c1.y += t * c1.dy;
    c2.x += t * c2.dx;
    c2.y += t * c2.dy;

}
float quadratic_formula_negative(float a, float b, float c)
{
    // function which always returns the negative solution to the quadratic formula.
    float result = (-b+sqrt(b*b-4*a*c))/(2*a);
    return result;
}


float time_of_collision(Circle &c1, Circle &c2)
{
    // using quadratic formula, the time of collisions is calculated.
    float mp1x = c1.x + c1.radius;
    float mp1y = c1.y + c1.radius;
    float mp2x = c2.x + c2.radius;
    float mp2y = c2.y + c2.radius;    
    float s_x = mp2x - mp1x;
    float s_y = mp2y - mp1y;
    float v_x = c2.dx - c1.dx;
    float v_y  = c2.dy - c1.dy;    
    float a = v_x * v_x + v_y * v_y;
    float b = -2 * (s_x * v_x + s_y * v_y);
    float c = s_x * s_x + s_y * s_y - pow((c1.radius + c2.radius), 2);
    float t = quadratic_formula_negative(a, b, c);    
//putting the particles back in time using the time of collision value.
    c1.x -= t * c1.dx;
    c1.y -= t * c1.dy;
    c2.x -= t * c2.dx;
    c2.y -= t * c2.dy;
 
    return t;
}


void collision_handling(float dt)
{
    // sort all the particles. (using quick sort)
    sort_circles();
    int i = 0;
    int y = 1;
    // checking for collisions using the sweep and prune algorithm, this algorithm sorts by the x-axis
    // and then checks for possible collisions, if collision isn't possible then it moves on to the next particle
    // if it is possible, it checks if collision is actually happening using circle theorom.
    while (i < circle_amount - y)
    {
        if ((circles[i].x + circles[i].radius * 2) > circles[i + y].x)
        {
            float distance = sqrt(pow(circles[i].x + circles[i].radius - (circles[i + y].x + circles[i + y].radius), 2) + pow(circles[i + y].y + circles[i + y].radius - (circles[i].y + circles[i].radius), 2));
            if (distance < circles[i].radius + circles[i + y].radius)
            {
                // registering the collisions in both of the circle structs
                circles[i].collisions += 1;
                circles[i + y].collisions += 1;
                // Time skip technique, uses algorithm to check how long ago the collision actually occured,
                // then it puts time back to that point in time, calculations are made, than time is put back forward.
                float t = time_of_collision(circles[i], circles[i + y]);
                calculate_velocities(circles[i], circles[i + y], t);
                i++;
                y = 1;
            }
            else{
                y++;
            }
        }
        else{
            i++;
            y = 1;
        }     
    }
}

void move_particles(float dt)
{
    int max_x, max_y;
    for (int i = 0; i < circle_amount; i++)
    {
        max_x = window_width - circles[i].radius * 2;
        max_y = window_height - circles[i].radius * 2;
        // Adjusing x-coordinate, if it is not within the area it is adjusted (deflects off wall)
        circles[i].x += (circles[i].dx * dt);
        if (circles[i].x <= 0)
        {
            circles[i].x *= -1.0f;
            circles[i].dx *= -1.0f;
        }
        else if (circles[i].x >= max_x)
        {
            circles[i].x = 2 * max_x - circles[i].x;
            circles[i].dx *= -1.0f;
        }
        // adjusting y coordinate
        circles[i].y += circles[i].dy * dt;
        // checking if it is still within the area, if not, it deflects off wall.
        if (circles[i].y <= 0)
        {
            circles[i].y *= -1.0f;
            circles[i].dy *= -1.0f;
        }
        else if (circles[i].y >= max_y)
        {
            circles[i].y = 2 * max_y - circles[i].y;
            circles[i].dy *= -1.0f;
        }
        // Putting the particle at it's next position.
        circles[i].circle.setPosition(sf::Vector2f(circles[i].x, circles[i].y));
    }
}

void display_particles(sf::RenderWindow &window)
{
    // function that draws every particle onto the window.
    for (int i = 0; i < circle_amount; i++)
    {
        window.draw(circles[i].circle);
    }
}

int collision_count()
{
    // function that calculates and returns the amount of collisions that have occured.
    int count = 0;
    for (int i = 0;i < circle_amount;i++)
    {
        count += circles[i].collisions;
    }
    count /= 2;
    std::cout << "Collisions in the last second: " << count - total_collisions << "\n";
    total_collisions = count;
    return count;
}
float magnitude(float x, float y)
{
    float result = sqrt(x*x + y*y);
    return result;
}
void draw_text(sf::Text &tke_text, sf::Text &ake_text, sf::Text &sd_text)
{   
    // calculating the total kinetic energy

    float sum_squared;
    float ke_sum = 0;
    for (int i = 0;i<circle_amount;i++)
    {
        // ke = v^2
        sum_squared = circles[i].dx * circles[i].dx + circles[i].dy * circles[i].dy;
        ke_sum += sum_squared;
    }
    // average kinetic energy.
    float ake = ke_sum / circle_amount;
    /// calculating standard deviation
    float sd = 0;
    // difference, sum squared
    float dss = 0;
    for (int i = 0;i<circle_amount;i++)
    {
        dss += pow(magnitude(circles[i].dx, circles[i].dy) - ake, 2);
    }
    sd = sqrt(dss/(circle_amount-1));
    sd = sd * 1/2 * circles[0].mass;
    // displaying average and total kinetic energy onto the screen.
    // ke = 1/2 * m * v^2
    ke_sum = ke_sum * 1/2 * circles[0].mass;
    std::string tke_string = "TKE: " + std::to_string((int)ke_sum);
    tke_text.setString(tke_string);
    std::string ake_string = "AKE: " + std::to_string((int)ake);
    ake_text.setString(ake_string);
    std::string sd_string = "SD: " + std::to_string(sd);
    sd_text.setString(sd_string);
    

}

int main()
{

    //initialising all the variables.
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Entropy simulator");
    srand(time(NULL));
    sf::CircleShape default_circle(circle_radius);
    default_circle.setFillColor(sf::Color(150, 50, 250));
    // initialising font
    sf::Font font;
    if(!font.loadFromFile("src/lib/fonts/KdamThmorPro-Regular.ttf"))
        throw("Could not load font!");

    
    //setting fps limit
    window.setFramerateLimit(fps_constant);

    int frame = 0;



    //initialising text variables.
    sf::Text tke_text;
    tke_text.setFont(font);
    tke_text.setCharacterSize(15);
    tke_text.setPosition(sf::Vector2f(window_width - 100, window_height - 15));

    sf::Text ake_text;
    ake_text.setFont(font);
    ake_text.setCharacterSize(15);
    ake_text.setPosition(sf::Vector2f(window_width - 200, window_height - 15));
    
    sf::Text sd_text;
    sd_text.setFont(font);
    sd_text.setCharacterSize(15);
    sd_text.setPosition(sf::Vector2f(10, window_height - 15));


    sf::Text FPS;
    FPS.setFont(font);
    FPS.setCharacterSize(15);
    FPS.setPosition(sf::Vector2f(window_width - 100, 0));

    sf::Clock deltaClock;
    int rows = ceil(sqrt(circle_amount));

    int random_number =  rand() % 1+ 10;
    // initialising every particle variable.
    for (int x = 0; x < circle_amount; x++)
    {
        // randomly gives radius, and corresponding mass. uses a bit of math to distribute the particles 
        // fairly evenly when the program starts (to an extent)
        circles[x].radius =  circle_radius * random_number /5;
        circles[x].mass = random_number;
        circles[x].x = 50 + x%rows * window_width / (rows) ;
        circles[x].y = 50 + (int)x/rows * (window_height / (rows));
        default_circle.setPosition(sf::Vector2f(circles[x].x, circles[x].y));
        default_circle.setRadius(circles[x].radius);
        circles[x].circle = default_circle;
        circles[x].dx = (rand() % 2 * 2 - 1) * velocity_constant * (rand() % 5 + 1);
        circles[x].dy = (rand() % 2 * 2 - 1) * velocity_constant * (rand() % 5 + 1);
        circles[x].collisions = 0;
        int random_number =  rand() % 1+ 10;
        
    }
    int fps = 0;
    while (window.isOpen())
    {
        sf::Time dt = deltaClock.restart();
        window.clear();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        move_particles(dt.asSeconds());
        collision_handling(dt.asSeconds());
        display_particles(window);
        // updating all the text on the screen once per second.
        if (frame % fps_constant == 0) {
            draw_text(tke_text, ake_text, sd_text);
            fps = (int)(round((1 /dt.asSeconds()) * 0.1) * 10);
            std::string fps_string = "FPS: " + std::to_string(fps);
            FPS.setString(fps_string);
        }
        window.draw(tke_text);
        window.draw(ake_text);
        window.draw(sd_text);
        window.draw(FPS);
        window.display();
        frame++;
    }
}