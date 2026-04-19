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
        bool visible = true;

        std::function<void()> on_click;
        Option(std::string name, std::function<void()> on_click, OptionValue associated_value, bool visible);

    };
    
    std::optional<Menus> menu_type;

    std::string title;
    std::array<uint8_t, 3> title_color = {255, 255, 255};
    std::vector<Option> options;
    std::function<void()> callback;
    bool selection_changed = false;
    int selected_index = 0;
    int scale = 2;
    int pos_x = SCREEN_WIDTH / 2;
    int pos_y = SCREEN_HEIGHT / 2;
    int arrow_pos_x = 0;
    int arrow_pos_y = 0;
    std::array<uint8_t, 3> arrow_color = {255, 255, 255};
    bool hide_menu = false;

    void displayMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75);
    void updateMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75);
    void executeSelectedOption();

    Menu(std::string title, std::vector<Option> options, int pos_x, int pos_y, std::function<void()> callback);
    Menu(std::vector<Option> options);
};
