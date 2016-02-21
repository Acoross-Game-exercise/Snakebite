#include "stdafx.h"
#include "moving_object.h"

bool acoross::snakebite::IsCrashed(const MovingObject & mo1, const MovingObject & mo2)
{
	double dist = acoross::snakebite::CalcDistance(mo1.GetPosition(), mo2.GetPosition());

	if (dist < mo1.GetRadius() + mo2.GetRadius())
	{
		return true;
	}

	return false;
}

void acoross::snakebite::MovingObject::Move(const DirVector2D & diff)
{
	// TODO: [�浹���� ��, ������ �ִ� ����, �浹���� ��� ��] �� �����ؼ� �̺�Ʈ �߻���Ų��.
	// �ٸ� ������Ʈ�� �浹�߳�?
	for (auto& mo2 : container_.GetMovingObjects())
	{
		if (this == mo2.get())
			continue;

		if (acoross::snakebite::IsCrashed(*this, *mo2))
		{
			//DoubleDispatch...
			std::cout << "crashed" << std::endl;
		}
	}

	// �׵θ� ������ ����� �ʵ��� ����.
	auto pos_new = pos_;
	pos_new.x += diff.x;
	pos_new.y += diff.y;

	if (pos_new.x > container_.Left && pos_new.x < container_.Right)
	{
		pos_.x = pos_new.x;
	}

	if (pos_new.y > container_.Top && pos_new.y < container_.Bottom)
	{
		pos_.y = pos_new.y;
	}
}
