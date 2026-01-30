// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

#include QMK_KEYBOARD_H
#include <stdio.h>
#include <string.h>
#include "manna-harbour_miryoku.h"

// ---------------------------------------------------------
// GLOBAL STORAGE
// ---------------------------------------------------------
char last_key_name[12] = "NONE";

// ---------------------------------------------------------
// TAP DANCE & LOGIC
// ---------------------------------------------------------
enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    reset_keyboard();
  }
}

#define MIRYOKU_X(LAYER, STRING) \
void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) { \
    default_layer_set((layer_state_t)1 << U_##LAYER); \
  } \
}
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {
    [U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

// keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

// shift functions
const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);
const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};

// thumb combos
#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, KC_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif

// ---------------------------------------------------------
// KEY LOGGER LOGIC
// ---------------------------------------------------------
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case KC_A ... KC_Z:
                last_key_name[0] = (char)keycode + ('A' - KC_A);
                last_key_name[1] = '\0';
                break;
            case KC_1 ... KC_0:
                last_key_name[0] = (char)keycode + ('1' - KC_1);
                last_key_name[1] = '\0';
                break;
            case KC_SPC: strcpy(last_key_name, "SPC"); break;
            case KC_ENT: strcpy(last_key_name, "ENT"); break;
            case KC_BSPC: strcpy(last_key_name, "BSP"); break;
            case KC_ESC: strcpy(last_key_name, "ESC"); break;
            case KC_TAB: strcpy(last_key_name, "TAB"); break;
            case KC_LSFT: strcpy(last_key_name, "SFT"); break;
            case KC_LCTL: strcpy(last_key_name, "CTL"); break;
            case KC_LALT: strcpy(last_key_name, "ALT"); break;
            case KC_LGUI: strcpy(last_key_name, "GUI"); break;
            default: strcpy(last_key_name, "*"); break;
        }
    }
    // FIX: Removed invalid function call. Return true to let QMK handle the key.
    return true;
}

// ---------------------------------------------------------
// OLED LOGIC (Safe Version)
// ---------------------------------------------------------
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) return OLED_ROTATION_180;
    return OLED_ROTATION_270;
}

const char *get_layer_name_string(void) {
    switch (get_highest_layer(layer_state)) {
        case U_BASE:   return "BASE";
        case U_EXTRA:  return "GAME";
        case U_TAP:    return "NUMS";
        case U_NAV:    return "NAV";
        case U_MOUSE:  return "MSE";
        case U_MEDIA:  return "MED";
        case U_NUM:    return "NUM";
        case U_SYM:    return "SYM";
        case U_FUN:    return "FUN";
        default:       return "UNK";
    }
}

void oled_write_ln_padded(const char *text) {
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%-5s", text);
    oled_write(buffer, false);
    oled_write_P(PSTR("\n"), false);
}

// SAFE Right-Side Logo (No heavy math)
void render_corne_logo(void) {
    oled_write_P(PSTR("CORNE\n"), false);
    oled_write_P(PSTR("SPLIT\n"), false);
    oled_write_P(PSTR("KBD\n\n"), false);
    oled_write_P(PSTR("RP2040"), false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        // --- MASTER SIDE (Left) ---
        oled_write_P(PSTR("LYR:\n"), false);
        oled_write_ln_padded(get_layer_name_string());
        oled_write_P(PSTR("\n"), false);
        oled_write_P(PSTR("KEY:\n"), false);
        oled_write_ln_padded(last_key_name);
    } else {
        // --- SLAVE SIDE (Right) ---
        render_corne_logo();
    }
    return false;
}
#endif
