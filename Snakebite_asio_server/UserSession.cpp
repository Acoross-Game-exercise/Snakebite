#include <string>

#include "UserSession.h"
#include "game_server.h"

namespace acoross {
namespace snakebite {

void UserSession::start()
{
	std::string myid = std::to_string((uintptr_t)this);
	game_session_->AddUpdateEventListner(
		myid,
		[us = this, rpcsocket_wp = std::weak_ptr<rpc::RpcSocket>(shared_from_this())]
	(
		int idx_x, int idx_y, 
		SbGeoZone::CloneZoneObjListT& snake_clone_list,
		SbGeoZone::CloneZoneObjListT& apple_clone_list)
	{
		if (auto rpcsocket = rpcsocket_wp.lock())
		{
			_ASSERT(0);
			//FIXME!!!!!!
			//us->send_update_game_object(snake_clone_list, apple_clone_list);
		}
	});

	messages::SnakebiteService::Service::start();
}

void UserSession::end()
{
	game_session_->AsyncRemoveSnake(user_snake_handle_);

	std::string myid = std::to_string((uintptr_t)this);
	game_session_->UnregisterEventListner(myid);
}

void UserSession::send_update_game_object(
	const std::list<std::pair<Handle<Snake>::Type, ZoneObjectClone>>& snake_clone_list, 
	const std::list<ZoneObjectClone>& apple_clone_list)
{
	if (!push_stub_)
	{
		return;
	}

	messages::UpdateGameObjectsEvent game_objects;
	{
		for (auto& pair : snake_clone_list)
		{
			auto& snake_clone = pair.second;
			auto* clone = game_objects.add_clone();

			clone->set_clone_type(0);
			clone->set_handle(Handle<Snake>(pair.first).handle);
			clone->set_obj_name(snake_clone.Name);

			auto* head = clone->mutable_head();
			head->set_radius(snake_clone.head_.GetRadius());
			head->set_x(snake_clone.head_.GetPosition().x);
			head->set_y(snake_clone.head_.GetPosition().y);

			for (auto& body : snake_clone.body_list_)
			{
				auto* pac_body = clone->add_body();
				pac_body->set_radius(body.GetRadius());
				pac_body->set_x(body.GetPosition().x);
				pac_body->set_y(body.GetPosition().y);
			}
		}

		for (auto& apple_clone : apple_clone_list)
		{
			auto* clone = game_objects.add_clone();
			clone->set_clone_type(1);
			clone->set_handle(0);
			clone->set_obj_name(apple_clone.Name);

			auto* head = clone->mutable_head();
			head->set_radius(apple_clone.head_.GetRadius());
			head->set_x(apple_clone.head_.GetPosition().x);
			head->set_y(apple_clone.head_.GetPosition().y);
		}
	}

	if (bool is_initialized = game_objects.IsInitialized())
	{
		push_stub_->UpdateGameObjects(game_objects, 
			[](rpc::ErrCode ec, messages::VoidReply&)
		{});
	}
}

acoross::rpc::ErrCode UserSession::InitPlayer(const acoross::snakebite::messages::InitPlayerSnakeRequest &rq, acoross::snakebite::messages::InitPlayerSnakeReply *rp)
{
	game_session_->AsyncRemoveSnake(user_snake_handle_);
	
	auto self_wp = std::weak_ptr<RpcSocket>(shared_from_this());
	user_snake_handle_ = game_session_->AsyncMakeNewSnake(rq.name(),
		[self_wp, this](Snake& snake)	//die callback
	{
		if (auto self = self_wp.lock())
		{
			user_snake_handle_ = 0;
			if (push_stub_)
			{
				messages::VoidReply vr;
				vr.set_err(0);

				push_stub_->ResetPlayer(vr,
					[](rpc::ErrCode ec, messages::VoidReply&)
				{});
			}
		}
	});

	if (rp)
	{
		rp->set_handle(user_snake_handle_);
	}

	return acoross::rpc::ErrCode();
}

acoross::rpc::ErrCode UserSession::SetKeyDown(const acoross::snakebite::messages::TurnKeyDownRequest &rq, acoross::snakebite::messages::VoidReply *rp)
{
	game_session_->RequestToSnake(user_snake_handle_,
		[pk = rq.key()](Snake& snake)
	{
		snake.SetKeyDown(PlayerKey(pk));
	});

	return acoross::rpc::ErrCode();
}

acoross::rpc::ErrCode UserSession::SetKeyUp(const acoross::snakebite::messages::TurnKeyUpRequest &rq, acoross::snakebite::messages::VoidReply *rp)
{
	game_session_->RequestToSnake(user_snake_handle_,
		[pk = rq.key()](Snake& snake)
	{
		snake.SetKeyUp(PlayerKey(pk));
	});

	return acoross::rpc::ErrCode();
}

}
}
