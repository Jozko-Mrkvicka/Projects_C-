#include <cstdlib>
#include <ctime>
#include <cmath>
#include "graphics.h"

void draw_screen_opening(void);
void draw_screen_controls(void);
void draw_screen_ready(void);

void draw_terrain(void);
void draw_flash_icon(void);
void draw_heart_icon(void);

void draw_weapon_icons(void);
void draw_wind(void);

void wait(const int loops);
void sound(const int freq, const int duration);
void flood(const int x, const int y, const int old_color, const int new_color);
void terrain_falldown(const int explosion_pos_x);
void draw_outcome(const int winner);
void draw_summary(void);

const int COLOR_FRAME       = YELLOW;
const int COLOR_BACKGND     = BLACK;
const int COLOR_TERRAIN     = GREEN;
const int COLOR_LIFE        = MAGENTA;
const int COLOR_ENERGY      = MAGENTA;
const int COLOR_TEXT        = YELLOW;
const int COLOR_TEXT_BLINK  = LIGHTGREEN;
const int COLOR_PROJECTILE  = YELLOW;
const int COLOR_WEAPON_ICON = RED;
const int COLOR_AMMO        = MAGENTA;
const int COLOR_WIND        = MAGENTA;

const int SOUND_TURN_BARREL_TRUE     = 100;
const int SOUND_TURN_BARREL_FALSE    = 2000;
const int SOUND_TURN_BARREL_DURATION = 1;
const int SOUND_TAB                  = 5000;
const int SOUND_TAB_DURATION         = 1;

const int BTN_ARROW_UP    = 72;
const int BTN_ARROW_DOWN  = 80;
const int BTN_ARROW_RIGHT = 77;
const int BTN_ARROW_LEFT  = 75;
const int BTN_SPACE       = 32;
const int BTN_TAB         = 9;
const int BTN_ESC         = 27;
const int BTN_ENTER       = 13;
const int BTN_BACKSPACE   = 8;

const int TERRAIN_HEIGHT      = 150;

const int OFFSET_LIFEBAR_X    = 206;
const int OFFSET_LIFEBAR_Y    = 2;

const int OFFSET_FORCEBAR_X   = 29;
const int OFFSET_FORCEBAR_Y   = 2;

const int BAR_WIDTH           = 5;

const int FORCE_MAX           = 100;
const int FORCE_MIN           = 0;

const int OFFSET_ICON_FLASH_X = 17;
const int OFFSET_ICON_FLASH_Y = 0;

const int OFFSET_ICON_HEART_X = 187;
const int OFFSET_ICON_HEART_Y = 0;

const float BARREL_RIGHT_INCR =  0.2;
const float BARREL_LEFT_INCR  = -0.2;

const float FORCE_INCR =  2;
const float FORCE_DECR = -2;

const int BARREL_LEN  = 12;
const int TANK_RADIUS = 5;

const int UPPER_SCREEN_LIMIT = 12;
const int LOWER_SCREEN_LIMIT = 188;

const int GRAVITY = 1;

const int EXPLOSION_HEIGHT = 5;
const int MAX_NUM_OF_FRAGS = 40;
const int MAX_EXPLOSION_FORCE = 20;

const int NUM_OF_WEAPONS = 5;


float g_wind;

bool g_exit_program;
bool g_esc_match;
bool g_loss;
bool g_next_player;

char g_button;

bool g_sound = true;

int g_match_count;
int g_winner;


class player_t
{
    public:
        bool active = false;
        bool destroyed;

        int won_matches;

        int tank_pos_x;
        int tank_pos_y;

        float barrel_angle;
        int barrel_pos_x;
        int barrel_pos_y;

        int life;
        int selected_weapon;
        int force;
        int color_tank;

        int ammo[5];

        int num_of_frags;
        int explosion_force;

        void init(const int color, const int life);
        void draw_tank(void);
        void generate_tank_position(const int x_range, const int x_shift);
        void draw_force_bar(void);
        void draw_life_bar(void);
        void draw_active_color(void);
        void draw_tabulator(void);
        void switch_weapon(void);
        void turn_barrel(const float direction);
        bool draw_projectile(int* projectile_pos_x, int* projectile_pos_y, float delta_x, float delta_y, const float wind);
        void draw_ammo(void);
        void compute_damage(const int projectile_pos_x, const int projectile_pos_y, const int explosion_force);
        void tank_drop(void);
        void tank_delete(void);
        void update_force(const int difference);
        void fire(void);
};


void draw_explosion(player_t *active_player, const int explosion_pos_x, const int explosion_pos_y, int num_of_frags, const int explosion_force, const int color);
int choose_winner(player_t* p1, player_t* p2);
void draw_top_panel(player_t active_player);
void draw_bottom_panel(player_t active_player);

player_t p1, p2;
player_t *p;

player_t* switch_player(void);


void player_t::init(const int color, const int life)
{
    this->color_tank = color;

    this->destroyed = false;

    /* Weapon = 0 .. 4 */
    this->selected_weapon = 0;

    this->barrel_angle = M_PI*3/2;

    /* 0 .. 100 */
    this->force = 50;

    /* min = 1, max = 100 */
    this->life = life;

    /* Load ammunition. */
    this->ammo[0] = 10;
    this->ammo[1] = 5;
    this->ammo[2] = 10;
    this->ammo[3] = 5;
    this->ammo[4] = 5;

    this->num_of_frags = 4;
    this->explosion_force = 3;
}


void player_t::draw_tank(void)
{
    int x, y;

    setcolor(this->color_tank);

    for (float i = 0; i < 2*M_PI; i = i + 0.1)
    {
        x = this->tank_pos_x + round(cos(i)*TANK_RADIUS);
        y = this->tank_pos_y + round(sin(i)*TANK_RADIUS);

        if (getpixel(x, y) == COLOR_BACKGND)
        {
            putpixel(x, y, this->color_tank);
        }
    }
    flood(this->tank_pos_x, this->tank_pos_y, COLOR_BACKGND, this->color_tank);
    
    /* Draw barrel. */
    setlinestyle(SOLID_LINE, 0xFFFF, NORM_WIDTH);
    line(this->tank_pos_x, this->tank_pos_y, this->barrel_pos_x, this->barrel_pos_y);
}


void player_t::generate_tank_position(const int x_range, const int x_shift)
{
    /* Generate tank position. */    
    this->tank_pos_x = rand() % x_range + x_shift;
    this->tank_pos_y = 2;

    /* Generate barrel position. */
    this->barrel_pos_x = this->tank_pos_x + cos(this->barrel_angle)*BARREL_LEN;
    this->barrel_pos_y = this->tank_pos_y + sin(this->barrel_angle)*BARREL_LEN;

    this->tank_drop();
}


void player_t::tank_delete(void)
{
    flood(this->tank_pos_x, this->tank_pos_y, this->color_tank, COLOR_BACKGND);
    setcolor(COLOR_BACKGND);
    line(this->tank_pos_x, this->tank_pos_y, this->barrel_pos_x, this->barrel_pos_y);
}


void player_t::tank_drop(void)
{
    while ((getpixel(this->tank_pos_x, this->tank_pos_y) != COLOR_TERRAIN) && (this->tank_pos_y < LOWER_SCREEN_LIMIT))
    {
        this->tank_pos_y++;
        this->barrel_pos_y++;
    }
    this->tank_pos_y -= 2;
    this->barrel_pos_y -= 2;

    /* Tank dropped out of the screen. */
    if (this->tank_pos_y + TANK_RADIUS >= LOWER_SCREEN_LIMIT)
    {
        g_loss = true;
    }
    else
    {
        this->draw_tank();
    }
}


void player_t::draw_force_bar(void)
{
    /* Draw constant line above force bar. */
    setcolor(COLOR_FRAME);
    line(OFFSET_FORCEBAR_X, OFFSET_FORCEBAR_Y - 1, OFFSET_FORCEBAR_X + 99, OFFSET_FORCEBAR_Y - 1);

    /* Delete old force bar. */
    setfillstyle(SOLID_FILL, COLOR_BACKGND);
    bar(OFFSET_FORCEBAR_X, OFFSET_FORCEBAR_Y, OFFSET_FORCEBAR_X + 100, OFFSET_FORCEBAR_Y + BAR_WIDTH);

    /* Draw new force bar. */
    setfillstyle(SOLID_FILL, COLOR_ENERGY);
    bar(OFFSET_FORCEBAR_X, OFFSET_FORCEBAR_Y, OFFSET_FORCEBAR_X + this->force, OFFSET_FORCEBAR_Y + BAR_WIDTH);
}


void player_t::draw_life_bar(void)
{
    /* Draw constant line above life bar. */
    setcolor(COLOR_FRAME);
    line(OFFSET_LIFEBAR_X, OFFSET_LIFEBAR_Y - 1, OFFSET_LIFEBAR_X + 99, OFFSET_LIFEBAR_Y - 1);

    /* Delete old life bar. */
    setfillstyle(SOLID_FILL, COLOR_BACKGND);
    bar(OFFSET_LIFEBAR_X, OFFSET_LIFEBAR_Y, OFFSET_LIFEBAR_X + 100, OFFSET_LIFEBAR_Y + BAR_WIDTH);

    /* Draw new life bar. */
    setfillstyle(SOLID_FILL, COLOR_LIFE);
    bar(OFFSET_LIFEBAR_X, OFFSET_LIFEBAR_Y, OFFSET_LIFEBAR_X + this->life, OFFSET_LIFEBAR_Y + BAR_WIDTH);
}


/* Draw the active player color rectangle. */
void player_t::draw_active_color(void)
{
    setfillstyle(SOLID_FILL, this->color_tank);
    bar(150, 0, 170, UPPER_SCREEN_LIMIT);
}


void player_t::draw_tabulator(void)
{
    /* Delete old tabulator. */
    setcolor(COLOR_BACKGND);
    rectangle(6, LOWER_SCREEN_LIMIT + 2, 211, LOWER_SCREEN_LIMIT + 9);
    line(6 + 1*41, LOWER_SCREEN_LIMIT + 2, 6 + 1*41, LOWER_SCREEN_LIMIT + 9);
    line(6 + 2*41, LOWER_SCREEN_LIMIT + 2, 6 + 2*41, LOWER_SCREEN_LIMIT + 9);
    line(6 + 3*41, LOWER_SCREEN_LIMIT + 2, 6 + 3*41, LOWER_SCREEN_LIMIT + 9);
    line(6 + 4*41, LOWER_SCREEN_LIMIT + 2, 6 + 4*41, LOWER_SCREEN_LIMIT + 9);

    /* Draw new tabulator. */
    setcolor(COLOR_FRAME);
    rectangle(this->selected_weapon*41 + 6, LOWER_SCREEN_LIMIT + 2, this->selected_weapon*41 + 6 + 41, LOWER_SCREEN_LIMIT + 9);
}


void player_t::switch_weapon(void)
{
    if (4 == this->selected_weapon)
    {
        this->selected_weapon = 0;
    }
    else
    {
        this->selected_weapon++;
    }

    switch (this->selected_weapon)
    {
        case 0:
            this->num_of_frags = 4;
            this->explosion_force = 3;
            break;

        case 1:
            this->num_of_frags = 10;
            this->explosion_force = 10;
            break;

        case 2:
            this->num_of_frags = 30;
            this->explosion_force = MAX_EXPLOSION_FORCE;
            break;

        case 3:
            this->num_of_frags = 4;
            this->explosion_force = 3;
            break;

        case 4:
            this->num_of_frags = 4;
            this->explosion_force = 20;
            break;
    }

    sound(SOUND_TAB, SOUND_TAB_DURATION);
}


void player_t::turn_barrel(const float direction)
{
    int x, y;

    /* Delete barrel from old coordinates. */
    setlinestyle(SOLID_LINE, 0xFFFF, THICK_WIDTH);
    setcolor(COLOR_BACKGND);
    line(this->tank_pos_x, this->tank_pos_y, this->barrel_pos_x, this->barrel_pos_y);

    x = this->tank_pos_x + round(cos(this->barrel_angle + direction)*BARREL_LEN);
    y = this->tank_pos_y + round(sin(this->barrel_angle + direction)*BARREL_LEN);

    /* Compute new coordinates of barrel if the barrel will not collide with terrain. */
    if (getpixel(x, y) != COLOR_TERRAIN)
    {
        this->barrel_angle = this->barrel_angle + direction;
        this->barrel_pos_x = this->tank_pos_x + round(cos(this->barrel_angle)*BARREL_LEN);
        this->barrel_pos_y = this->tank_pos_y + round(sin(this->barrel_angle)*BARREL_LEN);

        sound(SOUND_TURN_BARREL_TRUE, SOUND_TURN_BARREL_DURATION);
    }
    else
    {
        sound(SOUND_TURN_BARREL_FALSE, SOUND_TURN_BARREL_DURATION);
    }
}


bool player_t::draw_projectile(int* projectile_pos_x, int* projectile_pos_y, float delta_x, float delta_y, const float wind)
{
    bool end_of_flight = false;
    bool explosion = false;
    int color;

    while (false == end_of_flight)
    {
        /* Erase egdes of the screen. */
        setcolor(COLOR_BACKGND);
        line(0, 0, 0, 199);
        line(319, 0, 319, 199);
        line(0, UPPER_SCREEN_LIMIT, 319, UPPER_SCREEN_LIMIT);

        /* Update position of projectile. */
        delta_y = delta_y + GRAVITY;
        delta_x = delta_x + wind;
        *projectile_pos_x += round(delta_x);
        *projectile_pos_y += round(delta_y);

        /* Check for hit. */
        color = getpixel(*projectile_pos_x, *projectile_pos_y);
        if ((*projectile_pos_y > UPPER_SCREEN_LIMIT) && ((COLOR_TERRAIN == color) || (p1.color_tank == color) || (p2.color_tank == color)))
        {
            explosion = true;
            end_of_flight = true;
        }

        /* Projectil is already below the screen. */
        if (*projectile_pos_y > LOWER_SCREEN_LIMIT)
        {
            end_of_flight = true;
        }

        /* Projectile position is outside of the screen (left). */
        if ((*projectile_pos_y > UPPER_SCREEN_LIMIT) && (*projectile_pos_y < LOWER_SCREEN_LIMIT) && (*projectile_pos_x <= 0))
        {
            putpixel(0, *projectile_pos_y, COLOR_PROJECTILE);
        }

        /* Projectile position is outside of the screen (right). */
        if ((*projectile_pos_y > UPPER_SCREEN_LIMIT) && (*projectile_pos_y < LOWER_SCREEN_LIMIT) && (*projectile_pos_x >= 319))
        {
            putpixel(319, *projectile_pos_y, COLOR_PROJECTILE);
        }

        /* Projectile position is outside of the screen (above). */
        if ((*projectile_pos_y < UPPER_SCREEN_LIMIT) && (*projectile_pos_x <= 319) && (*projectile_pos_x >= 0))
        {
            putpixel(*projectile_pos_x, UPPER_SCREEN_LIMIT, COLOR_PROJECTILE);
        }

        /* Projectile position is inside of the screen. */
        if ((*projectile_pos_y > UPPER_SCREEN_LIMIT) && (*projectile_pos_y < LOWER_SCREEN_LIMIT) && (*projectile_pos_x < 319) && (*projectile_pos_x > 0) && (false == end_of_flight))
        {
            putpixel(*projectile_pos_x, *projectile_pos_y, COLOR_PROJECTILE);
            // delay(30);
            sound(pow(*projectile_pos_y + 200, 2) / 50, 30);
            // sound(abs(round(((400+yy)*(400+yy))/100)));

            putpixel(*projectile_pos_x, *projectile_pos_y, COLOR_BACKGND);
        }
        else
        {
            // delay(30);
            sound(pow(*projectile_pos_y + 200, 2) / 50, 30);
        }

    }

    return explosion;
}


void player_t::draw_ammo(void)
{
    int offset_x;

    setcolor(COLOR_AMMO);
    for (int weapon = 0; weapon < NUM_OF_WEAPONS; weapon++)
    {
        offset_x = 20 + weapon*41;
        for (int i = 0; i < this->ammo[weapon]; i++)
        {
            line(offset_x + i*2, LOWER_SCREEN_LIMIT + 4, offset_x + i*2, LOWER_SCREEN_LIMIT + 7);
        }
    }
}


void player_t::compute_damage(const int projectile_pos_x, const int projectile_pos_y, const int explosion_force)
{
    float distance_from_explosion = sqrt(pow(this->tank_pos_x - projectile_pos_x, 2) + pow(this->tank_pos_y - projectile_pos_y, 2)) + 1.0;

    if (distance_from_explosion < TANK_RADIUS*10)
    {
        this->life -= (int) (explosion_force*50 / distance_from_explosion);
    }

    /* We don`t want to draw negative life. */
    if (this->life <= 0)
    {
        this->life = 0;
        this->destroyed = true;
        g_loss = true;
    }

    // if zvuk_let sound(abs(round(((400+yy)*(400+yy))/100)));

    // if zvuk
    // {
    //     for stred_vybuchuX = 1 to 30 do
    //     {
    //         sound(random(100) + 20);
    //         delay(20);
    //     }
    // }
    // nosound;
}


void player_t::update_force(const int difference)
{
    if ((this->force + difference >= FORCE_MIN) && (this->force + difference <= FORCE_MAX))
    {
        this->force += difference;
        this->draw_force_bar();
    }
}


void player_t::fire(void)
{
    if (0 != p->ammo[p->selected_weapon])
    {
        p->ammo[p->selected_weapon]--;
        draw_bottom_panel(*p);

        int projectile_pos_x = p->barrel_pos_x;
        int projectile_pos_y = p->barrel_pos_y;
        bool explosion = p->draw_projectile(&projectile_pos_x,
                                            &projectile_pos_y,
                                            cos(p->barrel_angle) * p->force/5,
                                            sin(p->barrel_angle) * p->force/5,
                                            g_wind);

        if (true == explosion)
        {
            int color = getpixel(projectile_pos_x, projectile_pos_y);
            switch (p->selected_weapon)
            {
                /* Standard weapons 0 .. 2. */
                case 0:
                case 1:
                case 2:
                    if (COLOR_TERRAIN == color)
                    {
                        color = BROWN;
                    }

                    /* Explosion of projectile. */
                    draw_explosion(p, projectile_pos_x, projectile_pos_y, p->num_of_frags, p->explosion_force, color);

                    p1.compute_damage(projectile_pos_x, projectile_pos_y, p->explosion_force);
                    p2.compute_damage(projectile_pos_x, projectile_pos_y, p->explosion_force);

                    /* If tank is still on the screen but out of life already then it will explode. */
                    if ((p1.life <= 0) && (p1.tank_pos_y < LOWER_SCREEN_LIMIT))
                    {
                        draw_explosion(p, p1.tank_pos_x, p1.tank_pos_y, MAX_NUM_OF_FRAGS, MAX_EXPLOSION_FORCE, p1.color_tank);
                    }

                    /* If tank is still on the screen but out of life already then it will explode. */
                    if ((p2.life <= 0) && (p2.tank_pos_y < LOWER_SCREEN_LIMIT))
                    {
                        draw_explosion(p, p2.tank_pos_x, p2.tank_pos_y, MAX_NUM_OF_FRAGS, MAX_EXPLOSION_FORCE, p2.color_tank);
                    }
                    break;

                case 3:
                    break;

                /* Weapon 4 ("hill"). */
                case 4:
                    p1.tank_delete();
                    p2.tank_delete();

                    /* Draw hill. */
                    setcolor(BROWN);
                    setfillstyle(SOLID_FILL, BROWN);
                    circle(projectile_pos_x, projectile_pos_y, p->explosion_force);
                    floodfill(projectile_pos_x, projectile_pos_y, BROWN);
                    flood(projectile_pos_x, projectile_pos_y, BROWN, COLOR_TERRAIN);

                    p1.draw_tank();
                    p2.draw_tank();
                    break;
            }
        }

        g_next_player = true;
    }
}


int main(void)
{
    /* Initialize random seed. */
    srand(time(NULL));
    initwindow(320, 200, "", 70, 20, false, true);

    int g_screen_buffer_size = imagesize(0, 0, getmaxx(), getmaxy());
    void* screen_buffer = new char [g_screen_buffer_size];

    if (NULL == screen_buffer)
    {
        setcolor(YELLOW);
        outtextxy(20, 20, (char *) "Error: Not enough memory for screen buffer!!");
        getch();
        exit;
    }

    g_exit_program = false;

    while (false == g_exit_program)
    {
        if (false == g_loss)
        {
            draw_screen_opening();
            draw_screen_controls();
            draw_screen_ready();

            g_match_count = 0;
            p1.won_matches = 0;
            p2.won_matches = 0;
        }
        
        g_match_count++;

        draw_terrain();

        p1.init(YELLOW, 100);
        p1.generate_tank_position(100, 10);
        p1.tank_drop();
        p1.draw_tank();

        p2.init(RED, 100);
        p2.generate_tank_position(100, 210);
        p2.tank_drop();
        p2.draw_tank();

        p2.active = true;

        g_esc_match = false;
        g_loss = false;

        /* This loop represents one single match. */
        while ((false == g_esc_match) && (false == g_loss))
        {
            g_winner = 0;

            /* Generate random wind in range -0.50 .. +0.50. */
            g_wind = (rand() % 100 - 50) / 100.0;

            p = switch_player();
            draw_top_panel(*p);
            draw_bottom_panel(*p);

            g_next_player = false;
            g_loss = false;
            g_esc_match = false;

            /* This loop represents one turn of one player. */
            while ((false == g_next_player) && (false == g_esc_match) && (false == g_loss))
            {
                g_button = getch();
                switch (g_button)
                {
                    case BTN_ARROW_UP:
                        p->update_force(FORCE_INCR);
                        break;

                    case BTN_ARROW_DOWN:
                        p->update_force(FORCE_DECR);
                        break;

                    case BTN_ARROW_RIGHT:
                        p->turn_barrel(BARREL_RIGHT_INCR);
                        p->draw_tank();
                        break;

                    case BTN_ARROW_LEFT:
                        p->turn_barrel(BARREL_LEFT_INCR);
                        p->draw_tank();
                        break;

                    case BTN_SPACE:
                        p->fire();
                        break;
                        
                    case BTN_ENTER:
                        g_next_player = true;
                        break;

                    case BTN_TAB:
                        p->switch_weapon();
                        p->draw_tabulator();
                        break;

                    case BTN_ESC:
                        getimage(0, 0, getmaxx(), getmaxy(), screen_buffer);
                        draw_summary();

                        g_esc_match = true;
                        g_button = getch();

                        if (('q' == g_button) || ('Q' == g_button))
                        {
                            g_exit_program = true;
                        }
                        else
                        {
                            g_exit_program = false;   
                        }

                        if (BTN_ESC == g_button)
                        {
                            g_exit_program = false;
                            g_esc_match = false;
                            putimage(0, 0, screen_buffer, COPY_PUT);
                        }

                        break;
                }
            } /* Turn loop. */

            if (true == g_loss)
            {
                g_winner = choose_winner(&p1, &p2);
                draw_outcome(g_winner);
                g_button = getch();
            }

        } /* Match loop. */

    } /* Main loop. */

    // delete[] screen_buffer;
    closegraph();
    return 0;
}


player_t* switch_player(void)
{
    if (true == p1.active)
    {
        p1.active = false;
        p2.active = true;
        return &p2;
    }
    else
    {
        p1.active = true;
        p2.active = false;
        return &p1;
    }
}


void draw_bottom_panel(player_t active_player)
{
    /* Erase whole bottom panel. */
    setfillstyle(SOLID_FILL, COLOR_BACKGND);
    bar(0, LOWER_SCREEN_LIMIT, 319, 199);

    /* Draw separation lines. */
    setcolor(COLOR_FRAME);
    line(0, LOWER_SCREEN_LIMIT, 319, LOWER_SCREEN_LIMIT);
    line(220, LOWER_SCREEN_LIMIT, 220, 199);

    draw_weapon_icons();
    draw_wind();

    active_player.draw_ammo();
    active_player.draw_tabulator();

}


void draw_top_panel(player_t active_player)
{
    /* Erase whole top panel. */
    setfillstyle(SOLID_FILL, COLOR_BACKGND);
    bar(0, 0, getmaxx(), UPPER_SCREEN_LIMIT);

    draw_flash_icon();
    draw_heart_icon();

    active_player.draw_force_bar();
    active_player.draw_life_bar();
    active_player.draw_active_color();
}


void draw_weapon_icons(void)
{
    int offset_x = 41;
    int offset_y = LOWER_SCREEN_LIMIT + 6;

    setcolor(COLOR_WEAPON_ICON);

    /* Draw weapon 1 icon. */
    circle(offset_x*0 + 12, offset_y, 1);

    /* Draw weapon 2 icon. */
    circle(offset_x*1 + 12, offset_y, 2);

    /* Draw weapon 3 icon. */
    circle(offset_x*2 + 12, offset_y, 3);

    /* Draw weapon 4 icon. */
    line(offset_x*3 + 12 - 2, offset_y - 2, offset_x*3 + 12 - 2, offset_y + 2);
    line(offset_x*3 + 12 - 2, offset_y + 2, offset_x*3 + 12 + 2, offset_y);
    line(offset_x*3 + 12 + 2, offset_y,     offset_x*3 + 12 - 2, offset_y - 2);
    line(offset_x*3 + 12 - 5, offset_y,     offset_x*3 + 12,     offset_y);

    /* Draw weapon 5 icon. */
    circle(offset_x*4 + 12, offset_y, 3);
    setfillstyle(SOLID_FILL, COLOR_TERRAIN);
    floodfill(offset_x*4 + 12, offset_y, COLOR_WEAPON_ICON);
}


void draw_wind(void)
{
    int x = round(g_wind*90.0);

    setcolor(COLOR_WIND);
    line(270, LOWER_SCREEN_LIMIT + 6, 270 + x, LOWER_SCREEN_LIMIT + 6);

    if (x > 5)
    {
        line(265 + x, 192, 270 + x, 194);
        line(265 + x, 196, 270 + x, 194);
    }

    if (x < -5)
    {
        line(275 + x, 192, 270 + x, 194);
        line(275 + x, 196, 270 + x, 194);
    }
}


void draw_explosion(player_t *active_player, const int explosion_pos_x, const int explosion_pos_y, int num_of_frags, const int explosion_force, const int color)
{
    float q;
    int x = 0, y = 0;

    float frag_x[MAX_NUM_OF_FRAGS];
    float frag_y[MAX_NUM_OF_FRAGS];
    float delta_x[MAX_NUM_OF_FRAGS];
    float delta_y[MAX_NUM_OF_FRAGS];

    q = 0.2;

    if (MAX_NUM_OF_FRAGS < num_of_frags)
    {
        num_of_frags = MAX_NUM_OF_FRAGS;
    }

    p1.tank_delete();
    p2.tank_delete();

    /* Draw crater. */
    setcolor(BROWN);
    setfillstyle(SOLID_FILL, BROWN);
    circle(explosion_pos_x, explosion_pos_y, explosion_force);
    floodfill(explosion_pos_x, explosion_pos_y, BROWN);
    flood(explosion_pos_x, explosion_pos_y, BROWN, COLOR_BACKGND);

    terrain_falldown(explosion_pos_x);

    if (p1.life > 0)
    {
        p1.tank_drop();
    }

    if (p2.life > 0)
    {
        p2.tank_drop();
    }

    draw_bottom_panel(*p);

    /* Set initial positions and velocities of the fragments. */
    for (int i = 0; i < num_of_frags; i++)
    {
        delta_y[i] = rand() % explosion_force + EXPLOSION_HEIGHT;
        delta_x[i] = (rand() % explosion_force * 2) - explosion_force + 1;
        frag_x[i] = 0;
        frag_y[i] = 0;
    }

// int cnt = 0;
// for (int i = 0; i < 10; i++)
// {
//     int freq = (rand() % 200) + 37;
//     sound(freq, 10);
// }

    // sound(100,  2000);
    // sound(100, 1);

    // sound(20,  1);
    // sound(100, 1);

    // sound(20,  1);
    // sound(100, 1);

    // sound(20,  1);
    // sound(100, 1);

    while (y <= 180)
    {
        /* Draw all fragments. */
        for (int i = 2; i < num_of_frags; i++)
        {
            delta_y[i] = delta_y[i] - q;

            frag_x[i] += delta_x[i]*q;
            frag_y[i] -= delta_y[i]*q;

            x = round(frag_x[i]) + explosion_pos_x;
            y = round(frag_y[i]) + explosion_pos_y;

            if ((x > 0) && (x < 319) && (y > UPPER_SCREEN_LIMIT) && (y < LOWER_SCREEN_LIMIT) && (getpixel(x, y) == COLOR_BACKGND))
            {
                putpixel(x, y, color);
            }
        }

//         delay(10);
// cnt++;

//         if (cnt < 5)
//         {
//         }

        //     if ((true == prehra1) || (true == prehra2))
        //     {
        //         if (i < 130)
        //         {
        //             sound(random(800));
        //         }
        //         else
        //         {
        //             nosound;
        //         }

        //         else
        //         { 
        //             switch (zbran)
        //             {
        //         2: if i < 20 sound(random(500))
        //         else nosound;
        //         10: if i < 30 sound(random(50)+20)
        //         else nosound;
        //         20: if i < 100 sound(random(200))
        //         else nosound;
        //         }
        //     }

        /* Erase all fragments. */
        for (int i = 2; i < num_of_frags; i++)
        {
            x = round(frag_x[i]) + explosion_pos_x;
            y = round(frag_y[i]) + explosion_pos_y;

            if ((x > 0) && (x < 319) && (y > UPPER_SCREEN_LIMIT) && (y < LOWER_SCREEN_LIMIT) && (getpixel(x, y) == color))
            {
                putpixel(x, y, COLOR_BACKGND);
            }
        }
    }
}


void draw_terrain(void)
{
    float y, x;
    int i, j, k, f;
    bool border;

    /* Fill whole screen with blue color. */
    setfillstyle(SOLID_FILL, BLUE);
    bar(0, 0, 319, 199);

    /* Draw a frame around the screen. */
    setcolor(COLOR_TERRAIN);
    rectangle(0, 0, 319, 199);

    /* Draw strip of terrain at the bottom of the screen. */
    setfillstyle(SOLID_FILL, COLOR_TERRAIN);
    bar(0, LOWER_SCREEN_LIMIT - 20, 319, LOWER_SCREEN_LIMIT - 1);


    /* Draw two functions on screen, sinus and cosinus, to create terrain. */
    j = 0;
    while (2 != j)
    {
        j = j + 1;
        i = rand() % 80 + 30;

        while (k >= 75)
        {
            k = rand() % 75 + 30;
        }

        x = 0;
        f = rand() % 2;

        while (x <= 319)
        {
            x = x + 0.1;

            if (1 == f)
            {
                y = cos(x/i)*k + TERRAIN_HEIGHT;
            }                
            else
            {
                y = sin(x/i)*k + TERRAIN_HEIGHT;
            }

            putpixel(round(x), round(y), COLOR_TERRAIN);
        }
    }

    /* Change upper part of the screen from blue color to a background color. */
    setfillstyle(SOLID_FILL, COLOR_BACKGND);
    floodfill(2, 2, COLOR_TERRAIN);

    /* Change lower part of the screen from blue color to a terrain color. */
    setfillstyle(SOLID_FILL, COLOR_TERRAIN);
    for (i = 0; i < 320; i++)
    {
        for (j = 0; j < 200; j++)
        {
            if (getpixel(i, j) == BLUE)
            {
                floodfill(i, j, COLOR_TERRAIN);
            }
        }   
    }

    setcolor(COLOR_BACKGND);
    line(0, 0, 0, 199);
    line(319, 0, 319, 199);
}


void draw_heart_icon(void)
{
    int triangel[6] =
    {
        OFFSET_ICON_HEART_X + 0,  OFFSET_ICON_HEART_Y + 4,
        OFFSET_ICON_HEART_X + 6,  OFFSET_ICON_HEART_Y + 11,
        OFFSET_ICON_HEART_X + 12, OFFSET_ICON_HEART_Y + 4
    };

    /* Draw heart symbol. */
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    fillellipse(OFFSET_ICON_HEART_X + 3, OFFSET_ICON_HEART_Y + 4, 3, 3);
    fillellipse(OFFSET_ICON_HEART_X + 9, OFFSET_ICON_HEART_Y + 4, 3, 3);
    line(OFFSET_ICON_HEART_X,     OFFSET_ICON_HEART_Y + 4,  OFFSET_ICON_HEART_X + 6,  OFFSET_ICON_HEART_Y + 11);
    line(OFFSET_ICON_HEART_X + 6, OFFSET_ICON_HEART_Y + 11, OFFSET_ICON_HEART_X + 12, OFFSET_ICON_HEART_Y + 4);
    fillpoly(3, triangel);
}


void draw_flash_icon(void)
{
    setcolor(YELLOW);
    line(OFFSET_ICON_FLASH_X + 0,  OFFSET_ICON_FLASH_Y + 10,  OFFSET_ICON_FLASH_X + 0,  OFFSET_ICON_FLASH_Y + 5);
    line(OFFSET_ICON_FLASH_X + 0,  OFFSET_ICON_FLASH_Y + 5,   OFFSET_ICON_FLASH_X + 3,  OFFSET_ICON_FLASH_Y + 8);
    line(OFFSET_ICON_FLASH_X + 3,  OFFSET_ICON_FLASH_Y + 8,   OFFSET_ICON_FLASH_X + 3,  OFFSET_ICON_FLASH_Y + 0);
    line(OFFSET_ICON_FLASH_X + 3,  OFFSET_ICON_FLASH_Y + 1,   OFFSET_ICON_FLASH_X + 0,  OFFSET_ICON_FLASH_Y + 3);
    line(OFFSET_ICON_FLASH_X + 3,  OFFSET_ICON_FLASH_Y + 1,   OFFSET_ICON_FLASH_X + 6,  OFFSET_ICON_FLASH_Y + 3);
}


void terrain_falldown(const int explosion_pos_x)
{
    bool terrain_top_found = false; 
    int terrain_top = 0;
    int pixel_cnt;

    for (int x = explosion_pos_x - 20; x < explosion_pos_x + 20; x++)
    {
        pixel_cnt = 0;
        terrain_top = 0;
        terrain_top_found = false;

        /* Count number of terrain pixels in particular column. */
        for (int y = UPPER_SCREEN_LIMIT + 1; y < LOWER_SCREEN_LIMIT; y++)   
        {
            if (getpixel(x, y) == COLOR_TERRAIN)
            {
                pixel_cnt++;
            }

            /* Find the terrain height of the column. */
            if ((getpixel(x, y) == COLOR_TERRAIN) && (false == terrain_top_found))
            {
                terrain_top = y;
                terrain_top_found = true;
            }
        }

        /* If there is any terrain in the column then perform the terrain falldown. */
        if (true == terrain_top_found)
        {
            setcolor(COLOR_BACKGND);
            line(x, terrain_top, x, LOWER_SCREEN_LIMIT - 1);
            setcolor(COLOR_TERRAIN);
            line(x, LOWER_SCREEN_LIMIT - 1, x, LOWER_SCREEN_LIMIT - pixel_cnt);
        }
    }
}


void sound(const int freq, const int duration)
{
    if (true == g_sound)
    {
        Beep(freq, duration);
    }
}


int choose_winner(player_t* p1, player_t* p2)
{
    int winner = -1;

    if ((true == p1->destroyed) && (true == p2->destroyed))
    {
        winner = 0;
    }

    if ((false == p1->destroyed) && (true == p2->destroyed))
    {
        winner = 1;
        p1->won_matches++;
    }

    if ((true == p1->destroyed) && (false == p2->destroyed))
    {
        winner = 2;
        p2->won_matches++;
    }

    return winner;
}


void draw_outcome(const int winner)
{
    char buffer[50];

    setcolor(COLOR_BACKGND);
    cleardevice();

    if (winner > 0)
    {
        sprintf(buffer, (char *) "Player %d wins!!", winner);
        while (0 == kbhit())
        {
            setcolor(COLOR_TEXT_BLINK);
            outtextxy(90, 15*5 + 10, buffer);
            wait(7);

            setcolor(COLOR_BACKGND);
            outtextxy(90, 15*5 + 10, (char *) "                ");
            wait(7);
        }
    }
    else
    {
        setcolor(COLOR_TEXT_BLINK);
        outtextxy(90, 15*5 + 10, (char *) "   No winner    ");
    }
}


void draw_summary(void)
{
    char buffer[50];

    setcolor(COLOR_BACKGND);
    cleardevice();

    setcolor(COLOR_TEXT);
    outtextxy(70, 15*1 + 10, (char *) "      Summary     ");
    outtextxy(70, 15*2 + 10, (char *) "------------------");

    sprintf(buffer, (char *) "Battles (total): %d", g_match_count - 1);
    outtextxy(70, 15*3 + 10, buffer);

    sprintf(buffer, (char *) "Player 1 wins:   %d", p1.won_matches);
    outtextxy(70, 15*4 + 10, buffer);

    sprintf(buffer, (char *) "Player 2 wins:   %d", p2.won_matches);
    outtextxy(70, 15*5 + 10, buffer);

    while (0 == kbhit())
    {
        setcolor(COLOR_TEXT_BLINK);
        outtextxy(30, 15*8  + 10, (char *) "Press ENTER to start new game");
        outtextxy(30, 15*9  + 10, (char *) "    Press ESC to continue    ");
        outtextxy(30, 15*10 + 10, (char *) "       Press Q to quit       ");
        wait(7);

        setcolor(COLOR_BACKGND);
        outtextxy(30, 15*8  + 10, (char *) "                             ");
        outtextxy(30, 15*9  + 10, (char *) "                             ");
        outtextxy(30, 15*10 + 10, (char *) "                             ");
        wait(7);
    }
}


void draw_screen_opening(void)
{
    setbkcolor(COLOR_BACKGND);
    cleardevice();

    setcolor(COLOR_TEXT);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    outtextxy(20, 30, (char *) "TankWars");

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(150, 70, (char *) "by");

    settextstyle(SMALL_FONT, HORIZ_DIR, 6);
    outtextxy(115, 90, (char *) "Werbhofen");

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    while (0 == kbhit())
    {
        setcolor(COLOR_TEXT_BLINK);
        outtextxy(90, 150, (char *) "Press any key ...");
        wait(7);

        setcolor(BLACK);
        outtextxy(90, 150, (char *) "Press any key ...");
        wait(7);
    }
    getch();
}


void draw_screen_controls(void)
{
    setbkcolor(COLOR_BACKGND);
    cleardevice();

    setcolor(COLOR_TEXT);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);

    outtextxy(70, 15*1  + 10, (char *) "     Controls"      );
    outtextxy(70, 15*2  + 10, (char *) "-------------------");
    outtextxy(70, 15*3  + 10, (char *) "Incr. power = UP"   );
    outtextxy(70, 15*4  + 10, (char *) "Decr. power = DOWN" );
    outtextxy(70, 15*5  + 10, (char *) "Turn left   = LEFT" );
    outtextxy(70, 15*6  + 10, (char *) "Turn right  = RIGHT");
    outtextxy(70, 15*7  + 10, (char *) "Change ammo = TAB"  );
    outtextxy(70, 15*8  + 10, (char *) "Fire        = SPACE");
    outtextxy(70, 15*9  + 10, (char *) "Skip round  = ENTER");
    outtextxy(70, 15*10 + 10, (char *) "Exit        = ESC"  );

    getch();
}


void draw_screen_ready(void)
{
    setbkcolor(COLOR_BACKGND);
    cleardevice();

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    while (0 == kbhit())
    {
        setcolor(COLOR_TEXT_BLINK);
        outtextxy(70, 15*5 + 10, (char *)"Have a nice death!!!");
        wait(7);

        cleardevice();
        wait(7);
    }

    getch();
}


/* This is non-blocking wait (sleep) function. It is very inaccurate,
   time is set in some abstract "loops" instead of precise miliseconds. */
void wait(const int loops)
{
    for (int i = 0; i  <  loops; i++)
    {
        Sleep(20);
        if (0 != kbhit())
        {
            break;
        }
    }
}


/* This function replaces a closed area filled with one background color (old) with another
   background color (new). The diffrence between this function and the default one
   is that the default floodfill function fills an area encompassed by single border color
   whereas this function fills area which can be encompassed by multiple border colors. */
void flood(const int x, const int y, const int old_color, const int new_color)
{
    if (old_color == getpixel(x, y))
    {
        putpixel(x, y, new_color);
    }

    if (old_color == getpixel(x + 1, y))
    {
        flood(x + 1, y, old_color, new_color);
    }

    if (old_color == getpixel(x - 1, y))
    {
        flood(x - 1, y, old_color, new_color);
    }

    if (old_color == getpixel(x, y + 1))
    {
        flood(x, y + 1, old_color, new_color);
    }

    if (old_color == getpixel(x, y - 1))
    {
        flood(x, y - 1, old_color, new_color);
    }
}


// char buffer[50];
// setcolor(YELLOW);
// sprintf(buffer, "X = %d", this->tank_pos_x);
// outtextxy(20, 20, buffer);

// sprintf(buffer, "Y = %d", this->tank_pos_y);
// outtextxy(20, 40, buffer);

// sprintf(buffer, "C = %d", this->color_tank);
// outtextxy(20, 60, buffer);


/*
    TODO:
        - Zvuk.
        - Pridat municiu c.4 ("zabka"/"brokovnica"/"frag granat").
        - Pridat header file.
        - Nahradit vsetky hardcoded values konstantami.
        - BUG: Raz sa mi stalo ze tank po zasahu (spravne) vypadol z obrazovky a potom sa objavila hlaska "No winer" (nespravne).

        - EXTRA: Tanky sa mozu hybat. Pridat "fuel bar".
        - EXTRA: Nastavenie rozlisenia.
        - EXTRA: Pridat viac hracov.
        - EXTRA: Pridat vodu.
        - EXTRA: Namiesto povodneho naboja c.5 urobit "skakajucu" municiu - aby sa po prvom naraze este raz odrazila.
        - EXTRA: Multiplayer po sieti.
        - EXTRA: Pridat menu:
            - Nekonecna municia (On/Off).
            - Vietor (On/Off).
            - Nesmrtelnost (On/Off).
            - Efekt explozie (On/Off).
*/