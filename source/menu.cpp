#include "menu.h"
#include "game.h"
#include "helpers.h"


int32_t white = rgb_to_int32(255, 255, 255);
std::vector<std::vector<int>> arrow =
{
    {0,0,white,0},
    {0,0,0,white},
    {0,0,white,0}
};

Menu::Option::Option(std::string name, std::function<void()> on_click, OptionValue associated_value) : name(name), on_click(on_click), associated_value(associated_value) {}

Menu::Menu(std::vector<Option> options) : options(options) {}
Menu::Menu(std::string title, std::vector<Option> options, int pos_x, int pos_y) : title(title), options(options), pos_x(pos_x), pos_y(pos_y) {}

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
        //printf("printing option #%i\n\n", i);

        std::string text = options[i].name;
        int limit = tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
        if(text.size() * 4 >= limit)
            text.replace((limit / 4) - 2, text.size() - (limit / 4 - 2), ".");

        int x = (pos_x);
        int y = (pos_y);

        if(tetrismatrix::vertical)
        {
            std::swap(x, y);
        }

        y += (i * scale * 4);
        x -= ((text.size() / 2) * 3);

        //printf("pos_x: %i pos_y: %i", x, y);

        if(i == selected_index)
        {
            if(tetrismatrix::frames % 20 == 0)
            {
                arrow_pos_x = (arrow_pos_x == x - 5) ? x - 4 : x - 5;
                drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical);
            }
            arrow_pos_y = y;
            //printf("arrow pos_x: %i pos_y %i\n\n\n", x, y);
            drawBitmap(hub75, arrow, arrow.size(), arrow[0].size(), arrow_pos_x, arrow_pos_y, tetrismatrix::vertical);
        }
        //graphics.text(options[i].name, Point(x, y), 1 , 0.5f, angle);
        draw4x3Text(hub75, text, x, y, 255, 255, 255, tetrismatrix::vertical);
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