#include "menu.h"
#include "game.h"
#include "helpers.h"
#include "inputmanager.h"
#include "language.h"

int32_t white = rgb_to_int32(255, 255, 255);
std::vector<std::vector<int>> arrow =
    {
        {0, 0, white, 0},
        {0, 0, 0, white},
        {0, 0, white, 0}};

Menu::Option::Option(std::string name, std::function<void()> on_click,  OptionValue* associated_value, int associated_value_min, int associated_value_max, bool visible) : name(name), on_click(on_click), associated_value(associated_value), visible(visible), associated_value_min(associated_value_min), associated_value_max(associated_value_max){}
Menu::Option::Option(std::string name, std::function<void()> on_click, bool visible) : name(name), on_click(on_click), visible(visible) {}

Menu::Menu(std::vector<Option> options) : Menu("", options, nullptr)
{
}
Menu::Menu(std::string title, std::vector<Option> options, std::function<void()> callback) : title(title), options(options), callback(callback) 
{
    Option back_option("Back", []()
                       { tetrismatrix::menus.pop(); }, false);
    this->options.push_back(back_option);

    pos_x = tetrismatrix::vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2 - 2;
    pos_y = tetrismatrix::vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 3;
}

void Menu::displayMenu(PicoGraphics_PenRGB888 &graphics, Hub75 &hub75)
{

    std::string cropped_title = title;
    int horizontal_limit = tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
    cropTextToFit(cropped_title, 5, horizontal_limit);


    std::istringstream iss(title);
    std::vector<std::string> title_words;
    std::string word;
    while(iss >> word) 
    {
        title_words.push_back(word);
    }
    int draw_pos_y = pos_y + title_words.size() * 5;
    int draw_pos_x = pos_x;


    if(cropped_title != title)
    {
        static int title_pos_x = draw_pos_y;
        drawScrollingText(hub75, graphics, title, draw_pos_x, draw_pos_y, title_color[0], title_color[1], title_color[2], tetrismatrix::vertical, 4, title_pos_x, tetrismatrix::frames, 6, draw5x5Text);
    }
    else
        draw5x5Text(hub75, graphics, title, draw_pos_x, draw_pos_y, title_color[0], title_color[1], title_color[2], tetrismatrix::vertical);


    // draws options.
    for (int i = 0; i < options.size(); i++)
    {
        if (!options[i].visible)
            return;

        // THIS will only work with up to 32 options. note thatttt
        static uint32_t restart_option_scrolling = 0;

        // printf("printing option #%i\n\n", i);

        std::string text = options[i].name;

        int x = draw_pos_x;
        int y = draw_pos_y + 5;
        //printf("pos_x: %i pos_y: %i", pos_x, pos_y);



        const int vertical_limit = tetrismatrix::vertical ? SCREEN_WIDTH : SCREEN_HEIGHT;
        const int offset = draw_pos_y;
        int max_visible_options = (vertical_limit - draw_pos_y) / (scale * 4);
        int scroll_offset = 0;

        if(selected_index >= max_visible_options)
            scroll_offset = (selected_index - max_visible_options + 1) * scale * 4;
        y -= scroll_offset;
        
        std::string drawn_text = text;
        
        cropTextToFit(drawn_text, 4, horizontal_limit);
        x -= (i != selected_index) ? ((drawn_text.size() / 2) * 4) : ((text.size() / 2) * 4);
        y += (i * scale * 4);
        



        if(y >= draw_pos_y)
            if(i != selected_index)
                draw4x3Text(hub75, graphics, drawn_text, x, y, 255, 255, 255, tetrismatrix::vertical);

        // printf("pos_x: %i pos_y: %i", x, y);

        if (i == selected_index)
        {
            arrow_pos_y = y;
            int16_t keys_held = InputManager::keys_held[InputManager::controllers[0]];
            if (tetrismatrix::frames % 20 == 0 || selection_changed)
            {
                arrow_pos_x = (arrow_pos_x == x - 5) ? x - 4 : x - 5;
                drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical, 1);
                selection_changed = false;
            }
            // printf("arrow pos_x: %i pos_y %i\n\n\n", x, y);

            if(text != drawn_text)
            {
                static int option_pos_x = x;
                if(!(restart_option_scrolling & (1 << i)))
                    option_pos_x = x;
                drawScrollingText(hub75, graphics, text, x, y, 255, 255, 255, tetrismatrix::vertical, 3, option_pos_x, tetrismatrix::frames, 5, draw4x3Text);
                restart_option_scrolling |= 1 << i;
            }
            else
            {
                draw4x3Text(hub75, graphics, drawn_text, x, y, 255, 255, 255, tetrismatrix::vertical);
            }

            drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical, 1);        
        }
        else
            restart_option_scrolling &= ~(1 << i);

        for (int i = 0; i < arrow[0].size(); i++)
        {
            for (int j = 0; j < arrow.size(); j++)
            {
                if (arrow[j][i] == 0)
                    continue;
                arrow[j][i] = rgb_to_int32(arrow_color[0], arrow_color[1], arrow_color[2]);
            }
        }
        // printf("drawing menu!!\n\n");
    }
}

void Menu::updateMenu(PicoGraphics_PenRGB888 &graphics, Hub75 &hub75)
{
    if (hide_menu)
        return;

    displayMenu(graphics, hub75);
    if (InputManager::controllers.empty())
    {
        // printf("no controller!\n\n");
        return;
    }
    if (callback != NULL)
        this->callback();

    int16_t keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];

    if (keys_pressed & KEY_A)
    {
        executeSelectedOption();
    }

    if (keys_pressed & KEY_B)
    {
        // executes the back method.
        if(options.size() >= 1 && tetrismatrix::menus.size() > 1)
        {
            selected_index = options.size() - 1;
            executeSelectedOption();
        }
    }

    if (keys_pressed & KEY_DOWN)
        selected_index++;

    if (keys_pressed & KEY_UP)
        selected_index--;

    if (keys_pressed & KEY_DOWN || keys_pressed & KEY_UP)
        selection_changed = true;

    if (selected_index < 0)
    {
        selected_index = options.size() - 1;
        while(!options[selected_index].visible)
            selected_index--;
    }
    while(!options[selected_index].visible)
        selected_index++;

    if (selected_index > options.size() - 1)
        selected_index = 0;
}

void Menu::executeSelectedOption()
{
    if(options.empty())
        return;

    if (options[selected_index].on_click != NULL)
        options[selected_index].on_click();
    else
        hide_menu = false;

    if(options[selected_index].associated_value != nullptr)
        modifySelectedOptionValue();
}

void Menu::modifySelectedOptionValue()
{
    if(std::holds_alternative<std::string>(*options[selected_index].associated_value))
    {
        std::string& associated_string = std::get<std::string>(*options[selected_index].associated_value);
        if(associated_string.empty())
        {
            associated_string.resize(options[selected_index].associated_value_max);
            for(char& character : associated_string)
            {
                character = ' ';
            }
        }
    }

    tetrismatrix::menus.push(new Menu(options[selected_index].name, {}, 
    [this]()
    {
        int initial_left_pos_x;
        int initial_right_pos_x;
        if(std::holds_alternative<int>(*options[selected_index].associated_value))
        {
            initial_left_pos_x = pos_x - (std::to_string(std::abs(std::get<int>(*options[selected_index].associated_value))).size() + 4);
            initial_right_pos_x = pos_x + (std::to_string(std::abs(std::get<int>(*options[selected_index].associated_value))).size() + 5);
        }
        else if(std::holds_alternative<EnumOption>(*options[selected_index].associated_value))
        {
            auto value = std::get<EnumOption>(*options[selected_index].associated_value);
            int offset = 5;
            initial_left_pos_x = pos_x - (value.labels[value.value].size() / 2) * 4 - offset;
            initial_right_pos_x = pos_x + (value.labels[value.value].size() / 2) * 4 + offset;
        }
        static int left_pos_x = initial_left_pos_x;
        static int right_pos_x = initial_right_pos_x;

        int16_t keys_held = InputManager::keys_held[InputManager::controllers[0]];

        // for strings.
        static int selected_char_index = 0;

        if(tetrismatrix::frames % 5 == 0)
        {
            // arrow drawing logic
            if(keys_held & KEY_LEFT)
                left_pos_x = (left_pos_x == initial_left_pos_x) ? initial_left_pos_x + 2 : initial_left_pos_x;
            else 
                left_pos_x = initial_left_pos_x;

            if(keys_held & KEY_RIGHT)
                right_pos_x = (right_pos_x == initial_right_pos_x) ? initial_right_pos_x + 2 : initial_right_pos_x;
            else
                right_pos_x = initial_right_pos_x;

            // associated value changing logic

            if(std::holds_alternative<int>(*options[selected_index].associated_value) || std::holds_alternative<EnumOption>(*options[selected_index].associated_value))
            {
                int* p_value;
                if(std::holds_alternative<int>(*options[selected_index].associated_value))
                    p_value = &std::get<int>(*options[selected_index].associated_value);
                else if(std::holds_alternative<EnumOption>(*options[selected_index].associated_value))
                    p_value = (int*)(&std::get<EnumOption>(*options[selected_index].associated_value).value);

                int& value = *p_value;
                
                int min_value = options[selected_index].associated_value_min;
                int max_value = options[selected_index].associated_value_max;
                if(keys_held & KEY_LEFT)
                {
                    value--;
                    if(value < min_value)
                        value = min_value;
                }

                if(keys_held & KEY_RIGHT)
                {
                    value++;
                    if(value > max_value)
                        value = max_value;
                }
                //printf("value: %i", value);
                //printf("max: %i", max_value);
                //printf("min: %i", min_value);
            }
            else if(std::holds_alternative<bool>(*options[selected_index].associated_value))
            {
                bool& value = std::get<bool>(*options[selected_index].associated_value); 

                if((keys_held & KEY_LEFT) || (keys_held & KEY_RIGHT))
                    value = !value;
            }
            else if(std::holds_alternative<std::string>(*options[selected_index].associated_value))
            {
                std::string& associated_string = std::get<std::string>(*options[selected_index].associated_value);
                if(keys_held & KEY_LEFT)
                    selected_char_index--;

                if(keys_held & KEY_RIGHT)
                    selected_char_index++;

                if(selected_char_index < 0)
                    selected_char_index = 0;
                else if(selected_char_index > associated_string.size() - 1)
                    selected_char_index = associated_string.size() - 1;
                
                if(keys_held & KEY_UP)
                {
                    if(associated_string[selected_char_index] == ' ')
                    {
                        associated_string[selected_char_index] = 'A';
                    }
                    else if(associated_string[selected_char_index] == '`')
                    {
                        associated_string[selected_char_index] = '!';
                    }
                    else if(associated_string[selected_char_index] == '@')
                    {
                        associated_string[selected_char_index] = ' ';
                    }
                    else
                        associated_string[selected_char_index]++;
                }
                if(keys_held & KEY_DOWN)
                {
                    
                    if(associated_string[selected_char_index] == ' ')
                    {
                        associated_string[selected_char_index] = '@';
                    }
                    else if(associated_string[selected_char_index] == '!')
                    {
                        associated_string[selected_char_index] = '`';
                    }
                    else if(associated_string[selected_char_index] == 'A')
                    {
                        associated_string[selected_char_index] = ' ';

                    }
                    else
                        associated_string[selected_char_index]--;
                }
            }
        }
        // drawing logic
        std::stringstream ss(options[selected_index].name);
        std::string word;
        std::vector<std::string> title_words;

        while (ss >> word) 
        {
            title_words.push_back(word);
        }
        
        if(std::holds_alternative<int>(*options[selected_index].associated_value))
        {
            int value = std::get<int>(*options[selected_index].associated_value);
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, std::to_string(value), initial_left_pos_x + 5, pos_y + (title_words.size() + 1) * 5, 255, 255, 255, tetrismatrix::vertical);
        }
        else if(std::holds_alternative<EnumOption>(*options[selected_index].associated_value))
        {
            int value = (int)(std::get<EnumOption>(*options[selected_index].associated_value).value);
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, std::get<EnumOption>(*options[selected_index].associated_value).labels[value], initial_left_pos_x + 5, pos_y + (title_words.size() + 1) * 5, 255, 255, 255, tetrismatrix::vertical);
        }
        else if(std::holds_alternative<bool>(*options[selected_index].associated_value))
        {
            Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
            const std::string text = (std::get<bool>(*options[selected_index].associated_value)) ? (translate(current_language, Texts::YES)) : (translate(current_language, Texts::NO)); 
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, text, initial_left_pos_x + 5, pos_y + (title_words.size() + 1) * 5, 255, 255, 255, tetrismatrix::vertical);
        }
        else if(std::holds_alternative<std::string>(*options[selected_index].associated_value))
        {
            std::string& associated_string = std::get<std::string>(*options[selected_index].associated_value);
            initial_left_pos_x = pos_x - (associated_string.size() / 2) * 4;
            initial_right_pos_x = pos_x + (associated_string.size() / 2) * 4;
            int draw_pos_x = initial_left_pos_x + 5;

            static int scroll_offset = 0;
            int horizontal_limit = (tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH);
            int chars_visible = horizontal_limit / 4;
            if(selected_char_index >= scroll_offset + chars_visible)
                    scroll_offset = selected_char_index - chars_visible + 1;
            else if(selected_char_index < scroll_offset)
                scroll_offset = selected_char_index;
            std::string visible = associated_string.substr(scroll_offset, chars_visible);
            if(options[selected_index].associated_value_max > chars_visible && scroll_offset < associated_string.size() - chars_visible)
                draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, ">", initial_left_pos_x + (visible.length() * 4), pos_y + ((title_words.size() + 1) * 5) + 4, 255, 255, 255, tetrismatrix::vertical);
            
            if(scroll_offset > 0)
                draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, "<", initial_left_pos_x + 5, pos_y + ((title_words.size() + 1) * 5) + 4, 255, 255, 255, tetrismatrix::vertical);


            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, visible, draw_pos_x, pos_y + (title_words.size() + 1) * 5, 255, 255, 255, tetrismatrix::vertical);

            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, "^", initial_left_pos_x + (selected_char_index - scroll_offset + 1) * 4 + 1, pos_y + ((title_words.size() + 1) * 5) - 4, 255, 255, 255, tetrismatrix::vertical);
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, "v", initial_left_pos_x + (selected_char_index - scroll_offset + 1) * 4 + 1, pos_y + ((title_words.size() + 1) * 5) + 4, 255, 255, 255, tetrismatrix::vertical);
        }

        if(!std::holds_alternative<std::string>(*options[selected_index].associated_value))
        {
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, "<", left_pos_x, pos_y + ((title_words.size() + 1) * 5), 255, 255, 255, tetrismatrix::vertical);
            draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, ">", right_pos_x, pos_y + ((title_words.size() + 1) * 5), 255, 255, 255, tetrismatrix::vertical);
        }

    }));
}