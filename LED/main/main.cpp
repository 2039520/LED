#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "font.cpp"
using namespace std;

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO GPIO_NUM_18
#define LED_STRIP_LENGTH 256


typedef struct {
    uint8_t red = 255;
    uint8_t green = 0;
    uint8_t blue = 255;
} rgb_t;


// 创建一个RMT项数组表示一个WS2812的位
void Ws2812MakeBit(rmt_item32_t *item, uint8_t bit_val) {
    if (bit_val) {//1
        //printf("1");
        item->level0 = 1;
        item->duration0 = 56; // 高电平持续时间约  700ns
        item->level1 = 0;
        item->duration1 = 48; // 低电平持续时间约  600ns  
    } else {//0
        //printf("0");
        item->level0 = 1;
        item->duration0 = 28; // 高电平持续时间约 350ns
        item->level1 = 0;
        item->duration1 = 64; // 低电平持续时间约 800ns
    }
}

uint16_t GetUnicodeFromChar(string c){
if(c == string("0")){
    return 2 ;
}
if(c == string("余")){
    return 0 ;
}
if(c == string("敏")){
    return 1 ;
}
return 0;
}

class Char{
public:
    Char(string c);
    ~Char();
public:
    string  m_str;
    uint8_t* m_map;  //16*16
    uint16_t m_unicode;
    rgb_t m_color;
public:
    void Ws2812WriteLeds();
};


    Char::Char(string c){
        m_str = c;
        m_unicode = GetUnicodeFromChar(m_str);
        m_map =  read_font_data(m_unicode);
    }
    Char::~Char(){
        if(m_map != nullptr){
            delete[] m_map;
        }
        m_map = nullptr;
    }

void Char::Ws2812WriteLeds() {
    rmt_item32_t *items = (rmt_item32_t*)malloc(LED_STRIP_LENGTH * 24 * sizeof(rmt_item32_t));
    memset(items,0,LED_STRIP_LENGTH * 24 * sizeof(rmt_item32_t));
    for (int i = 0; i < LED_STRIP_LENGTH; i++) {
        if(((i/16)+1)%2){//板子寄行数据
         

 int k = i+15 - 2*(i%16);
            if(m_map[k/8] & (0x80 >> (k%8))){
                uint32_t bits = 0;
                bits = ((uint32_t)m_color.green << 16) |((uint32_t)m_color.red << 8) | m_color.blue;
                for (int j = 0; j < 24; j++) {
                    Ws2812MakeBit(&items[i * 24 + j], (bits >> (23 -j)) & 0x01);
                }
            }else{
                for (int j = 0; j < 24; j++) {
                    Ws2812MakeBit(&items[i * 24 + j], 0);
                }
            }
        }else{//板子偶行数据
              if(m_map[i/8] & (0x80 >> (i%8))){
                uint32_t bits = 0;
                bits = ((uint32_t)m_color.green << 16) |((uint32_t)m_color.red << 8) | m_color.blue;
                for (int j = 0; j < 24; j++) {
                    Ws2812MakeBit(&items[i * 24 + j], (bits >> (23 -j)) & 0x01);
                }
            }else{
                for (int j = 0; j < 24; j++) {
                    Ws2812MakeBit(&items[i * 24 + j], 0);
                }
            }

        }
        
    }
    rmt_write_items(RMT_TX_CHANNEL, items, LED_STRIP_LENGTH * 24, true);
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    free(items);
}



void InitRmt(){
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    config.clk_div = 1; 
    rmt_config(&config);
    rmt_driver_install(RMT_TX_CHANNEL, 0, 0);
}
extern "C" void app_main(void) {
    InitRmt();

    while (1) {
        Char c ("余");
        c.Ws2812WriteLeds();
        vTaskDelay(pdMS_TO_TICKS(1000));

         Char a ("0");
        a.Ws2812WriteLeds();
        vTaskDelay(pdMS_TO_TICKS(1000));

         Char b ("敏");
        b.Ws2812WriteLeds();
        vTaskDelay(pdMS_TO_TICKS(1000));
       
    }
}
