//
// Title.hpp
//

#pragma once

#include <memory>
#include <array>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "Base.hpp"
#include "../ManagerAccessor.hpp"
#include "../ui/Input.hpp"
#include "../ui/UILabel.hpp"
#include "../ui/UIButton.hpp"
#include "../Lobby.hpp"

namespace scene {

class Title : public Base {

public:
    Title(const ManagerAccessorPtr&);
    ~Title();
    void Begin();
    void Update();
    void ProcessInput(InputManager*);
    void Draw();
    void End();

private:
    void AsyncInitialize();

private:
    ManagerAccessorPtr manager_accessor_;
    CardManagerPtr card_manager_;
    AccountManagerPtr account_manager_;
    ConfigManagerPtr config_manager_;

    Input input_host_;
    UILabel label_;
    UIButton button_;

    Lobby lobby_;

    bool connect_flag_;
    int screen_count_;

    int title_image_handle_;
    bool about_open_;
    UILabel about_open_label_;
    UIButton about_open_button_;
    UILabel about_close_label_;
    UIButton about_close_button_;
    tstring about_message_;

    // Async resource loading
    boost::thread loader_thread_;
    boost::atomic<bool> resources_loaded_;
    

    // Positions and sizes
    int input_host_x_;
    int input_host_y_;
    int label_x_;
    int label_y_;
    int button_x_;
    int button_y_;
    int title_image_width_;
    int title_image_height_;
    int about_box_x_;
    int about_box_y_;
    int about_box_width_;
    int about_box_height_;

private:
    std::array<ImageHandlePtr,4> base_image_handle_;
    const static int BASE_BLOCK_SIZE;
};

} // namespace scene
