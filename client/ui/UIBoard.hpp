//
// UIBoard.hpp
//

#pragma once

#include "UIBase.hpp"
#include "../ResourceManager.hpp"
#include <array>

class UIBoard : public UIBase {
public:
    UIBoard();
    void ProcessInput(InputManager* input);
    void Update();
    void Draw();

    bool resizable() const;
    bool boardvisible() const;
    void set_resizable(bool resizable);
    void set_boardvisible(bool visible);

    // New background properties
    int bgcolor() const { return bgcolor_; }
    void set_bgcolor(int color) { bgcolor_ = color; }

    bool use_image_bg() const { return use_image_bg_; }
    void set_use_image_bg(bool b) { use_image_bg_ = b; }

public:
    static void DefineInstanceTemplate(Handle<ObjectTemplate>* object);

private:
    void UpdateDrag(InputManager* input, bool resizeable = true);

private:
    // Existing property accessors
    static Handle<Value> Property_max_width(Local<String> property, const AccessorInfo &info);
    static void Property_set_max_width(Local<String> property, Local<Value> value, const AccessorInfo& info);
    static Handle<Value> Property_min_width(Local<String> property, const AccessorInfo &info);
    static void Property_set_min_width(Local<String> property, Local<Value> value, const AccessorInfo& info);
    static Handle<Value> Property_max_height(Local<String> property, const AccessorInfo &info);
    static void Property_set_max_height(Local<String> property, Local<Value> value, const AccessorInfo& info);
    static Handle<Value> Property_min_height(Local<String> property, const AccessorInfo &info);
    static void Property_set_min_height(Local<String> property, Local<Value> value, const AccessorInfo& info);

    // New property accessors for JS exposure
    static Handle<Value> Property_bgcolor(Local<String> property, const AccessorInfo &info);
    static void Property_set_bgcolor(Local<String> property, Local<Value> value, const AccessorInfo& info);
    static Handle<Value> Property_use_image_bg(Local<String> property, const AccessorInfo &info);
    static void Property_set_use_image_bg(Local<String> property, Local<Value> value, const AccessorInfo& info);

private:
    std::array<ImageHandlePtr,4> base_image_handle_;

    bool resizable_;
    bool boardvisible_;

    int max_width_, min_width_;
    int max_height_, min_height_;

    Rect drag_offset_rect_, drag_resize_offset_rect_;

    // New properties
    int bgcolor_;          // ARGB or RGB value
    bool use_image_bg_;    // true = draw image, false = draw solid/transparent color

private:
    const static int BASE_BLOCK_SIZE;
};

typedef std::shared_ptr<UIBoard> UIBoardPtr;
