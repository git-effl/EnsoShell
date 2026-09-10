/*
  VitaShell
  Copyright (C) 2015-2018, TheFloW

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "init.h"
#include "file.h"
#include "config.h"
#include "theme.h"
#include "utils.h"

INCLUDE_EXTERN_RESOURCE(default_colors_txt);
INCLUDE_EXTERN_RESOURCE(default_archive_icon_png);
INCLUDE_EXTERN_RESOURCE(default_audio_icon_png);
INCLUDE_EXTERN_RESOURCE(default_battery_bar_charge_png);
INCLUDE_EXTERN_RESOURCE(default_battery_bar_green_png);
INCLUDE_EXTERN_RESOURCE(default_battery_bar_red_png);
INCLUDE_EXTERN_RESOURCE(default_battery_png);
INCLUDE_EXTERN_RESOURCE(default_cover_png);
INCLUDE_EXTERN_RESOURCE(default_fastforward_png);
INCLUDE_EXTERN_RESOURCE(default_fastrewind_png);
INCLUDE_EXTERN_RESOURCE(default_file_icon_png);
INCLUDE_EXTERN_RESOURCE(default_file_symlink_icon_png);
INCLUDE_EXTERN_RESOURCE(default_folder_icon_png);
INCLUDE_EXTERN_RESOURCE(default_folder_symlink_icon_png);
INCLUDE_EXTERN_RESOURCE(default_ftp_png);
INCLUDE_EXTERN_RESOURCE(default_image_icon_png);
INCLUDE_EXTERN_RESOURCE(default_pause_png);
INCLUDE_EXTERN_RESOURCE(default_play_png);
INCLUDE_EXTERN_RESOURCE(default_sfo_icon_png);
INCLUDE_EXTERN_RESOURCE(default_text_icon_png);

// Shell colors
int BACKGROUND_COLOR;
int TITLE_COLOR;
int PATH_COLOR;
int DATE_TIME_COLOR;

// Settings color
int SETTINGS_MENU_COLOR;
int SETTINGS_MENU_FOCUS_COLOR;
int SETTINGS_MENU_TITLE_COLOR;
int SETTINGS_MENU_ITEM_COLOR;
int SETTINGS_MENU_OPTION_COLOR;

// File browser colors
int FOCUS_COLOR;
int FILE_COLOR;
int FILE_SYMLINK_COLOR;
int FOLDER_SYMLINK_COLOR;
int SFO_COLOR;
int TXT_COLOR;
int FOLDER_COLOR;
int IMAGE_COLOR;
int ARCHIVE_COLOR;
int SCROLL_BAR_COLOR;
int SCROLL_BAR_BG_COLOR;
int MARKED_COLOR;

// Context menu colors
int CONTEXT_MENU_TEXT_COLOR;
int CONTEXT_MENU_FOCUS_COLOR;
int CONTEXT_MENU_COLOR;
int CONTEXT_MENU_MORE_COLOR;
int INVISIBLE_COLOR;

// Dialog colors
int DIALOG_COLOR;
int DIALOG_BG_COLOR;
int PROGRESS_BAR_COLOR;
int PROGRESS_BAR_BG_COLOR;

// Hex editor colors
int HEX_COLOR;
int HEX_OFFSET_COLOR;
int HEX_NIBBLE_COLOR;

// Text editor colors
int TEXT_COLOR;
int TEXT_FOCUS_COLOR;
int TEXT_LINE_NUMBER_COLOR;
int TEXT_LINE_NUMBER_COLOR_FOCUS;
int TEXT_HIGHLIGHT_COLOR;

// Photo viewer colors
int PHOTO_ZOOM_COLOR;

// Audio player colors
int AUDIO_INFO_ASSIGN;
int AUDIO_INFO;
int AUDIO_SPEED;
int AUDIO_TIME_CURRENT;
int AUDIO_TIME_SLASH;
int AUDIO_TIME_TOTAL;
int AUDIO_TIME_BAR;
int AUDIO_TIME_BAR_BG;

vita2d_texture *folder_icon = NULL, *folder_symlink_icon = NULL, *file_icon = NULL, *file_symlink_icon = NULL, *archive_icon = NULL, *image_icon = NULL,
    *audio_icon = NULL, *sfo_icon = NULL, *text_icon = NULL,
         *ftp_image = NULL, *dialog_image = NULL, *context_image = NULL, *context_more_image = NULL, *settings_image = NULL, *battery_image = NULL,
         *battery_bar_red_image = NULL, *battery_bar_green_image = NULL, *battery_bar_charge_image = NULL, *bg_browser_image = NULL, *bg_hex_image = NULL, *bg_text_image = NULL,
         *bg_photo_image = NULL, *bg_audio_image = NULL, *cover_image = NULL, *play_image = NULL, *pause_image = NULL, *fastforward_image = NULL, *fastrewind_image = NULL;

vita2d_texture *wallpaper_image = NULL;
vita2d_texture *previous_wallpaper_image = NULL, *current_wallpaper_image = NULL;

vita2d_pgf *font = NULL;
char font_size_cache[256];

typedef struct {
  char *name;
  void *default_buf;
  vita2d_texture **texture;
  int is_icon; // Flag to prevent assigning 24x24 place-holders to full-screen background textures
} ThemeImage;

ThemeImage theme_images[] = {
  { "archive_icon.png", &_binary_resources_default_archive_icon_png_start, &archive_icon, 1 },
  { "audio_icon.png", &_binary_resources_default_audio_icon_png_start, &audio_icon, 1 },
  { "battery.png", &_binary_resources_default_battery_png_start, &battery_image, 1 },
  { "battery_bar_charge.png", &_binary_resources_default_battery_bar_charge_png_start, &battery_bar_charge_image, 1 },
  { "battery_bar_green.png", &_binary_resources_default_battery_bar_green_png_start, &battery_bar_green_image, 1 },
  { "battery_bar_red.png", &_binary_resources_default_battery_bar_red_png_start, &battery_bar_red_image, 1 },
  { "bg_audioplayer.png", NULL, &bg_audio_image, 0 },
  { "bg_browser.png", NULL, &bg_browser_image, 0 },
  { "bg_hexeditor.png", NULL, &bg_hex_image, 0 },
  { "bg_photoviewer.png", NULL, &bg_photo_image, 0 },
  { "bg_texteditor.png", NULL, &bg_text_image, 0 },
  { "context.png", NULL, &context_image, 0 },
  { "context_more.png", NULL, &context_more_image, 0 },
  { "cover.png", &_binary_resources_default_cover_png_start, &cover_image, 1 },
  { "dialog.png", NULL, &dialog_image, 0 },
  { "fastforward.png", &_binary_resources_default_fastforward_png_start, &fastforward_image, 1 },
  { "fastrewind.png", &_binary_resources_default_fastrewind_png_start, &fastrewind_image, 1 },
  { "file_icon.png", &_binary_resources_default_file_icon_png_start, &file_icon, 1 },
  { "file_symlink_icon.png", &_binary_resources_default_file_symlink_icon_png_start, &file_symlink_icon, 1 },
  { "folder_icon.png", &_binary_resources_default_folder_icon_png_start, &folder_icon, 1 },
  { "folder_symlink_icon.png", &_binary_resources_default_folder_symlink_icon_png_start, &folder_symlink_icon, 1 },
  { "ftp.png", &_binary_resources_default_ftp_png_start, &ftp_image, 1 },
  { "image_icon.png", &_binary_resources_default_image_icon_png_start, &image_icon, 1 },
  { "pause.png", &_binary_resources_default_pause_png_start, &pause_image, 1 },
  { "play.png", &_binary_resources_default_play_png_start, &play_image, 1 },
  { "settings.png", NULL, &settings_image, 0 },
  { "sfo_icon.png", &_binary_resources_default_sfo_icon_png_start, &sfo_icon, 1 },
  { "text_icon.png", &_binary_resources_default_text_icon_png_start, &text_icon, 1 },
  { "wallpaper.png", NULL, &wallpaper_image, 0 },
};

#define N_THEME_IMAGES (sizeof(theme_images) / sizeof(ThemeImage))

static void fill_texture_color(vita2d_texture *tex, uint32_t color) {
  if (!tex) return;
  uint32_t *data = (uint32_t *)vita2d_texture_get_datap(tex);
  if (!data) return;
  
  unsigned int width = vita2d_texture_get_width(tex);
  unsigned int height = vita2d_texture_get_height(tex);
  unsigned int size = width * height;
  
  for (unsigned int i = 0; i < size; i++) {
    data[i] = color;
  }
}

void loadTheme() {
  #define COLOR_ENTRY(name) { #name, CONFIG_TYPE_HEXDECIMAL, (void *)&name }
  ConfigEntry colors_entries[] = {
    COLOR_ENTRY(BACKGROUND_COLOR),
    COLOR_ENTRY(TITLE_COLOR),
    COLOR_ENTRY(PATH_COLOR),
    COLOR_ENTRY(DATE_TIME_COLOR),
    COLOR_ENTRY(SETTINGS_MENU_COLOR),
    COLOR_ENTRY(SETTINGS_MENU_FOCUS_COLOR),
    COLOR_ENTRY(SETTINGS_MENU_TITLE_COLOR),
    COLOR_ENTRY(SETTINGS_MENU_ITEM_COLOR),
    COLOR_ENTRY(SETTINGS_MENU_OPTION_COLOR),
    COLOR_ENTRY(FOCUS_COLOR),
    COLOR_ENTRY(FILE_COLOR),
    COLOR_ENTRY(FILE_SYMLINK_COLOR),
    COLOR_ENTRY(FOLDER_SYMLINK_COLOR),
    COLOR_ENTRY(SFO_COLOR),
    COLOR_ENTRY(TXT_COLOR),
    COLOR_ENTRY(FOLDER_COLOR),
    COLOR_ENTRY(IMAGE_COLOR),
    COLOR_ENTRY(ARCHIVE_COLOR),
    COLOR_ENTRY(SCROLL_BAR_COLOR),
    COLOR_ENTRY(SCROLL_BAR_BG_COLOR),
    COLOR_ENTRY(MARKED_COLOR),
    COLOR_ENTRY(CONTEXT_MENU_TEXT_COLOR),
    COLOR_ENTRY(CONTEXT_MENU_FOCUS_COLOR),
    COLOR_ENTRY(CONTEXT_MENU_COLOR),
    COLOR_ENTRY(CONTEXT_MENU_MORE_COLOR),
    COLOR_ENTRY(INVISIBLE_COLOR),
    COLOR_ENTRY(DIALOG_COLOR),
    COLOR_ENTRY(DIALOG_BG_COLOR),
    COLOR_ENTRY(PROGRESS_BAR_COLOR),
    COLOR_ENTRY(PROGRESS_BAR_BG_COLOR),
    COLOR_ENTRY(HEX_COLOR),
    COLOR_ENTRY(HEX_OFFSET_COLOR),
    COLOR_ENTRY(HEX_NIBBLE_COLOR),
    COLOR_ENTRY(TEXT_COLOR),
    COLOR_ENTRY(TEXT_FOCUS_COLOR),
    COLOR_ENTRY(TEXT_LINE_NUMBER_COLOR),
    COLOR_ENTRY(TEXT_LINE_NUMBER_COLOR_FOCUS),
    COLOR_ENTRY(TEXT_HIGHLIGHT_COLOR),
    COLOR_ENTRY(PHOTO_ZOOM_COLOR),
    COLOR_ENTRY(AUDIO_INFO_ASSIGN),
    COLOR_ENTRY(AUDIO_INFO),
    COLOR_ENTRY(AUDIO_SPEED),
    COLOR_ENTRY(AUDIO_TIME_CURRENT),
    COLOR_ENTRY(AUDIO_TIME_SLASH),
    COLOR_ENTRY(AUDIO_TIME_TOTAL),
    COLOR_ENTRY(AUDIO_TIME_BAR),
    COLOR_ENTRY(AUDIO_TIME_BAR_BG),
  };

  int i;

  // Read default colors from internal embedded resource
  readConfigBuffer(&_binary_resources_default_colors_txt_start, 
                   (uintptr_t)&_binary_resources_default_colors_txt_size,
                   colors_entries, sizeof(colors_entries) / sizeof(ConfigEntry));

  if (use_custom_config) {
    char path[MAX_PATH_LENGTH];
    char *theme_name = NULL;
    ConfigEntry theme_entries[] = {
      { "THEME_NAME", CONFIG_TYPE_STRING, (void *)&theme_name },
    };

    readConfig("ux0:VitaShell/theme/theme.txt", theme_entries, sizeof(theme_entries) / sizeof(ConfigEntry));

    if (theme_name) {
      snprintf(path, MAX_PATH_LENGTH, "ux0:VitaShell/theme/%s/colors.txt", theme_name);
      readConfig(path, colors_entries, sizeof(colors_entries) / sizeof(ConfigEntry));
      
      snprintf(path, MAX_PATH_LENGTH, "ux0:VitaShell/theme/%s/font.pgf", theme_name);
      font = vita2d_load_custom_pgf(path);
      
      for (i = 0; i < N_THEME_IMAGES; i++) {
        snprintf(path, MAX_PATH_LENGTH, "ux0:VitaShell/theme/%s/%s", theme_name, theme_images[i].name);
        if (theme_images[i].texture && *(theme_images[i].texture) == NULL) {
          *(theme_images[i].texture) = vita2d_load_PNG_file(path);
        }
      }
    }
  }

  // Load default embedded icons or create fallback
  for (i = 0; i < N_THEME_IMAGES; i++) {
    if (theme_images[i].texture && *(theme_images[i].texture) == NULL) {
      if (theme_images[i].default_buf) {
        *(theme_images[i].texture) = vita2d_load_PNG_buffer(theme_images[i].default_buf);
      }
      
      // Fallback ONLY for missing standard icons (never full screen surfaces)
      if (*(theme_images[i].texture) == NULL && theme_images[i].is_icon) {
        vita2d_texture *placeholder = vita2d_create_empty_texture(24, 24);
        if (placeholder) {
          fill_texture_color(placeholder, 0xFF555555);
          *(theme_images[i].texture) = placeholder;
        }
      }
    }
  }

  // Handle procedural screen backgrounds with safe NULL bounds checking
  if (!dialog_image) {
    dialog_image = vita2d_create_empty_texture(SCREEN_WIDTH, SCREEN_HEIGHT);
    fill_texture_color(dialog_image, DIALOG_BG_COLOR);
  }

  if (!context_image) {
    context_image = vita2d_create_empty_texture(SCREEN_WIDTH, SCREEN_HEIGHT);
    fill_texture_color(context_image, CONTEXT_MENU_COLOR);
  }

  if (!context_more_image) {
    context_more_image = vita2d_create_empty_texture(SCREEN_WIDTH, SCREEN_HEIGHT);
    fill_texture_color(context_more_image, CONTEXT_MENU_MORE_COLOR);
  }

  if (!settings_image) {
    settings_image = vita2d_create_empty_texture(SCREEN_WIDTH, SCREEN_HEIGHT);
    fill_texture_color(settings_image, SETTINGS_MENU_COLOR);
  }

  // Load PGF font fallback if custom theme font wasn't present
  if (!font) {
    font = loadSystemFonts();
  }

  // Cache character dimensions
  for (i = 0; i < 256; i++) {
    char character[2];
    character[0] = (char)i;
    character[1] = '\0';
    font_size_cache[i] = pgf_text_width(character);
  }
}