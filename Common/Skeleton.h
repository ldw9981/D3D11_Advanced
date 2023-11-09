#pragma once

struct Bone
{
	Math::Matrix Offset;
	std::string Name;
	UINT ParentBoneIndex;
};
struct Skeleton
{
public:
	std::vector<Bone> Bones;
};

