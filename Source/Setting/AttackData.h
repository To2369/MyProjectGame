#pragma once
#include "json.hpp"
#include <optional>
#include <string>

using json = nlohmann::json;
struct AttackData
{
	float hitStartTime;							// �����蔻�肪�n�܂鎞��
	float hitEndTime;							// �����蔻�肪�I��鎞��
	float inputAcceptStartTime;					// ���̍U���ɑJ�ڂ��邽�߂̓��͎�t�J�n
	float inputAcceptEndTime;					// ���̍U���ɑJ�ڂ��邽�߂̓��͎�t�I��
	float progressLimit;						// ���̍U���Ɉڂ�ۂ̃A�j���[�V�����i�s�x���~�b�g
	float hitRadius;							// �����蔻��̑傫��
	std::string hitBoneName;					// �����蔻���t����{�[���̖��O
	int damage;

	/*�A�j���[�V�����ɓ�ȏ�U�����肪����Ƃ��̏��*/
	float secondHitStartTime;		// ���ڂ̓����蔻�肪�n�܂鎞��
	float secondHitEndTime;			// ���ڂ̓����蔻�肪�I��鎞��
	float secondHitRadius;			// ���ڂ̓����蔻��͈̔�
	std::string secondHitBoneName;	// ���ڂ̓����蔻�������{�[���̖��O
};

inline void to_json(json& j, const AttackData& atkData) {
	j = json
	{
		{"hitStartTime", atkData.hitStartTime},
		{"hitEndTime", atkData.hitEndTime},
		{"inputAcceptStartTime", atkData.inputAcceptStartTime},
		{"inputAcceptEndTime", atkData.inputAcceptEndTime},
		{"progressLimit", atkData.progressLimit},
		{"hitRadius", atkData.hitRadius},
		{"hitBoneName", atkData.hitBoneName},
		{"damage", atkData.damage},
		{"secondHitStartTime", atkData.secondHitStartTime},
		{"secondHitEndTime", atkData.secondHitEndTime},
		{"secondHitRadius", atkData.secondHitRadius},
		{"secondHitBoneName", atkData.secondHitBoneName}
	};
}

// JSON����AttackData�ɕϊ�����֐�
inline void from_json(const json& j, AttackData& atkData) 
{
	j.at("hitStartTime").get_to(atkData.hitStartTime);
	j.at("hitEndTime").get_to(atkData.hitEndTime);
	j.at("inputAcceptStartTime").get_to(atkData.inputAcceptStartTime);
	j.at("inputAcceptEndTime").get_to(atkData.inputAcceptEndTime);
	j.at("progressLimit").get_to(atkData.progressLimit);
	j.at("hitRadius").get_to(atkData.hitRadius);
	j.at("hitBoneName").get_to(atkData.hitBoneName);
	j.at("damage").get_to(atkData.damage);

	// optional
	if (j.contains("secondHitStartTime")) j.at("secondHitStartTime").get_to(atkData.secondHitStartTime);
	if (j.contains("secondHitEndTime")) j.at("secondHitEndTime").get_to(atkData.secondHitEndTime);
	if (j.contains("secondHitRadius")) j.at("secondHitRadius").get_to(atkData.secondHitRadius);
	if (j.contains("secondHitBoneName")) j.at("secondHitBoneName").get_to(atkData.secondHitBoneName);
}