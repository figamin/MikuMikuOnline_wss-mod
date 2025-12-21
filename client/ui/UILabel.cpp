//
// UILabel.cpp
//

/**
* @module global
* @submodule UI
 */

/**
 * @class Label
 * @namespace UI
 * @extends UI.Base
 */

#include "UILabel.hpp"
#include "../ScriptEnvironment.hpp"
#include "../ResourceManager.hpp"
#include "../../common/Logger.hpp"
#include <numeric>

const int UILabel::BASE_BLOCK_SIZE = 12;

UILabel::UILabel()
    : font_handle_(ResourceManager::default_font_handle()),
      shadowColor_(0, 0, 0),   // default shadow off
      shadowAlpha_(0)             // fully transparent
{
}


UILabel::~UILabel()
{
}

void UILabel::set_text(const tstring& text)
{
    text_ = text;

    // Calculate the character width
    char_width_list_.clear();
    for (auto it = text_.begin(); it != text_.end(); ++it) {
        #ifdef UNICODE
            TCHAR c = *it;
            int width = GetDrawStringWidthToHandle(&c, 1, font_handle_);
        #else
            unsigned char c = *it;
            TCHAR string[2] = {0, 0};

            int width;
            if (((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc)) && (it + 1) != text_.end()) {
                string[0] = c;
                string[1] = *(it + 1);
                ++it;
                width = GetDrawStringWidthToHandle(string, 2, font_handle_);
            } else {
                string[0] = c;
                width = GetDrawStringWidthToHandle(string, 1, font_handle_);
            }
        #endif
        char_width_list_.push_back(width);
    }
}

tstring UILabel::text() const
{
    return unicode::ToTString(text_);
}

void UILabel::set_textcolor(const Color& color)
{
    textcolor_ = color;
}

UIBase::Color UILabel::textcolor() const
{
    return textcolor_;
}

void UILabel::set_bgcolor(const Color& color)
{
    bgcolor_ = color;
}

UIBase::Color UILabel::bgcolor() const
{
    return bgcolor_;
}

// New shadow properties
void UILabel::set_shadow_color(const Color& color)
{
    shadowColor_ = color;
}

UIBase::Color UILabel::shadow_color() const
{
    return shadowColor_;
}

void UILabel::set_shadow_alpha(int alpha)
{
    shadowAlpha_ = alpha;
}

int UILabel::shadow_alpha() const
{
    return shadowAlpha_;
}

// --- JS bindings ---
Handle<Value> UILabel::Property_text(Local<String> property, const AccessorInfo &info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);
    return String::New(unicode::ToString(self->text()).c_str());
}

void UILabel::Property_set_text(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);

    self->set_text(unicode::ToTString(*String::Utf8Value(value->ToString())));
}

Handle<Value> UILabel::Property_bgcolor(Local<String> property, const AccessorInfo &info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);
    return String::New(self->bgcolor().ToString().c_str());
}

void UILabel::Property_set_bgcolor(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);

    self->set_bgcolor(UIBase::Color::FromString(*String::Utf8Value(value->ToString())));
}

Handle<Value> UILabel::Property_color(Local<String> property, const AccessorInfo &info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);
    return String::New(self->textcolor().ToString().c_str());
}

void UILabel::Property_set_color(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);

    self->set_textcolor(UIBase::Color::FromString(*String::Utf8Value(value->ToString())));
}

// Shadow JS bindings
Handle<Value> UILabel::Property_shadow_color(Local<String> property, const AccessorInfo &info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);
    return String::New(self->shadow_color().ToString().c_str());
}

void UILabel::Property_set_shadow_color(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);

    self->set_shadow_color(UIBase::Color::FromString(*String::Utf8Value(value->ToString())));
}

Handle<Value> UILabel::Property_shadow_alpha(Local<String> property, const AccessorInfo &info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);
    return Integer::New(self->shadow_alpha());
}

void UILabel::Property_set_shadow_alpha(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    assert(info.This()->InternalFieldCount() > 0);
    auto self = std::dynamic_pointer_cast<UILabel>(
        *static_cast<UIBasePtr*>(Local<External>::Cast(info.This()->GetInternalField(0))->Value())
    );
    assert(self);

    self->set_shadow_alpha(value->Int32Value());
}

void UILabel::DefineInstanceTemplate(Handle<ObjectTemplate>* object)
{
    UIBase::DefineInstanceTemplate(object);

    Handle<ObjectTemplate>& instance_template = *object;

    SetProperty(&instance_template, "text", Property_text, Property_set_text);
    SetProperty(&instance_template, "bgcolor", Property_bgcolor, Property_set_bgcolor);
    SetProperty(&instance_template, "color", Property_color, Property_set_color);

    // Shadow properties
    SetProperty(&instance_template, "shadow_color", Property_shadow_color, Property_set_shadow_color);
    SetProperty(&instance_template, "shadow_alpha", Property_shadow_alpha, Property_set_shadow_alpha);
}

void UILabel::ProcessInput(InputManager* input)
{
    bool hover = (absolute_x()<= input->GetMouseX() && input->GetMouseX() <= absolute_x()+ absolute_width()
            && absolute_y() <= input->GetMouseY() && input->GetMouseY() <= absolute_y() + absolute_height());
    if (input->GetMouseLeftCount() == 1 && hover && GetActiveFlag() != 0) {  
        if (!on_click_.IsEmpty() && on_click_->IsFunction()) {
            on_click_.As<Function>()->CallAsFunction(Context::GetCurrent()->Global(), 0, nullptr);
        } else if(!on_click_function_._Empty()) {
            on_click_function_(this);
            parent_c_->set_visible(false);
            input->CancelMouseLeft();
        }
    } else if(hover && !hover_flag_ && GetActiveFlag() != 0) {
        if(!on_hover_function_._Empty()) {
            on_hover_function_(this);
            hover_flag_ = true;
        }
    } else if(!hover && hover_flag_ && GetActiveFlag() != 0) {
        if(!on_out_function_._Empty()) {
            on_out_function_(this);
            hover_flag_ = false;
        }
    }
}

void UILabel::UpdatePosition()
{
    int parent_x, parent_y, parent_width, parent_height;

    if (parent_.IsEmpty()) {
        parent_x = 0;
        parent_y = 0;
        GetScreenState(&parent_width, &parent_height, nullptr);
    } else {
        UIBasePtr parent_ptr = *static_cast<UIBasePtr*>(Local<External>::Cast(parent_->GetInternalField(0))->Value());
        parent_x = parent_ptr->absolute_x();
        parent_y = parent_ptr->absolute_y();
        parent_width = parent_ptr->absolute_width();
        parent_height = parent_ptr->absolute_height();
    }

    if((docking_ & DOCKING_LEFT) && (docking_ & DOCKING_RIGHT)) {
        int left = parent_x + left_;
        int right = parent_x + parent_width - right_;
        absolute_rect_.width = right - left;
    } else {
        if (width_ > 0) { 
            absolute_rect_.width = width_;
        } else {
            absolute_rect_.width = std::accumulate(char_width_list_.begin(), char_width_list_.end(), 0);
        }
    }

    int line_width = 0;
    int line_num = 1;
    substr_list_.clear();
    substr_list_.push_back(0);
    auto text_cursor = 0;

    for (auto it = char_width_list_.begin(); it != char_width_list_.end(); ++it) {
        if ( text_[text_cursor] == _T('\n')) {
            line_num++;
            substr_list_.push_back(text_cursor);
            substr_list_.push_back(text_cursor + 1);
            line_width = 0;
        } else if (line_width + *it > absolute_width()) {
            line_width = *it;
            line_num++;
            substr_list_.push_back(text_cursor);
            substr_list_.push_back(text_cursor);
        } else {
            line_width += *it;
        }

        text_cursor++;
    }
    substr_list_.push_back(text_.length());

    if((docking_ & DOCKING_TOP) && (docking_ & DOCKING_BOTTOM)) {
        int top = parent_y + top_;
        int bottom = parent_y + parent_height - bottom_;
        absolute_rect_.height = bottom - top;
    } else {
        absolute_rect_.height = (ResourceManager::default_font_size() + 2) * line_num;
    }

    absolute_rect_.x = parent_x + left_;

    if(docking_ & DOCKING_VCENTER) {
        absolute_rect_.y = parent_y + parent_height / 2 - absolute_rect_.height / 2;
    } else if(docking_ & DOCKING_BOTTOM) {
        absolute_rect_.y = parent_y + parent_height - bottom_ - absolute_rect_.height;
    } else {
        absolute_rect_.y = parent_y + top_;
    }
}

void UILabel::Update()
{
    UpdatePosition();
}

void UILabel::Draw()
{
    if (!visible_) {
        return;
    }

    int color = GetColor(textcolor_.r, textcolor_.g, textcolor_.b);
    int bgcolor = GetColor(bgcolor_.r, bgcolor_.g, bgcolor_.b);

    if (bgcolor_.a > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, bgcolor_.a);
        DrawBox(absolute_x(), absolute_y(),
                absolute_x() + absolute_width(), absolute_y() + absolute_height(), bgcolor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    int line_y = 1;
    int shadowOffsetX = 1;
    int shadowOffsetY = 1;
    int shadowColorInt = GetColor(shadowColor_.r, shadowColor_.g, shadowColor_.b);

    for (auto it = substr_list_.begin(); it != substr_list_.end(); ++it) {
        int begin = *it;
        int end = *(++it);
        std::wstring lineText = unicode::ToTString(text_.substr(begin, end - begin));

        if (shadowAlpha_ > 0) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, shadowAlpha_);
            DrawStringToHandle(absolute_x() + shadowOffsetX,
                               absolute_y() + line_y + shadowOffsetY,
                               lineText.c_str(), shadowColorInt, font_handle_);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        DrawStringToHandle(absolute_x(),
                           absolute_y() + line_y,
                           lineText.c_str(), color, font_handle_);

        line_y += ResourceManager::default_font_size() + 2;
    }
}
