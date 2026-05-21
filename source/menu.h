#include "libs.h"
#include "types.h"




class Menu
{
    public:
    class Option
    {
        public:
        std::string name;
        OptionValue* associated_value = nullptr;
        int associated_value_min = 0;
        int associated_value_max = INT_MAX;
        bool visible = true;

        std::function<void()> on_click;
        Option(std::string name, std::function<void()> on_click, OptionValue* associated_value, int associateed_value_min, int associated_value_max, bool visible);
        Option(std::string name, std::function<void()> on_click, bool visible);
    };
    
    std::optional<Menus> menu_type;

    std::string title;
    std::array<uint8_t, 3> title_color = {255, 255, 255};
    std::vector<Option> options;
    std::function<void()> callback;
    std::function<void()> on_close;
    bool selection_changed = false;
    int selected_index = 0;
    int scale = 2;
    int pos_x = 0;
    int pos_y = 0;
    int arrow_pos_x = 0;
    int arrow_pos_y = 0;
    std::array<uint8_t, 3> arrow_color = {255, 255, 255};
    bool hide_menu = false;
    
    uni_controller_t* controller;

    void displayMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75);
    void updateMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75);
    void executeSelectedOption();
    void modifySelectedOptionValue();

    Menu(std::string title, std::vector<Option> options, std::function<void()> callback);
    Menu(std::string title, std::vector<Option> options, std::function<void()> callback, std::function<void()> on_close);
    Menu(std::string title, std::vector<Option> options, std::function<void()> callback, std::function<void()> on_close, uni_controller_t* controller);


    Menu(std::vector<Option> options);
    Menu(std::vector<Option> options, uni_controller_t* controller);

};
