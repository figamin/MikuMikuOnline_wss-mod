//
// ChannelChange.cpp
//

#include "MainLoop.hpp"
#include "ChannelChange.hpp"
#include "../CommandManager.hpp"
#include "../AccountManager.hpp"
#include "../../common/Logger.hpp"
#include "../../common/network/Utils.hpp"
#include "../3d/Stage.hpp"

namespace scene {

ChannelChange::ChannelChange(
    unsigned char channel,
    const std::shared_ptr<VECTOR>& init_position,
    const ManagerAccessorPtr& manager_accessor) :
    manager_accessor_(manager_accessor),
    card_manager_(manager_accessor->card_manager().lock()),
    command_manager_(manager_accessor->command_manager().lock()),
    account_manager_(manager_accessor->account_manager().lock()),
    config_manager_(manager_accessor->config_manager().lock()),
    player_manager_(manager_accessor->player_manager().lock()),
    channel_(channel),
    init_position_(init_position)
{
}

ChannelChange::~ChannelChange()
{
}

void ChannelChange::Begin()
{
    // Immediately perform all setup
    player_manager_->ResetStage();
    manager_accessor_->set_world_manager(WorldManagerPtr());

    ResourceManager::ClearModelHandle();

    // Send account info and channel to server
    auto name = account_manager_->name();
    command_manager_->Write(network::ServerUpdateAccountProperty(NAME, name));
    command_manager_->Write(network::ServerUpdateAccountProperty(CHANNEL, network::Utils::Serialize(channel_)));

    auto channel_ptr = command_manager_->channels().at(channel_);
    StagePtr stage = std::make_shared<Stage>(channel_ptr, manager_accessor_->config_manager().lock());
    world_manager_ = std::make_shared<WorldManager>(stage, manager_accessor_);
    manager_accessor_->set_world_manager(world_manager_);

    player_manager_->Init();
    world_manager_->Init();
    world_manager_->myself()->Init(unicode::ToTString(account_manager_->model_name()), init_position_);

    // Immediately switch to main loop
    next_scene_ = std::make_shared<scene::MainLoop>(manager_accessor_);
}

void ChannelChange::End()
{
}

void ChannelChange::Update()
{
    // No fade, so nothing to update
}

void ChannelChange::ProcessInput(InputManager* input)
{
    // No input handling needed here
}

void ChannelChange::Draw()
{
    // No fade, draw nothing
}

} // namespace scene
