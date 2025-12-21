//
// Connect.cpp
//

#include "MainLoop.hpp"
#include "Title.hpp"
#include "Connect.hpp"
#include "ChannelChange.hpp"

#include <vector>
#include <algorithm>

#include "../ResourceManager.hpp"
#include "../PlayerManager.hpp"
#include "../CommandManager.hpp"
#include "../AccountManager.hpp"

#include "../../common/Logger.hpp"

#include <boost/chrono.hpp>
#include <boost/thread/locks.hpp>

namespace scene {

Connect::Connect(const ManagerAccessorPtr& manager_accessor) :
    manager_accessor_(manager_accessor),
    card_manager_(manager_accessor->card_manager().lock()),
    account_manager_(manager_accessor->account_manager().lock()),
    config_manager_(manager_accessor->config_manager().lock()),
    command_manager_(std::make_shared<CommandManager>(manager_accessor_)),
    player_manager_(std::make_shared<PlayerManager>(manager_accessor_)),
    return_flag_(false),
    font_handle_(-1),
    progress_(0.0f),
    current_step_(0)
{
    manager_accessor_->set_command_manager(command_manager_);
    manager_accessor_->set_player_manager(player_manager_);

    connection_steps_.push_back(L"Sending client info...");
    connection_steps_.push_back(L"Connecting to server...");
    connection_steps_.push_back(L"Authenticating...");
    connection_steps_.push_back(L"Receiving stage data...");
    connection_steps_.push_back(L"Finalizing...");

    progress_width_  = 400;
    progress_height_ = 20;
    progress_x_      = 100;
    progress_y_      = 100;
}

Connect::~Connect()
{
    if (connect_thread_.joinable()) {
        connect_thread_.join();
    }
}

void Connect::Begin()
{
    // Create fonts if needed
    if (font_handle_ < 0) {
        font_handle_ = DxLib::CreateFontToHandle(
            _T("UmePlus P Gothic"), 20, 2, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
    }

    // Setup UI immediately
    message_.set_text(connection_steps_[0]);
    message_.set_bgcolor(UIBase::Color(50, 50, 50, 200));
    message_.set_textcolor(UIBase::Color(255, 255, 255, 255));

    button_.set_width(100);
    button_.set_height(30);

    // We will position the button relative to the progress bar in Draw()
    button_.set_on_click([this]() {
        boost::lock_guard<boost::mutex> lock(connect_mutex_);
        return_flag_ = true;
    });

    // Draw first frame immediately to avoid black screen
    Draw();
    DxLib::ScreenFlip();

    // Start async initialization thread
    connect_thread_ = boost::thread(&Connect::AsyncInitialize, this);
}


void Connect::AsyncInitialize()
{
    try {
        // Step 0: Sending client info (first message already displayed)
        {
            boost::lock_guard<boost::mutex> lock(connect_mutex_);
            current_step_ = 0;
        }

        boost::this_thread::sleep_for(boost::chrono::milliseconds(10)); // ensure UI update

        // Step 1: Connecting to server
        {
            boost::lock_guard<boost::mutex> lock(connect_mutex_);
            current_step_ = 1;
        }

        // Expensive operation: construct network client
        ClientUniqPtr client(new network::Client(
            account_manager_->host(),
            config_manager_->port(),
            (uint16_t)config_manager_->udp_port(),
            account_manager_->public_key(),
            account_manager_->private_key(),
            "server_key.pub",
            config_manager_->upnp()
        ));

        {
            boost::lock_guard<boost::mutex> lock(connect_mutex_);
            command_manager_->set_client(std::move(client));
            current_step_ = 2; // Authenticating
        }

        // Wait for ready or error
        for (;;) {
            CommandManager::Status s;
            {
                boost::lock_guard<boost::mutex> lock(connect_mutex_);
                s = command_manager_->status();
            }
            if (s == CommandManager::STATUS_READY || s >= CommandManager::STATUS_ERROR) {
                break;
            }
            boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
        }

        // Step 3: Receiving stage data
        {
            boost::lock_guard<boost::mutex> lock(connect_mutex_);
            current_step_ = 3;
        }

        // Step 4: Finalizing
        boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
        {
            boost::lock_guard<boost::mutex> lock(connect_mutex_);
            current_step_ = 4;
        }

    } catch (const std::exception& e) {
        Logger::Error(
            _T("Connect thread exception: %s"),
            unicode::ToTString(e.what()));
    }
}

void Connect::Update()
{
    command_manager_->Update();

    boost::lock_guard<boost::mutex> lock(connect_mutex_);

    switch (command_manager_->status()) {
    case CommandManager::STATUS_ERROR:
        message_.set_text(L"Error: Connection failed");
        progress_ = 0.0f;
        command_manager_->set_client(ClientUniqPtr());
        break;

    case CommandManager::STATUS_ERROR_CROWDED:
        message_.set_text(L"Error: Server full");
        progress_ = 0.0f;
        command_manager_->set_client(ClientUniqPtr());
        break;

    case CommandManager::STATUS_ERROR_VERSION:
        message_.set_text(L"Error: Version mismatch");
        progress_ = 0.0f;
        command_manager_->set_client(ClientUniqPtr());
        break;

    case CommandManager::STATUS_ERROR_NOSTAGE:
        message_.set_text(L"Error: Stage data missing");
        progress_ = 0.0f;
        command_manager_->set_client(ClientUniqPtr());
        break;

    case CommandManager::STATUS_READY:
        next_scene_ = std::make_shared<scene::ChannelChange>(
            0, std::shared_ptr<VECTOR>(), manager_accessor_);
        break;

    default:
        current_step_ = std::min(current_step_, connection_steps_.size() - 1);
        message_.set_text(connection_steps_[current_step_]);
        progress_ = (float(current_step_) + 0.5f) / float(connection_steps_.size());
        if (progress_ > 1.0f) progress_ = 1.0f;
        break;
    }

    button_.Update();
    message_.Update();

    if (return_flag_) {
        next_scene_ = std::make_shared<scene::Title>(manager_accessor_);
    }
}

void Connect::ProcessInput(InputManager* input)
{
    button_.ProcessInput(input);
}

void Connect::Draw()
{
    int width, height;
    GetScreenState(&width, &height, nullptr);

    DrawBox(0, 0, width, height, GetColor(0, 0, 0), TRUE);

    boost::lock_guard<boost::mutex> lock(connect_mutex_);

    // Position progress bar offset from bottom-right corner
    const int offset_x = 50;
    const int offset_y = 50;
    progress_x_ = width - progress_width_ - offset_x;
    progress_y_ = height - progress_height_ - offset_y;

    // Progress background
    DrawBox(progress_x_, progress_y_,
            progress_x_ + progress_width_,
            progress_y_ + progress_height_,
            GetColor(80, 80, 80), TRUE);

    // Progress fill
    int fill_width = int(progress_ * progress_width_);
    DrawBox(progress_x_, progress_y_,
            progress_x_ + fill_width,
            progress_y_ + progress_height_,
            GetColor(157, 212, 187), TRUE);

    // Step text above the progress bar
    DxLib::DrawStringToHandle(
        progress_x_,
        progress_y_ - 30,
        unicode::ToTString(message_.text()).c_str(),
        GetColor(255, 255, 255),
        font_handle_);

    // Position cancel button **below the progress bar**
    button_.set_left(progress_x_);
    button_.set_top(progress_y_ + progress_height_ + 10); // 10px gap
    button_.Draw();

    // Draw cancel text centered in the button
    const TCHAR* cancel_text = _T("Cancel");
    int text_w = DxLib::GetDrawStringWidthToHandle(
        cancel_text, _tcslen(cancel_text), ResourceManager::default_font_handle());
    int text_x = button_.left() + (button_.width() - text_w) / 2;
    int text_y = button_.top() + (button_.height() - 18) / 2;
    DxLib::DrawStringToHandle(text_x, text_y, cancel_text, GetColor(0, 0, 0), ResourceManager::default_font_handle());
}


void Connect::End()
{
    if (connect_thread_.joinable()) {
        connect_thread_.join();
    }
}

} // namespace scene
