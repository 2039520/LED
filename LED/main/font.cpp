#include "esp_partition.h"
#include "esp_log.h"
#define FONT_DATA_SIZE 32  // 假设我们要读取 1024 字节的数据

uint8_t* read_font_data(uint64_t unicode) {
    // // 获取分区信息
    // esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    // if (it == NULL) {
    //     printf("No partitions found");
    //     return;
    // }

    // do {
    //     const esp_partition_t *part = esp_partition_get(it);
    //     if (part != NULL) {
    //         printf("\n/////////////////////////////////////\n");
    //         printf("Partition label: %s\n", part->label);
    //         printf("Type: %d, Subtype: %d\n", part->type, part->subtype);
    //         printf("Address: 0x%#lx, Size: %#lx bytes\n", part->address, part->size);
    //         printf("\n/////////////////////////////////////\n");
    //     }
    // } while ((it = esp_partition_next(it)) != NULL);

    // // 释放迭代器
    // esp_partition_iterator_release(it);

   /// 查找分区
    const esp_partition_t* font_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "font");
    if (font_partition == NULL) {
        printf("Font partition not found");
        return nullptr;
    }

    // 读取数据
    uint8_t *font_data = new uint8_t[FONT_DATA_SIZE];
    esp_err_t err = esp_partition_read(font_partition, 32*unicode, font_data, FONT_DATA_SIZE);
    if (err != ESP_OK) {
        printf("Error reading font partition (%s)", esp_err_to_name(err));
        return nullptr;
    }
    printf("\n////////////////\n");
    printf("%lld  \n",unicode);
    for(int i=0;i<FONT_DATA_SIZE;i++){
        printf("%#x ",*(font_data+i));
    }
    printf("\n////////////////\n\n\n\n");

    return font_data;
}