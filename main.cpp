#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

#define PI 3.14159265

sf::RenderWindow Window(sf::VideoMode::getDesktopMode(), "TUDS Wave", sf::Style::Fullscreen); // Окно
sf::Clock Absolute; // Абсолютные часы
sf::Clock Frame; // Покадровые часы
float FrameTime; // Время одного кадра

sf::VertexArray WaveHead(sf::LineStrip, 5); // Голова волны
sf::VertexArray WaveTrail(sf::TriangleStrip, 4); // Хвост волны
sf::Vector2f WavePosition(500.f, 500.f); // Позиция волны

float WaveSpeed = 1000; // Скорость волны
float WaveAngle = 45; // Угол наклона волны
float WaveThickness = 40; // Толщина хвоста волны
float WaveHeadRotation = 0; // Вращение головы волны

float BPM = 110; // Ударов в минуту
// float BeatXPos = 0; // Позиция удара
float CurrentBeat = 0; // Текущий удар
sf::VertexArray BeatLines(sf::Lines, 2);

bool WasButtonPressed = false; // Была ли нажата кнопка кадр назад
bool IsButtonPressed = false; // Нажата ли кнопка сейчас
float LastWaveAngle = WaveAngle; // Угол волны кадр назад

sf::Font Font; // Шрифт
sf::Text Text; // Текст (в левом верхнем углу)

sf::Music Music; // Музыка

/*void display()
{
    Window.clear();

    for (float i = 0; i < Window.getDefaultView().getSize().x * 1.5; i += WaveSpeed * (60 / BPM))
    {
        BeatLines[0].position = sf::Vector2f(i - (CurrentBeat - trunc(CurrentBeat)) * (60 / BPM) * WaveSpeed, 0);
        BeatLines[1].position = sf::Vector2f(i - (CurrentBeat - trunc(CurrentBeat)) * (60 / BPM) * WaveSpeed, sf::VideoMode::getDesktopMode().height);

        Window.draw(BeatLines);
    }

    Window.draw(WaveTrail);
    Window.draw(WaveHead);
    Window.draw(Text);
    Window.display();
}*/

int main()
{
    Font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    Text.setFont(Font);
    Text.setFillColor(sf::Color::White);
    Music.openFromFile("1.wav");
    Music.setVolume(10);

    // Window.setFramerateLimit(6);

    BeatLines[0].position = sf::Vector2f();
    BeatLines[1].position = sf::Vector2f(0, sf::VideoMode::getDesktopMode().height);

    WaveHead[0].position = sf::Vector2f(WavePosition.x, WavePosition.y);
    WaveHead[1].position = sf::Vector2f(WavePosition.x - WaveThickness, WavePosition.y);
    WaveHead[2].position = sf::Vector2f(WavePosition.x + WaveThickness, WavePosition.y - WaveThickness);
    WaveHead[3].position = sf::Vector2f(WavePosition.x, WavePosition.y + WaveThickness);
    WaveHead[4].position = sf::Vector2f(WavePosition.x, WavePosition.y);

    WaveTrail[0].position = sf::Vector2f(WavePosition.x, WavePosition.y - WaveThickness / 2);
    WaveTrail[1].position = sf::Vector2f(WavePosition.x, WavePosition.y + WaveThickness / 2);
    WaveTrail[2].position = sf::Vector2f(WavePosition.x, WavePosition.y - WaveThickness / 2);
    WaveTrail[3].position = sf::Vector2f(WavePosition.x, WavePosition.y + WaveThickness / 2);

    bool l = false;

    while (Window.isOpen() && !l)
    {
        sf::Event Event;

        while (Window.pollEvent(Event))
        {
            switch (Event.type)
            {
                case sf::Event::EventType::Closed:
                    Window.close();
                    break;

                case sf::Event::EventType::KeyPressed:

                    switch (Event.key.code)
                    {
                        case sf::Keyboard::Key::Space:
                            l = true;
                            break;

                        default:
                            break;
                    }

                    break;

                default:
                    break;
            }
        }

        Window.clear();
        Window.display();
    }

    Window.setFramerateLimit(60);

    Absolute.restart();
    Frame.restart();
    Music.play();

    //sf::Thread Display(&display);

    while (Window.isOpen())
    {
        //Display.launch();

        sf::Event Event;
        FrameTime = Frame.restart().asSeconds();

        Text.setString("FPS: " + std::to_string(1 / FrameTime) + " " + std::to_string(BeatLines[0].position.x));

        while (Window.pollEvent(Event))
        {
            switch (Event.type)
            {
                case sf::Event::EventType::Closed:
                    Window.close();
                    break;

                default:
                    break;
            }
        }

        if (Absolute.getElapsedTime().asSeconds() >= (60 / BPM) * 16 && Absolute.getElapsedTime().asSeconds() <= (60 / BPM) * 32)
            WaveSpeed = 750;

        if (Absolute.getElapsedTime().asSeconds() >= (60 / BPM) * 32 && Absolute.getElapsedTime().asSeconds() <= (60 / BPM) * 96)
            WaveSpeed = 1000;

        if (Absolute.getElapsedTime().asSeconds() >= (60 / BPM) * 96)
            WaveSpeed = 250;

        CurrentBeat = Absolute.getElapsedTime().asSeconds() * BPM / 60.0f ;

        if (WavePosition.y <= WaveThickness)
        {
            WavePosition.y = WaveThickness;
        }
        else if (WavePosition.y >= sf::VideoMode::getDesktopMode().height - WaveThickness)
        {
            WavePosition.y = sf::VideoMode::getDesktopMode().height - WaveThickness;
        }
        else if ((int)trunc(CurrentBeat) % 2 == 1)
            WaveAngle = atan(2) / PI * 180;
        else
            WaveAngle = 45;


        // Механика нормальной волны

        IsButtonPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if (IsButtonPressed) WavePosition.y -= WaveSpeed * FrameTime * tan(WaveAngle * PI / 180); else WavePosition.y += WaveSpeed * FrameTime * tan(WaveAngle * PI / 180);

        /*  //Старый (неоптимизированный) трейл

        WaveTrail.resize(WaveTrail.getVertexCount() + 2);

        for (size_t i = WaveTrail.getVertexCount() - 1; i > 1; i--) WaveTrail[i].position = sf::Vector2f(WaveTrail[i - 2].position.x - WaveSpeed * FrameTime, WaveTrail[i - 2].position.y);

        WaveTrail[0].position = sf::Vector2f(WavePosition.x, WavePosition.y - WaveThickness / 2);
        WaveTrail[1].position = sf::Vector2f(WavePosition.x, WavePosition.y + WaveThickness / 2);

        if (WaveTrail[WaveTrail.getVertexCount() - 4].position.x < 0 && WaveTrail[WaveTrail.getVertexCount() - 3].position.x < 0)
            WaveTrail.resize(WaveTrail.getVertexCount() - 2);

        */

        // Новый трейл

        if (IsButtonPressed != WasButtonPressed || LastWaveAngle != WaveAngle)
        {
            WaveTrail.resize(WaveTrail.getVertexCount() + 2);
            for (size_t i = WaveTrail.getVertexCount() - 1; i > 1; i--) WaveTrail[i].position = sf::Vector2f(WaveTrail[i - 2].position.x, WaveTrail[i - 2].position.y);
        }

        for (size_t i = WaveTrail.getVertexCount() - 1; i > 1; i--) WaveTrail[i].position = sf::Vector2f(WaveTrail[i].position.x - WaveSpeed * FrameTime, WaveTrail[i].position.y - tan(WaveAngle * PI / 180) * FrameTime);

        WaveTrail[0].position = sf::Vector2f(WavePosition.x, WavePosition.y - WaveThickness / 2);
        WaveTrail[1].position = sf::Vector2f(WavePosition.x, WavePosition.y + WaveThickness / 2);

        if (WaveTrail[WaveTrail.getVertexCount() - 4].position.x < 0 && WaveTrail[WaveTrail.getVertexCount() - 3].position.x < 0)
            WaveTrail.resize(WaveTrail.getVertexCount() - 2);

        // Конец нового трейла

        if (IsButtonPressed)
        {
            if (WaveHeadRotation > -WaveAngle)
                WaveHeadRotation -= 1080 * FrameTime;
            else
                WaveHeadRotation = -WaveAngle;
        }
        else
        {
            if (WaveHeadRotation < WaveAngle)
                WaveHeadRotation += 1080 * FrameTime;
            else
                WaveHeadRotation = WaveAngle;
        }

        WaveHead[0].position = sf::Vector2f(WavePosition.x, WavePosition.y);
        WaveHead[1].position = sf::Vector2f(WavePosition.x + WaveThickness * cos((WaveHeadRotation + 180 + 45) * PI / 180), WavePosition.y + WaveThickness * sin((WaveHeadRotation + 180 + 45) * PI / 180));
        WaveHead[2].position = sf::Vector2f(WavePosition.x + WaveThickness * sqrt(2) * cos((WaveHeadRotation - 45 + 45) * PI / 180), WavePosition.y + WaveThickness * sqrt(2) * sin((WaveHeadRotation - 45 + 45) * PI / 180));
        WaveHead[3].position = sf::Vector2f(WavePosition.x + WaveThickness * cos((WaveHeadRotation + 90 + 45) * PI / 180), WavePosition.y + WaveThickness * sin((WaveHeadRotation + 90 + 45) * PI / 180));
        WaveHead[4].position = sf::Vector2f(WavePosition.x, WavePosition.y);

        WasButtonPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        LastWaveAngle = WaveAngle;

        Window.clear();

        for (float i = 0; i < Window.getDefaultView().getSize().x * 1.5; i += WaveSpeed * (60 / BPM))
        {
            BeatLines[0].position = sf::Vector2f(i - (CurrentBeat - trunc(CurrentBeat)) * (60 / BPM) * WaveSpeed, 0);
            BeatLines[1].position = sf::Vector2f(i - (CurrentBeat - trunc(CurrentBeat)) * (60 / BPM) * WaveSpeed, sf::VideoMode::getDesktopMode().height);

            Window.draw(BeatLines);
        }

        Window.draw(WaveTrail);
        Window.draw(WaveHead);
        Window.draw(Text);
        Window.display();

        //Display.wait();
    }

    return 0;
}
