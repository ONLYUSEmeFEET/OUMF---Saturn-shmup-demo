/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <jo/jo.h>
#include "ship.h"
#include "background.h"

#define CD_LOOP 1     
#define TRACK_LEVEL1 2 //1st level BGM 

static t_ship       ship;
static int          first_ship_sprite_id;
static int          blast_sprite_id;
static int          enemy_sprite_id;
//static int        enemy2_sprite_id; SOON
static int          shield_sprite_id;
static jo_list      laser_blast_list;
static jo_list      enemies_list;
static int          level = 0;
static int          gameover = 0;
static char         having_shield = 1;
static int          water_sprite_id = 0;
static jo_sound     blop;

static jo_datetime cow;

void draw_water(int sprite_id, int sprite_height, int speed)
{
    static int water_pos_y = 0;

    jo_sprite_enable_half_transparency();
    jo_sprite_draw3D(sprite_id, 0, water_pos_y - sprite_height - sprite_height, 400);
    jo_sprite_draw3D(sprite_id, 0, water_pos_y - sprite_height, 400);
    jo_sprite_draw3D(sprite_id, 0, water_pos_y, 400);
    jo_sprite_draw3D(sprite_id, 0, water_pos_y + sprite_height, 400);
    jo_sprite_disable_half_transparency();
    water_pos_y += speed;
    if (water_pos_y > sprite_height)
        water_pos_y = 0;
}

/* 
///NOT WORKING
void gimme_date (void) {
    while (1 == 1) {
        jo_getdate(&cow);
        break;
    }
    return cow;
} */

void                next_level(void)
{
    int             i;
    jo_list_data    enemy_spec;

    for (i = level; i >= 0; --i)
    {
        enemy_spec.coord.x = (jo_random(2) == 1 ? jo_random_using_multiple(141, 32) : -jo_random_using_multiple(141, 32));
        enemy_spec.coord.y = -JO_TV_HEIGHT_2 - jo_random_using_multiple(120, 40);
        jo_list_add(&enemies_list, enemy_spec);
    }
    ++level;
}

static inline void         draw_ship(void)
{
    /* Instead of loading the same animation when we move the ship to the right, we just flip the sprite horizontally */
    if (ship.move == SHIP_MOVE_RIGHT)
        jo_sprite_enable_horizontal_flip();
    /* We reverse the animation when the ship doesn't move horizontally (see line 74) */
    jo_sprite_draw3D(ship.reverse_animation ? jo_get_anim_sprite_reverse(ship.anim_id) : jo_get_anim_sprite(ship.anim_id), ship.x, ship.y, 500);
    if (ship.move == SHIP_MOVE_RIGHT)
        jo_sprite_disable_horizontal_flip();

}

static inline bool         check_if_laser_hit_enemy(jo_node *enemy, void *extra)
{
    jo_node         *blast;

    blast = (jo_node *)extra;
    if (!jo_hitbox_detection_custom_boundaries(blast_sprite_id, blast->data.coord.x, blast->data.coord.y, enemy->data.coord.x, enemy->data.coord.y, 20, 24))
        return false;
    jo_list_remove(&enemies_list, enemy);
    ++ship.score;
    if (ship.score > ship.hiScore)
    {
        ++ship.hiScore; //begin incrementing the new HiScore
    }
    return true;
}

static inline void         draw_laser_blast(jo_node *node)
{
    jo_sprite_draw3D(blast_sprite_id, node->data.coord.x, node->data.coord.y, 520);
    node->data.coord.y -= 4;
    if (jo_list_any(&enemies_list, check_if_laser_hit_enemy, node))
        jo_list_remove(&laser_blast_list, node);
    else if (node->data.coord.y < -JO_TV_HEIGHT_2)
        jo_list_remove(&laser_blast_list, node);
}

static inline void         draw_enemy(jo_node *node)
{
    jo_sprite_draw3D(enemy_sprite_id, node->data.coord.x, node->data.coord.y, 520);
    node->data.coord.y += 2;
    if (having_shield && jo_hitbox_detection(enemy_sprite_id, node->data.coord.x, node->data.coord.y, shield_sprite_id, ship.shield_pos.x, ship.shield_pos.y))
    {
        jo_list_remove(&enemies_list, node);
        having_shield = 0;
    }
    else if (jo_hitbox_detection_custom_boundaries(enemy_sprite_id, node->data.coord.x, node->data.coord.y, ship.x, ship.y, 28, 20))
    {
        gameover = 1;
        jo_list_remove(&enemies_list, node);
    }
    if (node->data.coord.y > JO_TV_HEIGHT_2 + 20)
        jo_list_remove(&enemies_list, node);
}

/* static inline void         move_background(void)
{
    static int      background_vertical_scrolling = 0;

    jo_move_background(ship.x, background_vertical_scrolling);
    --background_vertical_scrolling;
}
*/

void                my_draw(void)
{
if (!gameover && enemies_list.count <= 0)
        next_level();

    draw_water(water_sprite_id, 96, 3);

    jo_printf(1, 28, "OUMF Simulation Level: %d  ", level);
    jo_printf(1, 1, "Score: %d  ", ship.score);
    jo_printf(28, 1, "HiScore: %d  ", ship.hiScore); //HiScore support :)
//    jo_printf(1, 26, "Time: %d:%d:%d    ", cow.hour, cow.minute, cow.second); //Print current time

#ifdef JO_DEBUG
    jo_printf(1, 2, jo_get_last_error());
#endif
    if (!gameover)
    {
        draw_ship();
        if (having_shield)
            jo_sprite_draw3D(shield_sprite_id, ship.x + ship.shield_pos.x, ship.y + ship.shield_pos.y, 520);
    }
    else

    {
        jo_audio_stop_cd();
        jo_printf(8, 14, "Game over YEEEAAAHHHH!");
        jo_printf(8, 16, "Press Start to restart.");
        if (ship.score == ship.hiScore)
        {
            jo_printf(8, 18, "NEW HISCORE!"); //Congrats champ!
        }
    }


    jo_list_foreach(&laser_blast_list, draw_laser_blast);
    jo_list_foreach(&enemies_list, draw_enemy);
  //  move_background();    Initial implementation makes me queezy so this stays disabled for now.
}

static inline void         start_ship_animation(t_ship_horiz_move move, char is_moving_horizontally, char reverse_animation)
{
    jo_restart_sprite_anim(ship.anim_id);
    ship.is_moving_horizontally = is_moving_horizontally;
    ship.reverse_animation = reverse_animation;
    ship.move = move;
}

void                restart_game(void)
{
    jo_audio_play_cd_track(TRACK_LEVEL1, TRACK_LEVEL1, CD_LOOP);
    jo_list_clear(&laser_blast_list);
    jo_list_clear(&enemies_list);
    level = 0;
    ship.score = 0;
    gameover = 0;
    having_shield = 1;
    jo_clear_screen();
}

static inline void         shoot(void)
{
    if (laser_blast_list.count < 3) //limit laser amount
    {
    jo_list_data blast;
    blast.coord.x = ship.x;
    blast.coord.y = ship.y - 28;
    jo_list_add(&laser_blast_list, blast);
    jo_audio_play_sound_on_channel(&blop, 0);
    }
}

void load_blop_sound(void)
{
    /*
		To convert any audio file to PCM under Linux or Windows => http://ffmpeg.org

		Just some restriction due to the Sega Saturn hardware:

		Compatible Samplerate : 8000 to 44100 Hz (-ar option)
		Compatible PCM format : s8 (-f option)
		Compatible mode: 8/16 bit mono/stereo

		Some command line:

		ffmpeg -i A.WAV -f s16be -ar 44100 A.PCM => JoSoundMono16Bit
		ffmpeg -i A.MP3 -f s8 -ac 1 -ar 8000 A.PCM => JoSoundStereo8Bit
	*/
    jo_audio_load_pcm("A.PCM", JoSoundMono16Bit, &blop);
}

void			my_gamepad(void)
{
	if (!jo_is_pad1_available())
		return ;
    if (jo_is_pad1_key_down(JO_KEY_START))
        restart_game();
    if (gameover)
        return ;
    if (jo_is_pad1_key_down(JO_KEY_A))
        shoot();
    if (jo_is_pad1_key_pressed(JO_KEY_B)) //Autofire!
        shoot();
        asm("nop");
    if (jo_is_pad1_key_down(JO_KEY_C))
        shoot();
    if (jo_is_pad1_key_pressed(JO_KEY_LEFT) && ship.x > -(JO_TV_WIDTH_2 - 16))
    {
        /* If the ship doesn't move or on the opposite side */
        if ((!ship.is_moving_horizontally && jo_is_sprite_anim_stopped(ship.anim_id)) || (ship.is_moving_horizontally && ship.move == SHIP_MOVE_RIGHT))
            start_ship_animation(SHIP_MOVE_LEFT, 1, 0);
        ship.x -= ship.speed;
    }
	else if (jo_is_pad1_key_pressed(JO_KEY_RIGHT) && ship.x < (JO_TV_WIDTH_2 - 16))
    {
        if ((!ship.is_moving_horizontally && jo_is_sprite_anim_stopped(ship.anim_id)) || (ship.is_moving_horizontally && ship.move == SHIP_MOVE_LEFT))
            start_ship_animation(SHIP_MOVE_RIGHT, 1, 0);
        ship.x += ship.speed;
    }
    else
    {
        /* We restore the initial state (the trackbed) of the ship */
        if ((ship.is_moving_horizontally && jo_is_sprite_anim_stopped(ship.anim_id)))
            start_ship_animation(ship.move, 0, 1);
    }
	if (jo_is_pad1_key_pressed(JO_KEY_UP) && ship.y > -JO_TV_HEIGHT_2)
		ship.y -= ship.speed;
	if (jo_is_pad1_key_pressed(JO_KEY_DOWN) && ship.y < JO_TV_HEIGHT_2)
		ship.y += ship.speed;
}

void            init_game(void)
{

    jo_tile     ship_tileset[SHIP_TILE_COUNT] =
    {
        {200, 0, 40, 38},
        {160, 0, 40, 38},
        {120, 0, 40, 38},
        {80, 0, 40, 38},
        {40, 0, 40, 38},
        {0, 0, 40, 38},
    };

    first_ship_sprite_id = jo_sprite_add_tga_tileset(JO_ROOT_DIR, "SHIP.TGA", JO_COLOR_Red, ship_tileset, SHIP_TILE_COUNT);
    enemy_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "EN.TGA", JO_COLOR_Blue);
//  enemy2_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "EN.TGA", JO_COLOR_Blue); SOON
    blast_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "BLAST.TGA", JO_COLOR_Blue);
    shield_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "SHIELD.TGA", JO_COLOR_Blue);
    ship.anim_id = jo_create_sprite_anim(first_ship_sprite_id, SHIP_TILE_COUNT, 3);
    water_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "FOG.TGA", JO_COLOR_Green);
    ship.speed = 4;
    ship.score = 0;
    ship.shield_pos.x = 0;
    ship.shield_pos.y = 0;
    ship.move = SHIP_MOVE_NONE;
    jo_storyboard_move_object_in_circle(&ship.shield_pos, 30, 4, JO_STORYBOARD_INFINITE_DURATION);
    jo_list_init(&laser_blast_list);
    jo_list_init(&enemies_list);
}

void			jo_main(void)
{
    jo_core_init(JO_COLOR_Black);
    load_blop_sound();
    jo_set_background_sprite(&SpriteBg, 0, 0);
    init_game();
    jo_audio_play_cd_track(TRACK_LEVEL1, TRACK_LEVEL1, CD_LOOP);
    jo_core_add_callback(my_gamepad);
	jo_core_add_callback(my_draw);
//    jo_core_add_callback(gimme_date);
    jo_core_run();

}

/*
** END OF FILE
*/
