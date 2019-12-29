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

/*
** INCLUDES
*/
#include <stdbool.h>
#include "jo/sgl_prototypes.h"
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/sega_saturn.h"
#include "jo/smpc.h"
#include "jo/time.h"
#include "jo/core.h"
#include "jo/tools.h"
#include "jo/math.h"

#define JO_RANDOM_M             (2147483647)
#define JO_RANDOM_A             (16807)
#define JO_RANDOM_Q             (JO_RANDOM_M / JO_RANDOM_A)
#define JO_RANDOM_R             (JO_RANDOM_M % JO_RANDOM_A)

static int                      __jo_seed = 1;

#ifdef JO_COMPILE_WITH_SINUS_TABLE

/** @brief Sinus lookup table. Real value = JoSin[ANGLE] / 32768.0 */

int JoSinLookupTable[360] =
{
    0,
    571,
    1143,
    1714,
    2285,
    2855,
    3425,
    3993,
    4560,
    5126,
    5690,
    6252,
    6812,
    7371,
    7927,
    8480,
    9032,
    9580,
    10125,
    10668,
    11207,
    11743,
    12275,
    12803,
    13327,
    13848,
    14364,
    14876,
    15383,
    15886,
    16383,
    16876,
    17364,
    17846,
    18323,
    18794,
    19260,
    19720,
    20173,
    20621,
    21062,
    21497,
    21926,
    22347,
    22762,
    23170,
    23571,
    23964,
    24351,
    24730,
    25101,
    25465,
    25821,
    26169,
    26509,
    26841,
    27165,
    27481,
    27788,
    28087,
    28377,
    28659,
    28932,
    29196,
    29451,
    29697,
    29935,
    30163,
    30381,
    30591,
    30791,
    30982,
    31164,
    31336,
    31498,
    31651,
    31794,
    31928,
    32051,
    32165,
    32270,
    32364,
    32449,
    32523,
    32588,
    32643,
    32688,
    32723,
    32748,
    32763,
    32768,
    32763,
    32748,
    32723,
    32688,
    32643,
    32588,
    32523,
    32449,
    32364,
    32270,
    32165,
    32051,
    31928,
    31794,
    31651,
    31498,
    31336,
    31164,
    30982,
    30791,
    30591,
    30381,
    30163,
    29935,
    29697,
    29451,
    29196,
    28932,
    28659,
    28377,
    28087,
    27788,
    27481,
    27165,
    26841,
    26509,
    26169,
    25821,
    25465,
    25101,
    24730,
    24351,
    23964,
    23571,
    23170,
    22762,
    22347,
    21926,
    21497,
    21062,
    20621,
    20173,
    19720,
    19260,
    18794,
    18323,
    17846,
    17364,
    16876,
    16383,
    15886,
    15383,
    14876,
    14364,
    13848,
    13327,
    12803,
    12275,
    11743,
    11207,
    10668,
    10125,
    9580,
    9032,
    8480,
    7927,
    7371,
    6812,
    6252,
    5690,
    5126,
    4560,
    3993,
    3425,
    2855,
    2285,
    1714,
    1143,
    571,
    0,
    -571,
    -1143,
    -1714,
    -2285,
    -2855,
    -3425,
    -3993,
    -4560,
    -5126,
    -5690,
    -6252,
    -6812,
    -7371,
    -7927,
    -8480,
    -9032,
    -9580,
    -10125,
    -10668,
    -11207,
    -11743,
    -12275,
    -12803,
    -13327,
    -13848,
    -14364,
    -14876,
    -15383,
    -15886,
    -16384,
    -16876,
    -17364,
    -17846,
    -18323,
    -18794,
    -19260,
    -19720,
    -20173,
    -20621,
    -21062,
    -21497,
    -21926,
    -22347,
    -22762,
    -23170,
    -23571,
    -23964,
    -24351,
    -24730,
    -25101,
    -25465,
    -25821,
    -26169,
    -26509,
    -26841,
    -27165,
    -27481,
    -27788,
    -28087,
    -28377,
    -28659,
    -28932,
    -29196,
    -29451,
    -29697,
    -29935,
    -30163,
    -30381,
    -30591,
    -30791,
    -30982,
    -31164,
    -31336,
    -31498,
    -31651,
    -31794,
    -31928,
    -32051,
    -32165,
    -32270,
    -32364,
    -32449,
    -32523,
    -32588,
    -32643,
    -32688,
    -32723,
    -32748,
    -32763,
    -32768,
    -32763,
    -32748,
    -32723,
    -32688,
    -32643,
    -32588,
    -32523,
    -32449,
    -32364,
    -32270,
    -32165,
    -32051,
    -31928,
    -31794,
    -31651,
    -31498,
    -31336,
    -31164,
    -30982,
    -30791,
    -30591,
    -30381,
    -30163,
    -29935,
    -29697,
    -29451,
    -29196,
    -28932,
    -28659,
    -28377,
    -28087,
    -27788,
    -27481,
    -27165,
    -26841,
    -26509,
    -26169,
    -25821,
    -25465,
    -25101,
    -24730,
    -24351,
    -23964,
    -23571,
    -23170,
    -22762,
    -22347,
    -21926,
    -21497,
    -21062,
    -20621,
    -20173,
    -19720,
    -19260,
    -18794,
    -18323,
    -17846,
    -17364,
    -16876,
    -16384,
    -15886,
    -15383,
    -14876,
    -14364,
    -13848,
    -13327,
    -12803,
    -12275,
    -11743,
    -11207,
    -10668,
    -10125,
    -9580,
    -9032,
    -8480,
    -7927,
    -7371,
    -6812,
    -6252,
    -5690,
    -5126,
    -4560,
    -3993,
    -3425,
    -2855,
    -2285,
    -1714,
    -1143,
    -571
};

#endif

/** @brief Cosinus lookup table. Real value = JoCos[ANGLE] / 32768.0 */
int JoCosLookupTable[360] =
{
    32768,
    32763,
    32748,
    32723,
    32688,
    32643,
    32588,
    32523,
    32449,
    32364,
    32270,
    32165,
    32051,
    31928,
    31794,
    31651,
    31498,
    31336,
    31164,
    30982,
    30791,
    30591,
    30381,
    30163,
    29935,
    29697,
    29451,
    29196,
    28932,
    28659,
    28377,
    28087,
    27788,
    27481,
    27165,
    26841,
    26509,
    26169,
    25821,
    25465,
    25101,
    24730,
    24351,
    23964,
    23571,
    23170,
    22762,
    22347,
    21926,
    21497,
    21062,
    20621,
    20173,
    19720,
    19260,
    18794,
    18323,
    17846,
    17364,
    16876,
    16384,
    15886,
    15383,
    14876,
    14364,
    13848,
    13327,
    12803,
    12275,
    11743,
    11207,
    10668,
    10125,
    9580,
    9032,
    8480,
    7927,
    7371,
    6812,
    6252,
    5690,
    5126,
    4560,
    3993,
    3425,
    2855,
    2285,
    1714,
    1143,
    571,
    0,
    -571,
    -1143,
    -1714,
    -2285,
    -2855,
    -3425,
    -3993,
    -4560,
    -5126,
    -5690,
    -6252,
    -6812,
    -7371,
    -7927,
    -8480,
    -9032,
    -9580,
    -10125,
    -10668,
    -11207,
    -11743,
    -12275,
    -12803,
    -13327,
    -13848,
    -14364,
    -14876,
    -15383,
    -15886,
    -16383,
    -16876,
    -17364,
    -17846,
    -18323,
    -18794,
    -19260,
    -19720,
    -20173,
    -20621,
    -21062,
    -21497,
    -21926,
    -22347,
    -22762,
    -23170,
    -23571,
    -23964,
    -24351,
    -24730,
    -25101,
    -25465,
    -25821,
    -26169,
    -26509,
    -26841,
    -27165,
    -27481,
    -27788,
    -28087,
    -28377,
    -28659,
    -28932,
    -29196,
    -29451,
    -29697,
    -29935,
    -30163,
    -30381,
    -30591,
    -30791,
    -30982,
    -31164,
    -31336,
    -31498,
    -31651,
    -31794,
    -31928,
    -32051,
    -32165,
    -32270,
    -32364,
    -32449,
    -32523,
    -32588,
    -32643,
    -32688,
    -32723,
    -32748,
    -32763,
    -32768,
    -32763,
    -32748,
    -32723,
    -32688,
    -32643,
    -32588,
    -32523,
    -32449,
    -32364,
    -32270,
    -32165,
    -32051,
    -31928,
    -31794,
    -31651,
    -31498,
    -31336,
    -31164,
    -30982,
    -30791,
    -30591,
    -30381,
    -30163,
    -29935,
    -29697,
    -29451,
    -29196,
    -28932,
    -28659,
    -28377,
    -28087,
    -27788,
    -27481,
    -27165,
    -26841,
    -26509,
    -26169,
    -25821,
    -25465,
    -25101,
    -24730,
    -24351,
    -23964,
    -23571,
    -23170,
    -22762,
    -22347,
    -21926,
    -21497,
    -21062,
    -20621,
    -20173,
    -19720,
    -19260,
    -18794,
    -18323,
    -17846,
    -17364,
    -16876,
    -16384,
    -15886,
    -15383,
    -14876,
    -14364,
    -13848,
    -13327,
    -12803,
    -12275,
    -11743,
    -11207,
    -10668,
    -10125,
    -9580,
    -9032,
    -8480,
    -7927,
    -7371,
    -6812,
    -6252,
    -5690,
    -5126,
    -4560,
    -3993,
    -3425,
    -2855,
    -2285,
    -1714,
    -1143,
    -571,
    0,
    571,
    1143,
    1714,
    2285,
    2855,
    3425,
    3993,
    4560,
    5126,
    5690,
    6252,
    6812,
    7371,
    7927,
    8480,
    9032,
    9580,
    10125,
    10668,
    11207,
    11743,
    12275,
    12803,
    13327,
    13848,
    14364,
    14876,
    15383,
    15886,
    16384,
    16876,
    17364,
    17846,
    18323,
    18794,
    19260,
    19720,
    20173,
    20621,
    21062,
    21497,
    21926,
    22347,
    22762,
    23170,
    23571,
    23964,
    24351,
    24730,
    25101,
    25465,
    25821,
    26169,
    26509,
    26841,
    27165,
    27481,
    27788,
    28087,
    28377,
    28659,
    28932,
    29196,
    29451,
    29697,
    29935,
    30163,
    30381,
    30591,
    30791,
    30982,
    31164,
    31336,
    31498,
    31651,
    31794,
    31928,
    32051,
    32165,
    32270,
    32364,
    32449,
    32523,
    32588,
    32643,
    32688,
    32723,
    32748,
    32763
};

unsigned int        jo_sqrt(unsigned int value)
{
    unsigned int    start;
    unsigned int    end;
    unsigned int    res;
    unsigned int    mid;

    if (value == 0 || value == 1)
        return (value);
    JO_ZERO(res);
    start = 1;
    end = value;
    while (start <= end)
    {
        mid = JO_DIV_BY_2(start + end);
        if (mid * mid == value)
            return (mid);
        if (mid * mid < value)
        {
            start = mid + 1;
            res = mid;
        }
        else
            end = mid - 1;
    }
    return (res);
}

int     jo_gcd(int a, int b)
{
#ifdef JO_DEBUG
    if (a <= 0)
    {
        jo_core_error("a <= 0 in jo_gcd()");
        return (-1);
    }
    if (b <= 0)
    {
        jo_core_error("b <= 0 in jo_gcd()");
        return (-1);
    }
#endif
    while (a != b)
    {
        if (a > b)
            a -= b;
        else
            b -= a;
    }
    return (a);
}

float       jo_atan2f_rad(const float y, const float x)
{
    float   atan;
    float   z;

    if (JO_IS_FLOAT_NULL(x))
    {
        if (y > 0.0f)
            return (JO_PI_2);
        if (JO_IS_FLOAT_NULL(y))
            return 0.0f;
        return (-JO_PI_2);
    }
    z = y / x;
    if (JO_FABS(z) < 1.0f)
    {
        atan = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f)
        {
            if (y < 0.0f)
                return (atan - JO_PI);
            return (atan + JO_PI);
        }
    }
    else
    {
        atan = JO_PI_2 - z / (z * z + 0.28f);
        if (y < 0.0f)
            return (atan - JO_PI);
    }
    return (atan);
}

/* single phase linear congruential random integer generator */
int                             jo_random(int max)
{
    __jo_seed = JO_RANDOM_A * (__jo_seed % JO_RANDOM_Q) - JO_RANDOM_R * (__jo_seed / JO_RANDOM_Q);
    if (__jo_seed <= 0)
        __jo_seed += JO_RANDOM_M;
    return __jo_seed % max + 1;
}

void                jo_planar_rotate(const jo_pos2D * const point, const jo_pos2D * const origin, const int angle, jo_pos2D * const result)
{
    register int    dx;
    register int    dy;
    register int    cos_theta;
    register int    sin_theta;

    dx = point->x - origin->x;
    dy = point->y - origin->y;
    cos_theta = jo_cos(angle);
    sin_theta = jo_sin(angle);
    result->x = (JO_DIV_BY_32768(dx * cos_theta) - JO_DIV_BY_32768(dy * sin_theta)) + origin->x;
    result->y = (JO_DIV_BY_32768(dy * cos_theta) + JO_DIV_BY_32768(dx * sin_theta)) + origin->y;
}

/*
** END OF FILE
*/