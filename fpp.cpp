#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <ncurses.h>
#include <signal.h>
#include <vector>
using namespace std;

static void finish(int sig); // ncurses function
float ray(float, float, float, float, string, int, float, float);
void raycast_in_fov(float[], float, float, float, float, string, int, float, int, float);
int is_wall(float, float, string, int);
void set_steps(float &, float &, float, float magnitude = 1);
void render_view(float[], string, int, int, float, float, int, int, float, float, float, bool, string, bool);
void player_movement(int, float, float &, float &, float, string, int);
int get_predominant_bearing(float);
int get_player_char(float);
void turn(float, float &);
void change_tweakable(float &, char, float);
void set_starting_loc(float &, float &, string, int, int);

// vector<float> detected_x;
// vector<float> detected_y;

int main()
{
    int term_width = 120;
    int term_height = 55;
    float *distances = new float[term_width];

    string map = "";
    // map += "#####################  ";
    // int map_width = map.length();
    // map += "#...#...............#  ";
    // map += "#...#.........#.....#  ";
    // map += "#...#........###....#  ";
    // map += "#.............#.....#  ";
    // map += "#...................#  ";
    // map += "#...................#  ";
    // map += "#...................#  ";
    // map += "#.........###.......#  ";
    // map += "#.........###.......#  ";
    // map += "#.........###.......#  ";
    // map += "#...................#  ";
    // map += "#####################  ";
    // map += "                       ";

    // map += "#########################################################";
    // int map_width = map.length();
    // map += "#.......................................................#";
    // map += "#.......................................................#";
    // map += "#.......................................................#";
    // map += "#.......................................................#";
    // map += "#.....##############..........###..####.................#";
    // map += "#.....##.......####...........#.......#.................#";
    // map += "#.....#.........##............#.......#.................#";
    // map += "#...............###...........#.......#.................#";
    // map += "#...............####..........#.......#.................#";
    // map += "#...............#####.........#.#######.................#";
    // map += "#...............######.......#..#.......................#";
    // map += "#............................#..#.......................#";
    // map += "#......................#######..################........#";
    // map += "#......................#................................#";
    // map += "#......................#..######################........#";
    // map += "#......................#.........#......................#";
    // map += "#......................#.........#......................#";
    // map += "#......................#.........#......................#";
    // map += "#......................###########......................#";
    // map += "#.......................................................#";
    // map += "#.......................................................#";
    // map += "#.......................................................#";
    // map += "#########################################################";
    // int map_height = map.length() / map_width;

    map += "###########################";
    int map_width = map.length();
    map += "#S#..............###......#";
    map += "#...############.###.#.##.#";
    map += "#.###....#####.#.....#.##.#";
    map += "#.##..##.....#.#######.#..#";
    map += "#.######.###.#.###.##..#.##";
    map += "#........##........##.##..#";
    map += "#####################.###.#";
    map += "#...........!#........###.#";
    map += "#.############.##########.#";
    map += "#.#............#...#......#";
    map += "#.#.############.#...######";
    map += "#.#.....####.....#####....#";
    map += "#.#####...######.......##.#";
    map += "#.#.###.#...##.#######.##.#";
    map += "#.#.###.#.#.##.........##.#";
    map += "#.#.....######.##########.#";
    map += "#.#######.........#.......#";
    map += "#.........#######...###.###";
    map += "###########################";
    int map_height = map.length() / map_width;

    // Tweakables
    float fov = 90;
    float view_distance = 3.5;
    float player_x = 1.5;
    float player_y = 1.5;
    set_starting_loc(player_x, player_y, map, map_width, map_height);
    float player_a = 0.5;
    float movement_distance = 0.1;
    float sprinting_distance = .2;
    float retinal_distance = .5;
    float ray_resolution = .001;
    float key_turn_amt = .02;
    float mouse_sensitivity = .01;
    string tweakables_string;
    bool display_map = true;
    bool display_bar = true;
    // End Tweakables

    // ncurses intitialization, source: https://invisible-island.net/ncurses/ncurses-intro.html#updating
    signal(SIGINT, finish); /* arrange interrupts to terminate */
    initscr();              /* initialize the curses library */
    resize_term(term_height, term_width);
    // mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h\n");
    keypad(stdscr, TRUE); /* enable keyboard mapping */
    nonl();               /* tell curses not to do NL->CR/NL on output */
    cbreak();             /* take input chars one at a time, no wait for \n */
    nodelay(stdscr, TRUE);
    echo(); /* echo input - in color */
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }
    // end ncurses initialization

    // Game Loop
    int input;
    bool keep_going = true;
    float mouse_last = 0;
    char buffer[512];
    size_t max_size = sizeof(buffer);
    while (keep_going)
    {
        // detected_x.clear();
        // detected_y.clear();
        string tweakables_string = "~tweakables~ (c,f)fov=" + to_string(fov) + " (v,g)retinal=" + to_string(retinal_distance) + " (b,h)ray_res=" + to_string(ray_resolution) + " (n,j)view_distance=" + to_string(view_distance) + " ~";

        // User Input
        input = getch();
        switch (input)
        {
        case 'w':
            player_movement(0, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'd':
            player_movement(1, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 's':
            player_movement(2, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'a':
            player_movement(3, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'W':
            player_movement(0, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'D':
            player_movement(1, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'S':
            player_movement(2, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case 'A':
            player_movement(3, movement_distance, player_x, player_y, player_a, map, map_width);
            break;
        case KEY_BACKSPACE:
            keep_going = false;
            break;
        case 'm':
            display_map = !display_map;
            break;
        case 'k':
            display_bar = !display_bar;
            break;
        case KEY_LEFT:
        case 'q':
            turn(-1 * key_turn_amt, player_a);
            break;
        case KEY_RIGHT:
        case 'e':
            turn(key_turn_amt, player_a);
            break;
        case 'Q':
            turn(-.25, player_a);
            break;
        case 'E':
            turn(.25, player_a);
            break;
        case 'c':
            change_tweakable(fov, '+', -5);
            break;
        case 'f':
            if (display_bar)
            {
                change_tweakable(fov, '+', 5);
            }
            break;
        case 'v':
            if (display_bar)
            {
                change_tweakable(retinal_distance, '+', -.001);
            }
            break;
        case 'g':
            if (display_bar)
            {
                change_tweakable(retinal_distance, '+', .001);
            }
            break;
        case 'b':
            if (display_bar)
            {
                change_tweakable(ray_resolution, '*', .1);
            }
            break;
        case 'h':
            if (display_bar)
            {
                change_tweakable(ray_resolution, '*', 10);
            }
            break;
        case 'n':
            if (display_bar)
            {
                change_tweakable(view_distance, '+', -.5);
            }
            break;
        case 'j':
            if (display_bar)
            {
                change_tweakable(view_distance, '+', .5);
            }
            break;
        // case KEY_MOUSE:
        //     MEVENT event;
        //     if (getmouse(&event) == OK)
        //     {
        //         snprintf(buffer, max_size, "Mouse at row=%d, column=%d bstate=0x%08lx",
        //                  event.y, event.x, event.bstate);
        //     }
        //     else
        //     {
        //         snprintf(buffer, max_size, "Got bad mouse event.");
        //     }
        //     break;
        }
        // End User Input

        raycast_in_fov(distances, player_x, player_y, player_a, view_distance, map, map_width, fov, term_width, ray_resolution);
        render_view(distances, map, map_width, map_height, player_x, player_y, term_width, term_height, retinal_distance, player_a, view_distance, display_map, tweakables_string, display_bar);
    }
    // End Game Loops
    delete distances;
    finish(SIGINT);
    return 0;
}

// Renders the view to the screen, going column by column
// distances represents the distances to each object by each ray
// retinal_distance is the distance to a theoretical retina allowing for
//    accurate scaling of objects at distance
void render_view(
    float distances[], string map,
    int map_width, int map_height,
    float player_x, float player_y,
    int terminal_width, int terminal_height,
    float retinal_distance, float player_angle,
    float view_distance, bool display_map,
    string bottom_bar, bool display_bar)
{
    float sky_size, projection_height;

    for (int x = terminal_width - 1; x >= 0; x--)
    {

        float distance_at_x = distances[x];
        // if (distance_at_x >= 1)
        // {
            projection_height = terminal_height * (retinal_distance / distance_at_x); //(terminal_height * (1 / pow(distances[x], 1.3)));
        // }
        // else
        // {
        //     if (distances[x] > distances[x - 1] && distances[x] > distances[x + 1])
        //     {
        //         distance_at_x = (distances[x - 1] + distances[x + 1]) / 2;
        //     }
        //     projection_height = terminal_height * (retinal_distance / distance_at_x);
        // }

        for (int y = 0; y < terminal_height; y++)
        {
            // Start Minimap
            if (display_map && x < map_width && y < map_height) // In bounds of minimap area
            {
                char curr_char = map[((int(y) * map_width) + int(x))];
                if (x < map_width && y < map_height) // In bounds of minimap area
                {
                    if (x == int(player_x) && y == int(player_y)) // Player location on the map
                    {
                        mvaddch(y, x, 'X');
                        //mvaddch(y, x, char(get_player_char(player_angle)));
                    }
                    else
                    {
                        // // Visualizing field of view:
                        // for (int i = 0; i < detected_x.size(); i++)
                        // {
                        //     if (x == int(detected_x[i]) && y == int(detected_y[i]))
                        //     {
                        //         curr_char = '!';
                        //     }
                        // }
                        mvaddch(y, x, curr_char);
                    }
                }
            }
            // End Minimap
            // Start Bottom Bar
            else if (display_bar && x == 0 && y == terminal_height - 1)
            {
                mvprintw(y, x, bottom_bar.c_str());
            }
            // End Bottom Bar
            // Start View
            else
            {
                if (distances[x] >= 0)
                {

                    sky_size = (terminal_height - projection_height) * 2.f / 3.f;
                    if (y <= sky_size)
                    {
                        mvaddch(y, x, ' ');
                    }
                    else if (y <= (sky_size + projection_height))
                    {
                        if (distances[x] <= view_distance * 1.f / 2.f)
                        {
                            mvaddch(y, x, '#');
                        }
                        else if (distances[x] <= view_distance * 3.f / 4.f)
                        {
                            mvaddch(y, x, '%');
                        }
                        else
                        {
                            mvaddch(y, x, '*');
                        }
                    }
                    else
                    {
                        mvaddch(y, x, '.');
                    }
                }
                else // No wall in range, pretend like wall at distance view_distance but not displaying anything there.
                {
                    projection_height = terminal_height * (retinal_distance / (view_distance + 1));
                    sky_size = (terminal_height - projection_height) * 2.f / 3.f;
                    if (y <= sky_size + projection_height)
                    {
                        mvaddch(y, x, ' ');
                    }
                    else
                    {
                        mvaddch(y, x, '.');
                    }
                }
            }
            // End View
        }
    }
    refresh();
}

/*
** distances[] is the array that the distances will be recorded into,
** view_distance is the maximum distance the player can see before encountering fog,
** fov is the desired field of view in degrees
** resolution is the number of rays to send out (determined by terminal size)
*/
void raycast_in_fov(float distances[],
                    float player_x, float player_y,
                    float player_angle, float view_distance,
                    string map, int map_width,
                    float fov, int screen_resolution,
                    float ray_resolution)
{
    fov = fov / 360.f; // Convert fov to 0-1 float as in angle
    float x_step, y_step;
    float curr_ray_angle = player_angle - (fov / 2);
    float ray_angle_step = fov / float(screen_resolution);

    // Send out rays
    for (int i = 0; i < screen_resolution; i++)
    {
        set_steps(x_step, y_step, curr_ray_angle, ray_resolution);
        distances[i] = ray(player_x, player_y, x_step, y_step, map, map_width, view_distance, ray_resolution);
        curr_ray_angle += ray_angle_step;
    }
}

// Sends a single ray with a given x and y step value,
// Returns the distance from the player to the destination
//    up to view_distance, or 0 if nothing is found.
// ray_resolution is how far the ray moves each step (added accuracy at lower vals)
float ray(float x, float y,
          float x_step, float y_step,
          string map, int map_width,
          float view_distance, float ray_resolution)
{
    for (float i = ray_resolution; i < view_distance; i += ray_resolution)
    {
        float init_x = x;
        float init_y = y;
        x += x_step;
        y += y_step;

        if (is_wall(int(x), int(y), map, map_width) > 0)
        {
            // detected_x.push_back(x);
            // detected_y.push_back(y);
            
            return i;
        }
    }

    return -1.0;
}

// Changes player_angle by the specified amount (change > 0 --> clockwise)
void turn(float change, float &player_angle)
{
    player_angle += change;
    if (player_angle >= 1)
    {
        player_angle -= 1;
    }
    else if (player_angle < 0)
    {
        player_angle += 1;
    }
}

// Moves the player in a passed direction (0,1,2,3 --> up, right, left down),
//    unless that player_movement would put them too close to a wall;
void player_movement(int direction, float distance,
                     float &player_x, float &player_y,
                     float player_angle, string map,
                     int map_width)
{
    bool valid_move = true;
    float destination_x;
    float hitbox_x;
    float destination_y;
    float hitbox_y;

    switch (direction)
    {
    default:
        set_steps(destination_x, destination_y, player_angle); // Step of dist 1 forward
        break;
    case 1:
        set_steps(destination_x, destination_y, (player_angle + .25)); // Step of dist 1 right
        break;
    case 2:
        set_steps(destination_x, destination_y, (player_angle + .5)); // Step of dist 1 forward
        break;
    case 3:
        set_steps(destination_x, destination_y, (player_angle + .75)); // Step of distance 1 left
        break;
    }

    hitbox_x = (destination_x * distance * 2) + player_x;
    hitbox_y = (destination_y * distance * 2) + player_y;
    destination_x = (destination_x * distance) + player_x;
    destination_y = (destination_y * distance) + player_y;
    valid_move = is_wall(destination_x, destination_y, map, map_width) == 0 && is_wall(hitbox_x, hitbox_y, map, map_width) == 0;

    if (valid_move)
    {
        player_x = destination_x;
        player_y = destination_y;
    }
}

// Returns whether the given x and y are in a wall tile or exit
//    (0 for nothing, 1 for wall (#), 2 for exit (!))
int is_wall(float x, float y, string map, int map_width)
{
    if (map[int(y) * map_width + int(x)] == '#')
    {
        return 1;
    }
    else if (map[int(y) * map_width + int(x)] == '!')
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

// int get_player_char(float player_angle)
// {
//     int bearing = get_predominant_bearing(player_angle);
//     switch (bearing)
//     {
//     default: // Upward facing
//         return '↑';
//         break;
//     case 1:
//         return '↗';
//         break;
//     case 2:
//         return '→';
//         break;
//     case 3:
//         return '↘';
//         break;
//     case 4:
//         return '↓';
//         break;
//     case 5:
//         return '↙';
//         break;
//     case 6:
//         return '←';
//         break;
//     case 7:
//         return '↖';
//         break;
//     }
// }

// Returns the predominant bearing of the player
// (0,1,2,3,4,5,6,7 ==> up,up-right,right,down-right,down,down-left,left,up-left)
int get_predominant_bearing(float angle)
{
    if (angle >= 0 && angle < 1.0 / 8.0)
    {
        return 0;
    }
    else if (angle >= 1.0 / 8.0 && angle < 2.0 / 8.0)
    {
        return 1;
    }
    else if (angle >= 2.0 / 8.0 && angle < 3.0 / 8.0)
    {
        return 2;
    }
    else if (angle >= 3.0 / 8.0 && angle < 4.0 / 8.0)
    {
        return 3;
    }
    else if (angle >= 4.0 / 8.0 && angle < 5.0 / 8.0)
    {
        return 4;
    }
    else if (angle >= 5.0 / 8.0 && angle < 6.0 / 8.0)
    {
        return 5;
    }
    else if (angle >= 6.0 / 8.0 && angle < 7.0 / 8.0)
    {
        return 6;
    }
    else
    {
        return 7;
    }
}

// Sets the values of x_step and y_step based on
//    the passed angle of the player and the passed
//    magnitude, which is the length of the hypotenuse
void set_steps(float &x_step, float &y_step, float angle, float magnitude)
{
    if (angle >= .875)
    {
        angle -= 1;
    }

    float angle_in_rads = 2 * M_PI * angle; // Convert to radians
    angle_in_rads -= M_PI_2;                // Shift angle

    x_step = cos(angle_in_rads) * magnitude;
    y_step = sin(angle_in_rads) * magnitude;
}

// Function that runs at end of execution or at error.
static void finish(int sig)
{
    endwin();
    exit(0);
}

void change_tweakable(float &tweakable, char operation, float amt)
{
    switch (operation)
    {
    case '+':
        tweakable += amt;
        break;
    case '*':
        tweakable *= amt;
    }
}

void set_starting_loc(float &player_x, float &player_y, string map, int map_width, int map_height)
{
    for (int x = 0; x < map_width; x++)
    {
        for (int y = 0; y < map_height; y++)
        {
            if (map[int(y) * map_width + int(x)] == 'S')
            {
                player_x = x + .5;
                player_y = y + .5;
            }
        }
    }
}