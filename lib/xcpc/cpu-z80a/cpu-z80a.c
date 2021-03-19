/*
 * cpu-z80a.c - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu-z80a-priv.h"

static const uint16_t DAATable[2048] = {
    0x0044, 0x0100, 0x0200, 0x0304, 0x0400, 0x0504, 0x0604, 0x0700, 
    0x0808, 0x090c, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510, 
    0x1000, 0x1104, 0x1204, 0x1300, 0x1404, 0x1500, 0x1600, 0x1704, 
    0x180c, 0x1908, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530, 
    0x2020, 0x2124, 0x2224, 0x2320, 0x2424, 0x2520, 0x2620, 0x2724, 
    0x282c, 0x2928, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534, 
    0x3024, 0x3120, 0x3220, 0x3324, 0x3420, 0x3524, 0x3624, 0x3720, 
    0x3828, 0x392c, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510, 
    0x4000, 0x4104, 0x4204, 0x4300, 0x4404, 0x4500, 0x4600, 0x4704, 
    0x480c, 0x4908, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514, 
    0x5004, 0x5100, 0x5200, 0x5304, 0x5400, 0x5504, 0x5604, 0x5700, 
    0x5808, 0x590c, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534, 
    0x6024, 0x6120, 0x6220, 0x6324, 0x6420, 0x6524, 0x6624, 0x6720, 
    0x6828, 0x692c, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530, 
    0x7020, 0x7124, 0x7224, 0x7320, 0x7424, 0x7520, 0x7620, 0x7724, 
    0x782c, 0x7928, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590, 
    0x8080, 0x8184, 0x8284, 0x8380, 0x8484, 0x8580, 0x8680, 0x8784, 
    0x888c, 0x8988, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594, 
    0x9084, 0x9180, 0x9280, 0x9384, 0x9480, 0x9584, 0x9684, 0x9780, 
    0x9888, 0x998c, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515, 
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701, 
    0x0809, 0x090d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511, 
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705, 
    0x180d, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531, 
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725, 
    0x282d, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535, 
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721, 
    0x3829, 0x392d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511, 
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705, 
    0x480d, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515, 
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701, 
    0x5809, 0x590d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535, 
    0x6025, 0x6121, 0x6221, 0x6325, 0x6421, 0x6525, 0x6625, 0x6721, 
    0x6829, 0x692d, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531, 
    0x7021, 0x7125, 0x7225, 0x7321, 0x7425, 0x7521, 0x7621, 0x7725, 
    0x782d, 0x7929, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591, 
    0x8081, 0x8185, 0x8285, 0x8381, 0x8485, 0x8581, 0x8681, 0x8785, 
    0x888d, 0x8989, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595, 
    0x9085, 0x9181, 0x9281, 0x9385, 0x9481, 0x9585, 0x9685, 0x9781, 
    0x9889, 0x998d, 0xa0b5, 0xa1b1, 0xa2b1, 0xa3b5, 0xa4b1, 0xa5b5, 
    0xa0a5, 0xa1a1, 0xa2a1, 0xa3a5, 0xa4a1, 0xa5a5, 0xa6a5, 0xa7a1, 
    0xa8a9, 0xa9ad, 0xb0b1, 0xb1b5, 0xb2b5, 0xb3b1, 0xb4b5, 0xb5b1, 
    0xb0a1, 0xb1a5, 0xb2a5, 0xb3a1, 0xb4a5, 0xb5a1, 0xb6a1, 0xb7a5, 
    0xb8ad, 0xb9a9, 0xc095, 0xc191, 0xc291, 0xc395, 0xc491, 0xc595, 
    0xc085, 0xc181, 0xc281, 0xc385, 0xc481, 0xc585, 0xc685, 0xc781, 
    0xc889, 0xc98d, 0xd091, 0xd195, 0xd295, 0xd391, 0xd495, 0xd591, 
    0xd081, 0xd185, 0xd285, 0xd381, 0xd485, 0xd581, 0xd681, 0xd785, 
    0xd88d, 0xd989, 0xe0b1, 0xe1b5, 0xe2b5, 0xe3b1, 0xe4b5, 0xe5b1, 
    0xe0a1, 0xe1a5, 0xe2a5, 0xe3a1, 0xe4a5, 0xe5a1, 0xe6a1, 0xe7a5, 
    0xe8ad, 0xe9a9, 0xf0b5, 0xf1b1, 0xf2b1, 0xf3b5, 0xf4b1, 0xf5b5, 
    0xf0a5, 0xf1a1, 0xf2a1, 0xf3a5, 0xf4a1, 0xf5a5, 0xf6a5, 0xf7a1, 
    0xf8a9, 0xf9ad, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515, 
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701, 
    0x0809, 0x090d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511, 
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705, 
    0x180d, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531, 
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725, 
    0x282d, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535, 
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721, 
    0x3829, 0x392d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511, 
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705, 
    0x480d, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515, 
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701, 
    0x5809, 0x590d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535, 
    0x0604, 0x0700, 0x0808, 0x090c, 0x0a0c, 0x0b08, 0x0c0c, 0x0d08, 
    0x0e08, 0x0f0c, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510, 
    0x1600, 0x1704, 0x180c, 0x1908, 0x1a08, 0x1b0c, 0x1c08, 0x1d0c, 
    0x1e0c, 0x1f08, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530, 
    0x2620, 0x2724, 0x282c, 0x2928, 0x2a28, 0x2b2c, 0x2c28, 0x2d2c, 
    0x2e2c, 0x2f28, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534, 
    0x3624, 0x3720, 0x3828, 0x392c, 0x3a2c, 0x3b28, 0x3c2c, 0x3d28, 
    0x3e28, 0x3f2c, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510, 
    0x4600, 0x4704, 0x480c, 0x4908, 0x4a08, 0x4b0c, 0x4c08, 0x4d0c, 
    0x4e0c, 0x4f08, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514, 
    0x5604, 0x5700, 0x5808, 0x590c, 0x5a0c, 0x5b08, 0x5c0c, 0x5d08, 
    0x5e08, 0x5f0c, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534, 
    0x6624, 0x6720, 0x6828, 0x692c, 0x6a2c, 0x6b28, 0x6c2c, 0x6d28, 
    0x6e28, 0x6f2c, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530, 
    0x7620, 0x7724, 0x782c, 0x7928, 0x7a28, 0x7b2c, 0x7c28, 0x7d2c, 
    0x7e2c, 0x7f28, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590, 
    0x8680, 0x8784, 0x888c, 0x8988, 0x8a88, 0x8b8c, 0x8c88, 0x8d8c, 
    0x8e8c, 0x8f88, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594, 
    0x9684, 0x9780, 0x9888, 0x998c, 0x9a8c, 0x9b88, 0x9c8c, 0x9d88, 
    0x9e88, 0x9f8c, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515, 
    0x0605, 0x0701, 0x0809, 0x090d, 0x0a0d, 0x0b09, 0x0c0d, 0x0d09, 
    0x0e09, 0x0f0d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511, 
    0x1601, 0x1705, 0x180d, 0x1909, 0x1a09, 0x1b0d, 0x1c09, 0x1d0d, 
    0x1e0d, 0x1f09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531, 
    0x2621, 0x2725, 0x282d, 0x2929, 0x2a29, 0x2b2d, 0x2c29, 0x2d2d, 
    0x2e2d, 0x2f29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535, 
    0x3625, 0x3721, 0x3829, 0x392d, 0x3a2d, 0x3b29, 0x3c2d, 0x3d29, 
    0x3e29, 0x3f2d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511, 
    0x4601, 0x4705, 0x480d, 0x4909, 0x4a09, 0x4b0d, 0x4c09, 0x4d0d, 
    0x4e0d, 0x4f09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515, 
    0x5605, 0x5701, 0x5809, 0x590d, 0x5a0d, 0x5b09, 0x5c0d, 0x5d09, 
    0x5e09, 0x5f0d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535, 
    0x6625, 0x6721, 0x6829, 0x692d, 0x6a2d, 0x6b29, 0x6c2d, 0x6d29, 
    0x6e29, 0x6f2d, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531, 
    0x7621, 0x7725, 0x782d, 0x7929, 0x7a29, 0x7b2d, 0x7c29, 0x7d2d, 
    0x7e2d, 0x7f29, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591, 
    0x8681, 0x8785, 0x888d, 0x8989, 0x8a89, 0x8b8d, 0x8c89, 0x8d8d, 
    0x8e8d, 0x8f89, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595, 
    0x9685, 0x9781, 0x9889, 0x998d, 0x9a8d, 0x9b89, 0x9c8d, 0x9d89, 
    0x9e89, 0x9f8d, 0xa0b5, 0xa1b1, 0xa2b1, 0xa3b5, 0xa4b1, 0xa5b5, 
    0xa6a5, 0xa7a1, 0xa8a9, 0xa9ad, 0xaaad, 0xaba9, 0xacad, 0xada9, 
    0xaea9, 0xafad, 0xb0b1, 0xb1b5, 0xb2b5, 0xb3b1, 0xb4b5, 0xb5b1, 
    0xb6a1, 0xb7a5, 0xb8ad, 0xb9a9, 0xbaa9, 0xbbad, 0xbca9, 0xbdad, 
    0xbead, 0xbfa9, 0xc095, 0xc191, 0xc291, 0xc395, 0xc491, 0xc595, 
    0xc685, 0xc781, 0xc889, 0xc98d, 0xca8d, 0xcb89, 0xcc8d, 0xcd89, 
    0xce89, 0xcf8d, 0xd091, 0xd195, 0xd295, 0xd391, 0xd495, 0xd591, 
    0xd681, 0xd785, 0xd88d, 0xd989, 0xda89, 0xdb8d, 0xdc89, 0xdd8d, 
    0xde8d, 0xdf89, 0xe0b1, 0xe1b5, 0xe2b5, 0xe3b1, 0xe4b5, 0xe5b1, 
    0xe6a1, 0xe7a5, 0xe8ad, 0xe9a9, 0xeaa9, 0xebad, 0xeca9, 0xedad, 
    0xeead, 0xefa9, 0xf0b5, 0xf1b1, 0xf2b1, 0xf3b5, 0xf4b1, 0xf5b5, 
    0xf6a5, 0xf7a1, 0xf8a9, 0xf9ad, 0xfaad, 0xfba9, 0xfcad, 0xfda9, 
    0xfea9, 0xffad, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515, 
    0x0605, 0x0701, 0x0809, 0x090d, 0x0a0d, 0x0b09, 0x0c0d, 0x0d09, 
    0x0e09, 0x0f0d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511, 
    0x1601, 0x1705, 0x180d, 0x1909, 0x1a09, 0x1b0d, 0x1c09, 0x1d0d, 
    0x1e0d, 0x1f09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531, 
    0x2621, 0x2725, 0x282d, 0x2929, 0x2a29, 0x2b2d, 0x2c29, 0x2d2d, 
    0x2e2d, 0x2f29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535, 
    0x3625, 0x3721, 0x3829, 0x392d, 0x3a2d, 0x3b29, 0x3c2d, 0x3d29, 
    0x3e29, 0x3f2d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511, 
    0x4601, 0x4705, 0x480d, 0x4909, 0x4a09, 0x4b0d, 0x4c09, 0x4d0d, 
    0x4e0d, 0x4f09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515, 
    0x5605, 0x5701, 0x5809, 0x590d, 0x5a0d, 0x5b09, 0x5c0d, 0x5d09, 
    0x5e09, 0x5f0d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535, 
    0x0046, 0x0102, 0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702, 
    0x080a, 0x090e, 0x0402, 0x0506, 0x0606, 0x0702, 0x080a, 0x090e, 
    0x1002, 0x1106, 0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706, 
    0x180e, 0x190a, 0x1406, 0x1502, 0x1602, 0x1706, 0x180e, 0x190a, 
    0x2022, 0x2126, 0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726, 
    0x282e, 0x292a, 0x2426, 0x2522, 0x2622, 0x2726, 0x282e, 0x292a, 
    0x3026, 0x3122, 0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722, 
    0x382a, 0x392e, 0x3422, 0x3526, 0x3626, 0x3722, 0x382a, 0x392e, 
    0x4002, 0x4106, 0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706, 
    0x480e, 0x490a, 0x4406, 0x4502, 0x4602, 0x4706, 0x480e, 0x490a, 
    0x5006, 0x5102, 0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702, 
    0x580a, 0x590e, 0x5402, 0x5506, 0x5606, 0x5702, 0x580a, 0x590e, 
    0x6026, 0x6122, 0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722, 
    0x682a, 0x692e, 0x6422, 0x6526, 0x6626, 0x6722, 0x682a, 0x692e, 
    0x7022, 0x7126, 0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726, 
    0x782e, 0x792a, 0x7426, 0x7522, 0x7622, 0x7726, 0x782e, 0x792a, 
    0x8082, 0x8186, 0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786, 
    0x888e, 0x898a, 0x8486, 0x8582, 0x8682, 0x8786, 0x888e, 0x898a, 
    0x9086, 0x9182, 0x9282, 0x9386, 0x9482, 0x9586, 0x9686, 0x9782, 
    0x988a, 0x998e, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f, 
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 
    0x480f, 0x490b, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b, 
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 
    0x580b, 0x590f, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f, 
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 
    0x682b, 0x692f, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f, 
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 
    0x782f, 0x792b, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b, 
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 
    0x888f, 0x898b, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b, 
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 
    0x988b, 0x998f, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f, 
    0xa0a7, 0xa1a3, 0xa2a3, 0xa3a7, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3, 
    0xa8ab, 0xa9af, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3, 0xa8ab, 0xa9af, 
    0xb0a3, 0xb1a7, 0xb2a7, 0xb3a3, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7, 
    0xb8af, 0xb9ab, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7, 0xb8af, 0xb9ab, 
    0xc087, 0xc183, 0xc283, 0xc387, 0xc483, 0xc587, 0xc687, 0xc783, 
    0xc88b, 0xc98f, 0xc483, 0xc587, 0xc687, 0xc783, 0xc88b, 0xc98f, 
    0xd083, 0xd187, 0xd287, 0xd383, 0xd487, 0xd583, 0xd683, 0xd787, 
    0xd88f, 0xd98b, 0xd487, 0xd583, 0xd683, 0xd787, 0xd88f, 0xd98b, 
    0xe0a3, 0xe1a7, 0xe2a7, 0xe3a3, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7, 
    0xe8af, 0xe9ab, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7, 0xe8af, 0xe9ab, 
    0xf0a7, 0xf1a3, 0xf2a3, 0xf3a7, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3, 
    0xf8ab, 0xf9af, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3, 0xf8ab, 0xf9af, 
    0x0047, 0x0103, 0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703, 
    0x080b, 0x090f, 0x0403, 0x0507, 0x0607, 0x0703, 0x080b, 0x090f, 
    0x1003, 0x1107, 0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707, 
    0x180f, 0x190b, 0x1407, 0x1503, 0x1603, 0x1707, 0x180f, 0x190b, 
    0x2023, 0x2127, 0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727, 
    0x282f, 0x292b, 0x2427, 0x2523, 0x2623, 0x2727, 0x282f, 0x292b, 
    0x3027, 0x3123, 0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723, 
    0x382b, 0x392f, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f, 
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 
    0x480f, 0x490b, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b, 
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 
    0x580b, 0x590f, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f, 
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 
    0x682b, 0x692f, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f, 
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 
    0x782f, 0x792b, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b, 
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 
    0x888f, 0x898b, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b, 
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 
    0x988b, 0x998f, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f, 
    0xfabe, 0xfbba, 0xfcbe, 0xfdba, 0xfeba, 0xffbe, 0x0046, 0x0102, 
    0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702, 0x080a, 0x090e, 
    0x0a1e, 0x0b1a, 0x0c1e, 0x0d1a, 0x0e1a, 0x0f1e, 0x1002, 0x1106, 
    0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706, 0x180e, 0x190a, 
    0x1a1a, 0x1b1e, 0x1c1a, 0x1d1e, 0x1e1e, 0x1f1a, 0x2022, 0x2126, 
    0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726, 0x282e, 0x292a, 
    0x2a3a, 0x2b3e, 0x2c3a, 0x2d3e, 0x2e3e, 0x2f3a, 0x3026, 0x3122, 
    0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722, 0x382a, 0x392e, 
    0x3a3e, 0x3b3a, 0x3c3e, 0x3d3a, 0x3e3a, 0x3f3e, 0x4002, 0x4106, 
    0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706, 0x480e, 0x490a, 
    0x4a1a, 0x4b1e, 0x4c1a, 0x4d1e, 0x4e1e, 0x4f1a, 0x5006, 0x5102, 
    0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702, 0x580a, 0x590e, 
    0x5a1e, 0x5b1a, 0x5c1e, 0x5d1a, 0x5e1a, 0x5f1e, 0x6026, 0x6122, 
    0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722, 0x682a, 0x692e, 
    0x6a3e, 0x6b3a, 0x6c3e, 0x6d3a, 0x6e3a, 0x6f3e, 0x7022, 0x7126, 
    0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726, 0x782e, 0x792a, 
    0x7a3a, 0x7b3e, 0x7c3a, 0x7d3e, 0x7e3e, 0x7f3a, 0x8082, 0x8186, 
    0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786, 0x888e, 0x898a, 
    0x8a9a, 0x8b9e, 0x8c9a, 0x8d9e, 0x8e9e, 0x8f9a, 0x9086, 0x9182, 
    0x9282, 0x9386, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f, 
    0x3a3f, 0x3b3b, 0x3c3f, 0x3d3b, 0x3e3b, 0x3f3f, 0x4003, 0x4107, 
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b, 
    0x4a1b, 0x4b1f, 0x4c1b, 0x4d1f, 0x4e1f, 0x4f1b, 0x5007, 0x5103, 
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f, 
    0x5a1f, 0x5b1b, 0x5c1f, 0x5d1b, 0x5e1b, 0x5f1f, 0x6027, 0x6123, 
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f, 
    0x6a3f, 0x6b3b, 0x6c3f, 0x6d3b, 0x6e3b, 0x6f3f, 0x7023, 0x7127, 
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b, 
    0x7a3b, 0x7b3f, 0x7c3b, 0x7d3f, 0x7e3f, 0x7f3b, 0x8083, 0x8187, 
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b, 
    0x8a9b, 0x8b9f, 0x8c9b, 0x8d9f, 0x8e9f, 0x8f9b, 0x9087, 0x9183, 
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f, 
    0x9a9f, 0x9b9b, 0x9c9f, 0x9d9b, 0x9e9b, 0x9f9f, 0xa0a7, 0xa1a3, 
    0xa2a3, 0xa3a7, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3, 0xa8ab, 0xa9af, 
    0xaabf, 0xabbb, 0xacbf, 0xadbb, 0xaebb, 0xafbf, 0xb0a3, 0xb1a7, 
    0xb2a7, 0xb3a3, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7, 0xb8af, 0xb9ab, 
    0xbabb, 0xbbbf, 0xbcbb, 0xbdbf, 0xbebf, 0xbfbb, 0xc087, 0xc183, 
    0xc283, 0xc387, 0xc483, 0xc587, 0xc687, 0xc783, 0xc88b, 0xc98f, 
    0xca9f, 0xcb9b, 0xcc9f, 0xcd9b, 0xce9b, 0xcf9f, 0xd083, 0xd187, 
    0xd287, 0xd383, 0xd487, 0xd583, 0xd683, 0xd787, 0xd88f, 0xd98b, 
    0xda9b, 0xdb9f, 0xdc9b, 0xdd9f, 0xde9f, 0xdf9b, 0xe0a3, 0xe1a7, 
    0xe2a7, 0xe3a3, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7, 0xe8af, 0xe9ab, 
    0xeabb, 0xebbf, 0xecbb, 0xedbf, 0xeebf, 0xefbb, 0xf0a7, 0xf1a3, 
    0xf2a3, 0xf3a7, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3, 0xf8ab, 0xf9af, 
    0xfabf, 0xfbbb, 0xfcbf, 0xfdbb, 0xfebb, 0xffbf, 0x0047, 0x0103, 
    0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703, 0x080b, 0x090f, 
    0x0a1f, 0x0b1b, 0x0c1f, 0x0d1b, 0x0e1b, 0x0f1f, 0x1003, 0x1107, 
    0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707, 0x180f, 0x190b, 
    0x1a1b, 0x1b1f, 0x1c1b, 0x1d1f, 0x1e1f, 0x1f1b, 0x2023, 0x2127, 
    0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727, 0x282f, 0x292b, 
    0x2a3b, 0x2b3f, 0x2c3b, 0x2d3f, 0x2e3f, 0x2f3b, 0x3027, 0x3123, 
    0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f, 
    0x3a3f, 0x3b3b, 0x3c3f, 0x3d3b, 0x3e3b, 0x3f3f, 0x4003, 0x4107, 
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b, 
    0x4a1b, 0x4b1f, 0x4c1b, 0x4d1f, 0x4e1f, 0x4f1b, 0x5007, 0x5103, 
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f, 
    0x5a1f, 0x5b1b, 0x5c1f, 0x5d1b, 0x5e1b, 0x5f1f, 0x6027, 0x6123, 
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f, 
    0x6a3f, 0x6b3b, 0x6c3f, 0x6d3b, 0x6e3b, 0x6f3f, 0x7023, 0x7127, 
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b, 
    0x7a3b, 0x7b3f, 0x7c3b, 0x7d3f, 0x7e3f, 0x7f3b, 0x8083, 0x8187, 
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b, 
    0x8a9b, 0x8b9f, 0x8c9b, 0x8d9f, 0x8e9f, 0x8f9b, 0x9087, 0x9183, 
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f
};

static uint8_t Cycles[256] = {
    4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
    8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
    7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
    7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
    5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
    5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
    5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11
};

static uint8_t CyclesED[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   12,12,15,20, 8,14, 8, 9,12,12,15,20, 0,14, 0, 9,
   12,12,15,20, 0, 0, 8, 9,12,12,15,20, 0, 0, 8, 9,
   12,12,15,20, 0, 0, 0,18,12,12,15,20, 0, 0, 0,18,
   12, 0,15,20, 0, 0, 0, 0,12,12,15,20, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   16,16,16,16, 0, 0, 0, 0,16,16,16,16, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t CyclesXX[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0,14,20,10, 9, 9, 9, 0, 0,15,20,10, 9, 9, 9, 0,
    0, 0, 0, 0,23,23,19, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
   19,19,19,19,19,19,19,19, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,14, 0,23, 0,15, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0
};

static uint8_t ZSTable[256] = {
  _ZF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF,
  _SF,_SF,_SF,_SF,_SF,_SF,_SF,_SF
};

static uint8_t PZSTable[256] = {
  _ZF|_PF,0,0,_PF,0,_PF,_PF,0,
  0,_PF,_PF,0,_PF,0,0,_PF,
  0,_PF,_PF,0,_PF,0,0,_PF,_PF,0,0,_PF,0,_PF,_PF,0,
  0,_PF,_PF,0,_PF,0,0,_PF,_PF,0,0,_PF,0,_PF,_PF,0,
  _PF,0,0,_PF,0,_PF,_PF,0,0,_PF,_PF,0,_PF,0,0,_PF,
  0,_PF,_PF,0,_PF,0,0,_PF,_PF,0,0,_PF,0,_PF,_PF,0,
  _PF,0,0,_PF,0,_PF,_PF,0,0,_PF,_PF,0,_PF,0,0,_PF,
  _PF,0,0,_PF,0,_PF,_PF,0,0,_PF,_PF,0,_PF,0,0,_PF,
  0,_PF,_PF,0,_PF,0,0,_PF,_PF,0,0,_PF,0,_PF,_PF,0,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF,_SF|_PF,_SF|_PF,_SF,
  _SF|_PF,_SF,_SF,_SF|_PF
};

static void xcpc_cpu_z80a_trace(const char* function)
{
    xcpc_trace("XcpcCpuZ80a::%s()", function);
}

XcpcCpuZ80a* xcpc_cpu_z80a_alloc(void)
{
    xcpc_cpu_z80a_trace("alloc");

    return xcpc_new(XcpcCpuZ80a);
}

XcpcCpuZ80a* xcpc_cpu_z80a_free(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("free");

    return xcpc_delete(XcpcCpuZ80a, self);
}

XcpcCpuZ80a* xcpc_cpu_z80a_construct(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcCpuZ80aIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcCpuZ80aState));
    }
    return xcpc_cpu_z80a_reset(self);
}

XcpcCpuZ80a* xcpc_cpu_z80a_destruct(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("destruct");

    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_new(void)
{
    xcpc_cpu_z80a_trace("new");

    return xcpc_cpu_z80a_construct(xcpc_cpu_z80a_alloc());
}

XcpcCpuZ80a* xcpc_cpu_z80a_delete(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("delete");

    return xcpc_cpu_z80a_free(xcpc_cpu_z80a_destruct(self));
}

XcpcCpuZ80a* xcpc_cpu_z80a_reset(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("reset");

    /* reset state.regs */ {
        self->state.regs.AF.q = 0;
        self->state.regs.BC.q = 0;
        self->state.regs.DE.q = 0;
        self->state.regs.HL.q = 0;
        self->state.regs.IX.q = 0;
        self->state.regs.IY.q = 0;
        self->state.regs.SP.q = 0;
        self->state.regs.PC.q = 0;
        self->state.regs.IR.q = 0;
        self->state.regs.IF.q = 0;
    }
    /* reset state.ctrs */ {
        self->state.ctrs.m_cycles = 0;
        self->state.ctrs.t_states = 0;
        self->state.ctrs.ccounter = 0;
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_clock(XcpcCpuZ80a* self)
{
  uint8_t opcode;
  union XcpcCpuZ80aRegister T1;
  union XcpcCpuZ80aRegister T2;
  union XcpcCpuZ80aRegister WZ;

#define TMP1 T1.b.l
#define TMP2 T2.b.l

next:
  if(self->state.ctrs.ccounter <= 0) {
    return self;
  }
  else if(self->state.regs.IF.w.l & _NMI) {
    self->state.regs.IF.w.l &= ~(_HLT | _NMI | _INT | _IFF2);
    (*self->iface.mreq_wr)(self, --self->state.regs.SP.w.l, self->state.regs.PC.b.h);
    (*self->iface.mreq_wr)(self, --self->state.regs.SP.w.l, self->state.regs.PC.b.l);
    self->state.regs.PC.w.l = 0x0066;
    self->state.ctrs.m_cycles += 3;
    self->state.ctrs.t_states += 11;
    self->state.ctrs.ccounter -= 11;
    goto next;
  }
  else if(self->state.regs.IF.w.l & _INT) {
    self->state.regs.IF.w.l &= ~(_HLT | _NMI | _INT | _IFF2 | _IFF1);
    (void) (*self->iface.iorq_m1)(self, 0x0000);
    (*self->iface.mreq_wr)(self, --self->state.regs.SP.w.l, self->state.regs.PC.b.h);
    (*self->iface.mreq_wr)(self, --self->state.regs.SP.w.l, self->state.regs.PC.b.l);
    self->state.regs.PC.w.l = 0x0038;
    self->state.ctrs.m_cycles += 3;
    self->state.ctrs.t_states += 13;
    self->state.ctrs.ccounter -= 13;
    goto next;
  }
  else if(self->state.regs.IF.w.l & _HLT) {
    self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
    self->state.ctrs.m_cycles += 1;
    self->state.ctrs.t_states += 4;
    self->state.ctrs.ccounter -= 4;
    goto next;
  }
  else {
    goto fetch;
  }

fetch:
  self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
  opcode = (*self->iface.mreq_m1)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute;

decode_and_execute:
  switch(opcode) {
#include "cpu-z80a-opcodes.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x23: /* INC HL            */
      self->state.regs.HL.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x26: /* LD H,n            */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & _NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x2b: /* DEC HL            */
      self->state.regs.HL.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x2e: /* LD L,n            */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.ccounter -= 6;
      goto next;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x44: /* LD B,H            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x45: /* LD B,L            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x46: /* LD B,(HL)         */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x4c: /* LD C,H            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x4d: /* LD C,L            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x4e: /* LD C,(HL)         */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x54: /* LD D,H            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x55: /* LD D,L            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x56: /* LD D,(HL)         */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x5c: /* LD E,H            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x5d: /* LD E,L            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x5e: /* LD E,(HL)         */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x60: /* LD H,B            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x61: /* LD H,C            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x62: /* LD H,D            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x63: /* LD H,E            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x64: /* LD H,H            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x65: /* LD H,L            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x66: /* LD H,(HL)         */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x67: /* LD H,A            */
      self->state.regs.HL.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x68: /* LD L,B            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x69: /* LD L,C            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x6a: /* LD L,D            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x6b: /* LD L,E            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x6c: /* LD L,H            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x6d: /* LD L,L            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x6e: /* LD L,(HL)         */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x6f: /* LD L,A            */
      self->state.regs.HL.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x70: /* LD (HL),B         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x71: /* LD (HL),C         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x72: /* LD (HL),D         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x73: /* LD (HL),E         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x74: /* LD (HL),H         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x75: /* LD (HL),L         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x76: /* HALT              */
      self->state.regs.IF.w.l |= _HLT;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x77: /* LD (HL),A         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x7c: /* LD A,H            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x7d: /* LD A,L            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x7e: /* LD A,(HL)         */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x84: /* ADD A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x85: /* ADD A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x86: /* ADD A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x8c: /* ADC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x8d: /* ADC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x8e: /* ADC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x94: /* SUB A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x95: /* SUB A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x96: /* SUB A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x9c: /* SBC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x9d: /* SBC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0x9e: /* SBC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa4: /* AND A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa5: /* AND A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa6: /* AND A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xac: /* XOR A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xad: /* XOR A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xae: /* XOR A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb4: /* OR A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb5: /* OR A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb6: /* OR A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xbc: /* CP A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xbd: /* CP A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xbe: /* CP A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.ccounter -= 4;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xcb:
      goto fetch_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xdd:
      goto fetch_dd;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xed:
      goto fetch_ed;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
    case 0xfd:
      goto fetch_fd;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.ccounter -= 7;
      goto next;
  }
  self->state.ctrs.t_states += Cycles[opcode];
  self->state.ctrs.ccounter -= Cycles[opcode];
  goto next;

fetch_cb:
  self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
  opcode = (*self->iface.mreq_m1)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_cb;

decode_and_execute_cb:
  switch(opcode) {
    case 0x00: /* RLC B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x01: /* RLC C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x02: /* RLC D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x03: /* RLC E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x04: /* RLC H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x05: /* RLC L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x06: /* RLC (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x07: /* RLC A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x08: /* RRC B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x09: /* RRC C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0a: /* RRC D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0b: /* RRC E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0c: /* RRC H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0d: /* RRC L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0e: /* RRC (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x0f: /* RRC A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x10: /* RL  B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x11: /* RL  C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x12: /* RL  D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x13: /* RL  E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x14: /* RL  H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x15: /* RL  L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x16: /* RL  (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x17: /* RL  A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x18: /* RR  B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x19: /* RR  C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1a: /* RR  D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1b: /* RR  E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1c: /* RR  H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1d: /* RR  L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1e: /* RR  (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x1f: /* RR  A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x20: /* SLA B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x21: /* SLA C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x22: /* SLA D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x23: /* SLA E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x24: /* SLA H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x25: /* SLA L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x26: /* SLA (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x27: /* SLA A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x28: /* SRA B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x29: /* SRA C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2a: /* SRA D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2b: /* SRA E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2c: /* SRA H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2d: /* SRA L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2e: /* SRA (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x2f: /* SRA A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x30: /* SLL B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x31: /* SLL C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x32: /* SLL D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x33: /* SLL E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x34: /* SLL H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x35: /* SLL L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x36: /* SLL (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x37: /* SLL A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x38: /* SRL B             */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x39: /* SRL C             */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x3a: /* SRL D             */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x3b: /* SRL E             */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x3c: /* SRL H             */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x3d: /* SRL L             */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x3e: /* SRL (HL)          */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x3f: /* SRL A             */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x40: /* BIT 0,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x41: /* BIT 0,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x42: /* BIT 0,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x43: /* BIT 0,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x44: /* BIT 0,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x45: /* BIT 0,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x46: /* BIT 0,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x47: /* BIT 0,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x48: /* BIT 1,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x49: /* BIT 1,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4a: /* BIT 1,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4b: /* BIT 1,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4c: /* BIT 1,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4d: /* BIT 1,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4e: /* BIT 1,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x4f: /* BIT 1,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x50: /* BIT 2,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x51: /* BIT 2,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x52: /* BIT 2,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x53: /* BIT 2,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x54: /* BIT 2,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x55: /* BIT 2,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x56: /* BIT 2,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x57: /* BIT 2,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x58: /* BIT 3,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x59: /* BIT 3,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5a: /* BIT 3,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5b: /* BIT 3,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5c: /* BIT 3,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5d: /* BIT 3,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5e: /* BIT 3,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x5f: /* BIT 3,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x60: /* BIT 4,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x61: /* BIT 4,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x62: /* BIT 4,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x63: /* BIT 4,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x64: /* BIT 4,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x65: /* BIT 4,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x66: /* BIT 4,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x67: /* BIT 4,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x68: /* BIT 5,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x69: /* BIT 5,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6a: /* BIT 5,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6b: /* BIT 5,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6c: /* BIT 5,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6d: /* BIT 5,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6e: /* BIT 5,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x6f: /* BIT 5,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x70: /* BIT 6,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x71: /* BIT 6,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x72: /* BIT 6,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x73: /* BIT 6,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x74: /* BIT 6,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x75: /* BIT 6,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x76: /* BIT 6,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x77: /* BIT 6,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x78: /* BIT 7,B           */
      T1.b.l = self->state.regs.BC.b.h;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x79: /* BIT 7,C           */
      T1.b.l = self->state.regs.BC.b.l;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7a: /* BIT 7,D           */
      T1.b.l = self->state.regs.DE.b.h;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7b: /* BIT 7,E           */
      T1.b.l = self->state.regs.DE.b.l;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7c: /* BIT 7,H           */
      T1.b.l = self->state.regs.HL.b.h;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7d: /* BIT 7,L           */
      T1.b.l = self->state.regs.HL.b.l;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7e: /* BIT 7,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 12;
      self->state.ctrs.ccounter -= 12;
      goto next;
    case 0x7f: /* BIT 7,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x80: /* RES 0,B           */
      self->state.regs.BC.b.h &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x81: /* RES 0,C           */
      self->state.regs.BC.b.l &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x82: /* RES 0,D           */
      self->state.regs.DE.b.h &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x83: /* RES 0,E           */
      self->state.regs.DE.b.l &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x84: /* RES 0,H           */
      self->state.regs.HL.b.h &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x85: /* RES 0,L           */
      self->state.regs.HL.b.l &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x86: /* RES 0,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x87: /* RES 0,A           */
      self->state.regs.AF.b.h &= 0xfe;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x88: /* RES 1,B           */
      self->state.regs.BC.b.h &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x89: /* RES 1,C           */
      self->state.regs.BC.b.l &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8a: /* RES 1,D           */
      self->state.regs.DE.b.h &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8b: /* RES 1,E           */
      self->state.regs.DE.b.l &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8c: /* RES 1,H           */
      self->state.regs.HL.b.h &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8d: /* RES 1,L           */
      self->state.regs.HL.b.l &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8e: /* RES 1,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x8f: /* RES 1,A           */
      self->state.regs.AF.b.h &= 0xfd;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x90: /* RES 2,B           */
      self->state.regs.BC.b.h &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x91: /* RES 2,C           */
      self->state.regs.BC.b.l &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x92: /* RES 2,D           */
      self->state.regs.DE.b.h &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x93: /* RES 2,E           */
      self->state.regs.DE.b.l &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x94: /* RES 2,H           */
      self->state.regs.HL.b.h &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x95: /* RES 2,L           */
      self->state.regs.HL.b.l &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x96: /* RES 2,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x97: /* RES 2,A           */
      self->state.regs.AF.b.h &= 0xfb;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x98: /* RES 3,B           */
      self->state.regs.BC.b.h &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x99: /* RES 3,C           */
      self->state.regs.BC.b.l &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9a: /* RES 3,D           */
      self->state.regs.DE.b.h &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9b: /* RES 3,E           */
      self->state.regs.DE.b.l &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9c: /* RES 3,H           */
      self->state.regs.HL.b.h &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9d: /* RES 3,L           */
      self->state.regs.HL.b.l &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9e: /* RES 3,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0x9f: /* RES 3,A           */
      self->state.regs.AF.b.h &= 0xf7;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa0: /* RES 4,B           */
      self->state.regs.BC.b.h &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa1: /* RES 4,C           */
      self->state.regs.BC.b.l &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa2: /* RES 4,D           */
      self->state.regs.DE.b.h &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa3: /* RES 4,E           */
      self->state.regs.DE.b.l &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa4: /* RES 4,H           */
      self->state.regs.HL.b.h &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa5: /* RES 4,L           */
      self->state.regs.HL.b.l &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa6: /* RES 4,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xa7: /* RES 4,A           */
      self->state.regs.AF.b.h &= 0xef;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa8: /* RES 5,B           */
      self->state.regs.BC.b.h &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa9: /* RES 5,C           */
      self->state.regs.BC.b.l &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xaa: /* RES 5,D           */
      self->state.regs.DE.b.h &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xab: /* RES 5,E           */
      self->state.regs.DE.b.l &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xac: /* RES 5,H           */
      self->state.regs.HL.b.h &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xad: /* RES 5,L           */
      self->state.regs.HL.b.l &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xae: /* RES 5,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xaf: /* RES 5,A           */
      self->state.regs.AF.b.h &= 0xdf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb0: /* RES 6,B           */
      self->state.regs.BC.b.h &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb1: /* RES 6,C           */
      self->state.regs.BC.b.l &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb2: /* RES 6,D           */
      self->state.regs.DE.b.h &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb3: /* RES 6,E           */
      self->state.regs.DE.b.l &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb4: /* RES 6,H           */
      self->state.regs.HL.b.h &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb5: /* RES 6,L           */
      self->state.regs.HL.b.l &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb6: /* RES 6,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xb7: /* RES 6,A           */
      self->state.regs.AF.b.h &= 0xbf;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb8: /* RES 7,B           */
      self->state.regs.BC.b.h &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb9: /* RES 7,C           */
      self->state.regs.BC.b.l &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xba: /* RES 7,D           */
      self->state.regs.DE.b.h &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbb: /* RES 7,E           */
      self->state.regs.DE.b.l &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbc: /* RES 7,H           */
      self->state.regs.HL.b.h &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbd: /* RES 7,L           */
      self->state.regs.HL.b.l &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbe: /* RES 7,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xbf: /* RES 7,A           */
      self->state.regs.AF.b.h &= 0x7f;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc0: /* SET 0,B           */
      self->state.regs.BC.b.h |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc1: /* SET 0,C           */
      self->state.regs.BC.b.l |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc2: /* SET 0,D           */
      self->state.regs.DE.b.h |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc3: /* SET 0,E           */
      self->state.regs.DE.b.l |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc4: /* SET 0,H           */
      self->state.regs.HL.b.h |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc5: /* SET 0,L           */
      self->state.regs.HL.b.l |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc6: /* SET 0,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xc7: /* SET 0,A           */
      self->state.regs.AF.b.h |= 0x01;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc8: /* SET 1,B           */
      self->state.regs.BC.b.h |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc9: /* SET 1,C           */
      self->state.regs.BC.b.l |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xca: /* SET 1,D           */
      self->state.regs.DE.b.h |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xcb: /* SET 1,E           */
      self->state.regs.DE.b.l |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xcc: /* SET 1,H           */
      self->state.regs.HL.b.h |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xcd: /* SET 1,L           */
      self->state.regs.HL.b.l |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xce: /* SET 1,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xcf: /* SET 1,A           */
      self->state.regs.AF.b.h |= 0x02;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd0: /* SET 2,B           */
      self->state.regs.BC.b.h |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd1: /* SET 2,C           */
      self->state.regs.BC.b.l |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd2: /* SET 2,D           */
      self->state.regs.DE.b.h |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd3: /* SET 2,E           */
      self->state.regs.DE.b.l |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd4: /* SET 2,H           */
      self->state.regs.HL.b.h |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd5: /* SET 2,L           */
      self->state.regs.HL.b.l |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd6: /* SET 2,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xd7: /* SET 2,A           */
      self->state.regs.AF.b.h |= 0x04;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd8: /* SET 3,B           */
      self->state.regs.BC.b.h |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xd9: /* SET 3,C           */
      self->state.regs.BC.b.l |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xda: /* SET 3,D           */
      self->state.regs.DE.b.h |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xdb: /* SET 3,E           */
      self->state.regs.DE.b.l |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xdc: /* SET 3,H           */
      self->state.regs.HL.b.h |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xdd: /* SET 3,L           */
      self->state.regs.HL.b.l |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xde: /* SET 3,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xdf: /* SET 3,A           */
      self->state.regs.AF.b.h |= 0x08;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe0: /* SET 4,B           */
      self->state.regs.BC.b.h |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe1: /* SET 4,C           */
      self->state.regs.BC.b.l |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe2: /* SET 4,D           */
      self->state.regs.DE.b.h |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe3: /* SET 4,E           */
      self->state.regs.DE.b.l |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe4: /* SET 4,H           */
      self->state.regs.HL.b.h |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe5: /* SET 4,L           */
      self->state.regs.HL.b.l |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe6: /* SET 4,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xe7: /* SET 4,A           */
      self->state.regs.AF.b.h |= 0x10;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe8: /* SET 5,B           */
      self->state.regs.BC.b.h |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xe9: /* SET 5,C           */
      self->state.regs.BC.b.l |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xea: /* SET 5,D           */
      self->state.regs.DE.b.h |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xeb: /* SET 5,E           */
      self->state.regs.DE.b.l |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xec: /* SET 5,H           */
      self->state.regs.HL.b.h |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xed: /* SET 5,L           */
      self->state.regs.HL.b.l |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xee: /* SET 5,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xef: /* SET 5,A           */
      self->state.regs.AF.b.h |= 0x20;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf0: /* SET 6,B           */
      self->state.regs.BC.b.h |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf1: /* SET 6,C           */
      self->state.regs.BC.b.l |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf2: /* SET 6,D           */
      self->state.regs.DE.b.h |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf3: /* SET 6,E           */
      self->state.regs.DE.b.l |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf4: /* SET 6,H           */
      self->state.regs.HL.b.h |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf5: /* SET 6,L           */
      self->state.regs.HL.b.l |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf6: /* SET 6,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xf7: /* SET 6,A           */
      self->state.regs.AF.b.h |= 0x40;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf8: /* SET 7,B           */
      self->state.regs.BC.b.h |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xf9: /* SET 7,C           */
      self->state.regs.BC.b.l |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xfa: /* SET 7,D           */
      self->state.regs.DE.b.h |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xfb: /* SET 7,E           */
      self->state.regs.DE.b.l |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xfc: /* SET 7,H           */
      self->state.regs.HL.b.h |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xfd: /* SET 7,L           */
      self->state.regs.HL.b.l |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xfe: /* SET 7,(HL)        */
      T1.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 4;
      self->state.ctrs.t_states += 15;
      self->state.ctrs.ccounter -= 15;
      goto next;
    case 0xff: /* SET 7,A           */
      self->state.regs.AF.b.h |= 0x80;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
  }

fetch_dd:
  self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
  opcode = (*self->iface.mreq_m1)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_dd;

decode_and_execute_dd:
  switch(opcode) {
#include "cpu-z80a-opcodes-dd.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x23: /* INC IX            */
      self->state.regs.IX.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x26: /* LD IXh,n          */
      self->state.regs.IX.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & _NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IX            */
      self->state.regs.IX.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x2e: /* LD IXl,n          */
      self->state.regs.IX.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IXh          */
      self->state.regs.BC.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IXl          */
      self->state.regs.BC.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IX+d)       */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IXh          */
      self->state.regs.BC.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IXl          */
      self->state.regs.BC.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IX+d)       */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IXh          */
      self->state.regs.DE.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IXl          */
      self->state.regs.DE.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IX+d)       */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IXh          */
      self->state.regs.DE.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IXl          */
      self->state.regs.DE.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IX+d)       */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x60: /* LD IXh,B          */
      self->state.regs.IX.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x61: /* LD IXh,C          */
      self->state.regs.IX.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x62: /* LD IXh,D          */
      self->state.regs.IX.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x63: /* LD IXh,E          */
      self->state.regs.IX.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x64: /* LD IXh,IXh        */
      self->state.regs.IX.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x65: /* LD IXh,IXl        */
      self->state.regs.IX.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IX+d)       */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x67: /* LD IXh,A          */
      self->state.regs.IX.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x68: /* LD IXl,B          */
      self->state.regs.IX.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x69: /* LD IXl,C          */
      self->state.regs.IX.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6a: /* LD IXl,D          */
      self->state.regs.IX.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6b: /* LD IXl,E          */
      self->state.regs.IX.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6c: /* LD IXl,IXh        */
      self->state.regs.IX.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6d: /* LD IXl,IXl        */
      self->state.regs.IX.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IX+d)       */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x6f: /* LD IXl,A          */
      self->state.regs.IX.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x70: /* LD (IX+d),B       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x71: /* LD (IX+d),C       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x72: /* LD (IX+d),D       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x73: /* LD (IX+d),E       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x74: /* LD (IX+d),H       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x75: /* LD (IX+d),L       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x76: /* HALT              */
      self->state.regs.IF.w.l |= _HLT;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x77: /* LD (IX+d),A       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IXh          */
      self->state.regs.AF.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IXl          */
      self->state.regs.AF.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IX+d)       */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IXh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IXl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IX+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IXh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IXl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IX+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xcb:
      goto fetch_dd_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
  }
  self->state.ctrs.t_states += CyclesXX[opcode];
  self->state.ctrs.ccounter -= CyclesXX[opcode];
  goto next;

fetch_ed:
  self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
  opcode = (*self->iface.mreq_m1)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_ed;

decode_and_execute_ed:
  switch(opcode) {
#include "cpu-z80a-opcodes-ed.inc"
    default:
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
  }
  self->state.ctrs.t_states += CyclesED[opcode];
  self->state.ctrs.ccounter -= CyclesED[opcode];
  goto next;

fetch_fd:
  self->state.regs.IR.b.l = (self->state.regs.IR.b.l & 0x80) | ((self->state.regs.IR.b.l + 1) & 0x7f);
  opcode = (*self->iface.mreq_m1)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_fd;

decode_and_execute_fd:
  switch(opcode) {
#include "cpu-z80a-opcodes-fd.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x23: /* INC IY            */
      self->state.regs.IY.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x26: /* LD IYh,n          */
      self->state.regs.IY.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & _NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IY            */
      self->state.regs.IY.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x2e: /* LD IYl,n          */
      self->state.regs.IY.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IYh          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IYl          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IY+d)       */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IYh          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IYl          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IY+d)       */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IYh          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IYl          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IY+d)       */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IYh          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IYl          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IY+d)       */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x60: /* LD IYh,B          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x61: /* LD IYh,C          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x62: /* LD IYh,D          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x63: /* LD IYh,E          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x64: /* LD IYh,IYh        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x65: /* LD IYh,IYl        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IY+d)       */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x67: /* LD IYh,A          */
      self->state.regs.IY.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x68: /* LD IYl,B          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x69: /* LD IYl,C          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6a: /* LD IYl,D          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6b: /* LD IYl,E          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6c: /* LD IYl,IYh        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6d: /* LD IYl,IYl        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IY+d)       */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x6f: /* LD IYl,A          */
      self->state.regs.IY.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x70: /* LD (IY+d),B       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x71: /* LD (IY+d),C       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x72: /* LD (IY+d),D       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x73: /* LD (IY+d),E       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x74: /* LD (IY+d),H       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x75: /* LD (IY+d),L       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x76: /* HALT              */
      self->state.regs.IF.w.l |= _HLT;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x77: /* LD (IY+d),A       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)), self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IYh          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IYl          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IY+d)       */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.ccounter -= 8;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xcb:
      goto fetch_fd_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.ccounter -= 11;
      goto next;
  }
  self->state.ctrs.t_states += CyclesXX[opcode];
  self->state.ctrs.ccounter -= CyclesXX[opcode];
  goto next;

fetch_dd_cb:
  WZ.w.l = self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
  opcode = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_dd_cb;

decode_and_execute_dd_cb:
  switch(opcode) {
    case 0x00: /* LD B,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x01: /* LD C,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x02: /* LD D,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x03: /* LD E,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x04: /* LD H,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x05: /* LD L,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x06: /* RLC (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x07: /* LD A,RLC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x08: /* LD B,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x09: /* LD C,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0a: /* LD D,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0b: /* LD E,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0c: /* LD H,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0d: /* LD L,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0f: /* LD A,RRC (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x10: /* LD B,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x11: /* LD C,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x12: /* LD D,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x13: /* LD E,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x14: /* LD H,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x15: /* LD L,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x17: /* LD A,RL  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x18: /* LD B,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x19: /* LD C,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1a: /* LD D,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1b: /* LD E,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1c: /* LD H,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1d: /* LD L,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1f: /* LD A,RR  (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x20: /* LD B,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x21: /* LD C,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x22: /* LD D,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x23: /* LD E,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x24: /* LD H,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x25: /* LD L,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x27: /* LD A,SLA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x28: /* LD B,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x29: /* LD C,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2a: /* LD D,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2b: /* LD E,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2c: /* LD H,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2d: /* LD L,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2f: /* LD A,SRA (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x30: /* LD B,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x31: /* LD C,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x32: /* LD D,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x33: /* LD E,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x34: /* LD H,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x35: /* LD L,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x37: /* LD A,SLL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x38: /* LD B,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x39: /* LD C,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3a: /* LD D,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3b: /* LD E,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3c: /* LD H,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3d: /* LD L,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IX+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3f: /* LD A,SRL (IX+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x40: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x41: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x42: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x43: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x44: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x45: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x46: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x47: /* BIT 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x48: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x49: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4a: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4b: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4c: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4d: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4f: /* BIT 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x50: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x51: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x52: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x53: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x54: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x55: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x57: /* BIT 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x58: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x59: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5a: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5b: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5c: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5d: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5f: /* BIT 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x60: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x61: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x62: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x63: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x64: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x65: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x67: /* BIT 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x68: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x69: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6a: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6b: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6c: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6d: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6f: /* BIT 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x70: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x71: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x72: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x73: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x74: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x75: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x77: /* BIT 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x78: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x79: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7a: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7b: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7c: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7d: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7f: /* BIT 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x80: /* LD B,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x81: /* LD C,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x82: /* LD D,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x83: /* LD E,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x84: /* LD H,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x85: /* LD L,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x86: /* RES 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x87: /* LD A,RES 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x88: /* LD B,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x89: /* LD C,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8a: /* LD D,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8b: /* LD E,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8c: /* LD H,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8d: /* LD L,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8f: /* LD A,RES 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x90: /* LD B,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x91: /* LD C,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x92: /* LD D,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x93: /* LD E,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x94: /* LD H,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x95: /* LD L,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x97: /* LD A,RES 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x98: /* LD B,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x99: /* LD C,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9a: /* LD D,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9b: /* LD E,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9c: /* LD H,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9d: /* LD L,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9f: /* LD A,RES 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa0: /* LD B,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa1: /* LD C,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa2: /* LD D,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa3: /* LD E,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa4: /* LD H,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa5: /* LD L,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa7: /* LD A,RES 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa8: /* LD B,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa9: /* LD C,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xaa: /* LD D,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xab: /* LD E,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xac: /* LD H,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xad: /* LD L,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xaf: /* LD A,RES 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb0: /* LD B,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb1: /* LD C,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb2: /* LD D,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb3: /* LD E,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb4: /* LD H,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb5: /* LD L,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb7: /* LD A,RES 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb8: /* LD B,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb9: /* LD C,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xba: /* LD D,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbb: /* LD E,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbc: /* LD H,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbd: /* LD L,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbf: /* LD A,RES 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc0: /* LD B,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc1: /* LD C,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc2: /* LD D,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc3: /* LD E,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc4: /* LD H,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc5: /* LD L,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc7: /* LD A,SET 0,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc8: /* LD B,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc9: /* LD C,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xca: /* LD D,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcb: /* LD E,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcc: /* LD H,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcd: /* LD L,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcf: /* LD A,SET 1,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd0: /* LD B,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd1: /* LD C,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd2: /* LD D,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd3: /* LD E,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd4: /* LD H,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd5: /* LD L,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd7: /* LD A,SET 2,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd8: /* LD B,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd9: /* LD C,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xda: /* LD D,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdb: /* LD E,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdc: /* LD H,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdd: /* LD L,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdf: /* LD A,SET 3,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe0: /* LD B,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe1: /* LD C,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe2: /* LD D,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe3: /* LD E,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe4: /* LD H,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe5: /* LD L,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe7: /* LD A,SET 4,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe8: /* LD B,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe9: /* LD C,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xea: /* LD D,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xeb: /* LD E,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xec: /* LD H,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xed: /* LD L,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xef: /* LD A,SET 5,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf0: /* LD B,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf1: /* LD C,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf2: /* LD D,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf3: /* LD E,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf4: /* LD H,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf5: /* LD L,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf7: /* LD A,SET 6,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf8: /* LD B,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf9: /* LD C,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfa: /* LD D,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfb: /* LD E,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfc: /* LD H,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfd: /* LD L,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IX+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xff: /* LD A,SET 7,(IX+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
  }

fetch_fd_cb:
  WZ.w.l = self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
  opcode = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++);
  goto decode_and_execute_fd_cb;

decode_and_execute_fd_cb:
  switch(opcode) {
    case 0x00: /* LD B,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x01: /* LD C,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x02: /* LD D,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x03: /* LD E,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x04: /* LD H,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x05: /* LD L,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x06: /* RLC (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x07: /* LD A,RLC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x08: /* LD B,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x09: /* LD C,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0a: /* LD D,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0b: /* LD E,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0c: /* LD H,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0d: /* LD L,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x0f: /* LD A,RRC (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x10: /* LD B,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x11: /* LD C,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x12: /* LD D,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x13: /* LD E,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x14: /* LD H,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x15: /* LD L,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x17: /* LD A,RL  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x18: /* LD B,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x19: /* LD C,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1a: /* LD D,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1b: /* LD E,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1c: /* LD H,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1d: /* LD L,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x1f: /* LD A,RR  (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x20: /* LD B,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x21: /* LD C,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x22: /* LD D,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x23: /* LD E,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x24: /* LD H,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x25: /* LD L,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x27: /* LD A,SLA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) << 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x28: /* LD B,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x29: /* LD C,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2a: /* LD D,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2b: /* LD E,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2c: /* LD H,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2d: /* LD L,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x2f: /* LD A,SRA (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = ((int8_t) T1.b.l) >> 1;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x30: /* LD B,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x31: /* LD C,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x32: /* LD D,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x33: /* LD E,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x34: /* LD H,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x35: /* LD L,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x37: /* LD A,SLL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x38: /* LD B,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x39: /* LD C,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3a: /* LD D,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3b: /* LD E,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3c: /* LD H,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3d: /* LD L,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IY+d)        */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x3f: /* LD A,SRL (IY+d)   */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= _CF;
      }
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x40: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x41: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x42: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x43: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x44: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x45: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x46: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x47: /* BIT 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x48: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x49: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4a: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4b: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4c: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4d: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x4f: /* BIT 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x50: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x51: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x52: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x53: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x54: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x55: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x57: /* BIT 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x58: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x59: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5a: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5b: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5c: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5d: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x5f: /* BIT 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x60: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x61: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x62: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x63: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x64: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x65: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x67: /* BIT 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x68: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x69: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6a: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6b: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6c: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6d: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x6f: /* BIT 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x70: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x71: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x72: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x73: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x74: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x75: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x77: /* BIT 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x78: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x79: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7a: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7b: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7c: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7d: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x7f: /* BIT 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 20;
      self->state.ctrs.ccounter -= 20;
      goto next;
    case 0x80: /* LD B,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x81: /* LD C,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x82: /* LD D,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x83: /* LD E,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x84: /* LD H,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x85: /* LD L,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x86: /* RES 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x87: /* LD A,RES 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x88: /* LD B,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x89: /* LD C,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8a: /* LD D,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8b: /* LD E,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8c: /* LD H,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8d: /* LD L,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x8f: /* LD A,RES 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x90: /* LD B,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x91: /* LD C,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x92: /* LD D,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x93: /* LD E,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x94: /* LD H,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x95: /* LD L,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x97: /* LD A,RES 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x98: /* LD B,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x99: /* LD C,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9a: /* LD D,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9b: /* LD E,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9c: /* LD H,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9d: /* LD L,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0x9f: /* LD A,RES 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa0: /* LD B,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa1: /* LD C,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa2: /* LD D,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa3: /* LD E,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa4: /* LD H,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa5: /* LD L,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa7: /* LD A,RES 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa8: /* LD B,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xa9: /* LD C,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xaa: /* LD D,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xab: /* LD E,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xac: /* LD H,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xad: /* LD L,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xaf: /* LD A,RES 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb0: /* LD B,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb1: /* LD C,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb2: /* LD D,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb3: /* LD E,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb4: /* LD H,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb5: /* LD L,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb7: /* LD A,RES 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb8: /* LD B,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xb9: /* LD C,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xba: /* LD D,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbb: /* LD E,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbc: /* LD H,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbd: /* LD L,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xbf: /* LD A,RES 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc0: /* LD B,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc1: /* LD C,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc2: /* LD D,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc3: /* LD E,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc4: /* LD H,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc5: /* LD L,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc7: /* LD A,SET 0,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc8: /* LD B,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xc9: /* LD C,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xca: /* LD D,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcb: /* LD E,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcc: /* LD H,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcd: /* LD L,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xcf: /* LD A,SET 1,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd0: /* LD B,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd1: /* LD C,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd2: /* LD D,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd3: /* LD E,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd4: /* LD H,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd5: /* LD L,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd7: /* LD A,SET 2,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd8: /* LD B,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xd9: /* LD C,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xda: /* LD D,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdb: /* LD E,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdc: /* LD H,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdd: /* LD L,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xdf: /* LD A,SET 3,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe0: /* LD B,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe1: /* LD C,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe2: /* LD D,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe3: /* LD E,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe4: /* LD H,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe5: /* LD L,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe7: /* LD A,SET 4,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe8: /* LD B,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xe9: /* LD C,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xea: /* LD D,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xeb: /* LD E,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xec: /* LD H,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xed: /* LD L,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xef: /* LD A,SET 5,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf0: /* LD B,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf1: /* LD C,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf2: /* LD D,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf3: /* LD E,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf4: /* LD H,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf5: /* LD L,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf7: /* LD A,SET 6,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf8: /* LD B,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xf9: /* LD C,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.BC.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfa: /* LD D,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfb: /* LD E,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.DE.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfc: /* LD H,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfd: /* LD L,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.HL.b.l = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IY+d)      */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
    case 0xff: /* LD A,SET 7,(IY+d) */
      T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 6;
      self->state.ctrs.t_states += 23;
      self->state.ctrs.ccounter -= 23;
      goto next;
  }

#undef TMP1
#undef TMP2
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_pulse_int(XcpcCpuZ80a* self)
{
    if(IF_W & _IFF1) {
        IF_W |= _INT;
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_pulse_nmi(XcpcCpuZ80a* self)
{
    IF_W |= _NMI;

    return self;
}