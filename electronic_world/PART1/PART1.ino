#include <Arduboy2.h>
Arduboy2 arduboy;
#include <EEPROM.h>
/*=========================================================
                         动态变量
  =========================================================*/
bool player_move = false; //玩家是否移动
bool key_lock = false; //键盘锁
bool move_lock = false; //方向锁
bool dialog; //对话框
bool LA = false; //行动许可
bool DisplayInvert = false;
bool CN_text_BG = 0;
bool WOOPS=false; //世界崩坏开关

byte ROOM = 7; //当前房间号
byte DX, DY, CDX, CDY;
byte PMX, PMY; //玩家在地图中的位置 0:x 1:y


int CPDX, CPDY; //玩家朝向x和y轴
byte SBDPL[] = {2, 3, 5, 9, 10, 11, 12, 13, 14, 20, 21, 22, 24, 25, 26, 27}; //障碍物id
int Entity[1][2] = {    //实体坐标
  {208, 208},   //注册为玩家
};
byte player_dyn = 0;     //玩家动态帧
byte PlayerD = 1;       //玩家方向
byte KeyBack = 255;     //按键返回
unsigned long Timer[4];  //时间列表 0 1 2 3对话框冷却
byte room, room_f;
/*=========================================================
                         常量
  =========================================================*/
#define dialog_cool_time 500
#define mobile_frame_time 150
#define key_cool_time 30
#define player_move_cool_time 250
#define MISAKI_FONT_F2_H
#define MISAKI_FONT_F2_PAGE 0xF2
#define MISAKI_FONT_F2_W 7
#define MISAKI_FONT_F2_SIZE 0x46
#define TPNUM 18 //传送门个数
#define BNUM 28 //方块个数
/*=========================================================
                         位图
  =========================================================*/
/*
   方块材质
   0  报错
   1  土地
   2  水-1
   3  水-2
   4  草
   5  石头
   6  路
   7  花
   8  麦田
   9  门
   10 砖块
   11 窗户
   12 柱
   13 瓦
   14 灯-1 墙灯
   15 灯-2 路灯
*/
const uint8_t Block_0[] PROGMEM = {0x55, 0x88, 0x55, 0x22, 0x55, 0x88, 0x55, 0x22, 0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff,
                                   0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff, 0x55, 0x88, 0x55, 0x22, 0x55, 0x88, 0x55, 0x22,
                                  };
const uint8_t Block_2[] PROGMEM = {0xe3, 0x26, 0x24, 0x74, 0x9c, 0x84, 0x8c, 0x93, 0x50, 0x70, 0x5c, 0x87, 0x0c, 0x08, 0x88, 0xfc,
                                   0xb5, 0x4a, 0x44, 0x44, 0x44, 0x44, 0x46, 0xa3, 0x1c, 0x34, 0x44, 0xc2, 0xc5, 0xc9, 0xb8, 0x84,
                                  };
const uint8_t Block_3[] PROGMEM = {0xc4, 0x43, 0x66, 0xd4, 0x1c, 0x04, 0x0c, 0x14, 0x53, 0x70, 0x5c, 0x87, 0x0c, 0x08, 0x88, 0xfc,
                                   0x3b, 0x84, 0x48, 0x48, 0x49, 0x49, 0x4d, 0xa7, 0x14, 0x0c, 0x74, 0xc4, 0xc7, 0xc5, 0xc8, 0xb8,
                                  };
const uint8_t Block_4[] PROGMEM = {0xff, 0xfe, 0xff, 0xef, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xbf, 0xff, 0xff,
                                   0xff, 0x7f, 0x7f, 0xfb, 0xf7, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xb7, 0xf7, 0xff, 0xfb, 0xff, 0xff,
                                  };
const uint8_t Block_5[] PROGMEM = {0xdf, 0x4e, 0x9f, 0x6f, 0x6f, 0x6f, 0xaf, 0xcf, 0x6f, 0xef, 0xdf, 0xbb, 0x7f, 0xff, 0xbf, 0xff,
                                   0xff, 0x60, 0x1e, 0xbf, 0x3b, 0x9c, 0xb1, 0xaf, 0xbf, 0xbb, 0xb3, 0xbd, 0xde, 0xa9, 0x97, 0xff,
                                  };
const uint8_t Block_6[] PROGMEM = {0x77, 0xb3, 0xcd, 0xee, 0x77, 0x3b, 0xdd, 0xec, 0x73, 0xba, 0xdd, 0xce, 0x37, 0x3b, 0xdc, 0xee,
                                   0x75, 0xbb, 0xdd, 0xce, 0x37, 0xbb, 0xdc, 0xee, 0x77, 0xb3, 0xcd, 0xee, 0x77, 0x3b, 0xdc, 0xee,
                                  };
const uint8_t Block_7[] PROGMEM = {0xff, 0xff, 0xff, 0x5f, 0x3f, 0x5f, 0xab, 0x89, 0x13, 0x89, 0xab, 0x7f, 0xbf, 0xbf, 0x7f, 0xbf,
                                   0xff, 0xff, 0xff, 0xff, 0xfe, 0xfd, 0xea, 0xd0, 0x81, 0xd0, 0xea, 0xfb, 0xfb, 0xfc, 0xff, 0xff,
                                  };
const uint8_t Block_8[] PROGMEM = {0xff, 0x12, 0x25, 0x12, 0xff, 0x12, 0x25, 0x12, 0xff, 0x12, 0x25, 0x12, 0xff, 0x12, 0x25, 0x12,
                                   0xff, 0xe1, 0x02, 0xe1, 0xff, 0xe1, 0x02, 0xe1, 0xff, 0xe1, 0x02, 0xe1, 0xff, 0xe1, 0x02, 0xe1,
                                  };
const uint8_t Block_9[] PROGMEM = {0xff, 0x01, 0xfe, 0xfe, 0xe6, 0xca, 0xc2, 0xca, 0xc2, 0xca, 0xc2, 0xe6, 0x7e, 0xfe, 0x01, 0xff,
                                   0xff, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x80, 0xff,
                                  };
const uint8_t Block_10[] PROGMEM = {0x83, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x38, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
                                    0x83, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x38, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
                                   };
const uint8_t Block_11[] PROGMEM = {0xff, 0x07, 0xfb, 0xfd, 0xfd, 0xfd, 0xfd, 0x01, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfb, 0x07, 0xff,
                                    0xff, 0xe0, 0xde, 0xbe, 0xbe, 0xbe, 0xbe, 0x80, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xde, 0xe0, 0xff,
                                   };
const uint8_t Block_12[] PROGMEM = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   };
const uint8_t Block_13[] PROGMEM = {0xfe, 0x7d, 0xbd, 0xdb, 0xc7, 0xbf, 0x7f, 0xff, 0xfe, 0x7d, 0xbd, 0xdb, 0xc7, 0xbf, 0x7f, 0xff,
                                    0xfe, 0x7d, 0xbd, 0xdb, 0xc7, 0xbf, 0x7f, 0xfe, 0xfe, 0x7d, 0xbd, 0xdb, 0xc7, 0xbf, 0x7f, 0xfe,
                                   };
const uint8_t Block_14[] PROGMEM = {0x38, 0xfb, 0xbb, 0x5b, 0xdb, 0x4b, 0x5b, 0x5b, 0xbb, 0xfb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
                                    0x38, 0xbb, 0xbb, 0xbc, 0xb9, 0xa2, 0xa9, 0xbc, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
                                   };
const uint8_t Block_15[] PROGMEM = {0x3f, 0xdf, 0xef, 0xef, 0xdf, 0x33, 0x2d, 0x1e, 0x1e, 0x2d, 0x33, 0xdf, 0xef, 0xef, 0xdf, 0x3f,
                                    0xff, 0xfe, 0xfd, 0xfd, 0xfe, 0xff, 0x0f, 0xe0, 0xe0, 0x0f, 0xff, 0xfe, 0xfd, 0xfd, 0xfe, 0xff,
                                   };
const uint8_t Block_17[] PROGMEM = {0x07, 0xf3, 0x1b, 0x5b, 0xf3, 0x9b, 0x73, 0x9b, 0xf3, 0xdb, 0x13, 0xfb, 0xd3, 0x1b, 0xd3, 0x07,
                                    0xf0, 0xe7, 0xec, 0xe5, 0xef, 0xe4, 0xef, 0xe4, 0xef, 0xe5, 0xec, 0xe5, 0xef, 0xec, 0xe7, 0xf0,
                                   };
const uint8_t Block_18[] PROGMEM = {0xff, 0xff, 0xff, 0x1f, 0x5f, 0x0f, 0x6f, 0x07, 0xd7, 0xc3, 0xdb, 0x01, 0xf5, 0x70, 0x76, 0x00,
                                    0x01, 0x7d, 0x7d, 0x00, 0x2f, 0x07, 0x37, 0x00, 0x6b, 0x61, 0x6d, 0x60, 0x7a, 0x78, 0x7b, 0x00,
                                   };
const uint8_t Block_19[] PROGMEM = {0x00, 0x00, 0x00, 0x8f, 0xcf, 0xcf, 0x4f, 0x6f, 0x2f, 0x77, 0x17, 0x3b, 0xcb, 0xdd, 0xdd, 0x01,
                                    0x02, 0xf7, 0xf1, 0xf1, 0x00, 0xfd, 0xfc, 0x00, 0xff, 0xff, 0xff, 0x00, 0xdf, 0x0f, 0xef, 0x00,
                                   };
const uint8_t Block_20[] PROGMEM = {0x59, 0xc6, 0xd6, 0x14, 0x31, 0xc3, 0xeb, 0xe9, 0x2c, 0xad, 0x4c, 0x65, 0x63, 0x0b, 0xd8, 0x1b,
                                    0xb0, 0x0e, 0x66, 0x71, 0x22, 0x8c, 0x3e, 0x3c, 0x4c, 0x31, 0xeb, 0x07, 0xb3, 0xb8, 0x8d, 0x30,
                                   };
const uint8_t Block_21[] PROGMEM = {0xff, 0x1f, 0xef, 0xd7, 0xf7, 0xf7, 0xb7, 0xd7, 0xb7, 0xb7, 0xf7, 0xf7, 0xd7, 0xef, 0x1f, 0xff,
                                    0xff, 0x00, 0x7f, 0xde, 0xec, 0xda, 0xff, 0xec, 0xe9, 0xfa, 0xdf, 0xea, 0xfd, 0x7f, 0x00, 0xff,
                                   };
const uint8_t Block_22[] PROGMEM = {0xff, 0x00, 0xff, 0xfb, 0x1d, 0x0d, 0xc7, 0xe7, 0xe7, 0xf7, 0xed, 0x1d, 0xfb, 0xff, 0x00, 0xff,
                                    0xff, 0x00, 0xff, 0x95, 0xb6, 0xec, 0xf8, 0xf9, 0xf9, 0xf9, 0xec, 0xb6, 0x95, 0xff, 0x00, 0xff,
                                   };
const uint8_t Block_23[] PROGMEM = {0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0x3a, 0xda, 0xea, 0xe2, 0xea, 0xda, 0x3a, 0xfa, 0xfa, 0xfa, 0xfa,
                                    0xff, 0xff, 0xff, 0xff, 0xf9, 0xf4, 0xf6, 0xf4, 0xf4, 0xf6, 0xf6, 0xf4, 0xf9, 0xff, 0xff, 0xff,
                                   };
const uint8_t Block_24[] PROGMEM = {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
                                    0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0x01, 0xfd, 0xfd, 0x01, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd,
                                   };
const uint8_t Block_25[] PROGMEM = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0x00, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   };
const uint8_t Block_26[] PROGMEM = {0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0x00, 0xfe, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   };
const uint8_t Block_27[] PROGMEM = {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
                                    0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd,
                                   };
/*
  const uint8_t Block_[] PROGMEM = {};
*/
const unsigned char *Block[BNUM] = {Block_0, Block_2, Block_3, Block_4, Block_5, Block_6, Block_7, Block_8, Block_9, Block_10, Block_11, Block_12, Block_13, Block_14, Block_15, Block_10, Block_17, Block_18, Block_19, Block_20, Block_21, Block_22, Block_23, Block_24, Block_25, Block_26, Block_27, Block_0,};

/*
  玩家
*/
const uint8_t Man_U_0[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xfe, 0x7c, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x02, 0x03, 0x01, 0x02, 0x1f, 0xbc, 0x3e, 0x3d, 0xbc, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_U_1[] PROGMEM = {0x40, 0x20, 0x20, 0xc0, 0x00, 0x7c, 0xfe, 0x7c, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x08, 0x05, 0x1a, 0xba, 0x3b, 0x3d, 0x3c, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_U_2[] PROGMEM = {0x00, 0x00, 0x00, 0x40, 0x80, 0x7c, 0xfe, 0x7c, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x03, 0x04, 0x04, 0x02, 0x1c, 0x3d, 0xba, 0x3b, 0xbc, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_U_3[] PROGMEM = {0x40, 0x20, 0x20, 0xc0, 0x00, 0x7c, 0xfe, 0x7c, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x08, 0x05, 0x1a, 0xba, 0x3b, 0x3d, 0xbc, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_U_4[] PROGMEM = {0x00, 0x00, 0x00, 0x40, 0x80, 0x7c, 0xfe, 0x7c, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x03, 0x04, 0x04, 0x02, 0x1c, 0xbd, 0x3a, 0x3b, 0xbc, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_D_0[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x4e, 0x7c, 0x4c, 0x7e, 0x7c, 0x90, 0x50, 0x40, 0x80, 0x80,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_D_1[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x4e, 0x7c, 0x4c, 0x7e, 0x7c, 0x90, 0x60, 0x50, 0x10, 0x20,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0x3f, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_D_2[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x4e, 0x7c, 0x4c, 0x7e, 0x7c, 0xa0, 0x10, 0x00, 0xc0, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0xbf, 0x3f, 0xbf, 0x1f, 0x00, 0x01, 0x01, 0x00, 0x00,
                                  };
const uint8_t Man_D_3[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x4e, 0x7c, 0x4c, 0x7e, 0x7c, 0x90, 0x60, 0x50, 0x10, 0x20,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  };
const uint8_t Man_D_4[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x4e, 0x7c, 0x4c, 0x7e, 0x7c, 0xa0, 0x10, 0x00, 0xc0, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x01, 0x01, 0x00, 0x00,
                                  };
const uint8_t Man_L_0[] PROGMEM = {0x00, 0x00, 0x7c, 0x4e, 0x78, 0x48, 0x7c, 0x7e, 0x80, 0x40, 0x40, 0x40, 0x40, 0x20, 0x00, 0x00,
                                   0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x01, 0x02, 0x04, 0x04, 0x04, 0x03, 0x00, 0x00,
                                  };
const uint8_t Man_L_1[] PROGMEM = {0x00, 0x00, 0x7c, 0x4e, 0x78, 0x48, 0x78, 0x7c, 0x86, 0x40, 0x20, 0x20, 0x30, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x9f, 0x3f, 0x3f, 0x3f, 0x3f, 0x9f, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x0e, 0x00,
                                  };
const uint8_t Man_L_2[] PROGMEM = {0x00, 0x00, 0x7c, 0x4e, 0x7a, 0x48, 0x7c, 0x7e, 0x80, 0x40, 0x40, 0x40, 0x40, 0x20, 0x00, 0x00,
                                   0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0x3f, 0x9f, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00,
                                  };
const uint8_t Man_L_3[] PROGMEM = {0x00, 0x00, 0x7c, 0x4e, 0x78, 0x48, 0x78, 0x7c, 0x86, 0x40, 0x20, 0x20, 0x30, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x0e, 0x00,
                                  };
const uint8_t Man_L_4[] PROGMEM = {0x00, 0x00, 0x7c, 0x4e, 0x7a, 0x48, 0x7c, 0x7e, 0x80, 0x40, 0x40, 0x40, 0x40, 0x20, 0x00, 0x00,
                                   0x00, 0x00, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00,
                                  };

const uint8_t Man_R_0[] PROGMEM = {0x00, 0x00, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x7e, 0x7c, 0x48, 0x78, 0x4e, 0x7c, 0x00, 0x00,
                                   0x00, 0x00, 0x03, 0x04, 0x04, 0x04, 0x02, 0x01, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x00,
                                  };
const uint8_t Man_R_1[] PROGMEM = {0x00, 0x00, 0x00, 0x30, 0x20, 0x20, 0x40, 0x80, 0x7e, 0x7c, 0x48, 0x7a, 0x4e, 0x7c, 0x00, 0x00,
                                   0x00, 0x0e, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x9f, 0x3f, 0x3f, 0x3f, 0x3f, 0x9f, 0x00, 0x00,
                                  };
const uint8_t Man_R_2[] PROGMEM = {0x00, 0x00, 0x20, 0x40, 0x40, 0x40, 0x40, 0x86, 0x7c, 0x78, 0x48, 0x78, 0x4e, 0x7c, 0x00, 0x00,
                                   0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x9f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x80, 0x00,
                                  };
const uint8_t Man_R_3[] PROGMEM = {0x00, 0x00, 0x00, 0x30, 0x20, 0x20, 0x40, 0x80, 0x7e, 0x7c, 0x48, 0x7a, 0x4e, 0x7c, 0x00, 0x00,
                                   0x00, 0x0e, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x00,
                                  };
const uint8_t Man_R_4[] PROGMEM = {0x00, 0x00, 0x20, 0x40, 0x40, 0x40, 0x40, 0x86, 0x7c, 0x78, 0x48, 0x78, 0x4e, 0x7c, 0x00, 0x00,
                                   0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x1f, 0xbf, 0x3f, 0x3f, 0xbf, 0x1f, 0x00, 0x00,
                                  };
const unsigned char *T_Man_direction[8][4] = {
  {Man_U_3, Man_U_0, Man_U_4, Man_U_0},
  {Man_U_1, Man_U_0, Man_U_2, Man_U_0},
  {Man_D_3, Man_D_0, Man_D_4, Man_D_0},
  {Man_D_1, Man_D_0, Man_D_2, Man_D_0},
  {Man_L_3, Man_L_0, Man_L_4, Man_L_0},
  {Man_L_1, Man_L_0, Man_L_2, Man_L_0},
  {Man_R_3, Man_R_0, Man_R_4, Man_R_0},
  {Man_R_1, Man_R_0, Man_R_2, Man_R_0}
};


/*=========================================================
                         地图
                         第一个参数为房间号 第二三个参数为xy
  =========================================================*/
const PROGMEM byte MAP[16][16][16] = {
  //ROOM 0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 1
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 2
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 3
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 4
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 5
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //ROOM 6
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //Room 7
  4 , 7 , 4 , 7 , 4 , 7 , 4 , 7 , 4 , 7 , 6 , 5 , 8 , 8 , 20 , 20 , 7 , 4 , 7 , 4 , 7 , 4 , 7 , 4 , 6 , 6 , 6 , 8 , 8 , 5 , 8 , 20 , 4 , 7 , 4 , 7 , 4 , 6 , 6 , 6 , 6 , 21 , 8 , 5 , 1 , 1 , 1 , 20 , 7 , 4 , 7 , 4 , 7 , 6 , 1 , 1 , 1 , 1 , 1 , 5 , 1 , 1 , 20 , 20 , 4 , 7 , 4 , 7 , 21 , 6 , 1 , 15 , 1 , 2 , 2 , 2 , 2 , 2 , 28 , 28 , 15 , 4 , 15 , 6 , 6 , 6 , 1 , 12 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 28 , 12 , 7 , 12 , 6 , 8 , 1 , 20 , 2 , 2 , 2 , 2 , 2 , 28 , 2 , 28 , 2 , 6 , 6 , 6 , 6 , 6 , 15 , 12 , 2 , 2 , 2 , 2 , 21 , 28 , 28 , 21 , 2 , 15 , 6 , 15 , 6 , 6 , 12 , 12 , 2 , 2 , 2 , 2 , 28 , 24 , 28 , 22 , 20 , 12 , 8 , 12 , 6 , 6 , 20 , 20 , 2 , 2 , 1 , 4 , 12 , 28 , 1 , 12 , 28 , 8 , 8 , 6 , 6 , 6 , 1 , 12 , 2 , 2 , 4 , 4 , 21 , 4 , 28 , 28 , 20 , 8 , 15 , 6 , 6 , 6 , 15 , 12 , 2 , 2 , 2 , 1 , 22 , 28 , 24 , 22 , 20 , 8 , 12 , 6 , 6 , 6 , 12 , 20 , 2 , 2 , 2 , 1 , 1 , 1 , 4 , 24 , 28 , 8 , 8 , 6 , 6 , 6 , 1 , 12 , 2 , 2 , 2 , 2 , 28 , 1 , 4 , 21 , 20 , 8 , 8 , 15 , 6 , 6 , 1 , 12 , 2 , 2 , 2 , 2 , 2 , 2 , 4 , 22 , 20 , 8 , 8 , 12 , 6 , 6 , 1 , 20 , 2 , 2 , 2 , 2 , 2 , 2 , 6 , 22 , 20 ,
  //ROOM 8
  8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 6 , 8 , 8 , 8 , 8 , 4 , 4 , 8 , 4 , 4 , 4 , 4 , 8 , 8 , 8 , 8 , 6 , 8 , 8 , 8 , 8 , 8 , 8 , 4 , 8 , 8 , 8 , 8 , 8 , 8 , 4 , 8 , 6 , 8 , 8 , 8 , 4 , 8 , 8 , 4 , 1 , 1 , 1 , 1 , 4 , 4 , 4 , 8 , 6 , 8 , 8 , 4 , 4 , 8 , 8 , 1 , 10 , 13 , 13 , 13 , 13 , 13 , 10 , 8 , 6 , 8 , 8 , 6 , 8 , 8 , 8 , 1 , 10 , 13 , 13 , 13 , 13 , 13 , 10 , 8 , 6 , 8 , 8 , 6 , 8 , 8 , 8 , 4 , 12 , 10 , 10 , 10 , 10 , 10 , 12 , 8 , 6 , 8 , 8 , 4 , 8 , 8 , 8 , 4 , 12 , 10 , 11 , 10 , 11 , 10 , 12 , 4 , 6 , 8 , 8 , 6 , 8 , 8 , 8 , 4 , 12 , 10 , 10 , 9 , 10 , 10 , 12 , 4 , 6 , 8 , 13 , 4 , 4 , 8 , 8 , 4 , 4 , 4 , 7 , 6 , 7 , 4 , 4 , 5 , 6 , 8 , 8 , 4 , 4 , 8 , 8 , 8 , 4 , 1 , 4 , 6 , 6 , 6 , 6 , 6 , 6 , 8 , 8 , 6 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 , 1 , 1 , 1 , 8 , 6 , 8 , 8 , 6 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 4 , 8 , 6 , 8 , 8 , 6 , 6 , 6 , 6 , 8 , 8 , 8 , 4 , 4 , 4 , 4 , 8 , 1 , 6 , 6 , 6 , 6 , 8 , 8 , 8 , 8 , 8 , 4 , 4 , 8 , 4 , 4 , 8 , 1 , 6 , 8 , 8 , 8 , 8 , 4 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 6 , 8 , 4 , 4 , 4 , 4 , 8 ,
  //ROOM 9
  8 , 8 , 4 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 , 6 , 4 , 1 , 8 , 4 , 8 , 4 , 8 , 8 , 8 , 8 , 4 , 4 , 8 , 8 , 1 , 6 , 1 , 4 , 8 , 8 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 4 , 4 , 8 , 6 , 4 , 10 , 8 , 8 , 10 , 10 , 10 , 10 , 10 , 10 , 10 , 10 , 4 , 15 , 1 , 6 , 4 , 10 , 8 , 4 , 10 , 13 , 13 , 13 , 13 , 13 , 13 , 10 , 4 , 12 , 1 , 6 , 4 , 10 , 8 , 8 , 10 , 13 , 13 , 13 , 13 , 13 , 13 , 10 , 5 , 4 , 1 , 6 , 4 , 10 , 8 , 4 , 10 , 10 , 10 , 10 , 10 , 10 , 10 , 10 , 9 , 5 , 1 , 6 , 4 , 4 , 8 , 8 , 12 , 10 , 11 , 10 , 10 , 11 , 10 , 12 , 6 , 5 , 1 , 6 , 6 , 6 , 8 , 8 , 12 , 10 , 10 , 10 , 10 , 10 , 10 , 12 , 5 , 1 , 1 , 6 , 5 , 15 , 8 , 8 , 12 , 10 , 10 , 9 , 14 , 10 , 10 , 12 , 5 , 1 , 6 , 6 , 1 , 12 , 8 , 4 , 12 , 10 , 10 , 6 , 4 , 15 , 1 , 1 , 6 , 6 , 6 , 1 , 1 , 1 , 8 , 4 , 12 , 10 , 10 , 6 , 4 , 12 , 1 , 6 , 6 , 1 , 1 , 2 , 2 , 2 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 1 , 2 , 2 , 2 , 2 , 2 , 1 , 5 , 4 , 4 , 1 , 4 , 4 , 4 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 4 , 1 , 1 , 4 , 7 , 4 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 4 , 4 , 1 ,
  //ROOM 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //Room 11
  8 , 1 , 1 , 1 , 6 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 6 , 22 , 20 , 1 , 6 , 6 , 6 , 6 , 8 , 4 , 2 , 2 , 2 , 2 , 2 , 1 , 4 , 22 , 20 , 8 , 6 , 20 , 8 , 8 , 4 , 2 , 2 , 2 , 2 , 1 , 4 , 4 , 1 , 2 , 20 , 7 , 6 , 5 , 8 , 2 , 2 , 2 , 2 , 2 , 1 , 24 , 27 , 24 , 2 , 5 , 20 , 8 , 6 , 20 , 7 , 2 , 2 , 2 , 2 , 2 , 2 , 5 , 20 , 20 , 20 , 20 , 20 , 1 , 6 , 1 , 7 , 2 , 2 , 2 , 2 , 2 , 7 , 20 , 19 , 6 , 21 , 7 , 20 , 8 , 6 , 15 , 8 , 2 , 2 , 2 , 2 , 2 , 5 , 5 , 6 , 6 , 6 , 7 , 20 , 6 , 6 , 12 , 5 , 2 , 2 , 2 , 2 , 20 , 7 , 20 , 5 , 5 , 4 , 7 , 20 , 8 , 4 , 20 , 1 , 5 , 2 , 2 , 2 , 20 , 5 , 20 , 20 , 20 , 5 , 5 , 20 , 2 , 2 , 1 , 4 , 1 , 2 , 2 , 2 , 20 , 20 , 20 , 13 , 20 , 20 , 20 , 20 , 2 , 2 , 2 , 7 , 2 , 2 , 2 , 2 , 5 , 20 , 20 , 13 , 13 , 20 , 5 , 5 , 2 , 5 , 2 , 2 , 2 , 5 , 2 , 2 , 2 , 5 , 25 , 23 , 23 , 26 , 20 , 5 , 2 , 2 , 2 , 1 , 2 , 5 , 5 , 2 , 2 , 21 , 12 , 4 , 1 , 12 , 20 , 5 , 2 , 1 , 21 , 1 , 1 , 2 , 2 , 2 , 5 , 1 , 1 , 1 , 4 , 5 , 20 , 20 , 4 , 8 , 4 , 4 , 1 , 2 , 2 , 4 , 8 , 1 , 4 , 1 , 1 , 4 , 5 , 20 , 4 , 4 , 8 , 8 , 4 , 8 , 4 , 8 , 8 , 6 , 1 , 4 , 4 , 5 , 5 , 20 ,
  //ROOM 12
  4 , 4 , 4 , 8 , 8 , 8 , 8 , 8 , 1 , 6 , 1 , 8 , 8 , 1 , 2 , 2 , 4 , 7 , 4 , 4 , 4 , 8 , 8 , 1 , 1 , 6 , 1 , 8 , 1 , 2 , 2 , 2 , 8 , 4 , 4 , 8 , 8 , 8 , 6 , 6 , 6 , 6 , 1 , 1 , 2 , 2 , 2 , 2 , 8 , 8 , 8 , 8 , 4 , 1 , 6 , 4 , 4 , 4 , 1 , 2 , 2 , 2 , 2 , 2 , 8 , 8 , 4 , 5 , 1 , 1 , 6 , 1 , 4 , 1 , 2 , 2 , 2 , 2 , 7 , 7 , 8 , 8 , 4 , 4 , 4 , 1 , 6 , 1 , 1 , 2 , 2 , 2 , 2 , 7 , 1 , 8 , 8 , 4 , 8 , 1 , 1 , 7 , 6 , 1 , 2 , 2 , 2 , 2 , 1 , 1 , 8 , 8 , 8 , 8 , 1 , 7 , 1 , 1 , 4 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 8 , 8 , 1 , 1 , 1 , 1 , 1 , 1 , 16 , 2 , 2 , 2 , 2 , 2 , 1 , 8 , 8 , 8 , 2 , 2 , 2 , 2 , 2 , 2 , 16 , 2 , 2 , 5 , 2 , 2 , 1 , 8 , 5 , 8 , 2 , 2 , 2 , 2 , 2 , 2 , 16 , 2 , 5 , 4 , 2 , 2 , 1 , 8 , 8 , 8 , 2 , 2 , 2 , 2 , 2 , 1 , 6 , 7 , 4 , 4 , 2 , 2 , 1 , 8 , 8 , 6 , 2 , 2 , 2 , 2 , 1 , 1 , 6 , 6 , 1 , 16 , 16 , 16 , 16 , 6 , 6 , 6 , 8 , 8 , 2 , 2 , 1 , 1 , 6 , 1 , 4 , 2 , 2 , 2 , 1 , 1 , 8 , 8 , 8 , 8 , 5 , 4 , 1 , 6 , 6 , 4 , 8 , 4 , 2 , 2 , 1 , 7 , 8 , 8 , 8 , 8 , 8 , 4 , 4 , 1 , 1 , 8 , 8 , 4 , 2 , 2 , 1 , 1 , 1 , 8 ,
  //ROOM 13
  2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 4 , 7 , 1 , 1 , 7 , 2 , 2 , 2 , 2 , 1 , 4 , 4 , 1 , 1 , 1 , 4 , 4 , 1 , 8 , 8 , 1 , 2 , 4 , 1 , 1 , 1 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 , 1 , 1 , 1 , 6 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 7 , 1 , 4 , 4 , 6 , 8 , 8 , 8 , 7 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 , 4 , 1 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 1 , 4 , 7 , 1 , 6 , 8 , 4 , 8 , 4 , 1 , 1 , 1 , 7 , 8 , 1 , 7 , 1 , 4 , 7 , 4 , 6 , 8 , 8 , 2 , 2 , 2 , 2 , 4 , 1 , 8 , 8 , 1 , 1 , 4 , 4 , 5 , 6 , 8 , 2 , 2 , 4 , 4 , 2 , 2 , 1 , 8 , 8 , 8 , 1 , 1 , 4 , 4 , 6 , 8 , 2 , 2 , 5 , 4 , 2 , 2 , 2 , 1 , 8 , 8 , 1 , 6 , 6 , 6 , 6 , 8 , 8 , 2 , 2 , 2 , 7 , 2 , 2 , 1 , 8 , 8 , 6 , 6 , 1 , 8 , 6 , 8 , 8 , 4 , 4 , 2 , 2 , 2 , 2 , 1 , 8 , 8 , 6 , 5 , 1 , 1 , 6 , 8 , 8 , 8 , 8 , 8 , 2 , 2 , 7 , 1 , 8 , 8 , 1 , 8 , 4 , 8 , 6 , 6 , 6 , 6 , 6 , 8 , 4 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 4 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 7 , 8 , 8 , 8 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 ,
  //Room 14
  8 , 8 , 8 , 13 , 13 , 13 , 13 , 13 , 13 , 13 , 13 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 13 , 13 , 10 , 23 , 10 , 10 , 23 , 10 , 13 , 13 , 8 , 8 , 8 , 8 , 8 , 8 , 13 , 10 , 2 , 2 , 16 , 16 , 2 , 2 , 10 , 13 , 8 , 8 , 8 , 8 , 8 , 8 , 13 , 10 , 7 , 2 , 16 , 16 , 2 , 7 , 10 , 13 , 8 , 8 , 8 , 8 , 8 , 8 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 1 , 15 , 1 , 6 , 6 , 6 , 6 , 4 , 8 , 8 , 8 , 8 , 7 , 1 , 7 , 6 , 1 , 12 , 1 , 6 , 10 , 7 , 6 , 10 , 4 , 8 , 15 , 8 , 8 , 7 , 5 , 6 , 6 , 6 , 6 , 6 , 13 , 10 , 6 , 5 , 8 , 1 , 12 , 1 , 8 , 8 , 1 , 1 , 8 , 8 , 8 , 8 , 8 , 13 , 6 , 6 , 6 , 1 , 20 , 1 , 8 , 1 , 1 , 8 , 1 , 8 , 8 , 8 , 8 , 1 , 1 , 8 , 8 , 1 , 1 , 1 , 8 , 1 , 8 , 8 , 1 , 8 , 8 , 8 , 8 , 1 , 8 , 8 , 8 , 8 , 6 , 8 , 8 , 4 , 8 , 4 , 4 , 4 , 1 , 1 , 1 , 1 , 20 , 20 , 8 , 6 , 6 , 8 , 8 , 4 , 1 , 4 , 8 , 8 , 8 , 8 , 1 , 8 , 20 , 8 , 7 , 6 , 8 , 20 , 8 , 4 , 4 , 8 , 4 , 4 , 7 , 21 , 1 , 8 , 1 , 1 , 6 , 8 , 20 , 20 , 8 , 8 , 8 , 4 , 4 , 20 , 1 , 1 , 4 , 4 , 1 , 6 , 6 , 20 , 20 , 20 , 8 , 8 , 8 , 8 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 ,
  //Room 15
  7 , 4 , 8 , 13 , 13 , 8 , 8 , 4 , 8 , 6 , 8 , 8 , 8 , 8 , 8 , 20 , 4 , 4 , 8 , 8 , 13 , 4 , 4 , 4 , 8 , 6 , 8 , 8 , 8 , 8 , 20 , 20 , 8 , 25 , 23 , 8 , 8 , 4 , 4 , 8 , 6 , 15 , 8 , 8 , 8 , 4 , 20 , 20 , 8 , 12 , 8 , 8 , 6 , 6 , 6 , 6 , 6 , 12 , 1 , 8 , 8 , 8 , 20 , 20 , 6 , 6 , 6 , 6 , 6 , 1 , 1 , 1 , 8 , 20 , 1 , 8 , 4 , 4 , 20 , 20 , 8 , 1 , 1 , 1 , 1 , 8 , 8 , 1 , 1 , 1 , 1 , 4 , 4 , 20 , 20 , 20 , 1 , 1 , 8 , 8 , 8 , 4 , 8 , 8 , 1 , 23 , 26 , 4 , 2 , 20 , 20 , 20 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 12 , 8 , 2 , 2 , 2 , 20 , 1 , 13 , 8 , 4 , 8 , 8 , 4 , 4 , 8 , 8 , 8 , 8 , 2 , 8 , 20 , 20 , 1 , 8 , 8 , 8 , 4 , 8 , 4 , 8 , 8 , 21 , 8 , 2 , 4 , 21 , 4 , 20 , 1 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 22 , 4 , 8 , 4 , 22 , 4 , 20 , 1 , 1 , 1 , 1 , 8 , 1 , 1 , 1 , 8 , 1 , 24 , 24 , 24 , 1 , 1 , 20 , 20 , 20 , 4 , 13 , 8 , 13 , 8 , 1 , 1 , 4 , 12 , 13 , 12 , 1 , 4 , 20 , 20 , 20 , 20 , 4 , 4 , 8 , 8 , 8 , 4 , 21 , 25 , 23 , 26 , 21 , 4 , 20 , 20 , 20 , 20 , 20 , 13 , 4 , 20 , 2 , 1 , 22 , 20 , 15 , 20 , 22 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 20 , 2 , 2 , 4 , 4 , 1 , 4 , 1 , 20 , 20 , 20 ,
};
//设置传送触发房间和目标房间 {,},
const PROGMEM byte TPRoom[TPNUM][2] = {
  {9, 8},
  {8, 9},
  {8, 12},
  {12, 8},
  {12, 13},
  {12, 13},
  {13, 12},
  {13, 12},
  {13, 14},
  {14, 13},
  {14, 14},
  {14, 15},
  {15, 14},
  {15, 11},
  {11, 15},
  {11, 7},
  {7, 11},
  {7, 11},

};
//设置传送触发坐标和目标坐标 {{,}, {,}},
const PROGMEM byte TPXY[TPNUM][2][2] = {
  {{0, 12}, {15, 12}},
  {{15, 12}, {0, 12}},
  {{9, 15}, {9, 0}},
  {{9, 0}, {9, 15}},
  {{15, 11}, {0, 11}},
  {{15, 12}, {0, 12}},
  {{0, 11}, {15, 11}},
  {{0, 12}, {15, 12}},
  {{15, 5}, {0, 5}},
  {{0, 5}, {15, 5}},
  {{7, 14}, {7, 2}},
  {{15, 4}, {0, 4}},
  {{0, 4}, {15, 4}},
  {{9, 0}, {9, 15}},
  {{9, 15}, {9, 0}},
  {{4, 0}, {3, 15}},
  {{3, 15}, {4, 0}},
  {{4, 15}, {4, 0}},

};
//设置传送触发方向和目标方向 {,},
const PROGMEM byte TPC[TPNUM][2] = {
  {2, 2},
  {3, 3},
  {1, 1},
  {0, 0},
  {3, 3},
  {3, 3},
  {2, 2},
  {2, 2},
  {3, 3},
  {2, 2},
  {0, 1},
  {3, 3},
  {2, 2},
  {0, 0},
  {1, 1},
  {0, 0},
  {1, 1},
  {1, 1},
};
/*=========================================================
                          中文字库
  =========================================================*/
PROGMEM const uint8_t misaki_font_f2[ MISAKI_FONT_F2_SIZE + 1 ][ MISAKI_FONT_F2_W ] =
{
};
/*=========================================================
                          对话
  =========================================================*/
PROGMEM const uint8_t misaki_font_0x00[1] = { 0x00 };
//PROGMEM const uint8_t mes[] =
/*====================================================================
                             软重启函数
  ====================================================================*/
void(* resetFunc) (void) = 0; //制造重启命令

/*=========================================================
                     绘图
  =========================================================*/
void draw() {
  arduboy.clear();
  DrawMap();
  draw_player(64, 32);
  // arduboy.setCursor(0, 0);
  // arduboy.print(map(player_dyn, 0, 2, 0, 1));
  arduboy.display();
}
/*=========================================================
                      地图操作
  =========================================================*/
void DrawMap() {
  PMX = Entity[0][0] / 16;
  PMY = Entity[0][1] / 16;

  for (byte TPN = 0; TPN < TPNUM; TPN++) { //遍历传送列表 检查是否到传送门
    if (pgm_read_byte(&TPRoom[TPN][0]) == ROOM && pgm_read_byte(&TPXY[TPN][0][0]) == PMX && pgm_read_byte(&TPXY[TPN][0][1]) == PMY && pgm_read_byte(&TPC[TPN][0]) == PlayerD) {
      //符合目标传送门跳转条件
      ROOM = pgm_read_byte(&TPRoom[TPN][1]);
      PMX = pgm_read_byte(&TPXY[TPN][1][0]);
      PMY = pgm_read_byte(&TPXY[TPN][1][1]);
      Entity[0][0] = PMX * 16;
      Entity[0][1] = PMY * 16;
      PlayerD = pgm_read_byte(&TPC[TPN][1]);
    }
  }
  int MapReadLimit[4]; //显示地图缓存读取范围 防止溢出死循环
  MapReadLimit[0] = PMX - 4;
  MapReadLimit[1] = PMX + 5;
  MapReadLimit[2] = PMY - 2;
  MapReadLimit[3] = PMY + 3;
  if (MapReadLimit[0] < 0) CDX = 15 * (-MapReadLimit[0]); else CDX = 0;
  if (MapReadLimit[2] < 0) CDY = 15 * (-MapReadLimit[2]); else CDY = 0;
  DX = CDX;
  DY = CDY;
  for (byte i = 0; i < 4; i++) {
    if (MapReadLimit[i] < 0) MapReadLimit[i] = 0; else if (MapReadLimit[i] > 15) MapReadLimit[i] = 15;
  }
  byte CCDX, CCDY;
  for (byte MapReadY = MapReadLimit[2]; MapReadY <= MapReadLimit[3]; MapReadY++) {
    for (byte MapReadX = MapReadLimit[0]; MapReadX <= MapReadLimit[1]; MapReadX++) {
      CCDX = Entity[0][0] - PMX * 16;
      CCDY = Entity[0][1] - PMY * 16;
      switch (pgm_read_byte(&MAP[ROOM][MapReadY][MapReadX])) {
        case 1:
          arduboy.fillRect(DX - CCDX, DY - CCDY, 16, 16, 1);
          break;
        case 2:
          arduboy.drawBitmap(DX - CCDX, DY - CCDY, Block[map(player_dyn, 0, 2, 0, 1)] , 16, 16, 1); //动态水
          break;
        case 28:
          arduboy.drawBitmap(DX - CCDX, DY - CCDY, Block[random(0, BNUM - 1)] , 16, 16, 1);
          break;
        default :
          arduboy.drawBitmap(DX - CCDX, DY - CCDY, Block[pgm_read_byte(&MAP[ROOM][MapReadY][MapReadX]) - 1], 16, 16, 1);
          break;
      }
      DX += 16;
    }
    DX = CDX;
    DY += 16;
  }
}

void key() {
  if (millis() >= key_cool_time + Timer[1]) {
    Timer[1] = millis();   //重置移动帧计时器
    /*
        0  1  2  3  4  5
        ↑ ↓← →  A  B
    */
    KeyBack = 255;
    if (arduboy.pressed(UP_BUTTON)) KeyBack = 0;
    if (arduboy.pressed(DOWN_BUTTON)) KeyBack = 1;
    if (arduboy.pressed(LEFT_BUTTON)) KeyBack = 2;
    if (arduboy.pressed(RIGHT_BUTTON)) KeyBack = 3;
    if (arduboy.pressed(A_BUTTON)) KeyBack = 4;
    if (arduboy.pressed(B_BUTTON)) KeyBack = 5;
  }
}
void draw_player(byte x, byte y) {
  arduboy.drawBitmap(x - 8 , y - 8 , T_Man_direction[PlayerD * 2 + player_move][player_dyn], 16, 16, 0);
  if (millis() >= mobile_frame_time + Timer[0]) {  //移动帧时间
    Timer[0] = millis();   //重置移动帧计时器
    player_dyn++; //下一个动态帧
    if (player_dyn >= 4) player_dyn = 0;
  }
}
void SBDP() {
  //设置前一格方向向量
  switch (PlayerD) {
    case 0:
      CPDX = 0;
      CPDY = -1;
      break;
    case 1:
      CPDX = 0;
      CPDY = 1;
      break;
    case 2:
      CPDX = -1;
      CPDY = 0;
      break;
    case 3:
      CPDX = 1;
      CPDY = 0;
      break;
  }
  LA = true;
  byte length = sizeof(SBDPL) / sizeof(SBDPL[0]);
  for (byte i = 0; i < length; i++) {

    if (pgm_read_byte(&MAP[ROOM][(Entity[0][1] + 8  * CPDY) / 16][(Entity[0][0] + 8 * CPDX) / 16]) == SBDPL[i]) LA = false;
    if (Entity[0][0] + CPDX < 0 || Entity[0][0] + CPDX >= 248 || Entity[0][1] + CPDY < 0 || Entity[0][1] + CPDY >= 248) LA = false;

    // Serial.println(F("LA False"));
    //  Serial.println((Entity[0][0] + 4CPDX) / 16 );
    // Serial.println((Entity[0][1] + 4CPDY) / 16 );
    // Serial.println(pgm_read_byte(&MAP[ROOM][(Entity[0][1] + CPDY) / 16][(Entity[0][0] + CPDX) / 16]));
    //  Serial.println(length);
  }
}
void logic() {
  /*
     检测按键返回值 对相应方向进行移动障碍物判断
  */
  if (KeyBack < 4) {
    SBDP();
    player_move = true;
  } else player_move = false;
  switch (KeyBack) {
    case 0:
      if (!move_lock) {
        PlayerD = 0;
      }
      break;
    case 1:
      if (!move_lock) {
        PlayerD = 1;
      }
      break;
    case 2:
      if (!move_lock) {
        PlayerD = 2;
      }
      break;
    case 3:
      if (!move_lock) {
        PlayerD = 3;
      }
      break;
    case 4:
      if (dialog && millis() >= dialog_cool_time + Timer[3]) {
        room_f++;
        Timer[3] = millis();
      }
      break;
    case 5:
      break;
  }
  /*
     如果障碍物判断合法那么将会进行移动
  */
  if (player_move) SBDP();
  if (LA) {
    Entity[0][0] += CPDX;
    Entity[0][1] += CPDY;
    LA = false;
  }
}
void drawText(uint8_t x, uint8_t y, const uint8_t *mes, uint8_t cnt)
{
  uint8_t pb;
  uint8_t page;
  uint8_t screen_start = 0;
  uint8_t screen_end = 128;
  if (x < screen_start) {
    x = screen_start;
  }
  for (uint8_t i = 0; i < cnt; i++) {
    pb = pgm_read_byte_near(mes + i);
    page = MISAKI_FONT_F2_PAGE;
    if (pb > MISAKI_FONT_F2_SIZE) {
      continue;
    }
    if ((x + 8) > screen_end) {
      x = screen_start;
      y = y + 8;
    }
    arduboy.drawBitmap(x, y,  misaki_font_f2[ pb ], MISAKI_FONT_F2_W, 8, CN_text_BG);
    arduboy.drawBitmap(x + 7, y, misaki_font_0x00, 1, 8, CN_text_BG);
    x = x + 8;
  }
}
void setup() {
  arduboy.boot();
  arduboy.invert(DisplayInvert);
  //  Serial.begin(115200);
  draw(); //渲染画面
}
void loop() {
  if (!key_lock) key();
  logic();
  draw();
}
