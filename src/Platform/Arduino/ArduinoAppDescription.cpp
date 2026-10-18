#ifdef ARDUINO
#include <Data/AppVersion.h>
#include <esp_app_desc.h>
#include <sdkconfig.h>

// Override the weak descriptor supplied by the precompiled Arduino framework.
// The bootloader, OTA tools and esp_app_get_description() read this metadata.
extern "C" const esp_app_desc_t esp_app_desc
    __attribute__((section(".rodata_desc"), used)) = {
        .magic_word = ESP_APP_DESC_MAGIC_WORD,
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
        .secure_version = CONFIG_BOOTLOADER_APP_SECURE_VERSION,
#else
        .secure_version = 0,
#endif
        .reserv1 = {},
        .version = ULTIMATE_REMOTE_VERSION,
        .project_name = "ultimate-remote",
        .time = __TIME__,
        .date = __DATE__,
        .idf_ver = IDF_VER,
        .app_elf_sha256 = {},
#ifdef CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL
        .min_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL,
        .max_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL,
#endif
#ifdef CONFIG_MMU_PAGE_SIZE
        .mmu_page_size = __builtin_ctz(CONFIG_MMU_PAGE_SIZE),
#endif
    };
#endif
