//
//  MarchingScene.h
//  Created by 楊頌聲 on 2016/10/31.
//

#ifndef __marching__MarchingScene__
#define __marching__MarchingScene__

#include "common/scene/CastleScene.h"

#include "marching/MarchingSceneData.h"

USING_CLASS(marching, StatusLayer)

//
// 出撃・帰還シーン
//
class MarchingScene final : public CastleScene
{
// ---------- クラスメソッド
public:
    CREATE_FUNC_WITH_PARAM(MarchingScene, MarchingSceneData*)

// ---------- インスタンス変数
private:
    RefPtr<MarchingSceneData> data     { nullptr }; // シーンデータ
    marching::StatusLayer* statusLayer { nullptr }; // ステータスレイヤ

// ---------- インスタンスメソッド
private:
    MarchingScene();                            // コンストラクタ
    ~MarchingScene();                           // デストラクタ
    bool init(MarchingSceneData* data);         // 初期化
    void setupEnvironment();                    // 3D環境をセットアップ

public:
    void onEnter() override;                    // 画面表示時
    void onEnterTransitionDidFinish() override; // シーン開始トランジション終了時
    void onAndroidBackKeyTouched() override;    // Androidのバックキーがおされた時

private:
    void goBattle();                            // バトルへ遷移
    void goBack();                              // 帰還後のシーンへ遷移

};

#endif /* __marching__MarchingScene__ */
