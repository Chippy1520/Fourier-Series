#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <deque>
#include <fstream>
#include <vector>
#include <complex>


const double PI = 3.14159265359;

int height = 800;
int width = 800;
bool flip = false;
float thickness = 1;

using namespace std::complex_literals;
std::complex<float> custom(int k)
{
	return (std::complex<float>)((sin(k * PI / 6) / (k * PI) * pow(1i,k) - sin(k * PI / 6) / (k * PI) * pow(-1i, k))/(k * PI * 1.0i /3.0));
	//return (std::complex<float>)((sin(k * PI / 6) / (k * PI) * pow(1i, k) - sin(k * PI / 6) / (k * PI) * pow(-1i, k)));
	//return std::complex<float>(2.0f * (sin(k * PI / 3.0f) / (k * PI)) * std::exp( - 1.0i * (k * PI / 3.0f)) + sin(k*PI /3)/(k*PI) * pow(-1,k));
}

sf::Vector2f u = { 1,1 };
float  r;

float r2;
float delta = 0.01;
float radius = 100;
int n = 20;
float t = 0;
float omega = 1;
float L = 1;

int wave_type = 1;
bool square_wave = true;
bool saw_wave = false;
bool triangle_wave = false;

bool custom_wave = false;
int file_n = 0;

std::deque<sf::Vertex> points;
std::deque<sf::Vertex> points2;


const sf::Vector2f first_circle_center = sf::Vector2f(width / 2 - 200, height / 2 + 200);

std::vector <float> FS_Coefficients;



int main()
{

    sf::Clock deltaClock;
    sf::RenderWindow window(sf::VideoMode(height,width), "Fourier Series");
    ImGui::SFML::Init(window);

	
	
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        

		t += delta;
        sf::Event event;
        std::srand(std::time(nullptr));
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
			else if (event.type == sf::Event::Resized)
			{
				height = event.size.height;
				width = event.size.width;
                u = sf::Vector2f{ 800 /(float)width, 800 / (float)height};
				

                
			}
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("Window title");
		ImGui::SliderFloat("Radius", &radius, 0, 300);

		
		ImGui::SliderInt("Number of circles", &n, 1, 200);
		

		ImGui::SliderFloat("Speed", &delta, 0.01, 0.05);
		ImGui::SliderFloat("omega", &L, 0.1, 2.0);
		ImGui::SliderFloat("Thickness", &thickness, 0.1, 5.0);

		

		ImGui::RadioButton("Square wave", &wave_type,1);
		ImGui::RadioButton("Saw wave", &wave_type,2);
		ImGui::RadioButton("Triangle wave", &wave_type,3);
		ImGui::RadioButton("Custom wave", &wave_type, 4);

		ImGui::Checkbox("Flip real and imaginary axes", &flip);


        ImGui::End();

		switch (wave_type)
		{
		case 1:
			square_wave = true;
			saw_wave = false;
			triangle_wave = false;
			custom_wave = false;
			break;

		case 2:
			square_wave = false;
			saw_wave = true;
			triangle_wave = false;
			custom_wave = false;
			break;

		case 3:
			square_wave = false;
			saw_wave = false;
			triangle_wave = true;
			custom_wave = false;
			break;

		case 4:
			square_wave = false;
			saw_wave = false;
			triangle_wave = false;
			custom_wave = true;
			
			break;
	
		}

        window.clear(sf::Color::Black);
		sf::Vector2f circle_center = first_circle_center;
		sf::Vector2f circle_center2 = first_circle_center;

		
		int k;
		std::complex<float> (*A_k)(int k);
		

		for (int i = 0; i < n; i++)
		{
			
			k = i + 1;
			A_k = [](int k) {return std::complex<float>(0, 1.0f / k); };

			omega = L;

			if (custom_wave)
			{
				A_k = custom;
				
			}
			else
			{
				if (square_wave)
				{
					k = 2 * i + 1;
					A_k = [](int k) {return std::complex<float>(0, 1.0f / k); };

				}
				else if (saw_wave)
				{
					k = i + 1;
					A_k = [](int k) {return std::complex<float>(0, 1.0f / k); };
				}
				else if (triangle_wave)
				{
					k = 2 * i + 1;
					A_k = [](int k) {return std::complex<float>(0, 1.0f / pow(k, 2)); };
					
				}
				
			}
			float x1, x2, y1, y2;

			if (!flip)
			{
				x1 = radius * u.x * (A_k(k) * std::complex<float>(std::cos(-1 * k * omega * t), std::sin(-1 * k * omega * t))).real();
				x2 = radius * u.x * (A_k(-k) * std::complex<float>(std::cos(-1 * (-k) * omega * t), std::sin(-1 * (-k) * omega * t))).real();
				y1 = radius * u.y * (A_k(k) * std::complex<float>(std::cos(-1 * k * omega * t), std::sin(-1 * k * omega * t))).imag();
				y2 = radius * u.y * (A_k(-k) * std::complex<float>(std::cos(-1 * (-k) * omega * t), std::sin(-1 * (-k) * omega * t))).imag();

				r = radius * std::abs(A_k(k) * std::complex<float>(std::cos(-1 * k * omega * t), std::sin(-1 * k * omega * t)));
				r2 = radius * std::abs(A_k(k) * std::complex<float>(std::cos(-1 * (-k) * omega * t), std::sin(-1 * (-k) * omega * t)));

			}
			else
			{


				x1 = radius * u.x * (A_k(k) * std::complex<float>(std::sin(-1 * k * omega * t), std::cos(-1 * k * omega * t))).real();
				x2 = radius * u.x * (A_k(-k) * std::complex<float>(std::sin(-1 * (-k) * omega * t), std::cos(-1 * (-k) * omega * t))).real();
				y1 = radius * u.y * (A_k(k) * std::complex<float>(std::sin(-1 * k * omega * t), std::cos(-1 * k * omega * t))).imag();
				y2 = radius * u.y * (A_k(-k) * std::complex<float>(std::sin(-1 * (-k) * omega * t), std::cos(-1 * (-k) * omega * t))).imag();

				r = radius * std::abs(A_k(k) * std::complex<float>(std::sin(-1 * k * omega * t), std::cos(-1 * k * omega * t)));
				r2 = radius * std::abs(A_k(k) * std::complex<float>(std::sin(-1 * (-k) * omega * t), std::cos(-1 * (-k) * omega * t)));

			}
			
			
			
			
			
		



			sf::CircleShape circle(r);
			circle.setFillColor(sf::Color::Transparent);
			circle.setOutlineThickness(thickness);
			circle.setOutlineColor(sf::Color::Red);
			circle.setOrigin(r,r);
			circle.setScale(u);
			circle.setPosition(circle_center.x ,circle_center.y );
			window.draw(circle);

			

			sf::Vertex line[2];
			line[0].position = circle.getPosition();
			line[1].position = circle.getPosition() + sf::Vector2f(x1,y1);
			line[0].color = sf::Color::White;
			line[1].color = sf::Color::White;
			window.draw(line, 2, sf::Lines);


			


			sf::CircleShape circle2(r2);
			circle2.setFillColor(sf::Color::Transparent);
			circle2.setOutlineThickness(thickness);
			circle2.setOutlineColor(sf::Color::Red);
			circle2.setOrigin(r2, r2);
			circle2.setScale(u);
			circle2.setPosition(circle_center2.x, circle_center2.y);
			window.draw(circle2);

			sf::Vertex line2[2];
			line2[0].position = circle2.getPosition();
			line2[1].position = circle2.getPosition() + sf::Vector2f(x2,y2);
			line2[0].color = sf::Color::White;
			line2[1].color = sf::Color::White;
			
			window.draw(line2, 2, sf::Lines);

			circle_center2 = circle2.getPosition() + line2[1].position - line2[0].position;

			circle_center = circle.getPosition() + line[1].position - line[0].position;
			
			

        }
		
		sf::Vector2f middle = { (circle_center.x + circle_center2.x) / 2, (circle_center.y + circle_center2.y) / 2 };
		sf::Vertex line[2];
		line[0].position =  middle;
		line[1].position = { (first_circle_center.x + 200) ,middle.y};
		line[0].color = sf::Color::White;
		line[1].color = sf::Color::White;
		window.draw(line, 2, sf::Lines);

		sf::Vertex line2[2];
		line2[0].position = middle;
		line2[1].position = { middle.x ,first_circle_center.y - 200 };
		line2[0].color = sf::Color::White;
		line2[1].color = sf::Color::White;
		window.draw(line2, 2, sf::Lines);
		
		sf::Vertex line3[2];
		line3[0].position = circle_center;
		line3[1].position = middle;
		line3[0].color = sf::Color::Magenta;
		line3[1].color = sf::Color::Magenta;
		window.draw(line3, 2, sf::Lines);

		sf::Vertex line4[2];
		line4[0].position = circle_center2;
		line4[1].position = middle;
		line4[0].color = sf::Color::Green;
		line4[1].color = sf::Color::Green;
		window.draw(line4, 2, sf::Lines);

		

        points.push_front(sf::Vertex({ first_circle_center.x + 200  ,(circle_center.y + circle_center2.y) / 2 }, sf::Color::Yellow));
		points2.push_front(sf::Vertex({ (circle_center.x + circle_center2.x) / 2   ,first_circle_center.y - 200 }, sf::Color::Blue));
		if (points.size() > 2000)
		{
			points.pop_back();
		}
        for (sf::Vertex &point: points)
        {
			point.position.x += 0.2 * delta / 0.01;
			
        }
		for (size_t i = 0; i < points.size()-1; i++)
		{
			sf::Vertex line[2];
			
			line[0] = points[i];
			line[1] = points[i + 1];
			
			window.draw(line, 2, sf::Lines);

		}

		
		if (points2.size() > 2000)
		{
			points2.pop_back();
		}
		for (sf::Vertex& point2 : points2)
		{
			point2.position.y -= 0.2 * delta / 0.01;

		}
		for (size_t i = 0; i < points2.size() - 1; i++)
		{
			sf::Vertex line[2];

			line[0] = points2[i];
			line[1] = points2[i + 1];

			window.draw(line, 2, sf::Lines);

		}
		
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
