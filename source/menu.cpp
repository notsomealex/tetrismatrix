#include "menu.h"
#include "game.h"
#include "helpers.h"


int32_t white = rgb_to_int32(255, 255, 255);
const std::vector<std::vector<int>> arrow =
{
    {0,0,white,0},
    {0,0,0,white},
    {0,0,white,0}
};

Menu::Option::Option(std::string name, std::function<void()> on_click, OptionValue associated_value) : name(name), on_click(on_click), associated_value(associated_value) {}

Menu::Menu(std::vector<Option> options) : options(options) {}
Menu::Menu(std::vector<Option> options, int pos_x, int pos_y) : options(options), pos_x(pos_x), pos_y(pos_y) {}

void Menu::displayMenu(PicoGraphics_PenRGB888& graphics, Hub75& hub75)
{
    graphics.set_pen(0, 0, 0);
    //graphics.clear();
    graphics.set_pen(255, 255, 255);

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
        //hub75.update(&graphics);
        draw4x3Text(hub75, text, x, y, 255, 255, 255, tetrismatrix::vertical);
        //printf("drawing menu!!\n\n");
        
    }

}