//
// UILabel.hpp
//

#pragma once

#include "UIBase.hpp"
#include <array>
#include <string>
#include "../../common/unicode.hpp"

class UILabel : public UIBase {
public:
    UILabel();
    ~UILabel();

    void ProcessInput(InputManager* input);
    void Update();
    void Draw();

    void set_text(const tstring& text);
    tstring text() const;

    void set_textcolor(const Color& color);
    Color textcolor() const;

    void set_bgcolor(const Color& color);
    Color bgcolor() const;

    // Shadow support
    void set_shadow_color(const Color& color);
    Color shadow_color() const;

    void set_shadow_alpha(int alpha);
    int shadow_alpha() const;

public:
    static void DefineInstanceTemplate(Handle<ObjectTemplate>* object);

private:
    // property bindings for JS
    static Handle<Value> Property_text(Local<String> property, const AccessorInfo &info);
    static void Property_set_text(Local<String> property, Local<Value> value, const AccessorInfo& info);

    static Handle<Value> Property_bgcolor(Local<String> property, const AccessorInfo &info);
    static void Property_set_bgcolor(Local<String> property, Local<Value> value, const AccessorInfo& info);

    static Handle<Value> Property_color(Local<String> property, const AccessorInfo &info);
    static void Property_set_color(Local<String> property, Local<Value> value, const AccessorInfo& info);

    // Shadow JS properties
    static Handle<Value> Property_shadow_color(Local<String> property, const AccessorInfo &info);
    static void Property_set_shadow_color(Local<String> property, Local<Value> value, const AccessorInfo& info);

    static Handle<Value> Property_shadow_alpha(Local<String> property, const AccessorInfo &info);
    static void Property_set_shadow_alpha(Local<String> property, Local<Value> value, const AccessorInfo& info);

private:
    void UpdatePosition();

private:
    tstring text_;
    std::vector<int> substr_list_;
    std::vector<int> char_width_list_;
    int font_handle_;

    Color textcolor_;
    Color bgcolor_;

    // Shadow members
    Color shadowColor_;   // Default should be fully transparent (off)
    int shadowAlpha_;     // 0 = fully transparent

private:
    const static int BASE_BLOCK_SIZE;
};
