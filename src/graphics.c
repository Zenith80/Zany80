#include <Zany80/API/graphics.h>
#include "stretchy_buffer.h"
#include <stdio.h>

#include "Zany80/internal/graphics.h"

static window_t root = {
    .widgets = NULL,
    .name = "##MainMenuBar"
};

window_t *window_create(const char *name) {
    window_t *w = malloc(sizeof(window_t));
    w->initialX = w->initialY = w->maxX = w->maxY = w->minX = w->minY = -1;
    w->name = name;
    w->widgets = NULL;
    w->menus = NULL;
    return w;
}

void window_min_size(window_t *window, float x, float y) {
    window->minX = x;
    window->minY = y;
}

void window_max_size(window_t *window, float x, float y) {
    window->maxX = x;
    window->maxY = y;
}

void window_initial_size(window_t *window, float x, float y) {
    window->initialX = x;
    window->initialY = y;
}

void window_append(window_t *window, widget_t *widget) {
    sb_push(window->widgets, widget);
}

void window_append_menu(window_t *window, menu_t *menu) {
    sb_push(window->menus, menu);
}

void window_remove_menu(window_t *window, menu_t *menu) {
    // Find it
    for (int i = 0; i < sb_count(window->menus); i++) {
        if (menu == window->menus[i]) {
            // Construct new list containing all save for this one
            menu_t **menus = NULL;
            for (int j = 0; j < i; j++) {
                sb_push(menus, window->menus[j]);
            }
            for (int j = i + 1; j < sb_count(window->menus); j++) {
                sb_push(menus, window->menus[j]);
            }
            sb_free(window->menus);
            window->menus = menus;
            return;
        }
    }
    fputs("Menu not found for removal\n", stderr);
}

window_t *get_root() {
    return &root;
}

void window_destroy(window_t *window) {
    if (window == get_root()) {
        fputs("Attempted to destroy root window!\n", stderr);
    }
    else {
        sb_free(window->menus);
        sb_free(window->widgets);
        free(window);
    }
}

menu_t *menu_create(const char *name) {
    menu_t *m = malloc(sizeof(menu_t));
    m->widgets = NULL;
    m->label = name;
    return m;
}

void menu_append(menu_t *menu, widget_t *widget) {
    sb_push(menu->widgets, widget);
}

void menu_destroy(menu_t *menu) {
    sb_free(menu->widgets);
    free(menu);
}

static widget_t *widget_new(const char *label) {
    widget_t *w = malloc(sizeof(widget_t));
    w->label = label;
    w->visible = true;
    return w;
}

widget_t *button_create(const char *label, void(*handler)()) {
    widget_t *w = widget_new(label);
    w->type = button;
    w->button.handler = handler;
    return w;
}

widget_t *checkbox_create(const char *label, bool *value, void(*handler)()) {
    widget_t *w = widget_new(label);
    w->type = checkbox;
    w->checkbox.handler = handler;
    w->checkbox.value = value;
    return w;
}

widget_t *label_create(const char *_label) {
    widget_t *w = widget_new(_label);
    w->type = label;
    return w;
}

void label_set_wrapped(widget_t *widget, bool wrapped) {
    if (widget->type == label) {
        widget->_label.wrapped = wrapped;
    }
}

void widget_set_label(widget_t *widget, const char *label) {
    widget->label = label;
}

void widget_set_visible(widget_t *widget, bool visible) {
    widget->visible = visible;
}

void widget_destroy(widget_t *widget) {
    free(widget);
}

void append_main_menu(menu_t *menu) {
    window_append_menu(get_root(), menu);
}

widget_t *group_create() {

}

void group_add(widget_t *group, widget_t *widget) {

}

void group_remove(widget_t *group, widget_t *widget) {

}

void group_clear(widget_t *group) {

}

void group_destroy(widget_t *group) {

}

void group_setorientation(widget_t *group, group_orientation_t orientation) {

}
