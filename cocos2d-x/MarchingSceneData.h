//
//  MarchingSceneData.h
//  Created by 楊頌聲 on 2016/10/31.
//

#ifndef __marching__MarchingSceneData__
#define __marching__MarchingSceneData__

#include "common/scene/CastleSceneData.h"

#include "battle/BattleConstants.h"

#include "marching/constants/MarchingConstants.h"

USING_CLASS(DisplayUnitData)
USING_CLASS(QuestStageData)
USING_CLASS(battle,   AttackerData)
USING_CLASS(battle,   DefenderData)
USING_CLASS(battle,   BattleLog)
USING_CLASS(marching, MarchingMode)

//
// 出撃・帰還シーンデータ
//
class MarchingSceneData final : public CastleSceneData
{
// ---------- クラスメソッド
public:
    CREATE_FUNC_WITH_2PARAMS(MarchingSceneData, marching::Mode, battle::Mode)

// ---------- インスタンス変数
private:
    RefPtr<marching::MarchingMode> marchingMode { nullptr };               // マーチングモード

    battle::Mode battleMode                     { battle::Mode::INVALID }; // バトルモード
    int unitAnimationStartFrame                 { 0 };                     // ユニットのアニメーション開始フレーム

// ---------- インスタンスメソッド
private:
    MarchingSceneData();                                             // コンストラクタ
    ~MarchingSceneData();                                            // デストラクタ
    bool init(marching::Mode marchingMode, battle::Mode battleMode); // 初期化

public:
    marching::MarchingMode* getMarchingMode() const;                 // 進行モードの取得
    const battle::Mode      getBattleMode() const;                   // バトルモードの取得

    void setAttackerData(battle::AttackerData* attackerData);        // 侵略側情報の設定
    void setDefenderData(battle::DefenderData* defenderData);        // 防衛側情報の設定
    void setReferenceTime(int64_t referenceTime);                    // アップグレード等時刻比較が必要となった際の基準時刻の設定
    void setBattleId(CONSTREF(string) battleId);                     // バトルIDの設定
    void setBattleLog(battle::BattleLog* battleLog);                 // バトルログの設定
    void setQuestStageData(QuestStageData* questStageData);          // クエスト時のクエストステージデータの設定
    void setReturnSceneData(CastleSceneData* sceneData);             // 終了後遷移シーンデータの設定

    int  getUnitAnimationStartFrame() const;                         // ユニットのアニメーション開始フレームを取得
    void setUnitAnimationStartFrame(int animationStartFrame);        // ユニットのアニメーション開始フレームを設定

    // CastleSceneData
    vector<sprite::Type>        getSpriteFrameTypes() const override; // シーンで使うテクスチャタイプの取得
    scene::Type                 getSceneType() const override;        // シーンタイプの取得
    vector<sound::effect::Type> getSETypes() const override;          // シーンで使うSEタイプの取得
    sound::bgm::Type            getSceneBgm() const override;         // シーン遷移時に流すBGMの取得
};

#endif /* __marching__MarchingSceneData__ */
