#include "libs.h"
#include "types.h"

using OptionValue = std::variant<int, bool, float, std::string>;



class Menu
{
    public:
    class Option
    {
        public:
        std::string name;
        OptionValue associated_value;

        std::function<void()> on_click;
        Option(std::string name, std::function<void()> on_click, OptionValue associated_value);

    };


    std::vector<Option> options;
    int selected_index = 0;
    int scale = 2;
    int pos_x = SCREEN_WIDTH / 2;
    int pos_y = SCREEN_HEIGHT / 2;
    int arrow_pos_x = pos_x;
    int arrow_pos_y = pos_y;
    void displayMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75);

    Menu(std::vector<Option> options, int pos_x, int pos_y);
    Menu(std::vector<Option> options);
};