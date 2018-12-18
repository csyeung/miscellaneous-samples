//
//  MarchingSceneData.cpp
//  Created by 楊頌聲 on 2016/10/31.
//

#include "marching/MarchingSceneData.h"

#include "marching/mode/MarchingMode.h"

// ---------- インスタンスメソッド
// コンストラクタ
MarchingSceneData::MarchingSceneData()
{ CCLOGFUNC }

// デストラクタ
MarchingSceneData::~MarchingSceneData()
{ CCLOGFUNC }

// 初期化
bool MarchingSceneData::init(marching::Mode marchingMode, battle::Mode battleMode)
{
    if (!CastleSceneData::init()) return false;

    this->marchingMode = marching::MarchingMode::create(marchingMode, battleMode);
    this->battleMode   = battleMode;

    return true;
}

// 進行モードの取得
marching::MarchingMode* MarchingSceneData::getMarchingMode() const
{
    return this->marchingMode;
}

// バトルモードの取得
const battle::Mode MarchingSceneData::getBattleMode() const
{
    return this->battleMode;
}

// 侵略側情報の設定
void MarchingSceneData::setAttackerData(battle::AttackerData* attackerData)
{
    this->marchingMode->setAttackerData(attackerData);
}

// 防衛側情報の設定
void MarchingSceneData::setDefenderData(battle::DefenderData* defenderData)
{
    this->marchingMode->setDefenderData(defenderData);
}

// アップグレード等時刻比較が必要となった際の基準時刻の設定
void MarchingSceneData::setReferenceTime(int64_t referenceTime)
{
    this->marchingMode->setReferenceTime(referenceTime);
}

// バトルIDの設定
void MarchingSceneData::setBattleId(CONSTREF(string)battleId)
{
    this->marchingMode->setBattleId(battleId);
}

// バトルログの設定
void MarchingSceneData::setBattleLog(battle::BattleLog* battleLog)
{
    this->marchingMode->setBattleLog(battleLog);
}

// クエスト時のクエストステージデータの設定
void MarchingSceneData::setQuestStageData(QuestStageData* questStageData)
{
    this->marchingMode->setQuestStageData(questStageData);
}

// 終了後遷移シーンデータの設定
void MarchingSceneData::setReturnSceneData(CastleSceneData* sceneData)
{
    this->marchingMode->setReturnSceneData(sceneData);
}

// ユニットのアニメーション開始フレームを取得
int MarchingSceneData::getUnitAnimationStartFrame() const
{
    return this->unitAnimationStartFrame;
}

// ユニットのアニメーション開始フレームを設定
void MarchingSceneData::setUnitAnimationStartFrame(int animationStartFrame)
{
    this->unitAnimationStartFrame = animationStartFrame;
}

#pragma mark -
#pragma mark CastleSceneData

// シーンで使うテクスチャタイプの取得
vector<sprite::Type> MarchingSceneData::getSpriteFrameTypes() const
{
    vector<sprite::Type> types { CastleSceneData::getSpriteFrameTypes() };
    types.push_back(sprite::Type::MARCHING);

    // FIXME::使いまわさないようにする
    types.push_back(sprite::Type::MATCHING);

    return types;
}

// シーンタイプの取得
scene::Type MarchingSceneData::getSceneType() const
{
    return scene::Type::MARCHING;
}

// シーンで使うSEタイプの取得
vector<sound::effect::Type> MarchingSceneData::getSETypes() const
{
    vector<sound::effect::Type> types { CastleSceneData::getSETypes() };
    types.push_back(sound::effect::Type::MARCHING);
    types.push_back(sound::effect::Type::NONE);

    return types;
}

// シーン遷移時に流すBGMの取得
sound::bgm::Type MarchingSceneData::getSceneBgm() const
{
    return this->marchingMode->getBgmType();
}
