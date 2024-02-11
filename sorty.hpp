#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <algorithm>
#include <queue>
#include <cmath>
#include <iomanip>

namespace sal{

    std::string formatTime(long milliseconds) {
        long seconds = milliseconds / 1000;
        long minutes = seconds / 60;
        long remainingSeconds = seconds % 60;
        long remainingMilliseconds = milliseconds % 1000;

        std::ostringstream formattedTime;
        formattedTime << std::setfill('0') << std::setw(2) << minutes << ":"; // Minutos
        formattedTime << std::setfill('0') << std::setw(2) << remainingSeconds << "."; // Segundos
        formattedTime << std::setfill('0') << std::setw(3) << remainingMilliseconds; // Milisegundos

        return formattedTime.str();
    }

    struct SortyManInfo{
        const sf::Color boolcc_color = sf::Color::Cyan;
        const sf::Color assign_color = sf::Color::Magenta;
        const sf::Color normal_color = sf::Color::White;

        uint32_t time_ms;
        uint32_t boolean_comps;
        uint32_t assignations;

        uint32_t algorithm_id;

        const uint32_t sleep_time = 1;
        const uint32_t beep_limit = 170;

        std::shared_ptr<sf::SoundBuffer> beep_buffer;
        std::queue<std::shared_ptr<sf::Sound>> sounds;
        std::vector<std::string> *algorithms;

        void set_beep(sf::SoundBuffer& buffer){
            beep_buffer = std::make_shared<sf::SoundBuffer>(buffer);
        }

        void set_algorithms(std::vector<std::string>& algs){
            algorithms = &algs;
        }

        void kill_sounds(){
            beep_buffer.reset();
        }

        void manage_sounds(){
            while (sounds.size() > beep_limit){
                stop_beep();
            }
        }

        void stop_beep(){
            if (!sounds.empty()){
                auto sound = sounds.front();
                sound->stop();
                sounds.pop();
            }
        }

        void launch_beep(float pitch=1.f){
            if(beep_buffer){
                std::shared_ptr<sf::Sound> beep = std::make_shared<sf::Sound>(*beep_buffer);
                beep->setPitch(pitch);
                beep->play();
                sounds.push(beep);
            }
        }

        void reset(){
            time_ms = 0;
            boolean_comps = 0;
            assignations = 0;
        }

        friend std::ostream& operator<<(std::ostream& os, const SortyManInfo& obj) {
            os << ((obj.algorithm_id == 0)?"[ ":"< ... ") << (*obj.algorithms)[obj.algorithm_id] 
            << ((obj.algorithms->size() - obj.algorithm_id == 1)?" ]":" ... >");
            os << "\n+ time=[" << formatTime(obj.time_ms) << "]";
            os << "\n+ comparisons: " << obj.boolean_comps;
            os << "\n+ assignations: " << obj.assignations;
            return os;
        }
    };

    float map_value(int value, int from_min, int from_max, float to_min, float to_max) {
        value = std::min(std::max(value, from_min), from_max);
        float fraction = static_cast<float>(value - from_min) / (from_max - from_min);
        return to_min + fraction * (to_max - to_min);
    }

    class SortyObj {
    public:
        friend class sorty_vector;

        SortyObj() = default;

        SortyObj(const int& value): value(value) {}

        SortyObj(const SortyObj& other) {
            (*this) = other;
        }
        

        void make_drawable(int vec_index, const size_t& vec_len, const sf::FloatRect& render_area){
            if (stats != nullptr){
            
                if (shape == nullptr){
                    shape = std::make_shared<sf::RectangleShape>();
                    shape->setFillColor(stats->normal_color);
                }
                float x, y, w, h;
                x = map_value(vec_index, 0, vec_len, render_area.left, render_area.left + render_area.width);
                y = render_area.top + render_area.height;
                w = render_area.width / vec_len;
                h = map_value(value, 0, vec_len - 1, 0.f, render_area.height);              
                shape->setPosition(x, y);
                shape->setOrigin(0, h);
                shape->setSize(sf::Vector2f(w, h));
                //printf("(%f, %f, %f)\n", shape->getPosition().x, shape->getPosition().y, shape->getSize().x);
            }
        }

        bool operator<(const SortyObj& other) const{
            count_boolean_comps();
            return value < other.value;
        }

        bool operator>(const SortyObj& other) const{
            count_boolean_comps();
            return value > other.value;
        }

        bool operator>=(const SortyObj& other) const{
            count_boolean_comps();
            return value >= other.value;
        }

        bool operator<=(const SortyObj& other) const{
            count_boolean_comps();
            return value <= other.value;
        }

        bool operator==(const SortyObj& other) const{
            count_boolean_comps();
            return value == other.value;
        }

        bool operator!=(const SortyObj& other) const{
            count_boolean_comps();
            return value != other.value;
        }

        SortyObj& operator+=(const SortyObj& other) {
            value += other.value;
            return *this;
        }

        SortyObj& operator-=(const SortyObj& other) {
            value -= other.value;
            return *this;
        }

        SortyObj operator+(const SortyObj& other) {
            SortyObj temp(*this);
            temp += other;
            return temp;
        }

        SortyObj operator-(const SortyObj& other) {
            SortyObj temp(*this);
            temp -= other;
            return temp;
        }

        SortyObj operator++(int) {
            SortyObj temp(*this);
            value++;
            return temp;
        }

        SortyObj operator--(int) {
            SortyObj temp(*this);
            value--;
            return temp;
        }

        SortyObj& operator=(const int& other) {
            value = other;
            return *this;
        }

        SortyObj& operator=(const SortyObj& other) {
            if (this != &other) {
                if (other.stats != nullptr)
                    stats = other.stats;
                if (other.shape != nullptr)
                    shape = other.shape;
                if (stats != nullptr){
                    ++stats->assignations;
                    if (shape != nullptr){
                        shape->setFillColor(stats->assign_color);
                        stats->launch_beep(calculate_pitch());
                        ++stats->time_ms;
                        sf::sleep(sf::milliseconds(stats->sleep_time));
                        stats->stop_beep();
                        shape->setFillColor(stats->normal_color);
                    }
                }
                value = other.value;
            }
            return *this;
        }

        sf::RectangleShape& get_rect(){
            return *shape;
        }

        int get_value(){
            return value;
        }

        friend std::ostream& operator<<(std::ostream& os, const SortyObj& obj) {
            os << obj.value;
            return os;
        }

    private:
        int value;
        mutable std::shared_ptr<sf::RectangleShape> shape;
        mutable std::shared_ptr<SortyManInfo> stats;

        float calculate_pitch() const{
            return 0.1f*(std::log(1+shape->getSize().y));
        }

        void count_boolean_comps() const{
            if (stats != nullptr){
                ++stats->boolean_comps;
                if (shape != nullptr){
                    shape->setFillColor(stats->boolcc_color);
                    stats->launch_beep(calculate_pitch());
                    ++stats->time_ms;
                    sf::sleep(sf::milliseconds(stats->sleep_time));
                    stats->stop_beep();
                    shape->setFillColor(stats->normal_color);
                }
            }
        }
    };

    class sorty_vector {
    private:
        std::vector<SortyObj> vec;

    public:
        friend class SortyObj;

        sorty_vector(){}

        sorty_vector(const size_t& n, const int& c){
            vec.assign(n, SortyObj(c));
        }

        sorty_vector(const size_t& n){
            for (int i = 0; i < n; ++i){
                vec.push_back(i);
            }
        }

        bool empty(){
            return vec.empty();
        }

        std::vector<SortyObj>& mem(){return vec;}

        SortyObj& get(size_t index){
            return vec[index];
        }

        const SortyObj& get(size_t index) const{
            return vec[index];
        }

        SortyObj& operator[](size_t index) {
            auto& e = vec[index];
            return e;
        }

        const SortyObj& operator[](size_t index) const {
            auto& e = vec[index];
            return e;
        }

        size_t size(){return vec.size();}

        std::shared_ptr<SortyManInfo> make_group(){
            std::shared_ptr<SortyManInfo> single;
            if (!vec.empty()){
                single = std::make_shared<SortyManInfo>();
                for (auto& e : vec){
                    e.stats = single;
                }
                single->reset();
            }
            return single;
        }

        typedef typename std::vector<SortyObj>::iterator iterator;
        typedef typename std::vector<SortyObj>::const_iterator const_iterator;

        iterator begin() {
            return vec.begin();
        }
        iterator end() {
            return vec.end(); 
        }

        const_iterator begin() const {
            return vec.begin(); 
        }
        const_iterator end() const {
            return vec.end(); 
        }

        const_iterator cbegin() const { return vec.cbegin(); }
        const_iterator cend() const { return vec.cend(); }
        

    };

    void render(sorty_vector& vec, sf::RenderWindow& screen, 
    const sf::FloatRect& render_area){
        for (int i=0; i < vec.size(); ++i){
            vec.get(i).make_drawable(i, vec.size(), render_area);
            if (screen.isOpen())
                screen.draw(vec.get(i).get_rect());
        }
    }

}
