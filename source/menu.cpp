#include "menu.h"
#include "game.h"
#include "helpers.h"
#include "inputmanager.h"


int32_t white = rgb_to_int32(255, 255, 255);
std::vector<std::vector<int>> arrow =
{
    {0,0,white,0},
    {0,0,0,white},
    {0,0,white,0}
};

Menu::Option::Option(std::string name, std::function<void()> on_click, OptionValue associated_value, bool visible) : name(name), on_click(on_click), associated_value(associated_value), visible(visible) {}

Menu::Menu(std::vector<Option> options) : options(options) 
{
    Option back_option("Back", []() {tetrismatrix::menus.pop();},0, false);
    options.push_back(back_option);
}
Menu::Menu(std::string title, std::vector<Option> options, int pos_x, int pos_y, std::function<void()> callback) : title(title), options(options), pos_x(pos_x), pos_y(pos_y), callback(callback) {}

void Menu::displayMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75)
{
    float angle = tetrismatrix::vertical ? 270.0f : 0.0f;
    
    // i hate this and makes it take forever to upload to the pi but ohhhh well, its the right way to do it i guesss
    std::stringstream ss(title);
    std::vector<std::string> words;
    std::string word;
    while(ss >> word)
        words.push_back(word);
        
    auto longest_word = std::max_element(words.begin(), words.end());

    // I HAVE NO IDEA WHY THIS BELOW WORKS BUT ILL LEAVE IT OK
    int title_x = pos_x - ((longest_word->size() / 2) * 6);
    // i hate these hardcoded values but they must stick for now so that the title is actually centered :P
    title_x += tetrismatrix::vertical ? -2 : 6;
    int title_y = pos_y - 14;

    if(tetrismatrix::vertical)
    {
        title_x = SCREEN_HEIGHT - title_x;
        std::swap(title_x, title_y);
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();
    graphics.set_pen(title_color[0], title_color[1], title_color[2]);

    int count = std::ranges::count(title, ' ');
    graphics.text(title, Point(title_x, title_y - (5 * count)), 1, 1.0f, angle);

    hub75.update(&graphics);

    for(int i = 0; i < options.size(); i++)
    {
        if(!options[i].visible)
            return;

        //printf("printing option #%i\n\n", i);

        std::string text = options[i].name;

        int x = (pos_x);
        int y = (pos_y);

        if(tetrismatrix::vertical)
        {
            std::swap(x, y);
        }

        std::string drawn_text = text;
        int limit = tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
        cropTextToFit(drawn_text, 4, limit);

        y += (i * scale * 4);
        x -= ((drawn_text.size() / 2) * 3);

        draw4x3Text(hub75, drawn_text, x, y, 255, 255, 255, tetrismatrix::vertical);

        //printf("pos_x: %i pos_y: %i", x, y);

        if(i == selected_index)
        {
            arrow_pos_y = y;
            int16_t keys_held = InputManager::keys_held[InputManager::controllers[0]];
            if(tetrismatrix::frames % 20 == 0 || selection_changed)
            {
                arrow_pos_x = (arrow_pos_x == x - 5) ? x - 4 : x - 5;
                drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical);
                selection_changed = false;
            }
            //printf("arrow pos_x: %i pos_y %i\n\n\n", x, y);
            drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical);
        }

        for(int i = 0; i < arrow[0].size(); i++)
        {
            for(int j = 0; j < arrow.size(); j++)
            {
                if(arrow[j][i] == 0) continue;
                arrow[j][i] = rgb_to_int32(arrow_color[0], arrow_color[1], arrow_color[2]);
            }
        }
        //printf("drawing menu!!\n\n");
        
    }

}

void Menu::updateMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75)
{
    if(hide_menu) return;

    displayMenu(graphics, hub75);
    if(InputManager::controllers.empty())
    {
        //printf("no controller!\n\n");
        return;
    }
    if(callback != NULL)
        this->callback();

    int16_t keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];

    if(keys_pressed & KEY_A)
    {
        executeSelectedOption();
    }

    if(keys_pressed & KEY_DOWN)
        selected_index++;

    if(keys_pressed & KEY_UP)
        selected_index--;

    if(keys_pressed & KEY_DOWN || keys_pressed & KEY_UP)
        selection_changed = true;

    if(selected_index < 0)
        selected_index = options.size() - 1;
    if(selected_index > options.size() - 1)
        selected_index = 0;

}

void Menu::executeSelectedOption()
{
    if(options[selected_index].on_click != NULL)
        options[selected_index].on_click();
    else
        hide_menu = false;
}