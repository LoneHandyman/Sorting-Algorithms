#include "sorty.hpp"
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <stdio.h>

#include "sorts.hpp"

//g++ -o main main.cpp -lsfml-graphics -lsfml-window -lsfml-system

std::condition_variable cv;
std::mutex mtx;

bool ready = false;
bool alive = true;

int selector = 0;

std::vector<std::string> algorithms = {"Shuffle Mode",
                                       "insertion sort",
                                       "selection sort",
                                       "bubble sort",
                                       "std::sort",
                                       "merge sort",
                                       "quick sort",
                                       "std::stable_sort",
                                       "heap sort",
                                       "shell sort",
                                       "counting sort",
                                       "radix sort"};

void shuffleVector(sal::sorty_vector& v) {
    while(alive){
        {
            std::unique_lock<std::mutex> lock(mtx);
            while (!ready) {
                cv.wait(lock);
            }
            ready = false;
        }
        switch(selector){
            case 0:
                std::random_shuffle(v.begin(), v.end());
                break;
            case 1:
                insertion_sort(v.begin(), v.end());
                break;
            case 2:
                selection_sort(v.begin(), v.end());
                break;
            case 3:
                bubble_sort(v.begin(), v.end());
                break;
            case 4:
                std::sort(v.begin(), v.end());
                break;
            case 5:
                merge_sort(v.begin(), v.end());
                break;
            case 6:
                quick_sort(v.begin(), v.end());
                break;
            case 7:
                std::stable_sort(v.begin(), v.end());
                break;
            case 8:
                std::make_heap(v.begin(), v.end());
                std::sort_heap(v.begin(), v.end());
                break;
            case 9:
                shell_sort(v.begin(), v.end());
                break;
            case 10:
                counting_sort(v.begin(), v.end());
                break;
            case 11:
                radix_sort(v.begin(), v.end());
                break;
        }
    }
}

std::vector<sf::Int16> generateBeepSound(float duration, int sampleRate) {
    const int amplitude = 3000;
    const int frequency = 2000;
    const int numSamples = static_cast<int>(duration * sampleRate);
    std::vector<sf::Int16> samples(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        samples[i] = (i % (sampleRate / frequency)) < (sampleRate / (2 * frequency)) ? amplitude : -amplitude;
    }

    return samples;
}

int main(){

    const int sampleRate = 44100;

    std::vector<sf::Int16> samples = generateBeepSound(0.2f, sampleRate);

    sf::SoundBuffer buffer;
    buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate);

    uint32_t w = 800, h = 600;

    float text_area_h = 100.f;

    sf::RenderWindow window(sf::VideoMode(w, h), "Sorting Algorithms");
    window.setFramerateLimit(30);
    sf::FloatRect render_area(0, text_area_h, w, h - text_area_h);

    sal::sorty_vector vec(800);

    auto stats = vec.make_group();

    stats->set_beep(buffer);

    sal::render(vec, window, render_area);

    sf::Thread thread(&shuffleVector, std::ref(vec));
    thread.launch();

    sf::Text stats_log;
    sf::Font minecraft;
    if (minecraft.loadFromFile("minecraft_font.ttf")){
        stats_log.setFont(minecraft);
        stats_log.setCharacterSize(w/h * 15);
        stats_log.setPosition(10, 10);
    }

    bool changed = true;

    stats->set_algorithms(algorithms);

    while (window.isOpen()) {
        if (selector < algorithms.size() && changed){
            stats->algorithm_id = selector;
            changed = false;
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    ready = true;
                    alive = false;
                }
                cv.notify_one();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::C) {
                    selector = 0;
                    changed = true;
                    event.key.code = sf::Keyboard::Space;
                }
                else if (event.key.code == sf::Keyboard::Right) {
                    selector += 1;
                    changed = true;
                }
                else if (event.key.code == sf::Keyboard::Left) {
                    selector -= 1;
                    changed = true;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    stats->reset();
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        ready = true;
                    }
                    cv.notify_one();
                }
                selector = std::max(0, std::min(int(algorithms.size() - 1), selector));
            }
        }

        window.clear(sf::Color::Black);

        sal::render(vec, window, render_area);

        std::stringstream log_stream;

        log_stream << (*stats);

        stats_log.setString(log_stream.str());

        window.draw(stats_log);

        window.display();

        stats->manage_sounds();
    }

    stats->kill_sounds();

    thread.terminate();

    return 0;
}