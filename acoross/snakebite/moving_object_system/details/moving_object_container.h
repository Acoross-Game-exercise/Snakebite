#ifndef SNAKEBITE_MOVING_OBJECT_CONTAINER_H_
#define SNAKEBITE_MOVING_OBJECT_CONTAINER_H_

#include <list>
#include <map>
#include <memory>

#include "geo_types.h"

namespace acoross {
namespace snakebite {

template <typename MovingObject>
class MovingObjectContainer
{
public:
	using MovingObjectSP = std::shared_ptr<MovingObject>;
	using MovingObjectWP = std::weak_ptr<MovingObject>;
	typedef std::list<MovingObjectSP> ListMovingObject;

	// game field APIs
	const int Left{ 0 };
	const int Right{ 500 };
	const int Top{ 0 };
	const int Bottom{ 500 };

	int Width() const { return Right - Left; }
	int Height() const { return Top - Bottom; }

	void RegisterMovingObject(MovingObjectSP mo)
	{
		moving_objects_.push_back(mo);
	}

	void DeleteObject(MovingObjectSP mo)
	{
		for (auto it = moving_objects_.begin(); it != moving_objects_.end(); ++it)
		{
			if (*it == mo)
			{
				moving_objects_.erase(it);
				return;
			}
		}
	}

	void ProcessCollisions();

	//�ӽ÷� �����ִ� API
	ListMovingObject& GetMovingObjects() { return moving_objects_; }

private:
	// unique_ptr �̶� �ڵ� ������.
	ListMovingObject moving_objects_;
};

template <typename MovingObject>
inline void MovingObjectContainer<MovingObject>::ProcessCollisions()
{
	// ����Ʈ�� �����Ͽ� ó���Ѵ�.
	ListMovingObject mo_list = moving_objects_;

	//// clean up collision
	//for (auto& mo : mo_list)
	//{
	//	mo->Collided = false;
	//}

	for (auto& mo1 : mo_list)
	{
		for (auto& mo2 : mo_list)
		{
			if (mo1.get() == mo2.get())
				continue;

			if (IsCrashed(*mo1, *mo2))
			{
				mo1->Collide(*mo2);
				mo2->Collide(*mo1);
			}
			else
			{
				mo1->ReleaseCollision(*mo2);
				mo2->ReleaseCollision(*mo1);
			}
		}
	}
}

}
}
#endif SNAKEBITE_MOVING_OBJECT_CONTAINER_H_