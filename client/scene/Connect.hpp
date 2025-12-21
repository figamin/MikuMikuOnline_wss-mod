//
// Connect.hpp
//

#pragma once

#include <memory>
#include <vector>
#include <string>

// ✅ Boost thread primitives (NO <mutex>)
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "Base.hpp"
#include "../ui/UILabel.hpp"
#include "../ui/UIButton.hpp"
#include "../ManagerAccessor.hpp"

namespace scene {

class Connect : public Base {

public:
    Connect(const ManagerAccessorPtr&);
    ~Connect();

    void Begin();
    void Update();
    void ProcessInput(InputManager*);
    void Draw();
    void End();

private:
    // Background connection logic
    void AsyncInitialize();

private:
    ManagerAccessorPtr manager_accessor_;
    CardManagerPtr card_manager_;
    AccountManagerPtr account_manager_;
    ConfigManagerPtr config_manager_;
    CommandManagerPtr command_manager_;
    PlayerManagerPtr player_manager_;

    // UI
    UILabel message_;
    UILabel button_label_;
    UIButton button_;

    // Fonts
    int font_handle_;

    // Connection steps & progress
    std::vector<std::wstring> connection_steps_; // all connection stages
    size_t current_step_;                         // current step index
    float progress_;                              // 0.0 - 1.0 for progress bar

    // Progress bar position/size
    int progress_x_;
    int progress_y_;
    int progress_width_;
    int progress_height_;

    // State
    bool return_flag_;

    // ✅ Thread safety
    boost::mutex connect_mutex_;      // protects shared state below
    boost::thread connect_thread_;    // optional: safer than detach
};

} // namespace scene
